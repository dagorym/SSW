# Tactical Damage Summary Plan

## Feature Restatement

Record the effect of each tactical attack as combat resolves, then show players an end-of-phase summary dialog after both defensive fire and offensive fire so they can review what damage each ship took during that phase. Also record and display start-of-phase electrical fire damage when it is applied before movement, and mine damage when it is applied after the movement that triggered it.

## Confirmed Repository Facts

- Tactical phase transitions are handled in `src/tactical/FBattleDisplay.cpp` and `src/tactical/FBattleScreen.cpp`.
- `FBattleDisplay::onDefensiveFireDone()` and `FBattleDisplay::onOffensiveFireDone()` currently call `fireAllWeapons()` and then immediately advance phases.
- Weapon attacks are resolved in `src/weapons/FWeapon.cpp`.
- Ship damage effects are applied in `src/ships/FVehicle.cpp`, especially `takeDamage()`, `takeHullDamage()`, and `advancedDamage()`.
- There is already a pattern for modal dialogs in the tactical flow via `ShowModal()`, and the repo already contains GUI dialog code under `include/gui/` and `src/gui/`.

## Assumptions

- The requested dialog is tactical-only and does not need to reuse the strategic `include/gui/BattleResultsGUI.h` dialog, which is designed for manual post-battle editing rather than read-only combat summaries.
- “The effects for each attack should be recorded” means implementation should capture both per-attack resolution details and a per-ship summary suitable for display at phase end.
- The end-of-phase dialog should summarize the just-completed fire phase only, then clear before the next phase starts.
- Mine damage should be summarized and shown immediately after mine damage is applied at the end of the movement that triggered it.
- Electrical fire damage should be summarized and shown immediately when applied at the start of the moving player’s turn, before movement selection begins.

## Likely Files To Modify

- `include/tactical/FBattleScreen.h`
- `src/tactical/FBattleScreen.cpp`
- `include/tactical/FBattleDisplay.h`
- `src/tactical/FBattleDisplay.cpp`
- `include/weapons/FWeapon.h`
- `src/weapons/FWeapon.cpp`
- `include/ships/FVehicle.h`
- `src/ships/FVehicle.cpp`
- `include/tactical/FBattleBoard.h`
- `src/tactical/FBattleBoard.cpp`
- Likely new tactical results dialog files:
- `include/gui/TacticalDamageSummaryGUI.h`
- `src/gui/TacticalDamageSummaryGUI.cpp`
- Build wiring if a new dialog is added:
- `src/gui/Makefile`

## Artifact Handoff

- The results of Subtask 1 should be written into an appropriately labeled subdirectory under the top-level `artifacts/` directory.
- The exact directory name is intentionally not fixed yet, but it should be something in the style of `tactical-damage-summary_subtask1`.
- Subtask 2 must inspect that Subtask 1 artifact directory to discover which report-model files were created before making changes.
- Later subtasks may also rely on those created files, but Subtask 2 is the first required consumer of that artifact handoff.

## Subtasks

### 1. Define tactical damage-reporting data model and ownership

Describe and add a tactical reporting structure owned by `FBattleScreen` that can represent:
- each fired attack attempt returned from `FWeapon::fire()`
- hit or miss outcome
- target ship
- hull damage inflicted, if any
- non-hull effects caused by the advanced damage table
- a per-ship aggregate for the current report window

This subtask should establish report state in `FBattleScreen` for four reporting moments:
- start-of-turn electrical fire damage
- post-movement mine damage
- defensive fire end-of-phase summary
- offensive fire end-of-phase summary

The data model should explicitly support:
- raw per-attack/internal event storage
- read-only per-ship rollups for display
- report metadata identifying one shared dialog context such as `Electrical Fire Damage`, `Mine Damage`, `Defensive Fire Results`, or `Offensive Fire Results`

Acceptance criteria:
- There is a clearly defined in-memory representation for per-event results and per-ship summaries.
- `FBattleScreen` exposes APIs to begin a report, append raw events, build a display summary, and clear report state.
- The design distinguishes between raw per-attack/internal event records and per-ship display summaries, so detailed data is retained while the first UI stays focused on the ship rollup.
- The model supports both weapon-driven events and non-weapon damage events such as mines and electrical fires.
- No tactical phase advancement logic is changed yet beyond what is required to host the data model.

### 2. Make vehicle damage resolution emit structured applied-effects data

Update `FVehicle::takeDamage()` and `advancedDamage()` so they can populate an optional output/result parameter describing the exact effects applied to a ship.

This task should keep effect capture anchored at the point where the advanced-damage table result is actually known. It should cover hull damage and non-hull outcomes such as MR/ADF loss, weapon hits, defense hits, combat control damage, navigation damage, electrical fire, and power-system damage.

Acceptance criteria:
- `FVehicle::takeDamage()` and `advancedDamage()` populate an optional effects/result output parameter with all applied hull and advanced-damage effects needed by higher layers.
- Existing combat state changes remain behaviorally equivalent to current rules.
- The emitted effect data is explicit enough for higher layers to build tactical reports without inferring changes from before/after ship state.

### 3. Make `FWeapon::fire()` return structured attack results

Update `FWeapon::fire()` so it returns a structured attack-result object for the tactical layer to store.

This task should use the new optional vehicle damage-effects output so the returned attack result can capture:
- whether the shot resolved
- hit or miss outcome
- target identity
- hull damage inflicted
- non-hull effects applied
- any relevant skipped-fire state the tactical layer should preserve

This keeps weapon resolution self-contained and explicit while avoiding direct coupling from `FWeapon` to `FBattleScreen`.

Acceptance criteria:
- `FWeapon::fire()` returns a structured result object suitable for tactical reporting.
- The returned result distinguishes at least skipped/not-fired, fired-and-missed, and fired-and-hit outcomes.
- A successful hit records both hull damage and non-hull effects actually applied to the target.
- Existing weapon-resolution behavior remains intact.

### 4. Aggregate raw tactical events into damaged-ships-only display summaries

Build the summary transformation that groups raw attack and non-weapon event results by ship and prepares display-ready lines for the shared tactical dialog.

The summary should answer the player request directly: what damage each ship took during the relevant report window. It should show each damaged ship, total hull damage, and notable system effects gained during the report. Per-attack detail should remain stored internally and should not be shown in the first UI.

Acceptance criteria:
- The aggregation produces one summary entry per ship that actually sustained damage in the report window.
- Multiple raw events against one ship aggregate correctly into one display summary.
- Ships that took no damage do not appear in the display summary.
- Summary generation does not depend on the target ship still existing on the board after report creation.
- The aggregation is independent of the dialog so it can be tested without GUI dependencies.

### 5. Add a tactical summary dialog for immediate and end-of-phase reporting

Create one shared tactical dialog class for displaying tactical damage summaries. It should support:
- just-completed defensive fire results
- just-completed offensive fire results
- immediately applied electrical fire damage
- immediately applied mine damage

This should be read-only and focused on quick review, not manual editing.

It should include context such as “Defensive Fire Results,” “Offensive Fire Results,” “Electrical Fire Damage,” or “Mine Damage,” and list ship-by-ship outcomes. The first version should display only the per-ship damage rollup, not the internal per-attack detail.

Acceptance criteria:
- A single modal dialog class can be opened from the tactical battle flow for both immediate-damage and end-of-phase reports.
- The dialog title and content clearly identify which report type is being shown.
- The dialog displays only ships that actually sustained damage, in a readable ship-by-ship rollup format.
- The dialog can open with an empty summary without crashing and presents a clear no-damage message.
- The dialog is wired into the existing GUI build.

### 6. Integrate immediate electrical-fire reporting before movement

Integrate start-of-turn electrical fire damage into the reporting flow.

This work should:
- record fire-damage results when `FBattleScreen::applyFireDamage()` runs
- build the damaged-ships-only summary
- show the shared tactical report dialog before movement selection begins
- defer destroyed-ship cleanup until after dialog acknowledgement

Acceptance criteria:
- Electrical fire damage shows a summary before movement starts.
- The shared tactical summary dialog is used for the report.
- Destroyed ships remain on the board and are still shown in the summary for the report in which they were destroyed.
- `clearDestroyedShips()` runs only after the relevant dialog is acknowledged.
- Movement does not begin until the report dialog is closed.
- After the electrical-fire dialog is dismissed, control returns to the same moving player and the movement phase begins normally.

### 7. Integrate immediate mine-damage reporting after mine application

Integrate post-movement mine damage into the reporting flow.

This work should:
- record mine-damage results when `FBattleBoard::applyMineDamage()` runs
- build the damaged-ships-only summary
- show the shared tactical report dialog immediately after mine damage is applied
- defer destroyed-ship cleanup until after dialog acknowledgement

Acceptance criteria:
- Mine damage shows a summary immediately after mine damage is applied.
- The shared tactical summary dialog is used for the report.
- Destroyed ships remain on the board and are still shown in the summary for the report in which they were destroyed.
- `clearDestroyedShips()` runs only after the relevant dialog is acknowledged.
- Mine damage is applied exactly once per triggering movement event, and the dialog appears only after mine-damage resolution completes.

### 8. Integrate defensive and offensive fire end-of-phase reporting

Update the tactical fire-phase handlers so the shared summary dialog is shown after defensive fire and offensive fire resolution, with destroyed-ship cleanup deferred until after acknowledgement.

This is the main sequencing risk in the fire-phase portion of the feature. Right now `fireAllWeapons()` immediately calls `clearDestroyedShips()`, and the phase handlers then advance to the next phase. The implementation should defer `clearDestroyedShips()` until after dialog acknowledgement so destroyed ships remain visible on the board while the player reviews the report.

Acceptance criteria:
- After defensive fire resolves, the defensive-fire summary dialog is shown before the UI transitions to the next relevant step.
- After offensive fire resolves, the offensive-fire summary dialog is shown before the UI transitions back to movement or toggles the moving player.
- Destroyed ships remain on the board and are still shown in the summary for the report in which they were destroyed.
- `clearDestroyedShips()` runs only after the relevant dialog is acknowledged.
- The summary state is cleared after the dialog is acknowledged so one phase’s results do not leak into the next.
- After defensive-fire acknowledgement, the battle advances to offensive fire.
- After offensive-fire acknowledgement, the battle returns to movement and toggles the moving player exactly once.

### 9. Add automated coverage for result capture and tactical summary behavior

Add or extend tests around weapon fire and vehicle damage reporting, focusing on deterministic verification where feasible.

Because combat uses random rolls, this subtask should target the reporting APIs and aggregation logic in units that can be exercised without fragile UI automation. If direct deterministic testing of `fire()` is impractical, test the lower-level result construction and aggregation instead.

Acceptance criteria:
- There are automated tests covering at least:
- recording a hit with hull damage
- recording a non-hull advanced-damage effect
- recording electrical fire damage into an immediate report
- recording mine damage into an immediate report
- aggregating multiple attacks onto one ship
- omitting undamaged ships from the display summary
- preserving destroyed-ship reporting and delayed cleanup through dialog-triggered summary creation
- Tests are organized to cover raw effect capture, aggregation/filtering, and delayed-cleanup integration behavior.
- Tests build and run within the existing `tests/` framework, or the plan explicitly documents any gap if GUI testing is not practical.

## Dependency Ordering

1. Subtask 1 must complete first because the rest of the feature needs a stable reporting model and owner.
2. Subtask 2 depends on Subtask 1 because vehicle damage needs the reporting model shape.
3. Subtask 3 depends on Subtasks 1 and 2 because weapon results need the reporting model and structured vehicle effects output.
4. Subtask 4 depends on Subtasks 1, 2, and 3 because the display summaries are derived from recorded tactical events.
5. Subtask 5 depends on Subtask 4 because the dialog should render the defined summary shape.
6. Subtask 6 depends on Subtasks 1, 2, 4, and 5 because electrical-fire reporting needs the model, emitted effects, summary generation, and shared dialog.
7. Subtask 7 depends on Subtasks 1, 2, 4, and 5 because mine reporting needs the model, emitted effects, summary generation, and shared dialog.
8. Subtask 8 depends on Subtasks 3, 4, and 5 because end-of-phase fire reporting needs weapon results, summary generation, and the shared dialog.
9. Subtask 9 depends on Subtasks 2, 3, and 4 for core verification and can complete after Subtasks 6 through 8 for integration coverage.

## Parallelization Notes

- Subtask 5 can run in parallel with late work in Subtask 4 once the display data format is agreed.
- Subtasks 6 and 7 can run in parallel after Subtasks 1, 2, 4, and 5 are complete.
- Subtask 9 can begin after Subtasks 2, 3, and 4 and continue while Subtasks 6 through 8 are integrated.
- Subtasks 1 through 4 remain on the critical path and should not be split prematurely.

## Key Design Decisions To Resolve During Implementation

1. Resolved: `FWeapon::fire()` should return a structured result object, and the tactical layer should store it. This keeps combat resolution explicit and testable without coupling weapon code directly to `FBattleScreen`.
2. Resolved: `FVehicle::takeDamage()` and `advancedDamage()` should accept an optional output/result parameter describing applied effects. This keeps advanced-damage reporting anchored at the exact point where the damage table outcome is known, rather than reconstructing effects from changed ship state afterward.
3. Resolved: the summary dialog should show only ships that actually sustained damage, along with the damage and effects they took. Ships that were targeted but remained undamaged should be omitted from display.
4. Resolved: per-attack effects should be stored internally in the first version, but the UI should display only the per-ship damage rollup. This preserves detailed tactical data for future expansion while keeping the first dialog focused and readable.
5. Resolved: one shared tactical summary dialog class should handle all tactical report types, with the specific report title and context passed in by the caller. The reports are expected to remain aligned in presentation, so separate dialog variants are not justified.
6. Resolved: destroyed ships should remain on the board while the user reads the relevant report dialog, and `clearDestroyedShips()` should run after the dialog is acknowledged. This is preferred UX and the tactical timing impact is acceptable.

## Implementer Agent Prompts

### Prompt 1: Reporting model

You are the implementer agent.

Allowed files:
- `include/tactical/FBattleScreen.h`
- `src/tactical/FBattleScreen.cpp`
- Any new tactical report model header/source you need under `include/tactical/` and `src/tactical/`

Task:
Define the tactical combat reporting data structures with `FBattleScreen` as the owner of current tactical report state. The result must support per-attack recording, immediate reports for electrical fire and mines, and per-ship summaries for tactical fire phases.

Acceptance criteria:
- There is a clear data model for individual attack results and aggregated ship results.
- `FBattleScreen` exposes APIs to begin a report, append raw events, build a display summary, and clear report state.
- The model is suitable for defensive fire, offensive fire, electrical fire, and mine damage reporting.
- The model preserves raw per-attack/internal event data while supporting a separate per-ship display rollup.
- New reporting types live under `include/tactical/` and `src/tactical/` unless there is a specific, justified need to place them elsewhere.
- No implementation code changes phase behavior beyond setting up this reporting foundation.
- The subtask output is written into an appropriately labeled directory under `artifacts/`, with a name in the style of `tactical-damage-summary_subtask1`.
- The artifact output identifies the exact report-model files created so later subtasks can consume them reliably.

### Prompt 2: Vehicle damage-effects output

You are the implementer agent.

Allowed files:
- `include/ships/FVehicle.h`
- `src/ships/FVehicle.cpp`
- Any new tactical report model files created for Prompt 1

Task:
Update `FVehicle::takeDamage()` and `advancedDamage()` so they populate an optional effects/result output parameter describing the exact applied hull and non-hull effects during damage resolution.

Out of scope for this task:
- Changing `FWeapon::fire()`
- Adding aggregation logic
- Adding dialog or timing integration

Acceptance criteria:
- The implementer inspects the Subtask 1 artifact directory under `artifacts/` to identify the report-model files created there before editing.
- `FVehicle::takeDamage()` and `advancedDamage()` populate an optional effects/result output parameter with all applied hull and advanced-damage effects needed by higher layers.
- Existing combat state mutation behavior remains intact.
- The emitted effect data is explicit enough for higher layers to build tactical reports without reconstructing state changes indirectly.

### Prompt 3: Weapon attack-result return value

You are the implementer agent.

Allowed files:
- `include/weapons/FWeapon.h`
- `src/weapons/FWeapon.cpp`
- `include/ships/FVehicle.h`
- `src/ships/FVehicle.cpp`
- Any new tactical report model files created for Prompt 1

Task:
Update `FWeapon::fire()` so it returns a structured attack-result object using the vehicle damage-effects output added in Prompt 2. Preserve current combat rules while making each resolved attack explicit and storable by the tactical layer.

Out of scope for this task:
- Adding dialog logic
- Adding summary aggregation
- Integrating electrical-fire or mine reporting timing

Acceptance criteria:
- `FWeapon::fire()` returns a structured result object suitable for tactical reporting.
- The returned result distinguishes at least skipped/not-fired, fired-and-missed, and fired-and-hit outcomes.
- A successful hit records both hull damage and non-hull effects actually applied to the target.
- Existing weapon-resolution behavior remains intact.

### Prompt 4: Aggregation

You are the implementer agent.

Allowed files:
- `include/tactical/FBattleScreen.h`
- `src/tactical/FBattleScreen.cpp`
- Any new tactical report model files created for Prompt 1

Task:
Implement the transformation from recorded tactical events into a per-ship summary for any report window supported by this feature. The summary should be suitable for a player-facing dialog, include only ships that actually sustained damage, and retain any raw per-event detail internally rather than displaying it.

Acceptance criteria:
- Multiple attacks against one ship aggregate correctly.
- The resulting summary clearly represents hull damage and other effects per ship.
- Undamaged ships do not appear in the display summary.
- Summary generation does not depend on the target ship still existing on the board after report creation.
- The summary logic is independent of GUI code.

### Prompt 5: Tactical summary dialog

You are the implementer agent.

Allowed files:
- `include/gui/TacticalDamageSummaryGUI.h`
- `src/gui/TacticalDamageSummaryGUI.cpp`
- `src/gui/Makefile`
- `include/tactical/FBattleScreen.h`
- `src/tactical/FBattleScreen.cpp`
- Any new tactical report model files created for Prompt 1

Task:
Create one read-only modal dialog class for tactical damage reports. The dialog should support defensive fire, offensive fire, electrical fire damage, and mine damage summaries, with report type and title passed in by the caller.

Out of scope for this task:
- Owning aggregation logic
- Defining raw report-model event structures
- Integrating timing into fire, mine, or electrical-fire flow

Acceptance criteria:
- The single dialog class can render defensive-fire, offensive-fire, electrical-fire, and mine-damage summaries.
- The dialog displays only the per-ship rollup for damaged ships and does not expose internal per-attack detail.
- The dialog can open with an empty summary without crashing and presents a clear no-damage message.
- The dialog is wired into the GUI build.
- The dialog is read-only and focused on review rather than editing.

### Prompt 6: Electrical-fire integration

You are the implementer agent.

Allowed files:
- `include/tactical/FBattleScreen.h`
- `src/tactical/FBattleScreen.cpp`
- `include/gui/TacticalDamageSummaryGUI.h`
- `src/gui/TacticalDamageSummaryGUI.cpp`
- Any new tactical report model files created for Prompt 1

Task:
Integrate immediate electrical-fire reporting into the start-of-turn flow. Record electrical-fire damage results when `FBattleScreen::applyFireDamage()` runs, show the shared tactical summary dialog before movement begins, and defer destroyed-ship cleanup until after acknowledgement.

Out of scope for this task:
- Integrating mine-damage reporting
- Integrating defensive/offensive fire reporting
- Redefining the shared dialog or summary-generation format

Acceptance criteria:
- Electrical fire shows a summary before movement begins.
- Destroyed ships stay on the board while the relevant dialog is open and still appear in the summary for their final report.
- `clearDestroyedShips()` is deferred until after the dialog is acknowledged.
- Summary state is cleared after acknowledgement.
- Tactical movement does not begin until the dialog is closed.
- After the electrical-fire dialog is dismissed, control returns to the same moving player and the movement phase begins normally.

### Prompt 7: Mine-damage integration

You are the implementer agent.

Allowed files:
- `include/tactical/FBattleBoard.h`
- `src/tactical/FBattleBoard.cpp`
- `include/tactical/FBattleScreen.h`
- `src/tactical/FBattleScreen.cpp`
- `include/gui/TacticalDamageSummaryGUI.h`
- `src/gui/TacticalDamageSummaryGUI.cpp`
- Any new tactical report model files created for Prompt 1

Task:
Integrate immediate mine-damage reporting into the post-movement flow. Record mine-damage results when `FBattleBoard::applyMineDamage()` runs, show the shared tactical summary dialog immediately after mine damage is applied, and defer destroyed-ship cleanup until after acknowledgement.

Out of scope for this task:
- Modifying defensive/offensive fire handlers
- Redefining the shared dialog or summary-generation format

Acceptance criteria:
- Mine damage shows a summary immediately after mine damage is applied.
- The shared tactical summary dialog is used for the report.
- Destroyed ships stay on the board while the relevant dialog is open and still appear in the summary for their final report.
- `clearDestroyedShips()` is deferred until after the dialog is acknowledged.
- Mine damage is applied exactly once per triggering movement event, and the dialog appears only after mine-damage resolution completes.

### Prompt 8: Defensive/offensive fire integration

You are the implementer agent.

Allowed files:
- `include/tactical/FBattleDisplay.h`
- `src/tactical/FBattleDisplay.cpp`
- `include/tactical/FBattleScreen.h`
- `src/tactical/FBattleScreen.cpp`
- `include/gui/TacticalDamageSummaryGUI.h`
- `src/gui/TacticalDamageSummaryGUI.cpp`
- Any new tactical report model files created for Prompt 1

Task:
Integrate end-of-phase reporting into defensive and offensive fire. Show the shared tactical summary dialog after fire resolution, before advancing phases, and defer destroyed-ship cleanup until after acknowledgement.

Out of scope for this task:
- Redefining raw result capture
- Redefining aggregation logic
- Creating a second dialog variant

Acceptance criteria:
- Defensive fire shows a summary before transitioning to offensive fire.
- Offensive fire shows a summary before transitioning back to movement or switching the moving player.
- Destroyed ships stay on the board while the relevant dialog is open and still appear in the summary for their final report.
- `clearDestroyedShips()` is deferred until after the dialog is acknowledged.
- Summary state is cleared after acknowledgement.
- After defensive-fire acknowledgement, the battle advances to offensive fire.
- After offensive-fire acknowledgement, the battle returns to movement and toggles the moving player exactly once.

### Prompt 9: Tests

You are the implementer agent.

Allowed files:
- `tests/ships/FVehicleTest.h`
- `tests/ships/FVehicleTest.cpp`
- `tests/weapons/FWeaponTest.cpp`
- `tests/weapons/FDisruptorCannonTest.cpp`
- Any new test files under `tests/ships/`, `tests/weapons/`, or `tests/tactical/`
- Any build files under `tests/` needed to compile the new tests

Task:
Add automated coverage for the tactical damage reporting and summary behavior introduced by this feature. Favor deterministic tests around result capture and aggregation over fragile GUI automation.

Acceptance criteria:
- Tests cover result capture for at least one successful hit and one non-hull advanced damage effect.
- Tests cover immediate-report creation for electrical fire and mine damage.
- Tests cover aggregation of multiple attacks onto one target ship.
- Tests verify that undamaged ships are omitted from the display summary.
- Tests verify that destroyed ships remain represented in the phase summary and are eligible for delayed cleanup after acknowledgement.
- Tests are organized to cover model/effect capture, aggregation/filtering, and integration sequencing behavior.
- New tactical-report and aggregation tests should prefer new files under `tests/tactical/` where practical, rather than overloading unrelated ship or weapon tests.
- Tests integrate with the existing CppUnit-based test setup as far as practical.

## Output Artifact Path

- `plans/tactical-damage-summary-plan.md`
