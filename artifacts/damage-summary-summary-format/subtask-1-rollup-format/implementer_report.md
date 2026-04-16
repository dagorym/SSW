Implementer report: Subtask 1 (ship-summary rollup format)

Plan step status:
- Preflight scope check: done (task/allowed files/acceptance criteria/validation command confirmed)
- Implement incrementally: done
- Validate after change: done
- Completion gate checks: done

Files changed:
- include/tactical/FTacticalCombatReport.h
- tests/tactical/FTacticalCombatReportTest.cpp

Implementation summary:
- Refactored tactical ship-summary rollup formatting to produce model-owned multi-line display output per ship.
- Each damaged ship now emits a header line `<Ship Name>:` followed by bullet lines (` - ...`) for hull damage and grouped effect summaries.
- Preserved existing aggregation behavior for hull totals with attack counts, grouped weapon hits, grouped defense hits, and distinct non-hull effects.
- Updated tactical model tests to lock the multi-line contract and prevent regression to `effects:` flat-line formatting.

Validation commands run:
- cd tests/tactical && make && ./TacticalTests  (baseline before edits)
- cd tests/tactical && make && ./TacticalTests  (post-change)

Validation outcomes:
- PASS: TacticalTests (92 tests)
- No expected failing validations remain.
