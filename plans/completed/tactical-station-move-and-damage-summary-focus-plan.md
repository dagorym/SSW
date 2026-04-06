# Tactical Station Move And Damage Summary Focus Plan

## Feature Restatement
Make two tactical UX fixes:
1. When a station's orbital movement is already precomputed for the movement phase, the player should not need to click the station to have that move count as registered.
2. The tactical damage summary dialog should open with keyboard focus on its `Close` button so pressing Enter dismisses the dialog immediately.

## Confirmed Repository Facts
- [`src/tactical/FTacticalGame.cpp`](/home/tstephen/repos/SSW/src/tactical/FTacticalGame.cpp) already pre-seeds station movement in `resetTurnInfoForCurrentMover()` by adding the station start/end path points, setting `nMoved = 1`, and computing `finalHeading`.
- [`src/tactical/FTacticalGame.cpp`](/home/tstephen/repos/SSW/src/tactical/FTacticalGame.cpp) currently forces `setMoveComplete(false);` in `resetMovementState()` and does not immediately recompute move completion after the pre-seeded station turn data is created.
- [`src/gui/TacticalDamageSummaryGUI.cpp`](/home/tstephen/repos/SSW/src/gui/TacticalDamageSummaryGUI.cpp) creates and binds the `Close` button but does not currently set default-button behavior or explicit initial focus.
- Existing nearby regression coverage already exists in [`tests/tactical/FTacticalStationOrbitalMovementTest.cpp`](/home/tstephen/repos/SSW/tests/tactical/FTacticalStationOrbitalMovementTest.cpp), [`tests/tactical/FTacticalGameMechanicsTest.cpp`](/home/tstephen/repos/SSW/tests/tactical/FTacticalGameMechanicsTest.cpp), and [`tests/gui/TacticalGuiLiveTest.cpp`](/home/tstephen/repos/SSW/tests/gui/TacticalGuiLiveTest.cpp).

## Assumptions
- “You shouldn't have to click on the station” means the precomputed orbital move should already satisfy movement completion bookkeeping when appropriate, especially when the moving side only has the station left to resolve.
- No broader tactical UI rewrite is intended; the preferred fix is to honor existing model bookkeeping rather than introduce station-specific GUI flow.
- No documentation update is expected unless maintainers want this behavior called out in a milestone artifact or changelog.

## Files To Modify
- Likely: [`src/tactical/FTacticalGame.cpp`](/home/tstephen/repos/SSW/src/tactical/FTacticalGame.cpp)
- Likely: [`tests/tactical/FTacticalStationOrbitalMovementTest.h`](/home/tstephen/repos/SSW/tests/tactical/FTacticalStationOrbitalMovementTest.h)
- Likely: [`tests/tactical/FTacticalStationOrbitalMovementTest.cpp`](/home/tstephen/repos/SSW/tests/tactical/FTacticalStationOrbitalMovementTest.cpp)
- Likely: [`tests/tactical/FTacticalGameMechanicsTest.cpp`](/home/tstephen/repos/SSW/tests/tactical/FTacticalGameMechanicsTest.cpp)
- Likely: [`src/gui/TacticalDamageSummaryGUI.cpp`](/home/tstephen/repos/SSW/src/gui/TacticalDamageSummaryGUI.cpp)
- Possible: [`include/gui/TacticalDamageSummaryGUI.h`](/home/tstephen/repos/SSW/include/gui/TacticalDamageSummaryGUI.h) if a helper or declaration is needed for focus/default-button setup
- Likely: [`tests/gui/TacticalGuiLiveTest.cpp`](/home/tstephen/repos/SSW/tests/gui/TacticalGuiLiveTest.cpp)
- Possible: [`tests/gui/TacticalGuiLiveTest.h`](/home/tstephen/repos/SSW/tests/gui/TacticalGuiLiveTest.h) only if a separate regression case is added instead of extending the existing dialog test

## Overall Documentation Impact
- No user guide or design-note update is expected for these narrow UX fixes.
- `AGENTS.md` is likely unchanged because the tactical movement and dialog architecture do not change.
- If the team keeps milestone-level behavior notes elsewhere in `artifacts/` or `plans/completed/`, those notes may need a brief follow-up mention after implementation, but this is not expected to be a first-class deliverable for the story.

## Subtask 1: Auto-Register Precomputed Station Movement
### Description
Adjust tactical movement-phase initialization so a station whose orbital move is already precomputed in model turn data is counted immediately by movement completion bookkeeping, without requiring a click on the station to trigger `checkMoveStatus()`.

### Acceptance Criteria
- Entering `PH_MOVE` recomputes move completion after `resetTurnInfoForCurrentMover()` has populated current-mover turn data.
- A moving side with only an orbital station can advance past movement without first selecting the station.
- The station's existing orbital behavior remains unchanged: its position advances one hex in orbit and its heading updates according to the existing gravity-turn logic.
- Non-station ship movement route selection and completion behavior remain unchanged.
- Tactical regression coverage includes an observable case that fails under the old behavior and passes under the new behavior.
- Any text-level tactical mechanics assertions that lock in the previous `resetMovementState()` body are updated to reflect the new bookkeeping sequence.

### Documentation Impact
- No documentation update expected.

### Implementer Agent Prompt
You are the implementer agent

Allowed files:
- `/home/tstephen/repos/SSW/src/tactical/FTacticalGame.cpp`
- `/home/tstephen/repos/SSW/tests/tactical/FTacticalStationOrbitalMovementTest.h`
- `/home/tstephen/repos/SSW/tests/tactical/FTacticalStationOrbitalMovementTest.cpp`
- `/home/tstephen/repos/SSW/tests/tactical/FTacticalGameMechanicsTest.cpp`

Task:
- Update tactical movement-phase bookkeeping so pre-seeded station orbital movement is recognized immediately when movement starts, without requiring the player to click the station first.
- Preserve the existing station orbit path, heading, and speed behavior.
- Add or extend tactical regression coverage to prove the move is already registered at movement-phase entry and that non-station movement behavior is not regressed.

Acceptance criteria:
- `resetMovementState()` or its immediate call chain recomputes move completion after current-mover turn data is rebuilt.
- A tactical regression proves `isMoveComplete()` is already true for a moving side whose only required move is the pre-seeded station orbit.
- Existing station orbital movement assertions still pass conceptually: orbit distance remains 1, heading updates correctly, and speed is preserved.
- Existing non-station movement route behavior remains covered and unchanged.
- Any mechanics-source assertions updated by this change still describe the real code path accurately.

Do not report success unless all required artifacts exist and all changes are committed.

## Subtask 2: Put Initial Focus On Damage Summary Close Button
### Description
Update the tactical damage summary dialog so the `Close` button is both the default action and the initially focused control, allowing Enter to dismiss the dialog without first tabbing away from the read-only summary text control.

### Acceptance Criteria
- Opening `TacticalDamageSummaryGUI` gives initial keyboard focus to the `Close` button.
- Pressing Enter on dialog open dismisses the modal dialog through the normal `wxID_OK` close path.
- Existing dialog title, context text, summary text, and empty-state behavior remain unchanged.
- GUI regression coverage verifies both the focus/default-button behavior and the existing close result.

### Documentation Impact
- No documentation update expected.

### Implementer Agent Prompt
You are the implementer agent

Allowed files:
- `/home/tstephen/repos/SSW/src/gui/TacticalDamageSummaryGUI.cpp`
- `/home/tstephen/repos/SSW/include/gui/TacticalDamageSummaryGUI.h`
- `/home/tstephen/repos/SSW/tests/gui/TacticalGuiLiveTest.cpp`
- `/home/tstephen/repos/SSW/tests/gui/TacticalGuiLiveTest.h`

Task:
- Change the tactical damage summary dialog initialization so the `Close` button owns initial focus and default Enter-key activation.
- Keep the dialog's current content and modal close semantics intact.
- Extend the live GUI regression to assert the new keyboard-first behavior.

Acceptance criteria:
- The dialog constructor explicitly leaves the `Close` button as the initial focused control and default action.
- A GUI test proves the dialog can be dismissed with Enter without manually clicking the button first.
- Existing assertions for title, context text, populated summary text, empty-state text, and `wxID_OK` close result remain valid.
- No unrelated dialog behavior or layout is changed.

Do not report success unless all required artifacts exist and all changes are committed.

## Dependency Ordering
- Subtask 1 and Subtask 2 can run in parallel.
- The subtasks touch different runtime files, different test modules, and no shared interfaces.
- No cross-subtask design decision or sequencing dependency is currently visible from repository context.

## Output Artifact
- Plan file: [`plans/tactical-station-move-and-damage-summary-focus-plan.md`](/home/tstephen/repos/SSW/plans/tactical-station-move-and-damage-summary-focus-plan.md)
