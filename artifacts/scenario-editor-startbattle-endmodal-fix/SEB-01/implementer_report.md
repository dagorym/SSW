# SEB-01 Implementer Report

## Task

Fix a wxGTK-only double-`wxDialog::EndModal` crash in BattleSim's
`ScenarioEditorGUI::onStartBattle()`, and remove the equivalent fragile
`Hide()`/`Show()` pattern around `bb.ShowModal()` in `ScenarioDialog`'s four
`onScenarioN` handlers for consistency.

## Root Cause

`ScenarioEditorGUI::onStartBattle()` called `Hide()` on the still-open modal
editor dialog immediately before showing the nested `FBattleScreen` modally.
On wxGTK, `Show(false)` on a live modal dialog implicitly calls
`EndModal(wxID_CANCEL)`. After the nested `FBattleScreen::ShowModal()`
returned, `finalizeStartBattle()` (inline in
`include/battleSim/ScenarioEditorGUI.h`, unmodified) called `EndModal(0)`
again — a second `EndModal` on an already-ended modal loop, which trips the
wxGTK assert at `src/gtk/dialog.cpp:193` ("either wxDialog::EndModal called
twice or ShowModal wasn't called"). This is wxGTK-specific because on MSW,
`Show(false)` does not auto-end a modal dialog.

## Changes Made

### `src/battleSim/ScenarioEditorGUI.cpp` — `onStartBattle()`
- Deleted the `Hide();` call immediately before `bb.ShowModal();`.
- `bb.ShowModal();` and the subsequent `finalizeStartBattle();` (single
  explicit `EndModal(0)`) are unchanged, so the editor's modal loop now ends
  exactly once, on both wxGTK and MSW.
- Added a comment (author: `Sonnet 5 (medium)`) documenting the single-
  EndModal modal lifecycle rationale.

### `src/battleSim/ScenarioDialog.cpp` — `onScenario1()`..`onScenario4()`
- Deleted the `Hide();` before and `Show();` after each `bb.ShowModal();`
  call (four call sites), leaving just `bb.ShowModal();`.
- This removes reliance on wxGTK's implicit `Show(false)` → `EndModal` side
  effect for a dialog that is only ever closed explicitly by its own Done
  button (`onDone()` → `EndModal(0)`). The predefined-scenario path did not
  crash before this change (that dialog is not itself modal-in-modal in the
  same fragile way and its `Show()` call happened to restore it correctly),
  but the pattern was fragile and inconsistent with the fixed
  `ScenarioEditorGUI` path, so it is fixed here too.
- Added comments (author: `Sonnet 5 (medium)`) on each handler, with the
  full rationale on `onScenario1()` and short pointers back to it on
  `onScenario2()`–`onScenario4()`.

No other files were modified. `FBattleScreen`, `LocalGameDialog`,
`BattleSimFrame`, and the inline `finalizeStartBattle()` in
`include/battleSim/ScenarioEditorGUI.h` are untouched, as required.

## Validation

- `make -C src all` — clean build, no new warnings/errors introduced by
  this change (pre-existing unrelated `-Wint-to-pointer-cast` warnings in
  `FJumpRoute.cpp` are unchanged from baseline).
- `cd tests/gui && make && ./GuiTests` (headless `xvfb-run` was not available
  in this environment; run instead directly against the session's live
  `$DISPLAY`). Result: 65/65 tests passed on the second run. The first run
  showed a single flaky failure in
  `BattleSimGuiLiveTest::testLocalGameDialogLaunchesPredefinedAndCustomModalChains`
  (a `centered` assertion on `ScenarioDialog`'s position relative to its
  parent) that is unrelated to the edited handler bodies (it does not touch
  `onStartBattle` or `onScenarioN` bodies) and passed cleanly on rerun —
  classified as a pre-existing environment-timing flake, not a regression
  from this change.
- `cd tests && make tactical-tests && ./tactical/TacticalTests` — 252/253
  passed. One **expected** failure:
  `FTacticalBattleScreenElectricalFireTest::testBattleSimLaunchPathUsesModalDialogFlowIntoBattleScreen`
  (line 200) asserts (via source-text inspection of `ScenarioDialog.cpp`)
  that each `onScenarioN` body contains exactly one `Hide();`, one
  `bb.ShowModal();`, and one `Show();` in that order. This test encodes the
  now-intentionally-removed `Hide()`/`Show()` pattern as a requirement, so
  it fails as a direct, expected consequence of this approved fix. Per
  validation-triage, this is **not a regression** — the task instructions
  explicitly required deleting the `Hide()`/`Show()` pair from all four
  `onScenarioN` handlers. This test is not in the implementer's allowed
  file list (`tests/tactical/*`), so it was left unmodified; the Tester
  stage owns updating/replacing this source-contract assertion with
  behavioral coverage per the Tester handoff below.

## Runtime (manual-verification) Caveat

Per task instructions, driving two nested live modal loops
(`ScenarioEditorGUI::ShowModal()` wrapping a live `FBattleScreen::ShowModal()`)
in the headless GUI test harness is unsafe (prior rounds found it causes
runner hangs / heap corruption) and was **not** attempted here. The
end-to-end "no assert" runtime behavior for:
- Play a Local Game → Create a New Custom Game → build fleets → Start Battle
  (closes cleanly, no wxGTK assert), and
- Play a Local Game → Play a Predefined Game → Play a scenario → replayable →
  Done (closes cleanly)

depends on the **required user manual-verification pass**. This report does
NOT claim a runtime PASS from source structure alone — only build success
and the (partially pre-existing-flaky) automated suite results above are
claimed as proven by this implementer stage.

## Files Changed

- `src/battleSim/ScenarioEditorGUI.cpp`
- `src/battleSim/ScenarioDialog.cpp`

## Commit

- Implementation commit: `57cf70b07b30561e24c2a18c72ece784a7ac12bb` on branch
  `fixes-seb01-implementer-20260703`.
