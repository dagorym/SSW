# Requested agent: `planner`

- Repository-local definition found: no repository-local `planner` definition was identified in this repository.
- Shared definition found: `/home/tstephen/repos/agents/agents/planner.md` and `/home/tstephen/repos/agents/agents/planner.yaml`
- Definition path used: `/home/tstephen/repos/agents/agents/planner.md`
- Precedence decision: shared `planner` definition applies because this repository does not define a repository-local `planner` override.

## Confirmed Repository Facts

- The tactical standalone runner currently fails with `Run: 131, Failures: 11, Errors: 0` after the three previously unregistered suites were added to `tests/tactical/TacticalTests.cpp`.
- Two failures are current behavior mismatches:
  - `FTacticalCombatReportTest::testBuildTacticalCombatReportSummarySummarizesHullDamageAndEffects`
  - `FTacticalDamageSummaryGUITest::testDamageSummaryDialogBuildsShipRollupAndOptionalHitDetailSections`
- The remaining failures are stale source-inspection tests that no longer match the current architecture after report-lifecycle ownership moved into `FTacticalGame` and `FBattleScreen` began owning/installing `WXTacticalUI`.
- `FBattleScreen` currently forwards tactical-report lifecycle calls into `FTacticalGame`, and `FTacticalGame` owns `m_tacticalReport`.
- Parent-backed and parentless `WXTacticalUI` modal behavior is already exercised in `tests/gui/TacticalGuiLiveTest.cpp`, making it a better long-term home for behavior-driven UI coverage than tactical source-token inspection.

## Assumptions

- The remediation should preserve the current architectural split:
  - `FTacticalGame` owns tactical report state and normalization behavior.
  - `FBattleScreen` exposes the public forwarding surface.
  - `WXTacticalUI` remains the wx-backed tactical adapter used by `FBattleScreen`.
- Moving wx-modal behavior tests from `tests/tactical` to `tests/gui` is acceptable when the behavior requires a live wx harness to test correctly.

## Files To Modify

- `include/tactical/FTacticalCombatReport.h`
- `tests/tactical/FTacticalCombatReportTest.h`
- `tests/tactical/FTacticalCombatReportTest.cpp`
- `tests/tactical/FTacticalDamageSummaryGUITest.h`
- `tests/tactical/FTacticalDamageSummaryGUITest.cpp`
- `tests/tactical/WXTacticalUIAdapterTest.h`
- `tests/tactical/WXTacticalUIAdapterTest.cpp`
- `tests/tactical/TacticalTests.cpp`
- `tests/tactical/Makefile`
- `tests/gui/TacticalGuiLiveTest.h`
- `tests/gui/TacticalGuiLiveTest.cpp`
- `tests/gui/GuiTests.cpp`
- `tests/gui/Makefile`
- `AGENTS.md`

## Overall Documentation Impact

- Update `AGENTS.md` tactical/gui testing notes so they no longer imply that the tactical standalone runner still relies on stale source-inspection expectations for report-lifecycle ownership or pre-rewire adapter behavior.
- Document that behavioral tactical report API coverage stays in `tests/tactical`, while live tactical modal/adapter behavior belongs in `tests/gui`.

## Subtasks

### TACTICAL-FAIL-001

#### Description

Correct the tactical report summary aggregation so standalone `TDET_HullDamage` report events do not inflate the player-facing hull total when attack-level hull damage is already the canonical total for the attacked ship.

#### Allowed Files

- `include/tactical/FTacticalCombatReport.h`
- `tests/tactical/FTacticalCombatReportTest.cpp`

#### Acceptance Criteria

- `buildTacticalCombatReportSummary(...)` keeps attack-level hull damage canonical for the attacked ship when a standalone `TDET_HullDamage` event represents the same resolved hit.
- The failing scenario from `FTacticalCombatReportTest::testBuildTacticalCombatReportSummarySummarizesHullDamageAndEffects` produces `hullDamageTaken == 4` and a display line of ` - 4 hull damage from 1 attack`.
- Existing intended behaviors continue to hold:
  - nested duplicate hull-damage events for the attack target remain suppressed from player-facing totals
  - nested hull-damage events for other ships still count
  - standalone non-hull effects such as ADF, MR, defense hits, and electrical fire still appear in rollups
  - raw event storage remains available for detail/debugging use

#### Documentation Impact

- `AGENTS.md` may need a small update if its tactical-report summary description currently implies behavior that differs from the corrected canonical-damage rule.

#### Implementer Agent Prompt

You are the implementer agent.

Allowed files:
- `include/tactical/FTacticalCombatReport.h`
- `tests/tactical/FTacticalCombatReportTest.cpp`

Task:
- Fix tactical report summary aggregation so standalone `TDET_HullDamage` report events do not double-count player-facing hull totals when attack-level hull damage is already the canonical total for the attacked ship.
- Update the affected tactical test expectations so the scenario is internally consistent with that rule.

Acceptance criteria:
- The canonical hull total for the target ship remains the attack-level hull damage in the exposed failing scenario.
- The corrected summary for that scenario reports `4` hull damage, not `5`.
- Non-hull standalone effects remain summarized correctly.
- Existing raw report data remains preserved.

Validation guidance:
- Primary command: `cd tests && make tactical-tests && ./tactical/TacticalTests`
- If iteration is needed, likely focused command: `cd tests/tactical && make && ./TacticalTests`

Tester test-file location guidance:
- Primary validation stays in `tests/tactical/FTacticalCombatReportTest.cpp`

Artifact-directory guidance:
- Use repository-root-relative artifacts under `artifacts/tactical-runner-failing-tests-remediation/TACTICAL-FAIL-001`

Begin implementation immediately when the required blocking inputs are present; do not stop after preflight restatement alone.

Do not report success unless all required artifacts exist and all changes are committed.

### TACTICAL-FAIL-002

#### Description

Replace stale tactical source-inspection tests for report lifecycle/normalization with behavioral API tests against `FBattleScreen` and `FTacticalGame` public behavior, and correct the damage-summary hit-detail expectations so hit details only reflect per-attack internal events.

#### Allowed Files

- `tests/tactical/FTacticalCombatReportTest.h`
- `tests/tactical/FTacticalCombatReportTest.cpp`
- `tests/tactical/FTacticalDamageSummaryGUITest.h`
- `tests/tactical/FTacticalDamageSummaryGUITest.cpp`
- `tests/tactical/TacticalTests.cpp`
- `tests/tactical/Makefile`

#### Acceptance Criteria

- The following stale source-inspection expectations are removed or rewritten as behavioral tests:
  - direct `m_tacticalReport` ownership in `FBattleScreen`
  - direct summary-builder logic in `FBattleScreen`
  - direct normalization logic text in `FBattleScreen`
  - direct standalone-event normalization code text in `FBattleScreen`
- Behavioral/API coverage proves that:
  - `FBattleScreen` public tactical-report lifecycle methods still work end-to-end
  - appended attacks have normalized internal events with the expected `attackIndex`, `subject`, `source`, `target`, and `immediate` values
  - appended standalone report events are stored with `attackIndex == -1`
- `FTacticalDamageSummaryGUITest` no longer expects hit-detail text to include standalone `report.events`; hit-detail expectations are limited to per-attack internal events only.
- The tactical standalone runner no longer fails because of stale source-token assertions for these report-lifecycle behaviors.

#### Documentation Impact

- No documentation update is expected unless `AGENTS.md` tactical test notes explicitly describe the removed source-inspection tests as current repository policy.

#### Implementer Agent Prompt

You are the implementer agent.

Allowed files:
- `tests/tactical/FTacticalCombatReportTest.h`
- `tests/tactical/FTacticalCombatReportTest.cpp`
- `tests/tactical/FTacticalDamageSummaryGUITest.h`
- `tests/tactical/FTacticalDamageSummaryGUITest.cpp`
- `tests/tactical/TacticalTests.cpp`
- `tests/tactical/Makefile`

Task:
- Rewrite the stale tactical source-inspection tests around report lifecycle and normalization into behavioral/API tests that exercise current `FBattleScreen` and `FTacticalGame` behavior.
- Update the tactical damage-summary test so hit-detail expectations only cover per-attack internal events.
- Remove any remaining source-inspection tactical checks that cannot be converted into durable behavioral tests.

Acceptance criteria:
- Behavioral tests cover the report-lifecycle and normalization semantics through public APIs instead of source-token inspection.
- Hit-detail expectations align with the chosen rule that only per-attack internal events appear in hit details.
- Tactical source-inspection assertions that are architecture-coupled and non-convertible are removed.

Validation guidance:
- Primary command: `cd tests && make tactical-tests && ./tactical/TacticalTests`

Tester test-file location guidance:
- Tactical regression coverage remains in:
  - `tests/tactical/FTacticalCombatReportTest.cpp`
  - `tests/tactical/FTacticalDamageSummaryGUITest.cpp`

Artifact-directory guidance:
- Use repository-root-relative artifacts under `artifacts/tactical-runner-failing-tests-remediation/TACTICAL-FAIL-002`

Begin implementation immediately when the required blocking inputs are present; do not stop after preflight restatement alone.

Do not report success unless all required artifacts exist and all changes are committed.

### TACTICAL-FAIL-003

#### Description

Move stale `WXTacticalUIAdapterTest` source-inspection coverage into durable GUI behavior tests under the wx harness, remove milestone-era non-rewire assertions that are no longer valid, and keep the tactical suite focused on non-modal API/model behavior.

#### Allowed Files

- `tests/tactical/WXTacticalUIAdapterTest.h`
- `tests/tactical/WXTacticalUIAdapterTest.cpp`
- `tests/tactical/TacticalTests.cpp`
- `tests/tactical/Makefile`
- `tests/gui/TacticalGuiLiveTest.h`
- `tests/gui/TacticalGuiLiveTest.cpp`
- `tests/gui/GuiTests.cpp`
- `tests/gui/Makefile`
- `AGENTS.md`

#### Acceptance Criteria

- Source-token checks in `WXTacticalUIAdapterTest` that depend on exact implementation text or milestone-era architecture are removed or replaced.
- Live GUI behavior coverage under `tests/gui/TacticalGuiLiveTest.cpp` proves:
  - parent-backed `showMessage(...)` opens/dismisses correctly
  - `showDamageSummary(...)` works for parent-backed and parentless cases
  - `runICMSelection(...)` preserves the current guard behavior for invalid inputs
  - `notifyWinner(...)` still exercises the winner-modal path correctly
- The invalid old expectation that runtime tactical callers must not reference `WXTacticalUI` is removed.
- Any remaining tactical-side adapter coverage is limited to stable API shape or compile-surface checks that do not depend on implementation tokens.
- The test runners and makefiles are updated so moved coverage executes from `GuiTests` rather than remaining in the tactical standalone runner.

#### Documentation Impact

- Update `AGENTS.md` to reflect that adapter/modal behavior lives in GUI behavioral coverage and that stale non-rewire inspection assertions were retired.

#### Implementer Agent Prompt

You are the implementer agent.

Allowed files:
- `tests/tactical/WXTacticalUIAdapterTest.h`
- `tests/tactical/WXTacticalUIAdapterTest.cpp`
- `tests/tactical/TacticalTests.cpp`
- `tests/tactical/Makefile`
- `tests/gui/TacticalGuiLiveTest.h`
- `tests/gui/TacticalGuiLiveTest.cpp`
- `tests/gui/GuiTests.cpp`
- `tests/gui/Makefile`
- `AGENTS.md`

Task:
- Replace the stale `WXTacticalUIAdapterTest` source-inspection assertions with durable live GUI behavior coverage in `tests/gui`.
- Remove the invalid milestone-era assumption that runtime tactical callers must remain unre-wired to `WXTacticalUI`.
- Keep only stable tactical-side adapter checks that still make sense without inspecting implementation tokens.

Acceptance criteria:
- GUI behavior tests cover the current `WXTacticalUI` modal and winner paths.
- The stale runtime-non-rewire tactical assertion is removed.
- Tactical runner failures caused by adapter source-token expectations are eliminated.
- Runner wiring and makefiles execute the moved coverage in `GuiTests`.

Validation guidance:
- Primary GUI command: `cd tests/gui && make && xvfb-run -a ./GuiTests`
- Integration check after wiring: `cd tests && make gui-tests`
- Optional final combined check: `cd tests && make tactical-tests gui-tests && ./tactical/TacticalTests`

Tester test-file location guidance:
- GUI behavior coverage belongs in `tests/gui/TacticalGuiLiveTest.cpp`
- Tactical-side residual adapter coverage, if any, remains in `tests/tactical/WXTacticalUIAdapterTest.cpp`

Artifact-directory guidance:
- Use repository-root-relative artifacts under `artifacts/tactical-runner-failing-tests-remediation/TACTICAL-FAIL-003`

Begin implementation immediately when the required blocking inputs are present; do not stop after preflight restatement alone.

Do not report success unless all required artifacts exist and all changes are committed.

## Dependency Ordering

1. `TACTICAL-FAIL-001` must run first.
   - It resolves the real code defect in hull-damage aggregation and stabilizes the intended report contract before broader test rewrites lock in behavior.

2. `TACTICAL-FAIL-002` runs second.
   - It depends on the corrected report-summary behavior from `TACTICAL-FAIL-001` so the rewritten behavioral tests assert the intended canonical hull-damage rule.

3. `TACTICAL-FAIL-003` runs third.
   - It should follow the tactical report/test cleanup so the GUI migration can rely on the finalized behavior decisions and avoid duplicating transitional assertions.

## Parallelization Guidance

- Do not run these subtasks in parallel.
- `TACTICAL-FAIL-002` and `TACTICAL-FAIL-003` both touch overlapping tactical test intent and runner boundaries, and they depend on the behavior decisions finalized in `TACTICAL-FAIL-001`.

## Output Artifact Path

- `plans/tactical-runner-failing-tests-remediation-plan.md`
