Subtask: Milestone 8 Subtask 4 (tests/forwarding update)

Plan step status:
- Preflight scope/context checks: completed (worktree/branch matched assigned context).
- Tactical tests/forwarding updates: completed.
- Validation: completed with one passing scoped suite and one environment-limited aggregate build failure.
- Required artifacts generated: completed.

Implementation summary:
- Updated tactical source-inspection tests to reflect Milestone 8 model ownership boundaries.
- Added explicit assertions for FBattleScreen forwarding of selection/hex-interaction mutators.
- Added a new tactical test suite focused on FTacticalGame selection/hex-click API surface and routing flow.
- Kept tests explicitly asserting FBattleDisplay no longer owns fire-all-weapons transitional helper.
- Updated tactical test runner and tactical Makefile registration for the new test suite.

Files changed (implementation commit):
- tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp
- tests/tactical/FTacticalBattleDisplayFireFlowTest.h
- tests/tactical/FTacticalBattleScreenDelegationTest.cpp
- tests/tactical/FTacticalBattleScreenDelegationTest.h
- tests/tactical/FTacticalModelSelectionHexClickSurfaceTest.cpp
- tests/tactical/FTacticalModelSelectionHexClickSurfaceTest.h
- tests/tactical/TacticalTests.cpp
- tests/tactical/Makefile

Validation commands and outcomes:
1) `cd tests/tactical && make -s clean && make -s && ./TacticalTests`
   - Outcome: PASS (`OK (62 tests)`).
2) `cd tests && make -s`
   - Outcome: FAIL in this environment (missing module libraries: `-lstrategic -lships -lgui -ltactical -lweapons -ldefenses -lcore`).
   - Notes: failure is linker/environment dependency availability, not tactical test compilation logic.

Implementation/code commit:
- 856a8af

Branch:
- gui_sep-m8-subtask4-implementer-20260402
