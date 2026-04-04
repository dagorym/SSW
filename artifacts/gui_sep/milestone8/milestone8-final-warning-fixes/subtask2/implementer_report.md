Implementer execution summary (Subtask 2)

Plan step status
- Added direct tactical regression coverage for selected-ship cleanup behavior via a focused lifecycle seam extracted from `FBattleScreen::clearDestroyedShips()`.
- Updated tactical delegation assertions to track the new seam wiring instead of old inline cleanup lines.
- Wired new tactical suite into tactical runner/build files.
- Ran focused tactical validation.

Files changed
- src/tactical/FBattleScreen.cpp
- tests/tactical/FTacticalBattleScreenDelegationTest.cpp
- tests/tactical/FTacticalDestroyedShipCleanupLifecycleTest.h
- tests/tactical/FTacticalDestroyedShipCleanupLifecycleTest.cpp
- tests/tactical/Makefile
- tests/tactical/TacticalTests.cpp

Validation commands run
1) `cd tests/tactical && make clean && make && ./TacticalTests`
   - First attempt failed at link stage due to missing gui link dependencies in tactical test makefile.
2) `cd tests/tactical && make clean && make && ./TacticalTests`
   - Passed after adding `-lgui` and gui build dependency in `tests/tactical/Makefile`.

Validation outcome
- PASS (focused tactical suite built and executed successfully).

Acceptance criteria mapping
- Direct tactical coverage added for selected-ship cleanup branch using callable lifecycle seam (`runDestroyedShipCleanupLifecycle`).
- Tests explicitly verify selected-ship clearing, redraw behavior, single bookkeeping clear per lifecycle invocation, and winner handling ordering after bookkeeping clear.
- Seam keeps ownership contract: model destroyed IDs consumed by screen orchestration, bookkeeping clear invoked once after consumption, winner handled after cleanup.
- New suite wired into tactical test build/runner.
