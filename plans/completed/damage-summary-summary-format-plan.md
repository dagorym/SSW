# Damage Summary Summary Format Plan

## Confirmed Facts

- `include/tactical/FTacticalCombatReport.h` contains the tactical damage-summary rollup model and the inline ship-summary formatter (`buildShipSummaryDisplayLine(...)`) that currently emits a single flat line per ship with `effects: ...`.
- `src/gui/TacticalDamageSummaryGUI.cpp` does not synthesize summary content itself; it renders `shipSummary.displayLines` from the combat-report summary.
- Existing regression coverage already exercises both the model-side rollup and the GUI summary dialog in:
  - `tests/tactical/FTacticalCombatReportTest.cpp`
  - `tests/tactical/FTacticalDamageSummaryGUITest.cpp`
- The current summary aggregation tracks defense hits via abbreviations, but ADF/MR loss quantities are not preserved in the ship-summary rollup even though those values exist in the underlying damage-effect/event data.

## Assumptions

- No new dialog controls or layout changes are needed beyond rendering the revised multi-line summary text more readably inside the existing read-only multiline text control.
- The requested `ADF (-N)` and `MR (-N)` summary format should report total points lost for that ship within the summarized report scope, not per-hit detail duplicated at the ship-summary level.
- Existing tactical test runners remain the smallest relevant validation scope:
  - `cd tests/tactical && make && ./TacticalTests`
  - `cd tests/gui && make && xvfb-run -a ./GuiTests`

## Files To Modify

- `include/tactical/FTacticalCombatReport.h`
- `src/gui/TacticalDamageSummaryGUI.cpp`
- `tests/tactical/FTacticalCombatReportTest.cpp`
- `tests/tactical/FTacticalDamageSummaryGUITest.cpp`

## Overall Documentation Impact

- No user-facing documentation update is likely required unless the project keeps a release-note or artifact trail for tactical UI behavior changes.
- If the team records tactical-report formatting changes in plan or completion artifacts, note that ship-summary rollups now render as a ship header plus bullet-style per-effect lines and now carry ADF/MR loss counts into the summary.

## Subtasks

### Subtask 1: Restructure ship-summary rollup generation into itemized summary entries

Description:
- Change the tactical combat-report summary builder so each damaged ship emits a header line of `<Ship Name>:` followed by one summary item per line, each prefixed with ` - `.
- Preserve existing aggregation behavior for hull-damage totals, defense-hit grouping, weapon-hit grouping, and distinct effect labels, but emit them as separate lines instead of one semicolon/comma-joined sentence.
- Keep summary generation centralized in the model rollup layer so the GUI continues to render prepared display lines instead of re-deriving report semantics.

Acceptance Criteria:
- A ship with only hull damage renders as:
  - `<Ship Name>:`
  - ` - <N> hull damage from <M> attack(s)`
- A ship with hull damage plus multiple unique effects renders each summary item on its own line instead of using `; effects: ...`.
- Grouped defense hits remain grouped on one bullet line such as ` - Defense Hit: ICM, MS`.
- Existing non-hull effects that are currently surfaced in the summary continue to appear, but now one per bullet line.
- `summary.displayLines` and each ship’s `displayLines` remain populated consistently for downstream GUI rendering.

Documentation Impact:
- No documentation update expected beyond implementation/test artifacts.

### Subtask 2: Carry ADF/MR point-loss values into the ship-summary model rollup

Description:
- Extend the tactical summary aggregation so ADF-loss and MR-loss effects preserve quantitative loss information at the ship-summary level.
- Emit ADF and MR losses as dedicated summary bullets using the requested format, for example ` - ADF (-2)` and ` - MR (-1)`.
- Ensure these quantities are derived from report data already captured in the tactical damage/effect structures rather than inferred from GUI text.

Acceptance Criteria:
- When the report includes one or more ADF-loss effects for a ship, the ship summary includes exactly one `ADF (-N)` bullet reflecting the total ADF points lost in the summarized report scope.
- When the report includes one or more MR-loss effects for a ship, the ship summary includes exactly one `MR (-N)` bullet reflecting the total MR points lost in the summarized report scope.
- ADF/MR loss quantities do not disappear from the summary even when other effects for the same ship are also present.
- Existing hit-detail output remains intact unless a change is required to keep it consistent with the underlying rollup data.

Documentation Impact:
- If any artifact or release note mentions tactical damage-summary behavior, update it to note that ADF/MR losses now appear with explicit point-loss counts in the summary section.

### Subtask 3: Align dialog rendering and regression coverage with the new multi-line summary contract

Description:
- Update GUI-focused expectations so the damage summary dialog explicitly supports the new ship-header-plus-bullets summary format.
- Strengthen tactical model and GUI regression tests to cover the requested examples or equivalent cases, including mixed hull damage, grouped defense hits, standalone effects, and ADF/MR point-loss bullets.

Acceptance Criteria:
- Tactical model tests verify the ship-summary rollup now produces multiple lines per ship in the requested structure.
- Tactical model tests verify ADF/MR summary bullets include exact point-loss values.
- GUI tests verify the dialog still renders the ship rollup section correctly using `displayLines`, including multi-line per-ship output.
- Existing empty-state and hit-detail behavior covered by the current GUI tests remains preserved.

Documentation Impact:
- No separate documentation update expected; test names/comments should reflect the revised summary format expectations where helpful.

## Dependency Ordering

1. Subtask 1 must complete first because it establishes the new ship-summary output shape consumed by the dialog.
2. Subtask 2 should follow Subtask 1 because ADF/MR quantitative bullets belong to the same model-side rollup contract and should be designed against the finalized multi-line summary structure.
3. Subtask 3 depends on Subtasks 1 and 2 because the regression expectations should target the final model output and the GUI simply renders that finalized structure.

Parallelization:
- These subtasks should be treated as sequential. They all touch the same summary contract and overlapping files, so parallel execution would create avoidable conflicts and moving test expectations.

## Implementer Agent Prompts

### Prompt For Subtask 1

You are the implementer agent

Allowed files to change:
- `include/tactical/FTacticalCombatReport.h`
- `tests/tactical/FTacticalCombatReportTest.cpp`

Task:
- Refactor the ship-summary rollup generation so each damaged ship is summarized as a header line `<Ship Name>:` followed by one line per summary item prefixed with ` - `.
- Preserve existing rollup semantics for hull damage, grouped defense hits, grouped weapon hits, and distinct non-hull effects, but stop emitting the single-line `; effects: ...` format in the ship summary.
- Keep this logic in the tactical report model layer rather than moving formatting decisions into the GUI.

Acceptance criteria:
- Ship rollups no longer use a single flat `Ship: ...; effects: ...` line when multiple summary items exist.
- Hull damage totals remain aggregated correctly and still include attack counts.
- Grouped effect categories still aggregate correctly after the formatting change.
- Existing ship-summary model tests are updated or extended to lock the new multi-line output contract.

Validation:
- `cd tests/tactical && make && ./TacticalTests`

Expected Tester test file locations:
- `tests/tactical/FTacticalCombatReportTest.cpp`

Artifact directory guidance:
- Default to `artifacts/damage-summary-summary-format/subtask-1-rollup-format` unless orchestration provides a different shared artifact path.

Execution instruction:
- Begin implementation immediately when the prompt contains the required blocking inputs, and do not stop after preflight restatement alone.

Completion gate:
- Do not report success unless all required artifacts exist and all changes are committed.

### Prompt For Subtask 2

You are the implementer agent

Allowed files to change:
- `include/tactical/FTacticalCombatReport.h`
- `tests/tactical/FTacticalCombatReportTest.cpp`

Task:
- Extend the ship-summary aggregation so ADF-loss and MR-loss effects carry exact point-loss totals into the summary rollup.
- Emit those totals as summary bullets in the form ` - ADF (-N)` and ` - MR (-N)` while preserving the rest of the rollup behavior.
- Ensure the values come from the underlying tactical effect/event data rather than being guessed from already-formatted strings.

Acceptance criteria:
- ADF loss is represented in the ship summary with the exact total number of ADF points lost for the summarized report scope.
- MR loss is represented in the ship summary with the exact total number of MR points lost for the summarized report scope.
- The new ADF/MR bullets coexist correctly with hull-damage bullets, grouped defense-hit bullets, and other effects.
- Tactical model tests cover at least one ship-summary case matching the requested behavior for ADF and MR point loss.

Validation:
- `cd tests/tactical && make && ./TacticalTests`

Expected Tester test file locations:
- `tests/tactical/FTacticalCombatReportTest.cpp`

Artifact directory guidance:
- Default to `artifacts/damage-summary-summary-format/subtask-2-adf-mr-rollup` unless orchestration provides a different shared artifact path.

Execution instruction:
- Begin implementation immediately when the prompt contains the required blocking inputs, and do not stop after preflight restatement alone.

Completion gate:
- Do not report success unless all required artifacts exist and all changes are committed.

### Prompt For Subtask 3

You are the implementer agent

Allowed files to change:
- `src/gui/TacticalDamageSummaryGUI.cpp`
- `tests/tactical/FTacticalDamageSummaryGUITest.cpp`
- `tests/tactical/FTacticalCombatReportTest.cpp`

Task:
- Update the damage-summary dialog and regression coverage to align with the new ship-summary contract.
- Confirm the dialog still renders the prepared multi-line ship rollup cleanly, and strengthen tests to cover the requested ship-header-plus-bullets format and the explicit ADF/MR point-loss bullets.
- Preserve the existing empty-state, close-button, and optional hit-detail behaviors already covered by the GUI tests.

Acceptance criteria:
- GUI regression tests assert the new multi-line ship-summary output rather than the old single-line `effects:` format.
- Tactical model regression tests include examples equivalent to the requested summary-format cases.
- The dialog continues to render the summary and hit-detail sections without regressing the current empty-state behavior.
- GUI-specific validation passes in the existing tactical GUI test runner.

Validation:
- `cd tests/tactical && make && ./TacticalTests`
- `cd tests/gui && make && xvfb-run -a ./GuiTests`

Expected Tester test file locations:
- `tests/tactical/FTacticalDamageSummaryGUITest.cpp`
- `tests/tactical/FTacticalCombatReportTest.cpp`
- Assumption: if the GUI suite also mirrors this dialog behavior elsewhere, the existing `tests/gui/*` runner is sufficient without adding a new dedicated fixture.

Artifact directory guidance:
- Default to `artifacts/damage-summary-summary-format/subtask-3-gui-and-regressions` unless orchestration provides a different shared artifact path.

Execution instruction:
- Begin implementation immediately when the prompt contains the required blocking inputs, and do not stop after preflight restatement alone.

Completion gate:
- Do not report success unless all required artifacts exist and all changes are committed.

## Output Artifact Path

- `plans/damage-summary-summary-format-plan.md`
