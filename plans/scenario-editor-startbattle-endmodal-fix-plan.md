# BattleSim Scenario-Editor "Start Battle" `EndModal` Assert Fix — Implementation Plan

Feature slug: `scenario-editor-startbattle-endmodal-fix`
Plan artifact: `plans/scenario-editor-startbattle-endmodal-fix-plan.md`
Coordination base branch: dedicated per-plan branch off `fixes` (never `master`/`main`).
Specialist Security stage: **not required** (no permissions, secrets, untrusted
input, external integrations, or destructive operations).

## Feature Summary

Root-cause and fix a **pre-existing, wxGTK-specific** runtime defect in the
**BattleSim** app. Reproduction: BattleSim → "Play a Local Game" → "Create a New
Custom Game" → build fleets → **Start Battle**. On wxGTK this throws
`wxDialog::EndModal` (`../src/gtk/dialog.cpp:193`, "either `wxDialog::EndModal`
called twice or `ShowModal` wasn't called"). Confirmed by the user's captured
backtrace.

**This supersedes the previous draft.** The earlier `battle-quit-endmodal-fix`
plan targeted the tactical (main-game) `FBattleScreen` File→Quit path
(hypotheses H1/H2 involving `FBattleScreen`/`SelectCombatGUI`). **The captured
backtrace RULES THAT OUT** — the assert is in the BattleSim scenario-editor
"Start Battle" flow, not the tactical Quit path. The tactical `FBattleScreen`
close/modality code was just rolled back (RB-01, `d0e1134`) to the merge-base and
is **not changed by this fix**.

## Confirmed root cause (with captured trace + file:line)

Captured backtrace (wxGTK, `../src/gtk/dialog.cpp:193`):

```
[1] wxDialog::EndModal(int)
[2] ScenarioEditorGUI::finalizeStartBattle()   include/battleSim/ScenarioEditorGUI.h:97
[3] ScenarioEditorGUI::onStartBattle(wxCommandEvent&)  src/battleSim/ScenarioEditorGUI.cpp:424-439
    reached via: BattleSimFrame::onPlayLocal (ShowModal LocalGameDialog)
      -> LocalGameDialog::onCreateNew (ShowModal ScenarioEditorGUI)  src/battleSim/LocalGameDialog.cpp:67-71
      -> Start Battle button
```

Verified against the current tree:

- `LocalGameDialog::onCreateNew()` (`src/battleSim/LocalGameDialog.cpp:67-71`)
  runs `ScenarioEditorGUI d(this); d.ShowModal();` — so the modal object is a
  plain **`ScenarioEditorGUI`** instance (a `wxDialog`,
  `include/battleSim/ScenarioEditorGUI.h:38`). It is **not** a subclass, and
  neither `onStartBattle` nor `finalizeStartBattle` is overridden in production
  (the `ScenarioDialog` class is a *separate* predefined-scenario menu, unrelated
  to this instance). `BattleSimFrame::onPlayLocal()`
  (`src/battleSim/BattleSimFrame.cpp:58-60`) shows the `LocalGameDialog` modally
  above it.
- `ScenarioEditorGUI::onStartBattle()` (`src/battleSim/ScenarioEditorGUI.cpp:388-441`)
  ends with:
  ```
  FBattleScreen bb;                         // :424
  bb.setupFleets(&aList,&dList,hasPlanet,station); // :425
  Hide();                                   // :426
  bb.ShowModal();                           // :427
  ...
  finalizeStartBattle();                    // :439
  ```
- `finalizeStartBattle()` is defined inline as
  `virtual void finalizeStartBattle() { EndModal(0); }`
  (`include/battleSim/ScenarioEditorGUI.h:97`).

**Mechanism (double EndModal on wxGTK):** the `ScenarioEditorGUI` is modal (shown
via `d.ShowModal()`). On wxGTK, `wxDialog::Show(false)` — invoked by the `Hide()`
at line 426 — internally calls `EndModal(wxID_CANCEL)` when the dialog is modal,
ending its modal loop (**first EndModal**). After the nested
`bb.ShowModal()` battle returns, `finalizeStartBattle()` at line 439 calls
`EndModal(0)` on the now-already-ended (non-modal, `m_modalLoop` cleared) dialog
(**second EndModal**), tripping `dialog.cpp:193`.

This "`Show(false)` auto-ends a modal wxGTK dialog" behavior is the exact
behavior already documented in the prior tactical investigation
(`artifacts/fbattlescreen-menu-quit-fix.md`, §Attempt 3) — here it manifests in
`ScenarioEditorGUI` instead. Corroboration: the wx 3.3.2 gtk dialog header
(`/usr/local/include/wx-3.3/wx/gtk/dialog.h:47`) confirms the modal-state member
`m_modalLoop` that the assert tests.

**Platform note:** On MSW, `wxDialog::Show(false)` does **not** auto-end the modal
loop, so line 426's `Hide()` does not fire the first EndModal there; line 439's
single `EndModal(0)` runs on a still-modal dialog and closes it cleanly. That is
why this is a **wxGTK/Linux-only** crash — consistent with the Linux repro.

## Related path assessment — predefined scenarios

`ScenarioDialog::onScenario1..4` (`src/battleSim/ScenarioDialog.cpp:131-133,
206-208, 252-254, 336-338`) use the pattern `Hide(); bb.ShowModal(); Show();`
with **no explicit `EndModal`**. Analysis:

- **No crash:** only one EndModal occurs (the wxGTK `Hide()` side-effect); there is
  no second explicit `EndModal`, so `dialog.cpp:193` does not fire. The predefined
  path is NOT the reported crash.
- **Latent cross-platform quirk:** relying on `Hide()`'s wxGTK side-effect to end
  the modal loop means, on wxGTK, after one scenario the `ScenarioDialog`'s modal
  loop is already exiting, so the trailing `Show()` re-shows it only briefly and
  the dialog closes — you cannot play a second predefined scenario. On MSW,
  `Hide()` does not end the loop, so `Show()` genuinely re-shows it and multiple
  scenarios work. This is fragile and platform-inconsistent, though non-crashing.

For consistency and robustness, this plan fixes the predefined path with the same
approach (below), which also removes the fragile `Hide()`-side-effect reliance and
makes the "play another scenario" UX work uniformly. This is low-risk and directly
answers whether the predefined path should be fixed consistently: **yes.**

## Intended UX (confirmed from code intent)

- **Custom game (`onStartBattle`):** pressing Start Battle leaves the editor,
  plays the battle, and on return **closes the editor** (single `EndModal(0)` via
  `finalizeStartBattle()`), returning to `LocalGameDialog`.
- **Predefined (`onScenarioN`):** playing a scenario returns to the
  **scenario-list dialog** so the user can play another or press **Done**
  (`onDone` → `EndModal(0)`).

In both cases the `FBattleScreen` battle window is a separate, modal top-level
shown on top; the launching dialog does not need to be interactable during the
battle. Leaving it shown behind the modal battle window is exactly how the
confirmed-working tactical `SelectCombatGUI` → `FBattleScreen` flow already
behaves (`src/gui/SelectCombatGUI.cpp:277-285` does not hide itself).

## Chosen fix and justification

**Selected: (a) Remove the premature `Hide()` before `bb.ShowModal()`; end the
launching dialog's modal loop exactly once, at the correct point.**

- **Custom (`ScenarioEditorGUI::onStartBattle`):** delete the `Hide();` at
  `:426`. Keep `bb.ShowModal();` then `finalizeStartBattle();` (a single
  `EndModal(0)` after the battle returns). The editor stays modal behind the
  modal battle window; when the battle returns, exactly one `EndModal(0)` closes
  it.
- **Predefined (`ScenarioDialog::onScenarioN`, all four):** delete the `Hide();`
  and the trailing `Show();`, leaving `bb.ShowModal();`. The scenario dialog stays
  properly modal behind the battle; on return the user is back at the scenario
  list; `onDone` closes it with a single `EndModal(0)`.

**Why (a) over the alternatives:**

- **(b) Keep `Hide()` and guard the final call with `if (IsModal()) EndModal(0);`.**
  This does avoid the assert (on wxGTK `Hide()` already ended the loop, so
  `IsModal()` is false and the second call is skipped; on MSW `IsModal()` is true
  and `EndModal(0)` runs once). It even preserves "editor hidden during battle."
  **Rejected because** it makes the code depend on wxGTK's implicit
  `Show(false)`→`EndModal` side-effect *as a feature* to end the loop — the exact
  fragile, non-obvious, platform-divergent behavior that caused this bug. It is
  correct but obscure and easy to re-break. (a) ends the loop with a single
  explicit `EndModal` on all platforms.
- **(c) Restructure to hide-and-end-exactly-once explicitly.** Any variant that
  keeps `Hide()` inherits the same wxGTK-vs-MSW divergence around when the loop
  ends and adds complexity for a purely cosmetic "hide the editor" benefit. (a) is
  simpler, fully explicit, portable, and matches the working tactical topology.

**Cosmetic trade-off of (a):** the launching dialog remains visible behind the
`FBattleScreen` (1200×900, centered, modal), which typically covers it; minor
edge peeking is possible and is identical to the existing tactical flow. This is
an acceptable, intentional trade for a single, explicit, portable `EndModal`.

**Windows/VS build considerations:** the fix removes calls only; no new symbols,
includes, or types. It compiles identically under VS2022. Behavior on MSW is
unchanged for the observable outcome (dialog closes once after the battle); it
merely removes the now-unnecessary `Hide()`/`Show()` that were only ever a wxGTK
liability. No `.vcxproj` change is needed.

## Confirmed vs Assumed

- **Confirmed:** all file:line citations above; the double-EndModal mechanism
  matches the captured trace exactly; the modal object is a plain
  `ScenarioEditorGUI`; the predefined path does not double-`EndModal`; the bug is
  wxGTK-specific.
- **Assumed (low-risk):** that leaving the launching dialog shown behind the modal
  `FBattleScreen` is acceptable UX (strongly supported: it mirrors the
  confirmed-working tactical `SelectCombatGUI` flow). If the user insists the
  editor must be visually hidden during the battle, fall back to approach (b) for
  the custom path only and note it in the report — but (a) is the recommendation.

## Governing Rules Constraint

- `doc/rules/tactical_operations_manual.md` MUST NOT be edited (non-volatile
  external artifact). This fix is GUI modal-lifecycle only and does not touch game
  rules.
- Module boundaries: `ScenarioEditorGUI.cpp` and `ScenarioDialog.cpp` are
  **battleSim (gui-layer)** files; wx usage is expected and in-boundary. No wx is
  added to any non-GUI model module. `FBattleScreen` is **not** modified.
- The `src/battleSim/*.cpp` files carry a wxFormBuilder "do NOT edit" banner, but
  their **event-handler bodies** (`onStartBattle`, `onScenarioN`) are already
  hand-maintained game logic. Edits MUST be confined to those hand-written handler
  bodies (removing the `Hide()`/`Show()` calls); do not touch the generated
  widget-construction/layout code or the inline `finalizeStartBattle()` in the
  generated `.h`.

## Verification Policy (mandatory)

The full runtime reproduction requires the `ScenarioEditorGUI` to be **live-modal
(`ShowModal`) with a nested live `FBattleScreen::ShowModal`** — the "two nested
live modal loops" scenario that prior rounds found unsafe to drive in the headless
GUI harness (runner hangs / heap corruption). Note also that the existing test
peer `ScenarioEditorGUITestPeer` (`tests/gui/BattleSimGuiLiveTest.cpp:293-366`)
**overrides `finalizeStartBattle()` to a no-op flag** — deliberately skipping the
real `EndModal(0)` — which is exactly why the existing suite never caught this
bug. Therefore:

1. **A REQUIRED user manual-verification pass is the acceptance gate.** Before
   this feature is accepted, the user must confirm in the real BattleSim app that
   **Play a Local Game → Create a New Custom Game → Start Battle** plays the
   battle and, on return, closes the editor with **no `wxDialog::EndModal`
   assert**; and that **Play a Predefined Scenario → Play** returns to the scenario
   list (playable again) and **Done** closes it, with no assert. The Coordinator
   MUST surface this gate and MUST NOT declare the feature accepted until the user
   signs off.

2. **A genuine, SAFE behavioral regression test IS feasible and IS required** —
   without driving two nested live modal loops. The fix's observable behavioral
   change is that `onStartBattle` **no longer hides the editor before launching
   the battle** and still finalizes exactly once after it. This is directly
   observable by driving the real `onStartBattle` handler (as the existing
   lifecycle test already does via `clickStartBattle()`), with the nested
   `FBattleScreen` auto-dismissed by the harness, and asserting the editor was
   **not** `Hide()`-den before the battle screen appeared and that finalize ran
   exactly once. This test **fails against the unfixed code** (which calls
   `Hide()` at `:426`) and **passes after** the fix — a real behavioral
   discriminator that is safe (no real second `EndModal`, no live editor
   `ShowModal`). It is a legitimate behavioral proxy because removing that
   `Hide()` is precisely what eliminates the first of the two `EndModal` calls.

3. Source-inspection assertions (e.g. "the handler no longer contains `Hide()`")
   MAY supplement but MUST NOT be the sole backing for the behavior, per the
   repository's behavioral-verification rule. The end-to-end no-assert runtime
   behavior is proven by the manual pass (leg 1); the control-flow change is
   locked by the behavioral test (leg 2).

## Overall Acceptance Criteria

- **AC1:** The double `EndModal` on the custom "Start Battle" path is eliminated:
  `ScenarioEditorGUI::onStartBattle` ends the editor's modal loop exactly once
  (via `finalizeStartBattle()`/`EndModal(0)` after the battle returns) with no
  premature `Hide()` before `bb.ShowModal()`. Confirmed by the manual pass and by
  a behavioral test that fails pre-fix and passes post-fix.
- **AC2:** The predefined path (`ScenarioDialog::onScenario1..4`) no longer relies
  on `Hide()`'s wxGTK side-effect: each handler leaves `bb.ShowModal();` without
  the surrounding `Hide()`/`Show()`, the scenario dialog remains modal and
  replayable, and `Done`/title-bar close ends it exactly once. Confirmed by the
  manual pass; no assert on any scenario button.
- **AC3:** No regression to the `FBattleScreen` battle itself, the `LocalGameDialog`
  chain, or `BattleSimFrame::onPlayLocal`; `FBattleScreen` is unmodified; the
  `FBattleScreen` construct/destroy lifecycle balance asserted by the existing
  BattleSim GUI tests still holds.
- **AC4:** The change is confined to the two battleSim handler files (production)
  plus the BattleSim GUI test (Tester-owned); edits stay within hand-written
  handler bodies; no generated layout code or generated `.h` is altered.
- **AC5:** `doc/rules/tactical_operations_manual.md` is unedited; no wx dependency
  is added to any non-GUI model module; the BattleSim/GUI and tactical suites
  build and pass with no new regressions.

## Overall Documentation Impact

- `AGENTS.md`: add a short note on the BattleSim scenario launch modal-lifecycle
  (launching dialog stays modal behind the modal `FBattleScreen`; the editor/
  scenario dialog ends its modal loop exactly once; no `Hide()`-before-`ShowModal`)
  if the BattleSim modal flow is documented there.
- `doc/test-contracts.md`: add/adjust the BattleSim start-battle modal-lifecycle
  contract to reflect the removed `Hide()`/`Show()` and the new behavioral test;
  record that the end-to-end no-assert runtime behavior is backed by the user
  manual-verification pass.
- `doc/UsersGuide.md`: no change expected (behavior is "Start Battle no longer
  crashes"); update only if the BattleSim custom/predefined flow is documented.
- `doc/rules/tactical_operations_manual.md`: MUST NOT change.
- Doxygen: `ScenarioEditorGUI.cpp` / `ScenarioDialog.cpp` are generated files; add
  or update a concise function-level Doxygen/comment note on the edited handler
  bodies explaining why `Hide()`/`Show()` were removed (single-`EndModal` modal
  lifecycle), consistent with the repository convention for hand-edited handlers.

## Dependency Ordering and Parallelization

Single subtask — no ordering or parallelization concerns.

## Output Artifact Path

- This plan artifact: `plans/scenario-editor-startbattle-endmodal-fix-plan.md`
  (repo-root-relative).
- Per-subtask orchestration artifacts:
  `artifacts/scenario-editor-startbattle-endmodal-fix/SEB-01/` (repo-root-relative).

## Implementer Prompt

The subtask below provides its launch-ready Implementer prompt inline. It begins
with `Your role is 'implementer'. Your task is as follows:` and ends with the
explicit completion gate.

---

## SEB-01 — Remove premature `Hide()` so the scenario dialogs end their modal loop exactly once

Security review: not required.

### Task

Fix the wxGTK double-`EndModal` crash on BattleSim "Start Battle" (and remove the
same fragile pattern from the predefined-scenario path) by ending each launching
dialog's modal loop exactly once. In `ScenarioEditorGUI::onStartBattle()`
(`src/battleSim/ScenarioEditorGUI.cpp:388-441`), remove the `Hide();` at line 426
that precedes `bb.ShowModal();`; keep `bb.ShowModal();` followed by the single
`finalizeStartBattle();` (`EndModal(0)`) at line 439. In
`ScenarioDialog::onScenario1..4()` (`src/battleSim/ScenarioDialog.cpp:131-133,
206-208, 252-254, 336-338`), remove the `Hide();` before `bb.ShowModal();` and the
`Show();` after it, leaving just `bb.ShowModal();`. Do NOT modify `FBattleScreen`,
`LocalGameDialog`, `BattleSimFrame`, or the inline `finalizeStartBattle()` in
`include/battleSim/ScenarioEditorGUI.h`. Confine edits to the hand-written event-
handler bodies; do not touch generated widget-construction/layout code.

Root cause (captured wxGTK backtrace, `../src/gtk/dialog.cpp:193`): the modal
`ScenarioEditorGUI`'s `Hide()` (== `Show(false)`) auto-calls
`EndModal(wxID_CANCEL)` on wxGTK (first EndModal), then `finalizeStartBattle()`'s
explicit `EndModal(0)` after the battle returns double-ends the already-ended loop
(second EndModal) → the assert. Removing the premature `Hide()` leaves exactly one
explicit `EndModal` and closes the dialog correctly on both wxGTK and MSW (the bug
is wxGTK-only; MSW `Show(false)` does not auto-end). The predefined path does not
crash today but relies on the same fragile `Hide()` side-effect and is fixed for
consistency and correct replay UX.

### Allowed Files

- `src/battleSim/ScenarioEditorGUI.cpp`
- `src/battleSim/ScenarioDialog.cpp`

(The BattleSim GUI behavioral test is Tester-owned; see Tester Guidance. Do not
edit test files.)

### Acceptance Criteria

- `ScenarioEditorGUI::onStartBattle` no longer calls `Hide()` before
  `bb.ShowModal()`; it still calls `finalizeStartBattle()` exactly once after the
  battle returns, so the editor's modal loop is ended exactly once (no double
  `EndModal`).
- Each `ScenarioDialog::onScenarioN` no longer wraps `bb.ShowModal()` in
  `Hide()`/`Show()`; the scenario dialog remains modal and replayable and is closed
  once via `onDone`.
- `FBattleScreen`, `LocalGameDialog`, `BattleSimFrame`, and the generated `.h`
  (including inline `finalizeStartBattle()`) are unchanged; edits are confined to
  the hand-written handler bodies.
- Builds cleanly (`make`); the existing BattleSim/GUI and tactical suites pass with
  no new regressions; `FBattleScreen` construct/destroy lifecycle balance still
  holds.
- `doc/rules/tactical_operations_manual.md` is unedited; no wx added to any
  non-GUI model module.
- Concise Doxygen/comment note added to each edited handler explaining the
  single-`EndModal` modal lifecycle and why `Hide()`/`Show()` were removed, with
  `@author` (your model + reasoning level) and `Last Modified` per `AGENTS.md`
  convention for hand-edited handlers.

### Documentation Impact

- `AGENTS.md`: note the BattleSim scenario launch modal-lifecycle (launching
  dialog stays modal behind the modal `FBattleScreen`; single `EndModal`; no
  `Hide()`-before-`ShowModal`) if documented there.
- `doc/test-contracts.md`: adjust/add the BattleSim start-battle modal-lifecycle
  contract; record the manual-verification dependency for the end-to-end no-assert
  behavior.
- `doc/rules/tactical_operations_manual.md`: MUST NOT change.

### Tester Guidance

- The existing `ScenarioEditorGUITestPeer`
  (`tests/gui/BattleSimGuiLiveTest.cpp:293-366`) overrides `finalizeStartBattle()`
  to a no-op flag, which is why the bug was never caught. Add a **behavioral**
  regression test in `tests/gui/BattleSimGuiLiveTest.cpp` that drives the real
  `onStartBattle` (via `clickStartBattle()`) with the nested `FBattleScreen`
  auto-dismissed by the harness (as `testScenarioEditorStartBattleLaunchesBattleScreenWithLifecycleCoverage`
  already does) and asserts the editor was **not** `Hide()`-den before the battle
  screen appeared (e.g. a peer that overrides `Hide()` to count calls, or records
  `IsShown()` at battle-launch time) and that `finalizeStartBattle` ran exactly
  once. This assertion FAILS against the unfixed code (which calls `Hide()` at
  `:426`) and PASSES after — a safe behavioral discriminator that does NOT drive a
  live editor `ShowModal` nested around a live `FBattleScreen::ShowModal`.
- Add analogous coverage for at least one `ScenarioDialog::onScenarioN` asserting
  the `Hide()`/`Show()` wrapping is gone (scenario dialog stays shown/replayable
  after the battle is dismissed), reusing the existing `ScenarioDialogTestPeer`
  driving pattern.
- Do NOT attempt to reproduce the full double-`EndModal` by driving a live
  `ScenarioEditorGUI::ShowModal()` wrapping a live `FBattleScreen::ShowModal()`
  (two nested live modal loops) — prior rounds documented runner hangs / heap
  corruption. Explicitly defer the end-to-end no-assert runtime behavior to the
  required user manual-verification pass. Source-contract assertions may supplement
  but not solely back the behavior.

### Implementer Prompt

```
Your role is 'implementer'. Your task is as follows:

Fix a pre-existing wxGTK-only double-wxDialog::EndModal crash in the BattleSim
app. Repro: BattleSim -> "Play a Local Game" -> "Create a New Custom Game" ->
build fleets -> Start Battle -> assert at ../src/gtk/dialog.cpp:193 ("either
wxDialog::EndModal called twice or ShowModal wasn't called").

Confirmed root cause (captured backtrace): the modal ScenarioEditorGUI (shown via
LocalGameDialog::onCreateNew d.ShowModal(), src/battleSim/LocalGameDialog.cpp:67-71)
runs, in onStartBattle() (src/battleSim/ScenarioEditorGUI.cpp:388-441):
    FBattleScreen bb;                 // :424
    bb.setupFleets(...);              // :425
    Hide();                           // :426  <-- on wxGTK, Show(false) auto-calls
                                      //         EndModal(wxID_CANCEL): FIRST EndModal
    bb.ShowModal();                   // :427
    ...
    finalizeStartBattle();            // :439  -> EndModal(0): SECOND EndModal on the
                                      //         already-ended loop -> assert
finalizeStartBattle() is inline in include/battleSim/ScenarioEditorGUI.h:97 as
  virtual void finalizeStartBattle() { EndModal(0); }
On MSW, Show(false) does NOT auto-end a modal dialog, so the bug is wxGTK/Linux only.

Do the following (edit ONLY hand-written event-handler bodies; do NOT touch
generated widget/layout code or the generated .h):

1) src/battleSim/ScenarioEditorGUI.cpp, onStartBattle(): DELETE the `Hide();` at
   line 426. Keep `bb.ShowModal();` then `finalizeStartBattle();` so the editor's
   modal loop is ended exactly ONCE (a single EndModal(0)) after the battle
   returns. This closes the editor correctly on both wxGTK and MSW.

2) src/battleSim/ScenarioDialog.cpp, onScenario1()..onScenario4() (four handlers
   at :131-133, :206-208, :252-254, :336-338): DELETE the `Hide();` before
   `bb.ShowModal();` and the `Show();` after it, leaving just `bb.ShowModal();`.
   This removes the same fragile reliance on Hide()'s wxGTK side-effect; the
   scenario dialog stays properly modal behind the battle, is replayable, and is
   closed once by its Done button (onDone -> EndModal(0)). The predefined path does
   not crash today but is fixed here for consistency and correct replay UX.

Do NOT modify FBattleScreen, LocalGameDialog, BattleSimFrame, or the inline
finalizeStartBattle() in include/battleSim/ScenarioEditorGUI.h. The launching
dialog intentionally stays shown behind the modal FBattleScreen (identical to the
confirmed-working tactical SelectCombatGUI -> FBattleScreen flow); this is the
chosen approach over guarding the final EndModal with `if (IsModal())`, because
removing the premature Hide() ends the loop with a single EXPLICIT EndModal on all
platforms instead of relying on wxGTK's implicit Show(false)->EndModal side-effect.

Allowed files (modify only these):
- src/battleSim/ScenarioEditorGUI.cpp
- src/battleSim/ScenarioDialog.cpp

Acceptance criteria (implementation outcomes):
- onStartBattle no longer calls Hide() before bb.ShowModal(); it still calls
  finalizeStartBattle() exactly once after the battle returns (single EndModal).
- Each onScenarioN no longer wraps bb.ShowModal() in Hide()/Show(); just
  bb.ShowModal(); remains.
- FBattleScreen, LocalGameDialog, BattleSimFrame, and the generated .h are
  unchanged; edits confined to hand-written handler bodies.
- Builds cleanly; existing BattleSim/GUI and tactical suites pass; FBattleScreen
  construct/destroy balance still holds.

Verification note: the full end-to-end no-assert runtime behavior requires a live
ScenarioEditorGUI::ShowModal wrapping a live nested FBattleScreen::ShowModal (two
nested live modal loops), which prior rounds found unsafe to drive in the headless
GUI harness (runner hangs / heap corruption). Do NOT drive that in a test. The
end-to-end no-assert behavior is proven by the REQUIRED user manual-verification
pass (Play Local -> Create New Custom -> Start Battle closes with no assert; and
Play Predefined -> Play -> replayable -> Done closes). State in your report that
this runtime behavior depends on that manual pass; do NOT claim a runtime PASS from
source structure.

Constraints:
- Do NOT edit doc/rules/tactical_operations_manual.md.
- Add a concise Doxygen/comment note to each edited handler explaining the
  single-EndModal modal lifecycle and why Hide()/Show() were removed (@author with
  your model + reasoning level, Last Modified to today) per AGENTS.md.
- Do NOT modify test files; behavioral coverage is owned by the Tester stage.

Validation guidance:
- Build from the repo root with `make` (or `make -C src all` for production only).
- Run the GUI suite headless: `cd tests/gui && make && xvfb-run -a ./GuiTests`
  (BattleSimGuiLiveTest exercises the scenario/start-battle lifecycle).
- Run the tactical suite: `cd tests && make tactical-tests && ./tactical/TacticalTests`.
- The definitive proof is the user manual-verification pass; flag it in your report
  as the acceptance gate.

Tester handoff: add a behavioral test in tests/gui/BattleSimGuiLiveTest.cpp that
drives the real onStartBattle (clickStartBattle) with the harness auto-dismissing
the nested FBattleScreen and asserts the editor was NOT Hide()-den before the
battle appeared and finalize ran exactly once (fails pre-fix, passes post-fix);
add analogous coverage for a ScenarioDialog::onScenarioN (no Hide()/Show()
wrapping; dialog stays replayable). Do NOT drive two nested live modal loops;
defer end-to-end no-assert to the user manual pass.

Artifacts: write your report to
artifacts/scenario-editor-startbattle-endmodal-fix/SEB-01/ (implementer_report.md
and implementer_result.json), repo-root-relative.

Continue past preflight and complete the work when no blocking issues are present.

Do not report success unless all required artifacts exist and all changes are committed.
```
