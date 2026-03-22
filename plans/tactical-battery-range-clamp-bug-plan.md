# Tactical Battery Range Clamp Bug Plan

## Confirmed Repository Facts
- The bug is in `src/tactical/FBattleBoard.cpp` inside `FBattleBoard::computeBatteryRange`.
- The current code compares `xMax < m_nCol` and `yMax < m_nRow`, which expands the scan range instead of capping it.
- Tactical tests live under `tests/tactical/` and are wired through `tests/tactical/Makefile` and `tests/tactical/TacticalTests.cpp`.

## Assumptions
- Map coordinates are zero-based and inclusive, so the last valid column is `m_nCol - 1` and the last valid row is `m_nRow - 1`.
- The fix should preserve the existing hex-distance filtering and only correct the bounding box.
- A regression test can be added in `tests/tactical/` without changing production headers unless the test harness needs a small access helper.

## Files To Modify
- `src/tactical/FBattleBoard.cpp`
- `tests/tactical/FTacticalBatteryRangeClampTest.cpp` or another tactical test file with equivalent coverage, if a new file is not needed
- `tests/tactical/FTacticalBatteryRangeClampTest.h` if a new tactical test class is added
- `tests/tactical/Makefile` if a new test object must be compiled
- `tests/tactical/TacticalTests.cpp` if a new test suite must be registered

## Subtasks

### 1. Correct the battery-range clamp logic
- Update `FBattleBoard::computeBatteryRange` so the upper bounds are clamped to the valid inclusive board edge instead of being expanded to the map size.
- Keep the existing distance check intact so only in-range hexes remain eligible.
- Preserve current behavior for center-board positions so the fix is limited to boundary handling.

Acceptance criteria:
- The computed scan window never extends past the last valid column or row.
- Edge and corner positions no longer produce off-map points in the range set.
- The method still returns the same in-bounds results for positions fully inside the board.

### 2. Add regression coverage for boundary cases
- Add tactical test coverage that exercises at least one center position, one edge position, and one corner position.
- Verify that the returned point set contains only valid board coordinates and does not expand past the map edge.
- Wire the test into the tactical test build and runner if a new test class/file is introduced.

Acceptance criteria:
- The tactical test suite fails before the fix and passes after the fix for the boundary scenarios.
- The regression test explicitly covers edge and corner cases, not just a center-board case.
- The test harness builds cleanly with the new coverage registered.

## Dependency Ordering
1. Subtask 1 must land first because the regression test needs the corrected behavior.
2. Subtask 2 depends on Subtask 1 and can be implemented and verified immediately after it.

## Implementer Agent Prompts

### Prompt 1
You are the implementer agent.

Files you are allowed to change: `src/tactical/FBattleBoard.cpp`.

Task: Fix the tactical battery-range bounding logic in `FBattleBoard::computeBatteryRange` so the scan window clamps to the valid board edge instead of expanding toward the map size.

Acceptance criteria:
- The upper bounds for the scan window are capped at the last valid inclusive board coordinate.
- The existing hex-distance filter still determines the final highlighted set.
- Positions near the edge or corner of the board do not generate off-map coordinates.
- Center-board behavior remains unchanged except for the corrected clamping logic.

### Prompt 2
You are the implementer agent.

Files you are allowed to change: `tests/tactical/FTacticalBatteryRangeClampTest.cpp`, `tests/tactical/FTacticalBatteryRangeClampTest.h`, `tests/tactical/Makefile`, `tests/tactical/TacticalTests.cpp`, and any tightly related tactical test support file if the existing harness makes that more practical.

Task: Add regression tests for tactical battery range clamping. Cover at least one center-board case, one edge case, and one corner case, and verify the result set contains only in-bounds hexes.

Acceptance criteria:
- The test suite demonstrates the bug on the old logic and passes with the corrected logic.
- Edge and corner positions are explicitly covered.
- The test harness builds and runs with the new test registered.
- The regression test asserts that no off-map coordinates appear in the computed battery range.

## Output Artifact
- Written plan file: `plans/tactical-battery-range-clamp-bug-plan.md`
