Implementer report - Subtask 2 (ADF/MR rollup)

Plan step status:
- Preflight scope check: completed (task, allowed files, AC, validation command confirmed)
- Incremental implementation: completed
- Validation: completed
- Completion gate: code commit + artifact commit completed

Files changed:
- include/tactical/FTacticalCombatReport.h
- tests/tactical/FTacticalCombatReportTest.cpp

Implementation summary:
- Extended tactical report event data with numeric meter fields (`previousValue`, `newValue`, `amount`) so summary rollups can read point-loss values from structured event data.
- Extended summary accumulator with `adfLossTotal` and `mrLossTotal`.
- Updated rollup aggregation to sum ADF/MR loss totals from event numeric fields (preferring `amount`, fallback to `previousValue - newValue`).
- Added dedicated ship-summary bullets ` - ADF (-N)` and ` - MR (-N)`.
- Updated tactical model regression coverage in `FTacticalCombatReportTest` to validate aggregated ADF/MR bullets alongside hull damage, grouped weapon hits, grouped defense hits, and other effects.

Validation commands run:
1. `cd tests/tactical && make && ./TacticalTests` (baseline before edits) -> PASS (92 tests)
2. `cd tests/tactical && make && ./TacticalTests` (after edits) -> PASS (92 tests)

Validation outcome:
- PASS. No remaining known failures.
