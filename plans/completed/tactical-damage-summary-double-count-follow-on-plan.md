# Tactical Damage Summary Double Count Follow-On Plan

## Planner Agent Activation

- Requested agent: `planner`
- Repository-local definition found: no
- Shared definition found: yes
- Definition path used: `/home/tstephen/repos/agents/agents/planner.md`
- Precedence decision: shared `planner` definition in `~/repos/agents` applies because this repository does not define a repository-local `planner` override

Workflow obligations being followed:
- Stay in planning mode only and do not write implementation code.
- Decompose the deficiency into ordered, implementation-ready subtasks.
- Define observable acceptance criteria for each subtask.
- Include one Implementer Agent prompt per subtask.
- Write the final plan to a unique markdown file under `plans/`.

## Feature Restatement

Correct tactical damage summary rollups so player-facing hull totals equal the actual applied damage when an attack report carries both attack-level hull damage and effect-level/internal-event hull damage for the same resolution. Add deterministic tests that construct summaries from real attack-result shaped data, including hull-damage effects inside `internalEvents`, and verify no double counting occurs for weapon fire and mine damage.

## Confirmed Repository Facts

- Ship rollups are built in `include/tactical/FTacticalCombatReport.h` by `buildTacticalCombatReportSummary()`.
- That aggregation currently adds hull damage from `attack.hullDamage` and also adds `event.hullDamage` from `attack.internalEvents`, which can double count the same applied damage when both represent the same resolved hit.
- Attack reports are built from `FTacticalAttackResult` in `src/tactical/FBattleDisplay.cpp`.
- Mine damage attack reports are built from `FTacticalAttackResult` in `src/tactical/FBattleBoard.cpp`.
- Immediate non-weapon report events are also built from damage-resolution effects in `src/tactical/FBattleScreen.cpp`.
- Existing tactical tests already exercise the report model in `tests/tactical/FTacticalCombatReportTest.cpp`.
- Existing ship/weapon integration tests already create real `FTacticalAttackResult` data in `tests/ships/FTacticalAttackIntegrationTest.cpp`.

## Likely Files To Modify

- `include/tactical/FTacticalCombatReport.h`
- `src/tactical/FBattleDisplay.cpp`
- `src/tactical/FBattleBoard.cpp`
- Possibly `src/tactical/FBattleScreen.cpp` if immediate-event shaping needs to be aligned with the new aggregation contract
- `tests/tactical/FTacticalCombatReportTest.h`
- `tests/tactical/FTacticalCombatReportTest.cpp`
- Possibly a new tactical test file under `tests/tactical/` if separating aggregation regression coverage from existing report-model tests is cleaner
- `tests/tactical/Makefile`

## Assumptions

- The defect is in player-facing summary aggregation, not in underlying combat resolution or ship HP application.
- `attack.hullDamage` should remain the canonical attack-level total for hit rollups unless the implementer finds repository evidence that effect-level hull entries are intended to replace it.
- `internalEvents` should still preserve raw hull-damage effect data for diagnostics and future UI expansion, but those event records should not independently increment displayed hull totals when they describe damage already represented by the parent attack.
- Mine damage summaries should follow the same invariant as weapon fire summaries: displayed hull damage must equal actual applied hull damage exactly once.

## Subtasks

### 1. Define and document the no-double-count aggregation contract

Inspect the current report model and explicitly choose how attack-level hull totals and nested effect/event hull totals interact in ship rollups. The implementation should establish one canonical source for displayed hull totals and a clear rule for when `internalEvents` contribute hull damage independently versus when they are informational detail attached to an already-counted attack.

Acceptance criteria:
- The chosen aggregation rule is encoded in `buildTacticalCombatReportSummary()` and is understandable from the code structure.
- A hit represented by `attack.hullDamage > 0` and a matching hull-damage `internalEvent` produces a ship summary whose `hullDamageTaken` equals the actual applied hull damage once, not twice.
- The summary still preserves raw `internalEvents` for later inspection even when their hull damage is excluded from the player-facing total.
- Immediate report-level events in `report.events` continue to count toward ship hull totals when they represent standalone damage outside an already-counted attack.

### 2. Align attack and mine report construction with the aggregation contract

Review the report-building helpers in `src/tactical/FBattleDisplay.cpp` and `src/tactical/FBattleBoard.cpp` and make any shaping changes needed so attack reports remain internally consistent with the chosen contract. This may include tagging or structuring event data so the aggregator can distinguish “already represented by parent attack hull total” from “standalone hull-damage event that must count separately”.

Acceptance criteria:
- Weapon-fire attack reports built from `FTacticalAttackResult` are consistent with the new aggregation rule.
- Mine-damage attack reports built from `FTacticalAttackResult` are consistent with the new aggregation rule.
- Non-hull effects inside `internalEvents` continue to appear in per-ship effect summaries.
- If hull-damage events remain inside `internalEvents`, the aggregation path has enough information to avoid recounting them in ship totals.

### 3. Add deterministic regression tests using real attack-result shaped data

Add tactical tests that build summaries from realistic attack-result shaped report data rather than only synthetic summary-only fixtures. Cover both weapon-fire and mine-damage paths, and include cases where `internalEvents` contain hull-damage effects alongside parent `attack.hullDamage`.

Acceptance criteria:
- There is deterministic coverage for a weapon-fire report where `attack.hullDamage` and an `internalEvent.hullDamage` describe the same applied hit, and the summary total matches the actual applied damage once.
- There is deterministic coverage for a mine-damage report with the same shape and the same no-double-count invariant.
- At least one test also verifies non-hull internal effects still contribute to effect summaries while hull totals remain correct.
- The tests are derived from real attack-result shaped data structures used by the tactical reporting flow, not from GUI-only assertions.
- Tactical tests are wired into the existing `tests/tactical/Makefile`.

### 4. Add guardrail coverage for standalone immediate events versus nested attack effects

Add one targeted regression test that distinguishes standalone immediate damage events in `report.events` from nested `attack.internalEvents`, so future changes do not accidentally stop counting legitimate non-attack hull damage such as electrical fire or other report-level effects.

Acceptance criteria:
- A standalone report-level event with `event.hullDamage > 0` still increments `hullDamageTaken`.
- A nested hull-damage `internalEvent` attached to an attack with the same applied hull total does not increment `hullDamageTaken` a second time.
- The tests make the difference between standalone immediate events and nested attack detail explicit.

## Dependency Ordering

1. Subtask 1 must complete first because the implementation needs a single aggregation contract before changing builders or tests.
2. Subtask 2 depends on Subtask 1 because report construction should match the selected aggregation semantics.
3. Subtask 3 depends on Subtasks 1 and 2 because regression tests should lock the final intended shape, not the current defective behavior.
4. Subtask 4 depends on Subtask 1 and can be implemented alongside or immediately after Subtask 3.

## Parallelization Notes

- Subtask 4 can run in parallel with late work in Subtask 3 once the aggregation contract from Subtask 1 is settled.
- Subtasks 1 and 2 are on the critical path because they determine what the deterministic tests must assert.

## Implementer Agent Prompts

### Prompt 1: Aggregation contract

You are the implementer agent

Allowed files:
- `include/tactical/FTacticalCombatReport.h`

Task:
Update tactical combat summary aggregation so player-facing ship hull rollups cannot double count the same applied hull damage when an attack carries both `attack.hullDamage` and hull-damage entries in `attack.internalEvents`. Preserve raw event data for diagnostics and future UI use.

Acceptance criteria:
- A hit represented by both attack-level hull damage and matching nested hull-damage events contributes to `hullDamageTaken` exactly once.
- Standalone report-level events in `report.events` still contribute their hull damage to summaries.
- Non-hull nested effects still contribute to effect rollups.

### Prompt 2: Report builder alignment

You are the implementer agent

Allowed files:
- `src/tactical/FBattleDisplay.cpp`
- `src/tactical/FBattleBoard.cpp`
- `src/tactical/FBattleScreen.cpp`
- `include/tactical/FTacticalCombatReport.h`

Task:
Align weapon-fire, mine-damage, and any relevant immediate-event report construction with the new aggregation contract so the summary builder can distinguish attack totals from nested detail and avoid hull double counting.

Acceptance criteria:
- Weapon-fire and mine-damage report builders emit data consistent with the no-double-count summary rule.
- Raw internal event detail is still preserved.
- Non-hull effect reporting remains intact.

### Prompt 3: Deterministic regression coverage

You are the implementer agent

Allowed files:
- `tests/tactical/FTacticalCombatReportTest.h`
- `tests/tactical/FTacticalCombatReportTest.cpp`
- Any new tactical test file under `tests/tactical/`
- `tests/tactical/Makefile`

Task:
Add deterministic tactical regression tests that build summaries from real attack-result shaped data for weapon fire and mine damage, including hull-damage effects inside `internalEvents`, and assert displayed hull totals equal actual applied damage without double counting.

Acceptance criteria:
- Weapon-fire and mine-damage summary regressions are both covered.
- At least one test uses a parent attack total plus nested hull-damage event data and proves only one hull total is displayed.
- At least one test confirms non-hull effects still surface in the summary.
- Tests are wired into the tactical test build.

### Prompt 4: Standalone-event guardrail

You are the implementer agent

Allowed files:
- `tests/tactical/FTacticalCombatReportTest.h`
- `tests/tactical/FTacticalCombatReportTest.cpp`
- Any new tactical test file under `tests/tactical/`
- `tests/tactical/Makefile`

Task:
Add a focused regression test that distinguishes standalone report-level damage events from nested attack internal events so future changes do not break legitimate immediate-event hull accounting while fixing nested attack double counting.

Acceptance criteria:
- Standalone `report.events` hull damage still counts once.
- Nested attack internal-event hull damage does not recount already-applied attack hull damage.
- The test clearly documents the intended difference between the two cases.

## Output Artifact

- `/home/tstephen/repos/SSW/plans/tactical-damage-summary-double-count-follow-on-plan.md`
