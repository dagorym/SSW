Requested agent: `planner`

- Repository-local definition found: no
- Shared definition found: yes
- Definition path used: `/home/tstephen/repos/agents/agents/planner.md`
- Precedence decision: shared `planner` definition applies because this repository does not define a repository-local `planner` override

## Feature Restatement

Add a second, more detailed section to the tactical damage summary dialog so combat resolution is easier to inspect during play. The existing per-ship rollup must remain, and a new full hit-detail section must be conditionally shown by a toggleable flag that defaults to `true` for now.

## Confirmed Repository Facts

- Tactical fire resolution already produces `FTacticalCombatReportSummary` objects and those summaries drive the tactical damage summary dialog.
- The current dialog implementation in `src/gui/TacticalDamageSummaryGUI.cpp` renders a single multiline text control from `FTacticalShipReportSummary::displayLines`.
- The report model already preserves raw per-attack and per-event detail via `FTacticalShipReportSummary::rawAttacksReceived` and `rawEvents`.
- The dialog is launched through `WXTacticalUI::showDamageSummary(...)`, with tactical fire flows calling that seam from `FTacticalGame` and `FBattleScreen`.
- Existing regression tests explicitly assert the current rollup-only behavior in `tests/tactical/FTacticalDamageSummaryGUITest.cpp`, so those expectations will need to be updated.

## Assumptions

- “Full details” for this story means one entry per weapon attack that actually hit, identifying attacker ship, weapon, target ship, and the damage/effects attributable to that hit.
- Misses, unfired weapons, and intercepted/non-hit attack attempts should not appear in the new detail section.
- Immediate non-weapon reports such as electrical fire or mine damage should continue to work without forcing a synthetic “who fired what” section when no hit attacks exist.
- The least disruptive first implementation is to carry the toggle on the tactical report summary/view-model path, default it to `true`, and have the dialog honor it, without broadening `ITacticalUI` in this story.

## Files To Modify

- `include/tactical/FTacticalCombatReport.h`
- `include/gui/TacticalDamageSummaryGUI.h`
- `src/gui/TacticalDamageSummaryGUI.cpp`
- `tests/tactical/FTacticalCombatReportTest.h`
- `tests/tactical/FTacticalCombatReportTest.cpp`
- `tests/tactical/FTacticalDamageSummaryGUITest.h`
- `tests/tactical/FTacticalDamageSummaryGUITest.cpp`
- `tests/gui/TacticalGuiLiveTest.h`
- `tests/gui/TacticalGuiLiveTest.cpp`

Possible-but-not-guaranteed files if implementation reveals additional seam changes:

- `include/gui/WXTacticalUI.h`
- `src/gui/WXTacticalUI.cpp`
- `include/tactical/ITacticalUI.h`
- `tests/tactical/WXTacticalUIAdapterTest.cpp`
- `tests/tactical/ITacticalUIBoundaryTest.cpp`

## Overall Documentation Impact

- No mandatory user-facing documentation update appears required for the first pass if the flag remains always enabled internally.
- `doc/UsersGuide.md` should be reviewed only if it already documents the tactical results dialog contents.
- `doc/DesignNotes.md` may need a brief note only if the implementation adds a lasting report-summary display flag or a new dialog rendering contract that future work will depend on.

## Subtasks

### Subtask 1: Extend the tactical report summary with hit-detail presentation data and a display toggle

Scope:

- Keep the current ship rollup summary intact.
- Add a toggleable summary/display flag that defaults to `true`.
- Derive a detailed hit section from existing raw report data so the GUI does not need to reverse-engineer attack/effect relationships on its own.

Likely files:

- `include/tactical/FTacticalCombatReport.h`
- `tests/tactical/FTacticalCombatReportTest.h`
- `tests/tactical/FTacticalCombatReportTest.cpp`

Acceptance Criteria:

- `FTacticalCombatReportSummary` exposes a toggleable field controlling whether the detailed hit section should be shown, and the field defaults to `true`.
- The summary builder produces stable detail output for hit attacks only.
- Each detailed hit entry identifies attacker ship, weapon, target ship, and damage/effect outcome in a player-readable format.
- Existing ship rollup output remains available and semantically unchanged.
- Non-hit attacks do not appear in the detailed hit section.
- Reports with no hit attacks still build safely and preserve the existing rollup/empty-state behavior.
- Existing electrical-fire and mine-damage summary behavior remains valid.

Documentation Impact:

- No documentation update expected unless the added summary field becomes part of a documented tactical reporting contract.

### Subtask 2: Render the new detail section in the tactical results dialog and lock behavior with GUI regressions

Scope:

- Update the dialog layout/text generation to show two sections when the flag is enabled: the existing ship rollup and the new hit-detail section.
- Hide or omit the second section when the toggle is `false`.
- Preserve safe empty-state behavior and current modal wiring.

Likely files:

- `include/gui/TacticalDamageSummaryGUI.h`
- `src/gui/TacticalDamageSummaryGUI.cpp`
- `tests/tactical/FTacticalDamageSummaryGUITest.h`
- `tests/tactical/FTacticalDamageSummaryGUITest.cpp`
- `tests/gui/TacticalGuiLiveTest.h`
- `tests/gui/TacticalGuiLiveTest.cpp`

Acceptance Criteria:

- The dialog still shows the current context text and ship rollup section.
- When the summary toggle is `true` and hit details exist, the dialog shows a second clearly labeled detail section below or after the rollup section.
- When the summary toggle is `false`, the detail section is not rendered even if hit-detail data exists.
- The dialog handles reports with ship rollups but no hit-detail lines without broken formatting.
- The dialog continues to handle reports with no damaged ships using the current safe empty-state message.
- Tactical GUI regression coverage verifies the detailed section appears when enabled and is suppressed when disabled.
- Headless/live modal smoke coverage still passes through `WXTacticalUI::showDamageSummary(...)`.

Documentation Impact:

- No documentation update expected for the first pass unless the dialog contents are already described in `doc/UsersGuide.md`.

## Dependency Ordering

1. Subtask 1 must complete first.
   The dialog should consume a stable summary/detail shape rather than invent its own attack parsing.
2. Subtask 2 depends on Subtask 1.
   GUI rendering and regression assertions should be written against the finalized summary contract.

Parallelization guidance:

- Do not parallelize these subtasks.
- The UI task depends on the report-summary shape, naming, and toggle semantics established in Subtask 1.

## Implementer Agent Prompts

### Implementer Prompt: Subtask 1

You are the implementer agent

Allowed files:

- `/home/tstephen/repos/SSW/include/tactical/FTacticalCombatReport.h`
- `/home/tstephen/repos/SSW/tests/tactical/FTacticalCombatReportTest.h`
- `/home/tstephen/repos/SSW/tests/tactical/FTacticalCombatReportTest.cpp`

Task:

- Extend the tactical combat report summary contract so it can drive both the existing ship rollup and a new hit-detail section for the tactical results dialog.
- Add a toggleable display flag that defaults to enabled.
- Build detail output from the existing report/attack/event data for attacks that actually hit, without regressing the current ship-level rollup semantics.

Acceptance criteria:

- `FTacticalCombatReportSummary` contains a toggleable control for the hit-detail section and defaults it to `true`.
- The summary builder emits detail data only for attacks with `hit == true`.
- Detail data is player-readable and captures attacker ship, weapon, target ship, and damage/effect outcome.
- Existing ship rollup summaries continue to pass their current semantics.
- Reports with no hit attacks remain valid and do not invent empty detail entries.
- Immediate/non-weapon report types still build correctly.

Validation:

- Run `cd tests/tactical && make && ./TacticalTests`.

Expected Tester file locations:

- `/home/tstephen/repos/SSW/tests/tactical/FTacticalCombatReportTest.cpp`

Artifact directory guidance:

- If no orchestrator-provided artifact directory is supplied, default to `/home/tstephen/repos/SSW/artifacts/tactical-results-hit-detail-subtask-1`.

Execution instruction:

- Begin implementation immediately when the prompt contains the required blocking inputs, and do not stop after preflight restatement alone.

Completion gate:

- Do not report success unless all required artifacts exist and all changes are committed.

### Implementer Prompt: Subtask 2

You are the implementer agent

Allowed files:

- `/home/tstephen/repos/SSW/include/gui/TacticalDamageSummaryGUI.h`
- `/home/tstephen/repos/SSW/src/gui/TacticalDamageSummaryGUI.cpp`
- `/home/tstephen/repos/SSW/tests/tactical/FTacticalDamageSummaryGUITest.h`
- `/home/tstephen/repos/SSW/tests/tactical/FTacticalDamageSummaryGUITest.cpp`
- `/home/tstephen/repos/SSW/tests/gui/TacticalGuiLiveTest.h`
- `/home/tstephen/repos/SSW/tests/gui/TacticalGuiLiveTest.cpp`

Task:

- Update the tactical damage summary dialog to render two sections: the existing by-ship rollup and a new hit-detail section controlled by the summary toggle introduced in Subtask 1.
- Preserve existing context text, modal close behavior, and safe empty-state handling.
- Add or update tactical GUI regressions so enabled and disabled detail-section behavior is covered.

Acceptance criteria:

- The dialog renders the ship rollup section as before.
- When the toggle is enabled and detail lines exist, the dialog renders a second labeled hit-detail section.
- When the toggle is disabled, the dialog omits the hit-detail section.
- Reports with no hit-detail lines still render cleanly.
- Reports with no damaged ships still show the current empty-state text.
- Tactical GUI tests cover both enabled and disabled detail-section behavior.
- Live dialog smoke coverage through `WXTacticalUI::showDamageSummary(...)` still succeeds.

Validation:

- Run `cd tests/tactical && make && ./TacticalTests`.
- Run `cd tests/gui && make && xvfb-run -a ./GuiTests`.
- If `xvfb-run` is unavailable in the environment, treat the GUI command as the intended validation scope and state that assumption in artifacts.

Expected Tester file locations:

- `/home/tstephen/repos/SSW/tests/tactical/FTacticalDamageSummaryGUITest.cpp`
- `/home/tstephen/repos/SSW/tests/gui/TacticalGuiLiveTest.cpp`

Artifact directory guidance:

- If no orchestrator-provided artifact directory is supplied, default to `/home/tstephen/repos/SSW/artifacts/tactical-results-hit-detail-subtask-2`.

Execution instruction:

- Begin implementation immediately when the prompt contains the required blocking inputs, and do not stop after preflight restatement alone.

Completion gate:

- Do not report success unless all required artifacts exist and all changes are committed.

## Output Artifact Path

- `/home/tstephen/repos/SSW/plans/tactical-results-hit-detail-plan.md`
