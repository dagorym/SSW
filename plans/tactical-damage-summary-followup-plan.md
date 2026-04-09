# Tactical Damage Summary Follow-Up Plan

## Feature Restatement

Adjust the Tactical Damage Summary report formatting so the dialog shows:

1. Actual defense names in the ship-summary effects list instead of a generic defense-hit count.
2. Hit-detail lines without the redundant trailing `"(Attack hit Target)"` text.
3. Ship-summary lines that omit `0 hull damage` when a ship only suffered non-hull effects.
4. Ship-summary effects that do not repeat `Hull Damage xN` when the hull-damage count is already expressed as `N hull damage from N attacks`.

## Confirmed Repository Facts

- The Tactical Damage Summary dialog renders prebuilt summary strings from [`include/tactical/FTacticalCombatReport.h`](/home/tstephen/repos/SSW/include/tactical/FTacticalCombatReport.h) and does not independently format the ship rollup or hit-detail lines.
- [`src/gui/TacticalDamageSummaryGUI.cpp`](/home/tstephen/repos/SSW/src/gui/TacticalDamageSummaryGUI.cpp) displays `shipSummary.displayLines` and `hitDetails[i].displayLine` verbatim.
- Ship-summary and hit-detail formatting currently lives in inline helpers in [`include/tactical/FTacticalCombatReport.h`](/home/tstephen/repos/SSW/include/tactical/FTacticalCombatReport.h), including `appendEffectSummary(...)`, `buildShipSummaryDisplayLine(...)`, and `buildHitDetailSummary(...)`.
- Defense names already exist on low-level damage effects as `FTacticalDamageEffect::defenseName` and `FTacticalAttackEffect::defenseName`.
- Tactical report events currently preserve weapon-specific metadata but do not appear to preserve defense-specific metadata on `FTacticalReportEvent`.
- Existing regression coverage already exercises this report builder in [`tests/tactical/FTacticalCombatReportTest.cpp`](/home/tstephen/repos/SSW/tests/tactical/FTacticalCombatReportTest.cpp) and dialog-facing summary behavior in [`tests/tactical/FTacticalDamageSummaryGUITest.cpp`](/home/tstephen/repos/SSW/tests/tactical/FTacticalDamageSummaryGUITest.cpp).

## Assumptions

- The defense list should use the stored player-facing defense names (`defenseName` / long names), not introduce a new abbreviation scheme.
- The redundant `"(Attack hit Target)"` text is a hit-detail formatting concern only; raw attack notes should remain available for other report text where relevant.
- No wx layout or widget changes are required; this is a tactical-report formatting and regression update.

## Files To Modify

- [`include/tactical/FTacticalCombatReport.h`](/home/tstephen/repos/SSW/include/tactical/FTacticalCombatReport.h)
- [`src/tactical/FTacticalGame.cpp`](/home/tstephen/repos/SSW/src/tactical/FTacticalGame.cpp)
- [`tests/tactical/FTacticalCombatReportTest.cpp`](/home/tstephen/repos/SSW/tests/tactical/FTacticalCombatReportTest.cpp)
- [`tests/tactical/FTacticalDamageSummaryGUITest.cpp`](/home/tstephen/repos/SSW/tests/tactical/FTacticalDamageSummaryGUITest.cpp)

## Overall Documentation Impact

No external user or design documentation is likely required. The only documentation impact should be inline contract clarity in the tactical report model if new defense-name fields or summary-formatting rules are introduced.

## Subtasks

### Subtask 1: Carry defense identity through the tactical report event model

Ensure the tactical report event shape preserves defense-specific metadata so the ship-summary formatter can produce a comma-separated list of actual defenses hit rather than a generic count or label.

#### Acceptance Criteria

- `FTacticalReportEvent` carries the defense identity data needed for player-facing summary output.
- Defense metadata is populated both for attack-derived events and immediate/report-level damage-resolution events.
- Existing weapon metadata propagation remains intact.
- Tactical report tests prove the defense-name data survives from damage effects into the report-event layer.

#### Documentation Impact

Update inline report-model comments only if needed to clarify that report events now preserve both weapon and defense identity for summary rendering.

### Subtask 2: Rework ship-summary effect formatting to show only actual damage/effects

Update ship-summary aggregation/formatting so the rollup:

- lists actual defense names hit in a comma-separated `Defense Hit: ...` segment,
- omits `0 hull damage` when a ship only suffered non-hull effects, and
- suppresses redundant `Hull Damage xN` effect entries when hull damage is already represented by the main hull-damage clause.

#### Acceptance Criteria

- Ship rollups with defense damage show `Defense Hit: <name>, <name>` style output instead of a generic defense count/label.
- Ship rollups with zero hull damage do not include a `0 hull damage` clause.
- Ship rollups that include hull damage do not also include `Hull Damage xN` in the effects list.
- Existing weapon-hit list behavior remains unchanged.
- Regression tests cover mixed hull/non-hull cases, defense-hit-only cases, and combined hull-plus-effects cases.

#### Documentation Impact

No external docs expected. Inline formatting comments may need a small update if the summary-building rules are documented near the helper functions.

### Subtask 3: Remove redundant hit-detail suffix text and refresh summary-facing regressions

Adjust hit-detail string generation so the detail list only reports the meaningful outcome/effects for hits and no longer appends the redundant `"(Attack hit Target)"` text, then update the tactical dialog-facing regression expectations to match the new contract.

#### Acceptance Criteria

- Hit-detail `displayLine` output for successful attacks no longer includes the redundant trailing `"(Attack hit Target)"` text.
- Hit-detail output still includes hull damage and non-hull effect text when present.
- Tactical dialog-facing tests assert the revised hit-detail and ship-summary strings.
- No GUI code changes are required unless a test reveals a renderer-specific dependency on the old strings.

#### Documentation Impact

No external docs expected. If any inline comment describes hit-detail wording, update it to reflect the new concise display contract.

## Dependency Ordering

1. Subtask 1
2. Subtask 2
3. Subtask 3

Parallelization guidance:

- Keep these subtasks sequential.
- Subtasks 1 and 2 both touch the report model in [`include/tactical/FTacticalCombatReport.h`](/home/tstephen/repos/SSW/include/tactical/FTacticalCombatReport.h).
- Subtask 3 depends on the final report-string contract established by Subtasks 1 and 2.

## Implementer Agent Prompts

### Implementer Prompt: Subtask 1

You are the implementer agent.

Allowed files:

- `include/tactical/FTacticalCombatReport.h`
- `src/tactical/FTacticalGame.cpp`
- `tests/tactical/FTacticalCombatReportTest.cpp`

Task:

Preserve defense-specific metadata through the tactical report event model so downstream summary formatting can identify which concrete defenses were damaged. The repository already preserves `defenseName` on low-level tactical damage/attack effects; extend the report-event layer and the event-construction paths so that data survives into `buildTacticalCombatReportSummary(...)`.

Acceptance criteria:

- `FTacticalReportEvent` contains the defense metadata required by summary formatting.
- `appendTacticalDamageResolutionEvents(...)` and `buildTacticalAttackEvent(...)` populate that defense metadata from the incoming effect objects.
- Existing weapon metadata propagation remains unchanged.
- Tactical regression coverage proves the defense metadata is present in the report-event layer.

Validation:

- `cd tests/tactical && make && ./TacticalTests`

Expected Tester file locations:

- `tests/tactical/FTacticalCombatReportTest.cpp`

Artifact directory:

- Default to `artifacts/tactical-damage-summary-followup/subtask-1` if no shared artifact path is provided by orchestration context.

Execution instruction:

- Begin implementation immediately if no blocking inputs are missing. Do not stop after a preflight restatement.

Completion gate:

- Do not report success unless all required artifacts exist and all changes are committed.

### Implementer Prompt: Subtask 2

You are the implementer agent.

Allowed files:

- `include/tactical/FTacticalCombatReport.h`
- `tests/tactical/FTacticalCombatReportTest.cpp`
- `tests/tactical/FTacticalDamageSummaryGUITest.cpp`

Task:

Update ship-summary aggregation and formatting so the Tactical Damage Summary rollup only reports actual damage/effects. Specifically:

- emit comma-separated defense names for defense hits,
- omit `0 hull damage` when a ship only suffered non-hull effects, and
- suppress `Hull Damage xN` in the effects section when the hull-damage clause already reports that information.

Acceptance criteria:

- Defense-hit rollups show concrete defense names rather than a generic count/label.
- Zero-hull-damage summaries omit the hull-damage clause entirely while still reporting non-hull effects.
- Hull-damage summaries no longer repeat `Hull Damage xN` in the effects section.
- Weapon-hit rollups still use the existing comma-separated abbreviation list behavior.
- Tactical regression tests cover defense-only, mixed-effects, and hull-plus-effects rollups.

Validation:

- `cd tests/tactical && make && ./TacticalTests`

Expected Tester file locations:

- `tests/tactical/FTacticalCombatReportTest.cpp`
- `tests/tactical/FTacticalDamageSummaryGUITest.cpp`

Artifact directory:

- Default to `artifacts/tactical-damage-summary-followup/subtask-2` if no shared artifact path is provided by orchestration context.

Execution instruction:

- Begin implementation immediately if no blocking inputs are missing. Do not stop after a preflight restatement.

Completion gate:

- Do not report success unless all required artifacts exist and all changes are committed.

### Implementer Prompt: Subtask 3

You are the implementer agent.

Allowed files:

- `include/tactical/FTacticalCombatReport.h`
- `tests/tactical/FTacticalCombatReportTest.cpp`
- `tests/tactical/FTacticalDamageSummaryGUITest.cpp`
- `tests/gui/TacticalGuiLiveTest.cpp` if renderer-facing expectations need adjustment

Task:

Remove the redundant `"(Attack hit Target)"` suffix from Tactical Damage Summary hit-detail lines while preserving the meaningful outcome text for hits. Refresh tactical summary regressions to assert the new concise hit-detail wording and the final ship-summary contract from the preceding subtasks.

Acceptance criteria:

- Hit-detail `displayLine` strings no longer include the redundant `"(Attack hit Target)"` suffix.
- Hit-detail strings still include hull damage and non-hull effect wording when present.
- Tactical regression coverage asserts the new hit-detail wording.
- GUI live-test updates are limited to expectation changes only if they are actually needed by the new report contract.

Validation:

- `cd tests/tactical && make && ./TacticalTests`
- If `tests/gui/TacticalGuiLiveTest.cpp` changes: `cd tests/gui && make && xvfb-run -a ./GuiTests`

Expected Tester file locations:

- `tests/tactical/FTacticalCombatReportTest.cpp`
- `tests/tactical/FTacticalDamageSummaryGUITest.cpp`
- `tests/gui/TacticalGuiLiveTest.cpp` only if GUI-facing expectations are updated

Artifact directory:

- Default to `artifacts/tactical-damage-summary-followup/subtask-3` if no shared artifact path is provided by orchestration context.

Execution instruction:

- Begin implementation immediately if no blocking inputs are missing. Do not stop after a preflight restatement.

Completion gate:

- Do not report success unless all required artifacts exist and all changes are committed.

## Output Artifact Path

- [`plans/tactical-damage-summary-followup-plan.md`](/home/tstephen/repos/SSW/plans/tactical-damage-summary-followup-plan.md)
