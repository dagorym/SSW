# Ship Placement Orientation Regression Plan

## Feature Restatement
Restore the tactical ship-placement flow so it behaves as the display text already describes: the first board click places the ship, mouse movement previews a snapped heading toward one of the six hex faces, and the second board click commits the currently previewed heading before setup advances to initial speed selection.

## Confirmed Repository Facts
- `src/tactical/FBattleDisplay.cpp` already tells the user to click once, move the mouse to choose heading, and click again to finalize placement.
- `src/tactical/FBattleBoard.cpp` hover handling currently calls `m_parent->setShipPlacementHeadingByHex(FPoint(a,b))` while setup rotation is pending.
- `src/tactical/FTacticalGame.cpp` currently implements `setShipPlacementHeadingByHex()` by calling `setShipPlacementHeading()`, and `setShipPlacementHeading()` immediately toggles control state, advances to `PH_SET_SPEED`, and clears `m_setRotation`.
- `src/tactical/FTacticalGame.cpp` setup click dispatch currently routes setup-state clicks only through `placeShip(hex)`, so a second click during pending rotation cannot complete placement cleanly.
- The existing shared hex-heading math already snaps arbitrary positions to one of six headings via `FHexMap::computeHeading(...)`.

## Assumptions
- The fix should preserve the Milestone 10 ownership split: tactical placement state stays model-owned in `FTacticalGame`, while `FBattleBoard` remains a wx event-forwarding surface.
- The best fix is to separate heading preview from heading finalization rather than moving orientation state back into wx-only code.

## Likely Files To Modify
- `include/tactical/FTacticalGame.h`
- `src/tactical/FTacticalGame.cpp`
- `include/tactical/FBattleScreen.h`
- `src/tactical/FBattleScreen.cpp`
- `src/tactical/FBattleBoard.cpp`
- `tests/tactical/FTacticalGameMechanicsTest.cpp`
- `tests/tactical/FTacticalBattleScreenDelegationTest.cpp`
- `tests/tactical/FTacticalBattleBoardRendererDelegationTest.cpp`
- Likely new runtime regression fixture:
  - `tests/tactical/FTacticalShipPlacementOrientationTest.h`
  - `tests/tactical/FTacticalShipPlacementOrientationTest.cpp`
- If a new fixture is added:
  - `tests/tactical/TacticalTests.cpp`
  - `tests/tactical/Makefile`
- Optional, only if the implementer finds a lightweight deterministic hook worth covering at wx level:
  - `tests/gui/TacticalGuiLiveTest.cpp`

## Overall Documentation Impact
- No user-guide change is likely required because the current display prompt already describes the intended behavior.
- Review `doc/DesignNotes.md` only if the implementation adds a new named placement-preview/finalization seam or if the team wants this regression called out in the tactical GUI-separation notes.

## Subtasks

### Subtask 1: Restore a two-step model contract for ship placement heading
Description:
Split ship-placement orientation into two distinct model actions: a hover-preview path that updates the ship heading without ending placement, and a finalize path that commits the last previewed heading and advances setup to speed selection. Keep heading snapping derived from `FHexMap::computeHeading(...)`.

Acceptance Criteria:
1. `FTacticalGame` exposes a placement-heading preview path that accepts a hex, computes the snapped heading, and updates the selected ship heading without toggling control state, changing phase, or clearing `m_setRotation`.
2. `FTacticalGame` exposes or reworks the final placement-heading commit path so the second setup click completes placement only when a ship is selected and rotation is pending.
3. Finalization still performs the existing end-of-placement transitions: leave placement control mode, enter `PH_SET_SPEED`, and clear the pending-rotation flag.
4. Setup click dispatch in `FTacticalGame::handleHexClick(...)` distinguishes between first-click placement and second-click heading finalization instead of sending both clicks through `placeShip(...)`.
5. Tactical regression coverage proves that previewing heading does not prematurely advance phase/state, and that finalization commits the snapped heading and transitions to speed selection.

Documentation Impact:
- Likely no documentation update.
- If the implementation introduces new API names that become part of the tactical delegation surface, review `doc/DesignNotes.md` for accuracy.

### Subtask 2: Rewire the wx event path to use preview-on-motion and commit-on-click
Description:
Update `FBattleBoard` and the `FBattleScreen` forwarding seam so mouse movement only previews orientation, while the second board click commits the current snapped heading through the model contract from Subtask 1. Keep refresh behavior efficient by redrawing only when the snapped preview heading changes.

Acceptance Criteria:
1. `FBattleBoard::onMotion(...)` uses the non-finalizing placement-preview path while setup rotation is pending.
2. `FBattleBoard::onLeftUp(...)` continues to forward board clicks through `handleHexClick(...)`, and the resulting setup flow supports place-on-first-click and finalize-on-second-click.
3. `FBattleScreen` forwards any new or revised placement-preview/finalization APIs to `FTacticalGame` and preserves the existing redraw-on-change pattern.
4. Tactical delegation tests cover the revised `FBattleBoard` and `FBattleScreen` call sites so the separation boundary remains explicit.
5. Regression coverage proves the six-way snap behavior from hover input and proves that the committed heading matches the last previewed heading rather than the first hover event that entered rotation mode.

Documentation Impact:
- No documentation update expected unless a new wx-level test or seam needs to be called out in `doc/DesignNotes.md`.

## Dependency Ordering
1. Subtask 1 must complete first because it defines the model/API contract the board hover and click paths need.
2. Subtask 2 follows Subtask 1 and wires the wx event path onto that finalized contract.

Parallelization:
- Do not run these in parallel. They share the same behavior contract, setup-state logic, and overlapping tactical delegation files.

## Implementer Agent Prompts

### Implementer Prompt for Subtask 1
You are the implementer agent

Allowed files:
- `include/tactical/FTacticalGame.h`
- `src/tactical/FTacticalGame.cpp`
- `include/tactical/FBattleScreen.h`
- `src/tactical/FBattleScreen.cpp`
- `tests/tactical/FTacticalGameMechanicsTest.cpp`
- `tests/tactical/FTacticalBattleScreenDelegationTest.cpp`
- `tests/tactical/FTacticalShipPlacementOrientationTest.h`
- `tests/tactical/FTacticalShipPlacementOrientationTest.cpp`
- `tests/tactical/TacticalTests.cpp`
- `tests/tactical/Makefile`

Task:
Implement a two-step tactical ship-placement heading contract in the model. Hover updates must preview snapped heading without finalizing placement, and the second setup click must commit the currently previewed heading and advance to speed selection. Keep the tactical model as the owner of placement state and make `FBattleScreen` forward any revised placement API without embedding wx-only placement logic.

Acceptance criteria:
1. Previewing a heading by hex updates the selected ship heading but does not toggle control state, change phase, or clear pending rotation.
2. Finalizing placement from setup click flow commits the current snapped heading only when rotation is pending and leaves setup in `PH_SET_SPEED`.
3. `handleHexClick(...)` supports the intended first-click place / second-click finalize flow during `BS_SetupDefendFleet` and `BS_SetupAttackFleet`.
4. Tactical regression coverage proves the preview path and finalization path are behaviorally distinct and correct.

Do not report success unless all required artifacts exist and all changes are committed.

### Implementer Prompt for Subtask 2
You are the implementer agent

Allowed files:
- `src/tactical/FBattleBoard.cpp`
- `include/tactical/FBattleScreen.h`
- `src/tactical/FBattleScreen.cpp`
- `tests/tactical/FTacticalBattleBoardRendererDelegationTest.cpp`
- `tests/tactical/FTacticalBattleScreenDelegationTest.cpp`
- `tests/tactical/FTacticalShipPlacementOrientationTest.h`
- `tests/tactical/FTacticalShipPlacementOrientationTest.cpp`
- `tests/tactical/TacticalTests.cpp`
- `tests/tactical/Makefile`
- `tests/gui/TacticalGuiLiveTest.cpp`

Task:
Wire the tactical board hover and click flow onto the restored two-step placement contract. Mouse motion during pending rotation must only preview snapped heading, while the second board click must finalize the last previewed heading through the model-facing `FBattleScreen` seam. Preserve redraw-on-change behavior and the tactical GUI-separation boundary.

Acceptance criteria:
1. `FBattleBoard::onMotion(...)` forwards only non-finalizing heading preview updates during pending rotation.
2. `FBattleBoard::onLeftUp(...)` plus the setup click flow now allow first-click placement followed by second-click heading commit.
3. `FBattleScreen` forwarding remains a thin delegation layer with redraw behavior only when model state changes.
4. Tactical delegation tests and regression coverage prove the wx path now matches the documented UX and preserves six-way snapped orientation.
5. If a deterministic wx live regression is practical within existing harness patterns, add it; otherwise keep the coverage at tactical/model level and ensure the delegation tests explicitly lock the event-forwarding contract.

Do not report success unless all required artifacts exist and all changes are committed.

## Output Artifact
- `/home/tstephen/repos/SSW/plans/ship-placement-orientation-regression-plan.md`
