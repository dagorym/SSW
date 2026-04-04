Implementation Summary

- Goal: restore clean top-level test build/link flow for `tests/SSWTests` with minimal scope.
- Resolved missing app/model libs during test build by adding explicit model dependency builds in `tests/Makefile`.
- Resolved tactical registration drift by removing stale `FTacticalCombatReportTest` registration from `tests/SSWTests.cpp` so top-level runner matches current tactical runner wiring.

Files Changed

- tests/Makefile
- tests/SSWTests.cpp

Validation Commands and Outcomes

1. `cd /home/tstephen/.copilot/session-state/b31e2a83-8edf-4f07-b4f4-7913ac679318/files/worktrees/gui_sep-m10-remed-implementer-20260404 && make all_clean >/dev/null && make -s && cd tests && make -s && cd tactical && make -s && ./TacticalTests`
   - Result: PASS (command exited 0; tactical runner reported `OK (74 tests)`).

Implementation Commit

- `975f1e5` — Fix test build ordering and tactical registration drift
