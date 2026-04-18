Subtask: TACTICAL-FAIL-001
Branch: tactical-runner-failing-tests-remediation-tactical-fail-001-implementer-20260418
Implementation commit: 572b8459bd6c09d0d95ce65691a981de69f6ec57

Plan step status:
1. Preflight scope check: completed.
2. Implement incrementally: completed.
3. Validate after each change: completed (with pre-existing unrelated tactical runner failures).
4. Completion gate: completed.

Files changed:
- include/tactical/FTacticalCombatReport.h
- tests/tactical/FTacticalCombatReportTest.cpp

Implementation summary:
- Added standalone-event hull-counting guard so report-level TDET_HullDamage does not add player-facing hull totals when the same ship already has canonical attack-level hull damage.
- Kept raw report event capture unchanged (rawEvents/internalEvents tracking preserved).
- Updated affected tactical summary assertions to reflect canonical hull total of 4 and consistent internal event count.

Validation commands run:
- cd tests && make tactical-tests && ./tactical/TacticalTests
  - Baseline before changes: Run 131, Failures 11.
- cd tests && make tactical-tests && ./tactical/TacticalTests
  - After first change: Run 131, Failures 11 (targeted hull-total failure resolved; next assertion in same test surfaced).
- cd tests/tactical && make && ./TacticalTests
  - Final after expectation alignment: Run 131, Failures 10.

Validation outcome:
- Targeted behavior now matches acceptance criteria: canonical hull total remains attack-level (4), no double-count from standalone TDET_HullDamage in the exposed scenario, non-hull summary lines remain present, and raw events remain recorded.
- Remaining 10 failures are pre-existing tactical/gui source-contract failures outside allowed file scope for this subtask.
