# Tactical Unused Test Runner Registration Plan

## Request Restatement

Investigate why `WXTacticalUIAdapterTest.cpp`, `FTacticalCombatReportTest.cpp`, and `FTacticalDamageSummaryGUITest.cpp` show `0%` coverage after running the test suite, verify whether they are compiled and executed, and fix the tactical test runner wiring only.

## Confirmed Repository Facts

- `tests/tactical/Makefile` compiles every `*.cpp` file in `tests/tactical` into the tactical test library and runner.
- The three named files already have corresponding `.o`, `.gcno`, and `.gcda` files in `tests/tactical`, which is consistent with being compiled under coverage instrumentation.
- `tests/tactical/TacticalTests.cpp` does not include or register `WXTacticalUIAdapterTest`, `FTacticalCombatReportTest`, or `FTacticalDamageSummaryGUITest`.
- `tests/SSWTests.cpp` also does not register those three suites, but per the selected scope this plan does not expand the top-level runner.

## Assumptions

- The intended ownership boundary is that these three fixtures belong in the tactical module runner rather than the top-level `SSWTests` runner.
- Restoring tactical-runner registration is sufficient to verify that the coverage issue is not a reporting fluke.

## Files To Modify

- `tests/tactical/TacticalTests.cpp`

## Overall Documentation Impact

No user-facing or design documentation update is expected. If the repository keeps contributor notes about tactical test-runner coverage or fixture registration, that documentation may be reviewed, but no documentation change is currently required by the scoped fix.

## Subtasks

### TACTICAL-RUNNER-001: Register the three missing tactical suites in the tactical runner

Add the missing includes and `runner.addTest(...)` calls for:

- `WXTacticalUIAdapterTest`
- `FTacticalCombatReportTest`
- `FTacticalDamageSummaryGUITest`

Keep the change limited to the tactical module runner so the three fixtures are compiled, linked, and actually executed by `tests/tactical/TacticalTests`.

#### Acceptance Criteria

- `tests/tactical/TacticalTests.cpp` includes the headers for all three missing fixtures.
- `tests/tactical/TacticalTests.cpp` adds all three suites to the `CppUnit::TextUi::TestRunner`.
- Running the tactical module runner executes those suites instead of leaving them only compiled but unregistered.
- The investigation outcome is confirmed as “compiled before the fix, not executed before the fix.”

#### Documentation Impact

No documentation update expected.

## Dependency Ordering

1. `TACTICAL-RUNNER-001`

Parallelization:
None. This is a single-file, single-responsibility change.

## Implementer Agent Prompt

### Prompt for `TACTICAL-RUNNER-001`

You are the implementer agent

Allowed files:

- `tests/tactical/TacticalTests.cpp`

Task:

Investigate and fix the tactical test-runner wiring so that the existing fixtures `WXTacticalUIAdapterTest`, `FTacticalCombatReportTest`, and `FTacticalDamageSummaryGUITest` are actually executed by the tactical module runner. Repository evidence already shows these files are compiled by `tests/tactical/Makefile`; the missing piece is suite registration in `tests/tactical/TacticalTests.cpp`. Keep the scope limited to the tactical runner only and do not expand the top-level `tests/SSWTests.cpp` runner in this task.

Acceptance criteria:

- `tests/tactical/TacticalTests.cpp` includes `WXTacticalUIAdapterTest.h`, `FTacticalCombatReportTest.h`, and `FTacticalDamageSummaryGUITest.h`.
- `tests/tactical/TacticalTests.cpp` registers `WXTacticalUIAdapterTest::suite()`, `FTacticalCombatReportTest::suite()`, and `FTacticalDamageSummaryGUITest::suite()` with the tactical `CppUnit` runner.
- The tactical runner can be built and run with those suites enabled.
- The final implementation notes clearly distinguish the pre-fix state (“compiled but not registered/executed”) from the post-fix state.

Validation guidance:

- Primary validation command: `cd tests && make tactical-tests && ./tactical/TacticalTests`
- Optional narrower validation command: `cd tests/tactical && make && ./TacticalTests`

Tester handoff location guidance:

- Expected fixture locations remain:
  - `tests/tactical/WXTacticalUIAdapterTest.cpp`
  - `tests/tactical/FTacticalCombatReportTest.cpp`
  - `tests/tactical/FTacticalDamageSummaryGUITest.cpp`
- Runner wiring under test:
  - `tests/tactical/TacticalTests.cpp`

Artifact directory guidance:

- Use repository-root-relative artifact directory `artifacts/tactical-unused-test-runner-registration`

Execution behavior:

- Begin implementation immediately when the prompt contains the required blocking inputs, and do not stop after preflight restatement alone.

Completion gate:

- Do not report success unless all required artifacts exist and all changes are committed.

## Output Artifact Path

- `plans/tactical-unused-test-runner-registration-plan.md`
