Implementer report - Subtask 2 remediation (ADF/MR rollup)

Plan step status:
- Preflight scope check: completed (agent protocol, assigned worktree/branch, verifier findings, plan AC, validation command)
- Incremental implementation: completed
- Validation: completed
- Completion gate: pending artifact commit

Files changed:
- src/tactical/FTacticalGame.cpp
- tests/tactical/FTacticalCombatReportTest.cpp

Implementation summary:
- Fixed runtime tactical event conversion to copy numeric payload fields (`previousValue`, `newValue`, `amount`) from structured damage/effect data into `FTacticalReportEvent` in both conversion paths:
  - `appendTacticalDamageResolutionEvents(...)`
  - `buildTacticalAttackEvent(...)`
- Extended tactical model source-contract regression assertions to lock those assignments so ADF/MR total rollups continue receiving exact numeric deltas from live conversion paths.

Validation commands run:
1. `cd tests/tactical && make && ./TacticalTests` (baseline before edits) -> PASS (92 tests)
2. `cd tests/tactical && make && ./TacticalTests` (after edits) -> FAIL (segmentation fault due stale tactical object/header ABI mismatch in incremental build)
3. `cd src/tactical && make clean && make` -> PASS
4. `cd tests/tactical && make clean && make && ./TacticalTests` -> PASS (92 tests)

Validation outcome:
- PASS after clean rebuild confirmed runtime and regression coverage behavior.
- No remaining known failures.
