Implementation Report

Plan step status:
1. Preflight scope check - completed (goal, allowed files, acceptance criteria, validation command confirmed).
2. Implement incrementally - completed.
3. Validate after changes - completed.
4. Completion gate - in progress (artifacts written and committed in follow-up commit).

Task goal:
- Preserve defense-specific metadata through tactical report events so summary formatting can identify concrete damaged defenses.

Allowed files modified:
- include/tactical/FTacticalCombatReport.h
- src/tactical/FTacticalGame.cpp
- tests/tactical/FTacticalCombatReportTest.cpp

Summary of changes:
- Extended `FTacticalReportEvent` with defense metadata fields (`damagedDefenseType`, `damagedDefenseName`) and default initialization.
- Updated `appendTacticalDamageResolutionEvents(...)` to copy defense metadata from `FTacticalDamageEffect` into report events.
- Updated `buildTacticalAttackEvent(...)` to copy defense metadata from `FTacticalAttackEffect` into report events.
- Extended tactical regression coverage to assert defense metadata propagation in model/report events and source-guard checks for both event-construction paths.

Validation commands run:
- Baseline: `cd tests/tactical && make && ./TacticalTests` (passed in baseline run).
- Post-change initial run: `cd tests/tactical && make && ./TacticalTests` (segfault due stale incremental objects after header layout change).
- Post-change clean rebuild: `cd tests/tactical && make clean && make && ./TacticalTests` (passed: OK (92 tests)).

Validation outcome:
- Final validation passed with clean rebuild.

Implementation/code commit hash:
- f035e43
