Implementer Report

Plan step status:
- Preflight scope check: completed
- Implementation: completed
- Validation: completed
- Commit handling: code commit completed; artifact commit pending at report write time

Task goal:
- Extend tactical combat report summary contract with toggleable hit-detail output while preserving ship rollup behavior.

Allowed files changed:
- include/tactical/FTacticalCombatReport.h
- tests/tactical/FTacticalCombatReportTest.h
- tests/tactical/FTacticalCombatReportTest.cpp

Implementation summary:
- Added `FTacticalHitDetailSummary` model to carry attacker, weapon, target, hull damage, effects, outcome, and player-readable detail line.
- Added `showHitDetails` toggle flag (default `true`) and `hitDetails` collection to `FTacticalCombatReportSummary`.
- Updated summary builder to append hit details only for attacks where `hit == true`.
- Built hit-detail outcomes from existing attack + internal-event data, including damage/effect text.
- Kept existing ship rollup aggregation unchanged.
- Added tactical tests for default toggle behavior, hit-only detail emission, readable detail content, and no-hit behavior.

Validation commands run:
- cd tests/tactical && make && ./TacticalTests (baseline)
- cd tests/tactical && make && ./TacticalTests (post-change)

Validation outcomes:
- Pass: baseline TacticalTests OK.
- Pass: post-change TacticalTests OK (88 tests).

Implementation/code commit:
- 667f983
