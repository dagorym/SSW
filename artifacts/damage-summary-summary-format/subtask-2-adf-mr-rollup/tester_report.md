### Test Execution Report

**Attempt:** 1/3  
**Total Tests Written:** 0  
**Total Tests Run:** 92  
**Tests Passed:** 92  
**Tests Failed:** 0

Scope validated:
- ADF loss is represented with exact totaled point loss in ship summary rollups.
- MR loss is represented with exact totaled point loss in ship summary rollups.
- ADF/MR bullets coexist with hull-damage bullets, grouped defense-hit bullets, and other effect bullets.
- Tactical model coverage includes a ship-summary case validating ADF/MR rollup behavior.

Evidence:
- Reviewed `include/tactical/FTacticalCombatReport.h` for structured numeric fields (`amount`, `previousValue`, `newValue`) and ADF/MR rollup emission (` - ADF (-N)`, ` - MR (-N)`).
- Reviewed `tests/tactical/FTacticalCombatReportTest.cpp` (`testBuildTacticalCombatReportSummarySummarizesHullDamageAndEffects`) asserting ` - ADF (-3)`, ` - MR (-1)`, coexistence with hull/weapon/electrical-fire lines, and suppression of label-only ADF/MR lines.

Commands executed:
1. `cd tests/tactical && make && ./TacticalTests`
2. `cd tests/tactical && ./TacticalTests | tail -n 40`

Result:
- PASS. TacticalTests reported `OK (92 tests)`.
- No tester-side test file edits were required; existing implementer-added regression coverage satisfies acceptance criteria.

Commit status:
- Test commit: No Changes Made

Temporary byproducts cleanup:
- No repository byproducts created by tester workflow.
