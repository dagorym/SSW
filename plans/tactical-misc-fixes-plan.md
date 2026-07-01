# Tactical Misc Fixes — Implementation Plan

Feature slug: `tactical-misc-fixes`
Plan artifact: `plans/tactical-misc-fixes-plan.md`
Coordination base branch: dedicated per-plan branch (never `master`/`main`).
Specialist Security stage: **not required** for any subtask (no permissions, secrets,
untrusted input, external integrations, or destructive operations are involved).

## Feature Summary

A cleanup round addressing miscellaneous tactical-combat issues found during play
testing, plus two optional follow-ups carried forward from the round 6 reviewer
report:

1. (Round 6 follow-up) Single-source the three seeker-activation instruction
   strings to remove the residual measure-vs-draw drift, and harden the related
   test so the activation column's button-clearing rule has a discriminating
   assertion.
2. Restore the minimize button on the BattleBoard window.
3. Make the title-bar X reliably close the battle mid-game instead of tripping a
   `wxDialog::EndModal` assert.
4. Fix the "Movement Done" gating bug so a ship that cannot decelerate to 0 this
   turn cannot be skipped.
5. Allow a ship that finishes its move with unused Maneuver Rating (MR) to make a
   single in-place facing change, exposed through dedicated Turn Left / Turn Right
   buttons.
6. Skip the post-fire damage dialog only when no weapon was actually fired.

## Resolved Design Decisions

- **D1 (Round 6 follow-ups):** Include BOTH — extract the seeker-activation
  instruction strings into shared constants (mirroring the earlier SMRVI seeker fix), and strengthen
  the activation-column test to assert it also clears the Done-button right edge.
- **D2 (Minimize button):** Restore minimize by dropping the
  `wxTOPLEVEL_EX_DIALOG` extra-style hint ONLY. Preserve the existing custom modal
  event loop and GTK modal grab so battle modality is unchanged. The minimize box
  appearing must be confirmed behaviorally; if the GTK modal flag alone still
  suppresses it on the test WM, that is a finding to surface, not a silent change.
- **D3 (Title-bar X mid-game):** The X reliably closes the WHOLE battle. Any
  nested child modal (damage summary / ICM selection / winner-or-info message)
  must be torn down cleanly first, then the battle aborts and control returns to
  the strategic layer. No `EndModal`-twice / "ShowModal wasn't called" assert.
- **D4 (Facing change with leftover MR):** Manual-grounded interpretation. Allow a
  single one-hexside facing change in the ship's final occupied hex when it still
  has unused MR and has not already turned in that hex; additionally extend the
  existing free-rotation so a ship that decelerates to speed 0 THIS turn may
  rotate. Do not implement a broader house rule. `doc/rules/tactical_operations_manual.md`
  is NOT edited.
- **D4b (Facing-change UI):** Expose the end-of-move facing change through
  dedicated **Turn Left / Turn Right** action buttons (real `wxButton` controls,
  alongside Movement Done), to avoid the move-vs-face ambiguity of overloaded
  hex-clicks. The existing speed-zero-all-turn free-rotation hex-click interaction
  stays as-is.
- **D4d (Turn-buttons placement):** The Turn Left / Turn Right buttons are real
  `wxButton` controls placed in their own dedicated child panel, but they must NOT
  sit at the far-left default slot used by the existing single-column `actionSizer`.
  Position the Turn-buttons panel like the existing "Activated seekers" widget
  (which is the reference for WHERE, not for rendering style): at the TOP of the
  lower panel (`y = getActionPromptLineY(0)`), in the MIDDLE column to the RIGHT of
  the Movement Done button (`x = leftOffset + max(widestInstructionWidth,
  moveDoneButtonRightExtent) + 2*BORDER`, the same margin math
  `drawSeekerActivation()` uses), and kept to the LEFT of the ship-information
  column (`shipStatsLeftMargin`) so the columns never overlap. Show/hide the panel
  per move phase like the other action buttons.
- **D4c (Facing-change is an uncommitted, reversible preview):** The end-of-move
  facing change is NOT locked when made; it remains mutable while the ship is the
  active mover and is only finalized when Movement Done is clicked. Re-selecting a
  ship before Movement Done re-presents the Turn controls and the ship's current
  pending facing. "Undo" and "turn the other way" are both expressed through the
  same Turn Left / Turn Right buttons, clamped so the net change never exceeds one
  hexside (valid final facings: one-left / original / one-right): each button is
  enabled only when it leads to a valid final facing, so pressing the opposite
  direction returns the ship toward/back to its original heading. This behavior
  must be documented in `doc/UsersGuide.md`.
- **D5 (Movement Done gating):** Treat as a bug. Reported repro: a defending UPF
  Minelayer with initial speed 10 and ADF 1 (minimum move = 9) had Movement Done
  enabled at phase entry and could be clicked without moving, which then set its
  speed to 0; the attacking Sathar Heavy Cruiser was correctly forced to move.
  Prime suspect: `m_moveComplete` is only recomputed on a hex-click
  (`FTacticalGame.cpp:3427-3434`) and is not (re)evaluated at the START of each
  player's move phase, so the defending player inherits a stale
  `m_moveComplete=true` (and/or the defender's pre-move `getSpeed()` is not the
  expected value when status is computed). Root cause to be confirmed by the
  implementer/tester; the fix must make gating correct at phase entry for BOTH
  players before any interaction.
- **D6 (Skip damage dialog):** Skip the dialog ONLY when zero weapons were fired.
  An empty damage summary is NOT a valid proxy because a weapon can fire and miss
  (or be fully intercepted) yet produce no damage rows. Surface a weapons-fired
  count from `fireAllWeapons()` (currently discarded at `clearTacticalReport()`)
  to the GUI decision point and apply the guard to BOTH the defensive and
  offensive fire-phase completion paths.

## Confirmed Repository Facts (from investigation)

- `FBattleScreen` is a `wxFrame` (`include/tactical/FBattleScreen.h:45`) with a
  custom, `m_modalActive`-guarded `EndModal` (`src/tactical/FBattleScreen.cpp:271`).
  Its style already includes `wxMINIMIZE_BOX` (`FBattleScreen.h:66`), but
  `SetExtraStyle(... | wxTOPLEVEL_EX_DIALOG)` (`FBattleScreen.cpp:119`) plus
  `gtk_window_set_modal(...TRUE)` (`:253`,`:265`) suppress the minimize box on GTK.
- Title-bar X path: `onClose` (`FBattleScreen.cpp:994`) → `closeBattleScreen`
  (`:974`) → custom `EndModal` (`:271`) exits the outer loop and `Show(false)`s the
  frame while a nested real `wxDialog` modal loop is still live (damage summary /
  ICM / message), producing the duplicate `wxDialog::EndModal`. Nested dialogs are
  shown via `WXTacticalUI` (`src/gui/WXTacticalUI.cpp:53/69/84`). Mechanism is
  documented in `artifacts/fbattlescreen-menu-quit-fix.md`.
- "Movement Done" button: created `FBattleDisplay.cpp:116`, enabled each draw from
  `isMoveComplete()` (`:1099`), handler `onMoveDone()` (`:1214-1222`) →
  `completeMovePhase()`. Gating computed in `checkMoveStatus()`
  (`FTacticalGame.cpp:2547-2565`) using `minMove = getSpeed() - getADF()`;
  recomputed only after hex clicks (`:3427-3434`). `m_moveComplete` initial value is
  `true` (`:284`); `resetMovementState()` (`:782-790`) sets it false at phase start.
- Turning is currently coupled to entering a hex
  (`handleMoveHexSelection`, `FTacticalGame.cpp:2523-2526`); the only decoupled
  rotation is speed-zero free-rotation (`canUseStoppedShipFreeRotation`, `:199-206`;
  handling `:2419-2432`). Final heading/speed written back in
  `finalizeMovementState()` (`:805-806`). MR is a cap (`getMR()`,
  `include/ships/FVehicle.h:45`), consumed by counting `MR_TURN` path flags.
- Speed/ADF accessors: `FVehicle::getSpeed()`/`setSpeed()`
  (`include/ships/FVehicle.h:69-71`), `getADF()` (`:47`).
- Damage dialog shown unconditionally from the fire-done handlers
  (`FBattleDisplay.cpp:1279` defensive, `:1293` offensive) →
  `FBattleScreen::showTacticalDamageSummaryDialog` (`FBattleScreen.cpp:515`) →
  `WXTacticalUI::showDamageSummary` (`WXTacticalUI.cpp:62`). The per-weapon
  "did it fire" signal is `result.fired()` in `fireAllWeapons()`
  (`FTacticalGame.cpp:620`); one `attacks` entry is appended per fired weapon
  (`:623`); the count is discarded at `clearTacticalReport()` (`:627`) and is NOT
  carried on `FTacticalCombatReportSummary`
  (`include/tactical/FTacticalCombatReport.h:261-269`).
- Lower-panel layout: `FBattleScreen` stacks the hex board over the lower display
  via a 2-row `wxFlexGridSizer` (`FBattleScreen.cpp:128-143`). The lower display
  (`FBattleDisplay`, `m_display`) is a single custom-painted `wxPanel`: the only
  real controls are the phase action buttons in one horizontal `actionSizer`
  (`FBattleDisplay.cpp:130-140`), of which exactly one is shown per phase; the
  instruction/prompt text, ship-info stats, and the "Activated seekers" widget are
  all custom-painted by `draw()` (`:664`) using manual x/y math anchored at
  `getActionPromptLineY(0)` (`:249-251`). The "Activated seekers" widget
  (`drawSeekerActivation()`, `:1695-1781`) paints clickable `wxRect`s into
  `m_seekerActivationRegions` (`FBattleDisplay.h:130-132`) at a middle-column
  `lMargin = leftOffset + max(instrW, doneButtonRightExtent) + 2*BORDER`
  (`:1717-1730`); `leftOffset` is at `:57`, `BORDER` at `:55`; the ship-info column
  begins at `shipStatsLeftMargin` (`:94`, recomputed `:1116-1135`).

Assumptions are labeled inline above (notably the D5 root-cause hypothesis and the
exact teardown sequence for D3, both to be confirmed during implementation).

## Governing Rules Constraint

`doc/rules/tactical_operations_manual.md` MUST NOT be edited. The D4 behavior is
the manual-grounded "one turn per hex applied to the final occupied hex" reading
plus extension of the speed-zero free-rotation; if implementation reveals this cannot
be done without contradicting the manual, STOP and raise it with the user rather
than editing the manual.

## Overall Acceptance Criteria

- AC1: The three seeker-activation instruction strings exist as single shared
  constants referenced by both the measurement and the drawing sites in
  `drawSeekerActivation()`; no duplicated inline literal remains in production code.
- AC2: The BattleBoard window shows a working minimize button while preserving the
  existing battle modality (the strategic window underneath is not interactable
  during the battle).
- AC3: Clicking the title-bar X at any point during a battle — including while a
  damage-summary, ICM-selection, or winner/info dialog is open — does not raise the
  `wxDialog::EndModal` assert; the battle closes cleanly and returns to the
  strategic layer.
- AC4: A ship that cannot decelerate to 0 this turn (current speed > ADF) keeps
  "Movement Done" disabled at move-phase entry, before any hex-click, for BOTH the
  attacking and defending players, and the button only becomes enabled once every
  such ship has moved at least `speed - ADF` hexes.
- AC5: A ship that has met its minimum required move and still has unused MR can
  make a single one-hexside in-place facing change via a dedicated Turn Left or
  Turn Right button; the change consumes the allowed turn, respects the manual's
  one-turn-per-hex limit, and a ship that decelerates to speed 0 this turn can
  rotate.
- AC6: After a fire phase in which no weapon was fired, the damage dialog is not
  shown; after a fire phase in which at least one weapon fired (even if all shots
  missed or were fully intercepted), the dialog is still shown. Applies to both the
  defensive and offensive fire phases.
- AC7: `doc/rules/tactical_operations_manual.md` is unedited; the full tactical
  and GUI test suites pass with no new regressions.

## Overall Documentation Impact

- `doc/test-contracts.md`: add/adjust the contracts for the seeker-activation
  constants (TMF-01), the move-done gating (TMF-04), the end-of-move facing
  buttons (TMF-05), and the damage-dialog-skip (TMF-06).
- `AGENTS.md`: update the tactical/GUI test enumerations and the FBattleDisplay
  action-button / FBattleScreen ownership notes where new buttons, the close path,
  or the fired-count surface change observable behavior.
- `doc/UsersGuide.md`: note the new Turn Left / Turn Right buttons and the restored
  minimize button if user-facing UI documentation covers the tactical screen.
- `doc/rules/tactical_operations_manual.md`: MUST NOT change.
- Doxygen headers: update `@author`/`Last Modified` and descriptions on every
  edited function/class per repository convention.

## Dependency Ordering and Parallelization

Recommended execution is **sequential** in the order below. The work concentrates
in three hot files (`src/tactical/FBattleScreen.cpp`,
`src/tactical/FTacticalGame.cpp`, `src/tactical/FBattleDisplay.cpp`) that multiple
subtasks touch, so serial execution avoids merge conflicts and shared-state
ambiguity.

1. TMF-01 — seeker-activation constants (FBattleDisplay, disjoint region)
2. TMF-02 — minimize button (FBattleScreen constructor)
3. TMF-03 — reliable X close (FBattleScreen close path) — depends on TMF-02 (same file)
4. TMF-04 — movement-done gating bug fix (FTacticalGame)
5. TMF-05 — end-of-move facing buttons (FTacticalGame + FBattleDisplay + FBattleScreen) — depends on TMF-04 (shares move-completion gating and the action-button area)
6. TMF-06 — skip damage dialog (FTacticalGame + FBattleDisplay + FTacticalCombatReport) — sequenced last to avoid FBattleDisplay/FTacticalGame conflicts with TMF-05

Conservative parallelization note: only TMF-01 and TMF-02 are cleanly independent
(disjoint files) and could run concurrently if the Coordinator wants throughput;
all other pairs share a hot file and must stay serial.

## Output Artifact Path

- This plan artifact: `plans/tactical-misc-fixes-plan.md` (repository-root-relative).
- Per-subtask orchestration artifacts: `artifacts/tactical-misc-fixes/<SUBTASK-ID>/`
  (e.g. `artifacts/tactical-misc-fixes/TMF-01/`), repository-root-relative.

## Implementer Prompts

Each subtask below provides its launch-ready Implementer Prompt inline in an
`### Implementer Prompt` block, ready for the Coordinator to pass through directly.
Every prompt begins with `Your role is 'implementer'. Your task is as follows:` and
ends with the explicit completion gate
(`Do not report success unless all required artifacts exist and all changes are committed.`).

---

## TMF-01 — Single-source seeker-activation instruction strings + test hardening

Security review: not required.

### Task
Extract the three `PH_SEEKER_ACTIVATION` instruction strings —
`"Seeker activation phase."`, `"Click a seeker stack on the board to activate one
seeker."`, and `"Click a row below to deactivate an activated seeker."` — into
single shared constants in `FBattleDisplay`, and reference those constants from
BOTH the `GetTextExtent` measurement site and the `DrawText` site in
`drawSeekerActivation()` (`src/tactical/FBattleDisplay.cpp:1714-1734`), eliminating
the residual measure-vs-draw drift in the same way the earlier SMRVI seeker fix did for the
attack-phase string.

### Allowed Files
- `include/tactical/FBattleDisplay.h`
- `src/tactical/FBattleDisplay.cpp`

### Acceptance Criteria
- Each of the three instruction strings is defined exactly once as a shared
  constant and used by both the measurement and the drawing code paths in
  `drawSeekerActivation()`.
- No duplicated inline copy of any of the three literals remains in production code
  (a repo-wide search finds each literal only at its single constant definition).
- The seeker-activation panel renders identically to before (wording, layout, and
  dynamic margin behavior preserved).

### Documentation Impact
- `doc/test-contracts.md`: extend the seeker-activation contract entry to record
  the new shared constants and the literal-once / used-in-both-sites invariant.
- Doxygen header on `drawSeekerActivation()` updated (`@author`, `Last Modified`).

### Tester Guidance
- Behavioral coverage lives in the GUI suite
  (`tests/gui/TacticalGuiLiveTest.{cpp,h}`); supplemental source-contract checks
  live in `tests/tactical/FTacticalBattleDisplayFireFlowTest.{cpp,h}`.
- As part of this subtask's Tester stage, also implement round 6 follow-up (b):
  strengthen `testSeekerActivationRowTextShowsPositionAndMarginIsDynamic` so its
  AC-b asserts the activation column left edge clears the Seeker-Activation Done
  button's right edge (e.g. `region.GetX() >= leftOffset + doneButtonRightExtent +
  2*BORDER`), not only `region.GetX() > 310`.
- These are tester-owned files; they are intentionally excluded from the Implementer
  allowed-file list.

### Implementer Prompt
```
Your role is 'implementer'. Your task is as follows:

Single-source the three PH_SEEKER_ACTIVATION instruction strings in the tactical
battle display. Extract "Seeker activation phase.", "Click a seeker stack on the
board to activate one seeker.", and "Click a row below to deactivate an activated
seeker." into single shared constants in FBattleDisplay and reference each from
BOTH the GetTextExtent measurement and the DrawText call in drawSeekerActivation()
(src/tactical/FBattleDisplay.cpp:1714-1734), mirroring the the earlier SMRVI seeker fix fix for the
attack-phase string.

Allowed files (modify only these):
- include/tactical/FBattleDisplay.h
- src/tactical/FBattleDisplay.cpp

Acceptance criteria (implementation outcomes):
- Each of the three instruction strings is defined exactly once as a shared
  constant and used by both the measurement and drawing paths in
  drawSeekerActivation().
- No duplicated inline copy of any of the three literals remains in production
  code.
- The seeker-activation panel renders identically (wording, layout, dynamic
  margin unchanged).

Constraints:
- Do NOT edit doc/rules/tactical_operations_manual.md.
- Update the Doxygen header(s) on edited functions (@author with your model and
  reasoning level, Last Modified to today) per AGENTS.md.
- Do NOT modify test files; behavioral/contract tests and the round 6 test
  hardening are owned by the downstream Tester stage.

Validation guidance:
- Build from the repo root with `make`.
- Build and run the tactical suite: `cd tests && make tactical-tests &&
  ./tactical/TacticalTests`.
- Run the GUI suite headless: `cd tests/gui && make && xvfb-run -a ./GuiTests`.

Tester handoff: behavioral coverage is in tests/gui/TacticalGuiLiveTest.{cpp,h};
supplemental contracts in tests/tactical/FTacticalBattleDisplayFireFlowTest.{cpp,h}.

Artifacts: write your report to artifacts/tactical-misc-fixes/TMF-01/
(implementer_report.md and implementer_result.json), repository-root-relative.

Continue past preflight and complete the work when no blocking issues are present.

Do not report success unless all required artifacts exist and all changes are committed.
```

---

## TMF-02 — Restore BattleBoard minimize button

Security review: not required.

### Task
Restore a working minimize button on the `FBattleScreen` (BattleBoard) window by
removing the `wxTOPLEVEL_EX_DIALOG` extra-style hint
(`src/tactical/FBattleScreen.cpp:119`) so the GTK window manager shows standard
frame controls, WITHOUT removing the existing custom modal event loop or the GTK
modal grab (`:253`,`:265`). Battle modality must be preserved.

### Allowed Files
- `include/tactical/FBattleScreen.h`
- `src/tactical/FBattleScreen.cpp`

### Acceptance Criteria
- The BattleBoard window presents a functional minimize button.
- The battle remains modal: while the BattleBoard is shown, the strategic window
  underneath cannot be interacted with.
- No regression to the existing centering / first-show sizing of the BattleBoard.
- If removing the dialog hint alone does not make the minimize box appear on the
  test window manager, document the observed behavior and the minimal additional
  change required rather than weakening the modal grab silently.

### Documentation Impact
- `AGENTS.md`: update the FBattleScreen window-style note if the documented
  extra-style behavior changes.
- `doc/UsersGuide.md`: note the minimize button if the tactical window is
  documented for users.
- Doxygen header on the `FBattleScreen` constructor updated.

### Tester Guidance
- BattleBoard window behavior is covered by the GUI suite
  (`tests/gui/TacticalGuiLiveTest.{cpp,h}`) using the `WXGuiTestHarness` helpers;
  assert minimize-capability/style and that modality is preserved. Tester-owned.

### Implementer Prompt
```
Your role is 'implementer'. Your task is as follows:

Restore a working minimize button on the FBattleScreen (BattleBoard) window. The
minimize box is already requested in the style but is suppressed on GTK by the
dialog type-hint. Remove the wxTOPLEVEL_EX_DIALOG extra-style applied at
src/tactical/FBattleScreen.cpp:119 so the window manager shows standard frame
controls. Do NOT remove the custom modal event loop or the gtk_window_set_modal
grab (src/tactical/FBattleScreen.cpp:253,265) — battle modality must be preserved.

Allowed files (modify only these):
- include/tactical/FBattleScreen.h
- src/tactical/FBattleScreen.cpp

Acceptance criteria (implementation outcomes):
- The BattleBoard window presents a functional minimize button.
- The battle remains modal: the strategic window underneath is not interactable
  while the BattleBoard is shown.
- Existing centering/first-show sizing of the BattleBoard is unchanged.
- If dropping the dialog hint alone does not surface the minimize box on the test
  WM, document the observed behavior and the minimal additional change needed
  rather than weakening the modal grab silently.

Constraints:
- Do NOT edit doc/rules/tactical_operations_manual.md.
- Update the Doxygen header on the edited constructor (@author with your model and
  reasoning level, Last Modified to today) per AGENTS.md.
- Do NOT modify test files; behavioral coverage is owned by the Tester stage.

Validation guidance:
- Build from the repo root with `make`.
- Run the GUI suite headless: `cd tests/gui && make && xvfb-run -a ./GuiTests`.

Tester handoff: BattleBoard window/style coverage belongs in
tests/gui/TacticalGuiLiveTest.{cpp,h} using the WXGuiTestHarness helpers.

Artifacts: write your report to artifacts/tactical-misc-fixes/TMF-02/
(implementer_report.md and implementer_result.json), repository-root-relative.

Continue past preflight and complete the work when no blocking issues are present.

Do not report success unless all required artifacts exist and all changes are committed.
```

---

## TMF-03 — Title-bar X reliably closes the battle mid-game

Security review: not required.

### Task
Make the BattleBoard title-bar X (and the equivalent menu Quit path) reliably close
the entire battle at any point, including while a nested child modal is open
(damage summary, ICM selection, or winner/info message). Eliminate the
`../src/gtk/dialog.cpp(193)` `EndModal` assert by tearing down any active child
`wxDialog` modal loop BEFORE the `FBattleScreen` frame exits its own event loop and
hides, then aborting the battle so control returns cleanly to the strategic layer.

### Allowed Files
- `include/tactical/FBattleScreen.h`
- `src/tactical/FBattleScreen.cpp`
- `include/gui/WXTacticalUI.h`
- `src/gui/WXTacticalUI.cpp`

### Acceptance Criteria
- Clicking the title-bar X while a damage-summary, ICM-selection, or winner/info
  dialog is open does not raise the `wxDialog::EndModal` "called twice / ShowModal
  wasn't called" assert.
- Clicking the title-bar X with no child modal open continues to close the battle
  cleanly (no regression to the existing close path).
- After the X closes the battle, control returns to the strategic layer in a
  consistent state (the wrapping `SelectCombatGUI` modal resolves without a double
  `EndModal`).
- The custom `FBattleScreen::EndModal` guard (`m_modalActive`) remains intact and
  is not bypassed.

### Documentation Impact
- `AGENTS.md`: update the FBattleScreen close-vector ownership notes
  (`ID_TacticalQuit` / `wxEVT_CLOSE_WINDOW`) to reflect the nested-modal teardown
  ordering.
- `doc/test-contracts.md`: add a contract for the mid-game X-close teardown if a
  source/behavioral contract is established.
- Doxygen headers on edited close-path functions updated.

### Tester Guidance
- Cover via the GUI suite (`tests/gui/TacticalGuiLiveTest.{cpp,h}`): post a close
  request (`wxEVT_CLOSE_WINDOW`) while a child modal is showing and assert the
  battle screen ends hidden/absent with no assert, using the harness close-wait
  helpers (`waitForTopLevelWindowClosed(...)`). Tester-owned.

### Implementer Prompt
```
Your role is 'implementer'. Your task is as follows:

Make the BattleBoard title-bar X reliably close the entire battle mid-game without
tripping the ../src/gtk/dialog.cpp(193) wxDialog::EndModal assert. The assert
occurs because FBattleScreen::onClose -> closeBattleScreen exits the frame's outer
event loop and Show(false)s the frame while a nested real wxDialog (damage summary
/ ICM selection / winner-or-info message, shown via WXTacticalUI) still has a live
modal loop, producing a duplicate wxDialog::EndModal. Fix the close path so any
active child modal is torn down cleanly FIRST, then the frame exits its loop and
the battle aborts, returning control to the strategic layer. See
artifacts/fbattlescreen-menu-quit-fix.md for the documented mechanism.

Relevant current code: FBattleScreen::onClose (src/tactical/FBattleScreen.cpp:994),
closeBattleScreen (:974), custom guarded EndModal (:271); child dialogs shown via
WXTacticalUI (src/gui/WXTacticalUI.cpp:53/69/84).

Allowed files (modify only these):
- include/tactical/FBattleScreen.h
- src/tactical/FBattleScreen.cpp
- include/gui/WXTacticalUI.h
- src/gui/WXTacticalUI.cpp

Acceptance criteria (implementation outcomes):
- Clicking the X while a damage-summary, ICM-selection, or winner/info dialog is
  open does not raise the EndModal assert.
- Clicking the X with no child modal open still closes the battle cleanly.
- After closing, control returns to the strategic layer consistently (the wrapping
  SelectCombatGUI modal resolves without a double EndModal).
- The custom FBattleScreen::EndModal m_modalActive guard remains intact.

Constraints:
- Do NOT edit doc/rules/tactical_operations_manual.md.
- Update the Doxygen headers on edited functions (@author with your model and
  reasoning level, Last Modified to today) per AGENTS.md.
- Do NOT modify test files; behavioral coverage is owned by the Tester stage.

Validation guidance:
- Build from the repo root with `make`.
- Run the GUI suite headless: `cd tests/gui && make && xvfb-run -a ./GuiTests`.
- Run the tactical suite: `cd tests && make tactical-tests &&
  ./tactical/TacticalTests`.

Tester handoff: add a mid-game X-close teardown test in
tests/gui/TacticalGuiLiveTest.{cpp,h} using the harness close-wait helpers
(waitForTopLevelWindowClosed).

Artifacts: write your report to artifacts/tactical-misc-fixes/TMF-03/
(implementer_report.md and implementer_result.json), repository-root-relative.

Continue past preflight and complete the work when no blocking issues are present.

Do not report success unless all required artifacts exist and all changes are committed.
```

---

## TMF-04 — Fix "Movement Done" gating at move-phase entry

Security review: not required.

### Task
Fix the bug where a ship that cannot decelerate to 0 this turn (current
speed > ADF, so `minMove = speed - ADF > 0`) can be skipped: the "Movement Done"
button is wrongly enabled at move-phase entry for the defending player. Ensure the
move-completion gating (`checkMoveStatus()` / `m_moveComplete`) is correctly
evaluated at the START of each player's move phase — before any hex-click — for BOTH
the attacking and defending players, using the correct pre-move speed/ADF values,
so the button stays disabled until every must-move ship has moved at least
`speed - ADF` hexes.

### Allowed Files
- `include/tactical/FTacticalGame.h`
- `src/tactical/FTacticalGame.cpp`

### Acceptance Criteria
- At move-phase entry, before any hex-click, a ship with current speed > ADF keeps
  "Movement Done" disabled (`isMoveComplete()` returns false) for BOTH players.
- Reproduction case fixed: a defending ship with speed 10 / ADF 1 cannot have its
  move phase completed until it has moved at least 9 hexes; the button is not
  clickable beforehand, and its speed is not silently set to 0 by an empty move.
- "Movement Done" becomes enabled once every must-move ship has satisfied its
  minimum move; ships that may legitimately stop (speed <= ADF, or no legal next
  hex) do not block completion.
- No regression to the attacker's existing correct gating behavior.

### Documentation Impact
- `doc/test-contracts.md`: add/adjust the move-done gating contract to record the
  phase-entry evaluation invariant for both players.
- `AGENTS.md`: update the move-phase ownership notes if the recompute timing
  surface changes observably.
- Doxygen headers on edited functions updated.

### Tester Guidance
- Behavioral coverage belongs in the tactical model suite (e.g.
  `tests/tactical/FTacticalGameMechanicsTest.{cpp,h}` or
  `FTacticalMoveRouteSelectionTest`): construct a defender ship with speed 10 /
  ADF 1, enter its move phase, and assert `isMoveComplete()` is false at entry and
  remains false until `nMoved >= 9`. Author a behavioral test that fails against
  the unfixed code and passes after the fix. Tester-owned.

### Implementer Prompt
```
Your role is 'implementer'. Your task is as follows:

Fix the "Movement Done" gating bug in the tactical model. Repro: a defending UPF
Minelayer with initial speed 10 and ADF 1 (minMove = speed - ADF = 9) had the
"Movement Done" button enabled at move-phase entry and could be clicked without
moving, which then set the ship's speed to 0; the attacking ship was correctly
forced to move. Prime suspect: m_moveComplete is only recomputed on a hex-click
(FTacticalGame.cpp:3427-3434) and is not re-evaluated at the START of each player's
move phase, so the defending player inherits a stale m_moveComplete=true (and/or
the defender's pre-move getSpeed() is not the expected value when checkMoveStatus()
runs). Confirm the root cause, then make the gating correct at move-phase entry —
before any hex-click — for BOTH the attacking and defending players, using the
correct pre-move speed/ADF, so checkMoveStatus()/m_moveComplete keep the button
disabled until every ship with speed > ADF has moved at least speed - ADF hexes.

Relevant current code: checkMoveStatus() (FTacticalGame.cpp:2547-2565),
resetMovementState() (:782-790), m_moveComplete init (:284) and accessors
(include/tactical/FTacticalGame.h:366-367,1485), click-time recompute (:3427-3434).

Allowed files (modify only these):
- include/tactical/FTacticalGame.h
- src/tactical/FTacticalGame.cpp

Acceptance criteria (implementation outcomes):
- At move-phase entry, before any hex-click, a ship with current speed > ADF keeps
  isMoveComplete() == false for BOTH players.
- A defending ship with speed 10 / ADF 1 cannot complete its move phase until it
  has moved at least 9 hexes; the button is not clickable beforehand and its speed
  is not silently zeroed by an empty move.
- "Movement Done" enables once every must-move ship has satisfied its minimum move;
  ships that may legitimately stop do not block completion.
- No regression to the attacker's existing correct gating.

Constraints:
- Do NOT edit doc/rules/tactical_operations_manual.md (the speed - ADF minimum-move
  rule derives from its Acceleration/Deceleration section and must be honored, not
  changed).
- Update the Doxygen headers on edited functions (@author with your model and
  reasoning level, Last Modified to today) per AGENTS.md.
- Do NOT modify test files; behavioral coverage is owned by the Tester stage.

Validation guidance:
- Build from the repo root with `make`.
- Build and run the tactical suite: `cd tests && make tactical-tests &&
  ./tactical/TacticalTests`.

Tester handoff: add a phase-entry gating test (defender speed 10 / ADF 1) in
tests/tactical/FTacticalGameMechanicsTest.{cpp,h} (or
FTacticalMoveRouteSelectionTest) that fails on the unfixed code and passes after.

Artifacts: write your report to artifacts/tactical-misc-fixes/TMF-04/
(implementer_report.md and implementer_result.json), repository-root-relative.

Continue past preflight and complete the work when no blocking issues are present.

Do not report success unless all required artifacts exist and all changes are committed.
```

---

## TMF-05 — End-of-move facing change via Turn Left / Turn Right buttons

Security review: not required.

### Task
Add the ability for a ship that has met its minimum required move and still has
unused MR to make a single one-hexside in-place facing change, exposed through
dedicated **Turn Left** and **Turn Right** action buttons in the tactical action
panel (alongside "Movement Done"). Implement the model support for an in-place,
non-advancing single-hexside heading change that consumes one MR (counts as a turn
in the final occupied hex, honoring the manual's one-turn-per-hex limit), and
extend the existing free-rotation so a ship that decelerates to speed 0 THIS turn
may rotate. Keep the existing speed-zero-all-turn free-rotation hex-click interaction
working.

Placement: the Turn Left / Turn Right buttons are real `wxButton` controls placed
in their own dedicated child panel; they must NOT use the far-left default slot of
the existing single-column action sizer. Position the Turn-buttons panel to the
RIGHT of the Movement Done button and aligned with the TOP of the lower panel,
between the instruction/Done-button column and the ship-information column — using
the existing "Activated seekers" widget (`drawSeekerActivation()`) as the placement
reference only (not its rendering style): top anchor `y = getActionPromptLineY(0)`;
middle-column x `= leftOffset + max(widestInstructionWidth, moveDoneButtonRightExtent)
+ 2*BORDER`; keep the panel left of `shipStatsLeftMargin`. Show/hide the panel per
move phase like the other action buttons.

The facing change must be an uncommitted, reversible preview: it stays mutable
while the ship is the active mover and is finalized only when Movement Done is
clicked. Re-selecting a ship before Movement Done re-presents the Turn controls and
the ship's current pending facing. The Turn Left / Turn Right buttons remain
available whenever the ship is active and are clamped so the net change never
exceeds one hexside (valid final facings: one-left / original / one-right) — each
button is enabled only when it leads to a valid final facing, so pressing the
opposite direction undoes / reverses the turn back toward the original heading.

### Allowed Files
- `include/tactical/FBattleDisplay.h`
- `src/tactical/FBattleDisplay.cpp`
- `include/tactical/FBattleScreen.h`
- `src/tactical/FBattleScreen.cpp`
- `include/tactical/FTacticalGame.h`
- `src/tactical/FTacticalGame.cpp`

### Acceptance Criteria
- Turn Left and Turn Right buttons exist in the tactical action panel and are
  available for the active ship whenever it has met its minimum required move
  (`nMoved >= speed - ADF`) and has the MR budget for an end-of-move turn; each
  button is enabled only when it yields a valid final facing within one hexside of
  the ship's pre-turn heading (from the original heading both are enabled; from a
  one-hexside-turned state only the reverse direction is enabled), clamping the net
  change to a single hexside.
- The Turn Left / Turn Right buttons are real `wxButton` controls placed in their
  own dedicated child panel positioned to the right of the Movement Done button,
  aligned with the top of the lower panel, and to the left of the ship-information
  column (using the Activated-seekers widget as the placement reference); the panel
  overlaps neither the instruction/Done-button column nor the ship-information
  column, and is shown/hidden per move phase.
- Clicking a Turn button rotates the active ship exactly one hexside in place
  WITHOUT moving it and updates the rendered heading.
- The facing change is uncommitted and reversible until Movement Done: pressing the
  opposite Turn button undoes / reverses the turn back toward the original heading,
  and re-selecting the active ship before Movement Done re-presents the Turn
  controls and the ship's current pending facing.
- A ship that decelerates to speed 0 this turn can rotate (free-rotation gate
  extended accordingly); a ship at speed 0 for the whole turn retains its existing
  full free-rotation behavior.
- The pending facing is finalized only at move-phase finalization (Movement Done),
  and that final heading is persisted and used for subsequent fire-arc/facing logic.
- No regression to normal move/turn route building or to "Movement Done" gating
  (TMF-04).

### Documentation Impact
- `AGENTS.md`: document the new Turn Left / Turn Right action buttons and the
  in-place-turn model seam in the FBattleDisplay/FBattleScreen/FTacticalGame
  ownership notes; add the tests to the tactical/GUI enumerations.
- `doc/test-contracts.md`: add contracts for the facing-button enable conditions
  and the in-place single-hexside turn behavior.
- `doc/UsersGuide.md`: REQUIRED — document the Turn Left / Turn Right buttons,
  including that the end-of-move facing change is reversible (press the opposite
  button to undo / turn the other way) and is only committed when Movement Done is
  clicked.
- `doc/rules/tactical_operations_manual.md`: MUST NOT change.
- Doxygen headers on all new/edited functions and any new button members.

### Tester Guidance
- Model behavior: cover the in-place single-hexside turn, MR/one-turn-per-hex
  enforcement, and the just-stopped free-rotation extension in the tactical suite
  (e.g. `tests/tactical/FTacticalGameMechanicsTest.{cpp,h}`,
  `FTacticalMoveRouteSelectionTest`, `FTacticalShipPlacementOrientationTest`).
- Button presence/enable-state and click→turn wiring: cover via the GUI suite
  (`tests/gui/TacticalGuiLiveTest.{cpp,h}`). All tester-owned.

### Implementer Prompt
```
Your role is 'implementer'. Your task is as follows:

Add an end-of-move single facing change for a ship that finishes its move with
unused Maneuver Rating (MR), exposed through dedicated Turn Left and Turn Right
action buttons in the tactical action panel (next to "Movement Done"). The current
UI couples turning with entering a hex (handleMoveHexSelection,
FTacticalGame.cpp:2523-2526); the only decoupled rotation today is speed-zero
free-rotation (canUseStoppedShipFreeRotation, :199-206; handling :2419-2432). Add
model support for an in-place, non-advancing single-hexside heading change that
consumes one MR (counts as a turn in the final occupied hex, honoring the manual's
one-turn-per-hex limit), wire it to the new buttons via FBattleScreen forwarding,
and extend the free-rotation gate so a ship that decelerates to speed 0 THIS turn
may rotate. Keep the existing speed-zero-all-turn free-rotation hex-click working.

Placement: the Turn Left / Turn Right buttons are real wxButton controls placed in
their own dedicated child panel. The lower display (FBattleDisplay) is one panel; the
existing single-column actionSizer (FBattleDisplay.cpp:130-140) only stacks the Done
buttons at the far left, so do NOT add the Turn buttons to it. Instead position the
Turn-buttons panel like the existing "Activated seekers" widget (use
drawSeekerActivation() ONLY as the placement reference, not its custom-paint rendering
style): anchor the panel at the TOP of the lower panel (y = getActionPromptLineY(0),
FBattleDisplay.cpp:249-251, used at :1702), in the MIDDLE column to the RIGHT of the
Movement Done button (x = leftOffset + max(widestInstructionWidth,
moveDoneButtonRightExtent) + 2*BORDER, mirroring the lMargin math at :1717-1730), and
keep it to the LEFT of the ship-information column (shipStatsLeftMargin, constructor
:94, recomputed :1116-1135) so columns never overlap. Show/hide the panel per move
phase like the other action buttons (Show pattern e.g. :1102).

The facing change must be an uncommitted, reversible preview, finalized only when
Movement Done is clicked. While the ship is the active mover: the Turn Left / Turn
Right buttons remain available; each is enabled only when it yields a valid final
facing within one hexside of the ship's pre-turn heading (from the original heading
both are enabled; from a one-hexside-turned state only the reverse direction is
enabled), so the net change is clamped to a single hexside and pressing the
opposite button undoes / reverses the turn. Re-selecting the active ship before
Movement Done must re-present the Turn controls and the ship's current pending
facing. The pending facing is committed to the ship only at move-phase
finalization.

Relevant current code: action buttons and enable logic in FBattleDisplay.cpp
(Movement Done created :116, enabled :1099, handler :1214-1222); move-completion
gating checkMoveStatus() (FTacticalGame.cpp:2547-2565); finalizeMovementState()
heading/speed write-back (:805-806); MR cap getMR() (include/ships/FVehicle.h:45);
heading accessors getHeading()/setHeading() (include/ships/FVehicle.h:73-75).

Allowed files (modify only these):
- include/tactical/FBattleDisplay.h
- src/tactical/FBattleDisplay.cpp
- include/tactical/FBattleScreen.h
- src/tactical/FBattleScreen.cpp
- include/tactical/FTacticalGame.h
- src/tactical/FTacticalGame.cpp

Acceptance criteria (implementation outcomes):
- Turn Left / Turn Right buttons exist in the action panel and are available for
  the active ship whenever it has met its minimum required move (nMoved >= speed -
  ADF) and has the MR budget for an end-of-move turn; each button is enabled only
  when it yields a valid final facing within one hexside of the pre-turn heading
  (both enabled from the original heading; only the reverse direction enabled from
  a one-hexside-turned state), clamping the net change to a single hexside.
- The Turn buttons are real wxButton controls in their own dedicated child panel,
  positioned to the right of the Movement Done button, aligned with the top of the
  lower panel, and to the left of the ship-information column (Activated-seekers
  widget used as placement reference); the panel overlaps neither the
  instruction/Done-button column nor the ship-information column, and is shown/hidden
  per move phase.
- Clicking a Turn button rotates the active ship exactly one hexside in place
  WITHOUT moving it and updates the rendered heading.
- The facing change is uncommitted and reversible until Movement Done: pressing the
  opposite Turn button undoes / reverses the turn back toward the original heading,
  and re-selecting the active ship before Movement Done re-presents the Turn
  controls and the ship's current pending facing.
- A ship that decelerates to speed 0 this turn can rotate; a ship at speed 0 all
  turn retains its existing full free-rotation behavior.
- The pending facing is committed only at move finalization (Movement Done) and the
  final heading is used by subsequent facing/fire-arc logic.
- No regression to normal move/turn route building or to Movement Done gating.

Constraints:
- Do NOT edit doc/rules/tactical_operations_manual.md. Implement the manual-grounded
  reading (one turn per hex applied to the final occupied hex, plus extending
  free-rotation to a just-stopped ship). If this cannot be done without
  contradicting the manual, STOP and report rather than editing the manual.
- Update the Doxygen headers on all new/edited functions and button members
  (@author with your model and reasoning level, Last Modified to today) per
  AGENTS.md.
- Do NOT modify test files; behavioral coverage is owned by the Tester stage.

Validation guidance:
- Build from the repo root with `make`.
- Build and run the tactical suite: `cd tests && make tactical-tests &&
  ./tactical/TacticalTests`.
- Run the GUI suite headless: `cd tests/gui && make && xvfb-run -a ./GuiTests`.

Tester handoff: model behavior in tests/tactical/FTacticalGameMechanicsTest.{cpp,h}
(and FTacticalMoveRouteSelectionTest); button presence/enable/click wiring in
tests/gui/TacticalGuiLiveTest.{cpp,h}.

Artifacts: write your report to artifacts/tactical-misc-fixes/TMF-05/
(implementer_report.md and implementer_result.json), repository-root-relative.

Continue past preflight and complete the work when no blocking issues are present.

Do not report success unless all required artifacts exist and all changes are committed.
```

---

## TMF-06 — Skip the damage dialog when no weapon was fired

Security review: not required.

### Task
Suppress the post-fire damage-summary dialog only when zero weapons were actually
fired in the phase. Surface a weapons-fired count from `fireAllWeapons()`
(currently `result.fired()` is tallied via the appended `attacks` entries and then
discarded at `clearTacticalReport()`, `FTacticalGame.cpp:627`) onto
`FTacticalCombatReportSummary`, and guard the damage-dialog display in BOTH the
defensive and offensive fire-done handlers (`FBattleDisplay.cpp:1279/1293`) so the
dialog is shown whenever at least one weapon fired (even if all shots missed or
were fully intercepted) and skipped only when none fired.

### Allowed Files
- `include/tactical/FTacticalCombatReport.h`
- `include/tactical/FTacticalGame.h`
- `src/tactical/FTacticalGame.cpp`
- `include/tactical/FBattleDisplay.h`
- `src/tactical/FBattleDisplay.cpp`

### Acceptance Criteria
- `FTacticalCombatReportSummary` carries a weapons-fired count (or equivalent
  boolean) populated from `fireAllWeapons()` before the report is cleared.
- After a defensive or offensive fire phase in which no weapon fired, the damage
  dialog is not shown.
- After a fire phase in which at least one weapon fired — including the case where
  every shot missed or was fully intercepted (empty damage rows) — the damage
  dialog is still shown.
- No regression to the existing damage-summary contents or to the mine-damage
  report path (which intentionally shows even when empty).

### Documentation Impact
- `doc/test-contracts.md`: add a contract for the weapons-fired count on the
  summary and the dialog-skip guard.
- `AGENTS.md`: update the FBattleDisplay/FTacticalGame fire-phase report ownership
  notes to record the fired-count surface.
- Doxygen headers on edited functions and the summary struct updated.

### Tester Guidance
- Cover in the tactical suite (`tests/tactical/FTacticalCombatReportTest.{cpp,h}`,
  `FTacticalBattleDisplayFireFlowTest.{cpp,h}`): assert the fired-count is zero when
  no weapon fires and positive when one fires-and-misses, and assert the dialog is
  skipped only in the zero case. Tester-owned.

### Implementer Prompt
```
Your role is 'implementer'. Your task is as follows:

Skip the post-fire damage-summary dialog only when no weapon was actually fired in
the phase. Today the dialog is shown unconditionally from the fire-done handlers
(FBattleDisplay.cpp:1279 defensive, :1293 offensive). An empty damage summary is
NOT a valid "nothing fired" proxy because a weapon can fire and miss (or be fully
intercepted) yet produce no damage rows. In fireAllWeapons() the per-weapon
result.fired() signal (FTacticalGame.cpp:620) is tallied via appended attacks
entries (:623) and then discarded at clearTacticalReport() (:627), and is not
carried on FTacticalCombatReportSummary
(include/tactical/FTacticalCombatReport.h:261-269). Surface a weapons-fired count
(or boolean) onto the summary, populate it from fireAllWeapons() before the report
is cleared, and guard the damage-dialog display in BOTH fire-done handlers so the
dialog shows when >= 1 weapon fired and is skipped only when none fired.

Allowed files (modify only these):
- include/tactical/FTacticalCombatReport.h
- include/tactical/FTacticalGame.h
- src/tactical/FTacticalGame.cpp
- include/tactical/FBattleDisplay.h
- src/tactical/FBattleDisplay.cpp

Acceptance criteria (implementation outcomes):
- FTacticalCombatReportSummary carries a weapons-fired count (or equivalent
  boolean) populated from fireAllWeapons() before the report is cleared.
- After a defensive or offensive fire phase with no weapon fired, the damage
  dialog is not shown.
- After a fire phase with at least one weapon fired — including all-miss /
  fully-intercepted (empty damage rows) — the damage dialog is still shown.
- No regression to existing damage-summary contents or to the mine-damage report
  path that intentionally shows when empty.

Constraints:
- Do NOT edit doc/rules/tactical_operations_manual.md.
- Update the Doxygen headers on edited functions and the summary struct (@author
  with your model and reasoning level, Last Modified to today) per AGENTS.md.
- Do NOT modify test files; behavioral coverage is owned by the Tester stage.

Validation guidance:
- Build from the repo root with `make`.
- Build and run the tactical suite: `cd tests && make tactical-tests &&
  ./tactical/TacticalTests`.
- Run the GUI suite headless: `cd tests/gui && make && xvfb-run -a ./GuiTests`.

Tester handoff: cover the fired-count and dialog-skip behavior in
tests/tactical/FTacticalCombatReportTest.{cpp,h} and
FTacticalBattleDisplayFireFlowTest.{cpp,h}.

Artifacts: write your report to artifacts/tactical-misc-fixes/TMF-06/
(implementer_report.md and implementer_result.json), repository-root-relative.

Continue past preflight and complete the work when no blocking issues are present.

Do not report success unless all required artifacts exist and all changes are committed.
```
