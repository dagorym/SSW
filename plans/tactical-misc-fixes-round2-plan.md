# Tactical Misc Fixes Round 2 — Implementation Plan

Feature slug: `tactical-misc-fixes-round2`
Plan artifact: `plans/tactical-misc-fixes-round2-plan.md`
Coordination base branch: dedicated per-plan branch (never `master`/`main`).
Specialist Security stage: **not required** for any subtask (no permissions,
secrets, untrusted input, external integrations, or destructive operations).

## Feature Summary

Follow-up round correcting three items from the prior `tactical-misc-fixes` round
that passed on source-contract tests but failed at runtime, plus refinements to the
end-of-move turn feature and reviewer-flagged test-robustness cleanup:

1. Battle window (`FBattleScreen`): restore a working minimize button, make the
   title-bar X reliably close the battle mid-game, and eliminate the residual
   `wxDialog::EndModal` assert on Quit.
2. Turn Left/Right buttons: fix the enable logic so a ship with remaining MR can
   still turn (the buttons are wrongly greyed in the hex right after a turn).
3. Turn-buttons panel layout: wrap the over-long move-phase instruction text, fix
   the panel placement so the buttons are visible at the default window size, and
   add an explanatory caption above the buttons.
4. GUI test-suite robustness: make the GUI source-contract tests
   cwd-independent and de-duplicate accumulated `@author` tags.

## Root-Cause Findings (from investigation of the current merged code)

- **Minimize still absent:** the round 1 minimize fix removed the
  `wxTOPLEVEL_EX_DIALOG` type-hint, but the actual suppressor is
  `gtk_window_set_modal(GTK_WINDOW(m_widget), TRUE)` (`FBattleScreen.cpp:252`),
  which was intentionally kept. A GTK **modal** top-level has no minimize button
  regardless of `wxMINIMIZE_BOX`. The style already includes `wxMINIMIZE_BOX`
  (`FBattleScreen.h:66`) and always has.
- **Title-bar X inert mid-game:** `FBattleScreen` is a `wxFrame` with a hand-rolled
  modal loop, while the battle child dialogs (damage summary / ICM / winner-info,
  in `WXTacticalUI`) are real `wxDialog`s run with `ShowModal()`. While a child
  holds the grab, the frame's X is swallowed and never reaches `onClose`
  (`FBattleScreen.cpp:1020-1027`). Quit only works via a bespoke GTK "activate"
  bypass (`FBattleScreen.cpp:191-217`); the X has no equivalent.
- **Residual Quit assert:** `WXTacticalUI::dismissActiveDialog()` clears a single
  `m_activeDialog` pointer (`WXTacticalUI.cpp:113-117`), overwritten (not stacked)
  on each `ShowModal`. If two dialogs are ever live at once, closing dismisses one
  and then `Show(false)` auto-`EndModal`s the other → duplicate-`EndModal` assert.
- **Why it shipped broken:** the round 1 minimize/close subtasks were verified by
  style-flag / source-contract tests only; the verifiers explicitly documented that
  the live `ShowModal` path and rendered decorations cannot run in the headless
  harness, so nothing exercised the real behavior.
- **Turn buttons greyed with MR remaining:** `canUseEndOfMoveTurn()`
  (`FTacticalGame.cpp:843-859`) gates on `getFlag(endPoint()) & MR_TURN`
  (`:851-855`) — "was the FINAL hex entered via a turn?". But `MR_TURN` is recorded
  on the hex a ship turns INTO (`handleMoveHexSelection`, `:2631-2632`), so a
  turn-then-advance on the last leg flags the current hex and disables the buttons
  even though MR remains. The correct budget rule — the one the route highlights
  already use — is `countFlags(MR_TURN) < getMR()`
  (`computeRemainingMoves`, `:2375-2379`).
- **Turn panel invisible at default size:** the panel X is computed from the
  **unwrapped** width of the prompt strings (`instrW`, `FBattleDisplay.cpp:1152-1156`),
  dominated by the long line `"Press the 'Movement Done' button when all ships have
  been assigned their movement instructions."` (`:1119`). That pushes `lMargin`
  (`:1164`) past the ship-stats column (`shipStatsLeftMargin` = 300), so the fit
  guard (`:1172`) hides the panel every paint (`:1187`). The drawn text itself does
  wrap (`drawWrappedActionPrompt`, `:340-369`); only the placement math uses the
  unwrapped extent. The panel is a free-floating child (not in `rootSizer`) and its
  height is not grown for the turn content.

## Confirmed vs Assumed

- Confirmed: all file:line citations above reflect the current merged code on the
  coordination branch.
- Assumed (to be confirmed during implementation): the exact set of other
  top-level windows that must be disabled/hidden to preserve modality once the GTK
  grab is dropped (at minimum the strategic main frame and the launching
  `SelectCombatGUI` parent, which is already hidden before `ShowModal`); and whether
  the title-bar X needs a GTK-level bypass analogous to Quit's to reliably reach the
  close path when a child dialog is active.

## Governing Rules Constraint

`doc/rules/tactical_operations_manual.md` MUST NOT be edited. The turn-button
enable fix aligns the buttons to the manual's existing per-turn budget (MR = number
of turns), matching the route-highlight rule; if any change appears to require
editing the manual, STOP and raise it with the user.

## Verification Policy (applies to TMFR-01 and TMFR-03 runtime behaviors)

The minimize decoration, the live mid-battle close path, and the buttons'
visibility at the default window size cannot be exercised in the headless harness
(prior attempts to drive `ShowModal` blocked the runner and caused heap
corruption). Therefore:

- Automated tests MUST cover everything testable without a live modal window:
  child-dialog dismissal-stack ordering (via a mock `ITacticalUI`), close-path
  state transitions, the window enable/disable bookkeeping, the corrected
  turn-enable model rule, and the wrapped-width placement computation.
- A **required manual verification pass by the user** is a feature-level
  acceptance gate before this round is considered complete: confirm (a) the
  BattleBoard shows and honors a minimize button, (b) the title-bar X closes the
  battle mid-dialog with no assert, (c) File->Quit never throws, and (d) the Turn
  Left/Right buttons are visible and correctly placed at the default window size.
  This gate is performed outside the automated Implementer→...→Verifier chain; the
  Coordinator should surface it and NOT declare the feature fully accepted until the
  user signs off. Automated subtask completion (artifacts + commits) is still
  required per the normal completion gate.

## Overall Acceptance Criteria

- AC1: The BattleBoard window presents a working minimize button while battle
  modality is preserved (no other top-level window is interactable while the battle
  is active), confirmed by the manual verification pass.
- AC2: The title-bar X reliably closes the battle at any point — including while a
  damage-summary, ICM-selection, or winner/info dialog is open — with no
  `wxDialog::EndModal` assert; and File->Quit likewise never throws, including when
  more than one child dialog has been live. Automated coverage for the
  dismissal-stack/close logic plus the manual verification pass.
- AC3: A ship with remaining MR (e.g. a minelayer with MR=2 that has used one turn)
  can make an end-of-move turn; the Turn buttons are enabled whenever the
  route-highlight rule (`countFlags(MR_TURN) < getMR()`) would allow another turn,
  including in the hex immediately after a turn. Verified behaviorally.
- AC4: The over-long move-phase instruction text is wrapped, the Turn-buttons panel
  is placed from the wrapped line widths (not the unwrapped extent), and the panel
  is visible and correctly positioned (right of the Movement Done button, top of the
  lower panel, left of the ship-info column) at the default window size, with the
  lower panel expanding as needed. Placement logic covered by automated tests plus
  the manual verification pass.
- AC5: A 2-4 line explanatory caption is drawn above the Turn buttons, sized to
  roughly the width of the two buttons side by side, and the panel grows to contain
  it without clipping.
- AC6: The GUI source-contract tests resolve source paths via `GUI_TEST_REPO_ROOT`
  and pass regardless of the working directory from which the runner is launched;
  duplicate `@author` tags in `tests/gui/TacticalGuiLiveTest.h` are collapsed to one
  per distinct author.
- AC7: `doc/rules/tactical_operations_manual.md` is unedited; the tactical and GUI
  suites pass with no new regressions.

## Overall Documentation Impact

- `doc/test-contracts.md`: update the battle-window close/modality contracts and the
  turn-button contracts to reflect the new behavior and the corrected enable rule;
  record the manual-verification dependency for the WM behaviors.
- `AGENTS.md`: update the `FBattleScreen`/`WXTacticalUI` modality and close-vector
  ownership notes (grab removed, other-window disabling, dismissal stack) and the
  turn-panel layout notes.
- `doc/UsersGuide.md`: note the minimize button and the new caption above the Turn
  Left/Right buttons if the tactical window is user-documented.
- `doc/rules/tactical_operations_manual.md`: MUST NOT change.
- Doxygen headers updated on every edited function/class per repository convention.

## Dependency Ordering and Parallelization

The four subtasks are largely file-disjoint:
- TMFR-01 → `FBattleScreen.{h,cpp}`, `WXTacticalUI.{h,cpp}`, `SelectCombatGUI.cpp`
- TMFR-02 → `FTacticalGame.{h,cpp}`
- TMFR-03 → `FBattleDisplay.{h,cpp}`
- TMFR-04 → `tests/gui/*`

Recommended order: **TMFR-01 first** (largest and highest-risk), then TMFR-02,
TMFR-03, TMFR-04. Because the file sets do not overlap, TMFR-02, TMFR-03, and
TMFR-04 are operationally independent of one another and of TMFR-01 and MAY be run
in parallel if the Coordinator wants throughput; sequential execution is equally
safe. Note the manual verification pass (Verification Policy) spans TMFR-01 and
TMFR-03 runtime behavior and is performed once both are merged.

## Output Artifact Path

- This plan artifact: `plans/tactical-misc-fixes-round2-plan.md` (repo-root-relative).
- Per-subtask orchestration artifacts: `artifacts/tactical-misc-fixes-round2/<SUBTASK-ID>/`
  (e.g. `artifacts/tactical-misc-fixes-round2/TMFR-01/`), repo-root-relative.

## Implementer Prompts

Each subtask below provides its launch-ready Implementer Prompt inline in an
`### Implementer Prompt` block, ready for the Coordinator to pass through directly.
Every prompt begins with `Your role is 'implementer'. Your task is as follows:` and
ends with the explicit completion gate
(`Do not report success unless all required artifacts exist and all changes are committed.`).

---

## TMFR-01 — Battle window modality, minimize button, and reliable close

Security review: not required.

### Task
Rework the `FBattleScreen` modality so the BattleBoard behaves as a normal
top-level window with a working minimize button and a working title-bar X, while
still preventing interaction with the strategic layer during the battle, and
eliminate the residual `EndModal` assert. Concretely: drop the GTK modal grab
(`gtk_window_set_modal(TRUE/FALSE)`, `FBattleScreen.cpp:252/264`) so the frame is
not a GTK-modal window (restoring the minimize button and a normal X); preserve
modality by disabling and/or hiding the other top-level windows (the strategic main
frame and the launching parent) for the battle's duration and restoring them on
close; make the title-bar X reliably route to the close path even when a child
dialog is active (generalizing the existing Quit GTK bypass at
`FBattleScreen.cpp:191-217` if needed); and convert child-dialog dismissal to an
innermost-first stack in `WXTacticalUI` so closing dismisses ALL live child dialogs
(fixing the duplicate-`EndModal` assert that occurs when more than one dialog is
live).

### Allowed Files
- `include/tactical/FBattleScreen.h`
- `src/tactical/FBattleScreen.cpp`
- `include/gui/WXTacticalUI.h`
- `src/gui/WXTacticalUI.cpp`
- `src/gui/SelectCombatGUI.cpp`
- `include/gui/SelectCombatGUI.h`

### Acceptance Criteria
- The GTK modal grab is no longer used to enforce battle modality; the frame is a
  normal top-level with `wxMINIMIZE_BOX` effective.
- While the battle is shown, the strategic main frame and the launching parent
  cannot be interacted with (disabled and/or hidden), and are restored to their
  prior state when the battle closes.
- The title-bar X closes the battle at any point, including while a damage-summary,
  ICM-selection, or winner/info dialog is open, with no `wxDialog::EndModal` assert.
- File->Quit closes the battle with no assert, including when more than one child
  dialog has been live during the battle.
- Child-dialog dismissal is a stack: closing the battle dismisses every live child
  `wxDialog` (innermost-first) before the frame hides; the custom
  `FBattleScreen::EndModal` `m_modalActive` guard remains intact.
- Automated tests cover the dismissal-stack ordering (via a mock `ITacticalUI`), the
  close-path state transitions, and the other-window enable/disable bookkeeping.
- The minimize decoration, the live mid-battle X close, and the Quit-no-assert
  behavior are confirmed by the required manual verification pass (see the plan's
  Verification Policy); note in the implementer report that these runtime behaviors
  require that manual pass.

### Documentation Impact
- `AGENTS.md`: update the `FBattleScreen`/`WXTacticalUI` modality and close-vector
  ownership notes (grab removed; other-window disabling; dismissal stack).
- `doc/test-contracts.md`: update the close/modality contracts and record the
  manual-verification dependency.
- `doc/UsersGuide.md`: note the minimize button if the tactical window is
  user-documented.
- Doxygen headers on all edited functions updated.

### Tester Guidance
- Cover the dismissal-stack ordering and close-path transitions via the tactical /
  mock-`ITacticalUI` suites (`tests/tactical/*`, e.g. `WXTacticalUIAdapterTest`,
  `FTacticalGameMockUITest`) and the GUI suite (`tests/gui/TacticalGuiLiveTest.{cpp,h}`)
  where feasible without driving a live `ShowModal`.
- Do NOT rely on source-contract assertions as the sole coverage for any behavior
  that can be exercised at the logic level. Where a behavior genuinely cannot run
  headless (WM decoration, live modal close), state that explicitly and defer it to
  the manual verification pass rather than asserting a PASS from source structure.

### Implementer Prompt
```
Your role is 'implementer'. Your task is as follows:

Rework FBattleScreen modality so the BattleBoard is a normal top-level window with
a working minimize button and a working title-bar X, while still blocking
interaction with the strategic layer during battle, and eliminate the residual
wxDialog::EndModal assert.

Root causes (current merged code): the minimize button is suppressed by
gtk_window_set_modal(GTK_WINDOW(m_widget), TRUE) at src/tactical/FBattleScreen.cpp:252
(a GTK modal top-level has no minimize button regardless of wxMINIMIZE_BOX, which is
already in the style at include/tactical/FBattleScreen.h:66). The title-bar X is
swallowed while a child wxDialog (damage summary / ICM / winner-info, shown via
WXTacticalUI ShowModal) holds the grab, so it never reaches onClose
(FBattleScreen.cpp:1020-1027); Quit works only via the GTK activate bypass at
FBattleScreen.cpp:191-217. The residual Quit assert is because
WXTacticalUI::dismissActiveDialog() tracks a single m_activeDialog pointer
(WXTacticalUI.cpp:113-117), so when two dialogs are live, Show(false) auto-EndModals
the untracked one.

Do the following:
- Drop the GTK modal grab (gtk_window_set_modal TRUE/FALSE at FBattleScreen.cpp:252/264)
  so the frame is not GTK-modal.
- Preserve modality by disabling and/or hiding the other top-level windows (the
  strategic main frame and the launching SelectCombatGUI parent) for the battle's
  duration, restoring their prior state on close.
- Make the title-bar X reliably reach the close path even when a child dialog is
  active (generalize the Quit GTK bypass at FBattleScreen.cpp:191-217 to the X if
  needed).
- Convert WXTacticalUI child-dialog dismissal to an innermost-first STACK so closing
  dismisses ALL live child dialogs before the frame hides; keep the custom
  FBattleScreen::EndModal m_modalActive guard intact.

Allowed files (modify only these):
- include/tactical/FBattleScreen.h
- src/tactical/FBattleScreen.cpp
- include/gui/WXTacticalUI.h
- src/gui/WXTacticalUI.cpp
- src/gui/SelectCombatGUI.cpp
- include/gui/SelectCombatGUI.h

Acceptance criteria (implementation outcomes):
- The GTK modal grab is no longer used; the frame is a normal top-level with
  wxMINIMIZE_BOX effective.
- While the battle is shown, the strategic main frame and the launching parent are
  not interactable, and are restored on close.
- The title-bar X closes the battle at any point, including while a damage-summary,
  ICM-selection, or winner/info dialog is open, with no EndModal assert.
- File->Quit closes with no assert, including when more than one child dialog has
  been live.
- Child-dialog dismissal is a stack (all live dialogs dismissed innermost-first
  before the frame hides).
- Automated tests cover the dismissal-stack ordering (mock ITacticalUI), close-path
  transitions, and the other-window enable/disable bookkeeping.

Verification note: the minimize decoration, the live mid-battle X close, and the
Quit-no-assert behavior cannot be exercised in the headless harness; do NOT assert
a PASS for those from source structure. Cover the logic-level parts with real
behavioral tests and explicitly defer the WM/runtime behaviors to the required user
manual verification pass, stating this in your report.

Constraints:
- Do NOT edit doc/rules/tactical_operations_manual.md.
- Update the Doxygen headers on all edited functions (@author with your model and
  reasoning level, Last Modified to today) per AGENTS.md.
- Do NOT modify test files; behavioral coverage is owned by the Tester stage.

Validation guidance:
- Build from the repo root with `make`.
- Run the tactical suite: `cd tests && make tactical-tests && ./tactical/TacticalTests`.
- Run the GUI suite headless: `cd tests/gui && make && xvfb-run -a ./GuiTests`.

Tester handoff: dismissal-stack/close-path logic in the tactical / mock-ITacticalUI
suites (WXTacticalUIAdapterTest, FTacticalGameMockUITest) and
tests/gui/TacticalGuiLiveTest.{cpp,h}; WM/runtime behaviors deferred to the user
manual verification pass per the plan.

Artifacts: write your report to artifacts/tactical-misc-fixes-round2/TMFR-01/
(implementer_report.md and implementer_result.json), repo-root-relative.

Continue past preflight and complete the work when no blocking issues are present.

Do not report success unless all required artifacts exist and all changes are committed.
```

---

## TMFR-02 — Fix Turn Left/Right enable rule (greying with MR remaining)

Security review: not required.

### Task
Fix the end-of-move turn-button enable logic so a ship with remaining Maneuver
Rating can still turn. Replace the per-hex final-hex flag test in
`canUseEndOfMoveTurn()` (`FTacticalGame.cpp:851-855`, currently
`getFlag(endPoint()) & MR_TURN`) with the whole-path budget rule the route
highlights already use — `countFlags(MR_TURN) < getMR()`
(`computeRemainingMoves`, `:2375-2379`) — while keeping the existing
`pendingEndOfMoveFacing != -1` guard (`:857`) and the `nMoved >= speed - ADF`
minimum-move guard (`:847-848`). The bug: `MR_TURN` is attributed to the hex a ship
turns INTO (`handleMoveHexSelection`, `:2631-2632`), so a turn-then-advance on the
last leg flags the current hex and wrongly disables the buttons even though MR
remains (a minelayer with MR=2 that used one turn should still be able to turn).

### Allowed Files
- `include/tactical/FTacticalGame.h`
- `src/tactical/FTacticalGame.cpp`

### Acceptance Criteria
- The Turn buttons are enabled whenever another turn is within the MR budget
  (`countFlags(MR_TURN) < getMR()`), including in the hex immediately after a turn,
  matching the route-highlight availability.
- A ship (e.g. minelayer, MR=2) that has used one turn can still make an end-of-move
  turn; it is only blocked once its turns used reach its MR (or a turn is already
  pending, or its minimum move is unmet).
- The reverse/undo behavior for an already-pending turn is unchanged; the
  one-hexside clamp and commit-at-Movement-Done behavior are unchanged.
- No regression to normal in-route turning or to Movement Done gating.

### Documentation Impact
- `doc/test-contracts.md`: update the turn-button enable contract to the budget rule.
- Doxygen headers on edited functions updated.

### Tester Guidance
- Behavioral coverage in the tactical suite (e.g.
  `tests/tactical/FTacticalGameMechanicsTest.{cpp,h}`,
  `FTacticalMoveRouteSelectionTest`): construct a ship with MR=2, record one turn,
  advance one hex, and assert `canApplyEndOfMoveTurnLeft/Right()` is true in the hex
  after the turn; assert it becomes false once turns used == MR. Author a test that
  fails against the unfixed rule and passes after. Tester-owned.

### Implementer Prompt
```
Your role is 'implementer'. Your task is as follows:

Fix the end-of-move Turn Left/Right button enable logic so a ship with remaining
Maneuver Rating (MR) can still turn. In canUseEndOfMoveTurn()
(src/tactical/FTacticalGame.cpp:843-859), the per-hex test at :851-855
(getFlag(endPoint()) & MR_TURN) is wrong: MR_TURN is recorded on the hex a ship
turns INTO (handleMoveHexSelection, :2631-2632), so a turn-then-advance on the last
leg flags the current hex and disables the buttons even though MR remains (repro:
minelayer MR=2, used one turn, buttons greyed in the hex right after the turn).
Replace that per-hex flag test with the whole-path budget rule the route highlights
already use: countFlags(MR_TURN) < getMR() (see computeRemainingMoves, :2375-2379).
Keep the existing pendingEndOfMoveFacing != -1 guard (:857) and the nMoved >=
speed - ADF minimum-move guard (:847-848). Do not change the reverse/undo path, the
one-hexside clamp, or the commit-at-Movement-Done behavior.

Allowed files (modify only these):
- include/tactical/FTacticalGame.h
- src/tactical/FTacticalGame.cpp

Acceptance criteria (implementation outcomes):
- Turn buttons are enabled whenever countFlags(MR_TURN) < getMR() would allow
  another turn, including in the hex immediately after a turn.
- A minelayer with MR=2 that used one turn can still make an end-of-move turn; it is
  blocked only when turns used reach MR (or a turn is already pending, or the
  minimum move is unmet).
- Reverse/undo of a pending turn, the one-hexside clamp, and commit-at-Movement-Done
  are unchanged.
- No regression to normal in-route turning or Movement Done gating.

Constraints:
- Do NOT edit doc/rules/tactical_operations_manual.md (the fix aligns the buttons to
  the manual's existing MR-as-turn-budget rule).
- Update the Doxygen headers on edited functions (@author with your model and
  reasoning level, Last Modified to today) per AGENTS.md.
- Do NOT modify test files; behavioral coverage is owned by the Tester stage.

Validation guidance:
- Build from the repo root with `make`.
- Run the tactical suite: `cd tests && make tactical-tests && ./tactical/TacticalTests`.

Tester handoff: add a turn-budget enable test (MR=2, one turn used, assert enabled
in the next hex; assert disabled when turns used == MR) in
tests/tactical/FTacticalGameMechanicsTest.{cpp,h} or FTacticalMoveRouteSelectionTest.

Artifacts: write your report to artifacts/tactical-misc-fixes-round2/TMFR-02/
(implementer_report.md and implementer_result.json), repo-root-relative.

Continue past preflight and complete the work when no blocking issues are present.

Do not report success unless all required artifacts exist and all changes are committed.
```

---

## TMFR-03 — Turn-buttons panel layout: wrap instruction, fix visibility, add caption

Security review: not required.

### Task
Fix the Turn-buttons panel so it is visible and correctly placed at the default
window size, and add an explanatory caption above the buttons. Two coupled fixes in
`drawMoveShip()` (`FBattleDisplay.cpp`): (1) wrap the over-long move-phase
instruction line (`"Press the 'Movement Done' button when all ships have been
assigned their movement instructions."`, `:1119`) into two lines, and compute the
panel placement (`instrW`, `:1152-1156`) from the WRAPPED line widths rather than
the full unwrapped extents so `lMargin` (`:1164`) no longer overshoots the ship-info
column and the fit guard (`:1172`) stops hiding the panel; ensure the lower panel
expands (raise `requestedDisplayHeight` and re-apply, cf. `applyRequestedDisplayHeight`,
`:668-685`) if needed so the panel fits. (2) Draw a 2-4 line caption above the Turn
buttons, sized to roughly the width of the two buttons side by side, and grow the
panel height to contain the caption plus the buttons without clipping.

Caption text (use this wording, corrected spelling): "If at the end of a ship's
movement you want to make a facing change (and have remaining MR), use these buttons
to make a single final turn." Split it across 2-4 lines so the block is about as
wide as the two buttons side by side.

### Allowed Files
- `include/tactical/FBattleDisplay.h`
- `src/tactical/FBattleDisplay.cpp`

### Acceptance Criteria
- The move-phase instruction text is wrapped, and the Turn-buttons panel X is
  computed from the wrapped line widths (not the unwrapped extent).
- At the default window size, the Turn-buttons panel is visible and positioned to
  the right of the Movement Done button, at the top of the lower panel, and left of
  the ship-info column; the lower panel expands as needed so the panel is not hidden
  or clipped.
- A 2-4 line caption with the specified wording is drawn directly above the Turn
  buttons, sized to roughly the width of the two buttons side by side.
- The panel height grows to contain the caption plus the button row without
  clipping; no overlap with the instruction column or the ship-info column.
- No regression to the seeker/mine/other move-phase draw paths.

### Documentation Impact
- `doc/test-contracts.md`: update/add the turn-panel layout contract (wrapped-width
  placement, caption presence, panel expansion).
- `AGENTS.md`: update the turn-panel layout notes in the FBattleDisplay section.
- `doc/UsersGuide.md`: note the caption above the Turn Left/Right buttons if the
  tactical UI is user-documented.
- Doxygen headers on edited functions updated.

### Tester Guidance
- Cover the placement math and caption/panel-expansion via the GUI suite
  (`tests/gui/TacticalGuiLiveTest.{cpp,h}`), asserting the panel's computed X is left
  of the ship-info column and the panel is shown at the default size, and that the
  panel height accounts for the caption. The visible-at-default-size runtime check is
  also part of the user manual verification pass. Tester-owned.

### Implementer Prompt
```
Your role is 'implementer'. Your task is as follows:

Fix the Turn-buttons panel layout in drawMoveShip() (src/tactical/FBattleDisplay.cpp)
so the buttons are visible and correctly placed at the default window size, and add
an explanatory caption above them.

Root cause: the panel X is computed from the UNWRAPPED width of the prompt strings
(instrW, :1152-1156), dominated by the long line "Press the 'Movement Done' button
when all ships have been assigned their movement instructions." (:1119). That pushes
lMargin (:1164) past the ship-stats column (shipStatsLeftMargin = 300), so the fit
guard (:1172) hides the panel every paint (:1187). The drawn text already wraps
(drawWrappedActionPrompt, :340-369); only the placement math uses the unwrapped
extent. The panel is a free-floating child not in the sizer, and its height is not
grown for the turn content.

Do the following:
- Wrap the long move-phase instruction line into two lines, and compute the
  Turn-panel placement (instrW at :1152-1156) from the WRAPPED line widths so
  lMargin no longer overshoots the ship-info column and the panel is shown.
- Ensure the lower panel expands when needed (raise
  m_lowerPanelLayoutState.requestedDisplayHeight and re-apply via
  applyRequestedDisplayHeight, :668-685) so the panel fits at the default size.
- Draw a 2-4 line caption directly above the Turn buttons, sized to roughly the
  width of the two buttons side by side, and grow the panel height to contain the
  caption plus the button row without clipping.

Caption text (use exactly this wording): "If at the end of a ship's movement you
want to make a facing change (and have remaining MR), use these buttons to make a
single final turn." Split across 2-4 lines so the block is about as wide as the two
buttons side by side.

Allowed files (modify only these):
- include/tactical/FBattleDisplay.h
- src/tactical/FBattleDisplay.cpp

Acceptance criteria (implementation outcomes):
- The instruction text is wrapped and the panel X is computed from wrapped line
  widths (not the unwrapped extent).
- At the default window size the panel is visible and placed right of Movement Done,
  at the top of the lower panel, left of the ship-info column; the lower panel
  expands as needed.
- A 2-4 line caption with the specified wording is drawn above the buttons, roughly
  the width of the two buttons side by side.
- The panel height grows to contain the caption plus the button row without
  clipping; no overlap with the instruction or ship-info columns.
- No regression to the seeker/mine/other move-phase draw paths.

Verification note: the visible-at-default-size runtime behavior is also confirmed by
the required user manual verification pass; cover the placement math and panel
expansion with automated tests where feasible.

Constraints:
- Do NOT edit doc/rules/tactical_operations_manual.md.
- Update the Doxygen headers on edited functions (@author with your model and
  reasoning level, Last Modified to today) per AGENTS.md.
- Do NOT modify test files; behavioral coverage is owned by the Tester stage.

Validation guidance:
- Build from the repo root with `make`.
- Run the GUI suite headless: `cd tests/gui && make && xvfb-run -a ./GuiTests`.
- Run the tactical suite: `cd tests && make tactical-tests && ./tactical/TacticalTests`.

Tester handoff: cover placement (panel X left of ship-info column), panel shown at
default size, caption presence, and panel-height expansion in
tests/gui/TacticalGuiLiveTest.{cpp,h}.

Artifacts: write your report to artifacts/tactical-misc-fixes-round2/TMFR-03/
(implementer_report.md and implementer_result.json), repo-root-relative.

Continue past preflight and complete the work when no blocking issues are present.

Do not report success unless all required artifacts exist and all changes are committed.
```

---

## TMFR-04 — GUI test-suite robustness (cwd-independent paths + @author dedup)

Security review: not required.

### Task
Make the GUI source-contract tests cwd-independent and de-duplicate accumulated
`@author` tags. Resolve source-file paths through the existing `GUI_TEST_REPO_ROOT`
build define instead of hard-coded `../../src/...` relative paths, so the GUI suite
passes regardless of the directory the runner is launched from (this eliminates the
~8 false failures seen when `GuiTests` runs from the repo root). Also collapse
repeated identical `@author` model tags in `tests/gui/TacticalGuiLiveTest.h` to a
single entry per distinct author. This is a justified test-infrastructure fix (it
removes a false-failure mode); it is scoped to the GUI test files.

### Allowed Files
- `tests/gui/TacticalGuiLiveTest.cpp`
- `tests/gui/TacticalGuiLiveTest.h`
- Other `tests/gui/*.cpp` / `tests/gui/*.h` files that hard-code `../../src/...`
  source paths
- `tests/gui/Makefile` (only if needed to ensure `GUI_TEST_REPO_ROOT` is defined for
  the affected tests)

### Acceptance Criteria
- GUI source-contract tests resolve source paths via `GUI_TEST_REPO_ROOT` and pass
  when the runner is launched both from `tests/gui` and from the repository root.
- No remaining hard-coded `../../src/...` path in the affected GUI tests.
- Duplicate `@author` tags in `tests/gui/TacticalGuiLiveTest.h` are collapsed to one
  entry per distinct author (no author removed).
- The full GUI suite passes; no behavioral test coverage is lost.

### Documentation Impact
- `AGENTS.md` / `doc/test-contracts.md`: note the `GUI_TEST_REPO_ROOT`-based path
  resolution for GUI source-contract tests if test conventions are documented there.
- No production-code documentation impact.

### Tester Guidance
- This subtask is itself test-infrastructure; the implementer edits the GUI test
  files directly. The downstream Tester stage should confirm the suite passes from
  both working directories and that no behavioral coverage regressed.

### Implementer Prompt
```
Your role is 'implementer'. Your task is as follows:

Make the GUI source-contract tests cwd-independent and de-duplicate accumulated
@author tags. Currently some tests/gui source-contract tests read source files via
hard-coded ../../src/... relative paths, so they falsely fail when GuiTests is
launched from the repository root (~8 false failures). Resolve those source paths
through the existing GUI_TEST_REPO_ROOT build define instead, so the suite passes
regardless of launch directory. Also collapse repeated identical @author model tags
in tests/gui/TacticalGuiLiveTest.h to a single entry per distinct author (do not
remove any distinct author).

This is a justified test-infrastructure change (it removes a false-failure mode),
scoped to the GUI test files.

Allowed files (modify only these):
- tests/gui/TacticalGuiLiveTest.cpp
- tests/gui/TacticalGuiLiveTest.h
- other tests/gui/*.cpp / tests/gui/*.h files that hard-code ../../src/... paths
- tests/gui/Makefile (only if needed to define GUI_TEST_REPO_ROOT for the affected
  tests)

Acceptance criteria (implementation outcomes):
- GUI source-contract tests resolve source paths via GUI_TEST_REPO_ROOT and pass
  when launched both from tests/gui and from the repository root.
- No remaining hard-coded ../../src/... path in the affected GUI tests.
- Duplicate @author tags in tests/gui/TacticalGuiLiveTest.h collapsed to one per
  distinct author (none removed).
- The full GUI suite passes; no behavioral coverage lost.

Constraints:
- Do NOT edit doc/rules/tactical_operations_manual.md.
- Preserve all existing behavioral assertions; this is a path-resolution and
  comment-hygiene change, not a coverage change.

Validation guidance:
- Build the GUI suite: `cd tests/gui && make`.
- Run it from tests/gui: `xvfb-run -a ./GuiTests`.
- Run it from the repo root to confirm cwd-independence:
  `xvfb-run -a ./tests/gui/GuiTests` (or the equivalent) and confirm no false
  failures.

Artifacts: write your report to artifacts/tactical-misc-fixes-round2/TMFR-04/
(implementer_report.md and implementer_result.json), repo-root-relative.

Continue past preflight and complete the work when no blocking issues are present.

Do not report success unless all required artifacts exist and all changes are committed.
```
