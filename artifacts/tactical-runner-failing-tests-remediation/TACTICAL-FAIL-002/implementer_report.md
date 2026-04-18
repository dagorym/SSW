Plan step status:
- Preflight scope check: completed (task goal, allowed files, acceptance criteria, and primary validation command confirmed).
- Implement incrementally: completed.
- Validate after each change: completed.
- Completion gate: completed with code commit and artifact generation.

Task summary:
- Replaced stale tactical report lifecycle/normalization source-inspection assertions with behavioral API coverage using FTacticalGame lifecycle methods used by FBattleScreen.
- Updated tactical damage-summary coverage to assert hit-details from per-attack internal events only.
- Removed non-convertible tactical source-inspection checks in the touched tactical report/damage-summary test fixtures.

Files changed (code commit):
- tests/tactical/FTacticalCombatReportTest.h
- tests/tactical/FTacticalCombatReportTest.cpp
- tests/tactical/FTacticalDamageSummaryGUITest.h
- tests/tactical/FTacticalDamageSummaryGUITest.cpp

Validation commands run:
1) cd tests && make tactical-tests && ./tactical/TacticalTests
   - Baseline before changes: failed (10 failures total).
   - After implementation: failed (4 failures total), all in pre-existing WXTacticalUIAdapterTest:
     - testShowMessageMatchesRepositoryInformationalPattern
     - testDamageSummaryAndICMSelectionWrapDialogsWithGuards
     - testNotifyWinnerPreservesBattleScreenWinnerMessageSemantics
     - testRuntimeTacticalCallersNotRewiredToAdapter

Validation outcome:
- Acceptance criteria for TACTICAL-FAIL-002 implemented and related failures resolved.
- Remaining failures are existing unrelated tactical adapter source-inspection checks outside allowed-file scope.
