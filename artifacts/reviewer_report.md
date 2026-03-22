Reviewer Report

Feature plan reviewed:
- `plans/tactical-damage-summary-plan.md`

Commit scope reviewed:
- All commits from `37459eb5cf3238925b4efef9cf5847d9a68e4cc8` through `HEAD` (`1b45f171d9b5a45ec7b288ccde2b830e22ed15a9`), inclusive of the starting commit.

Inputs reviewed:
- `artifacts/tactical-damage-summary_subtask1` through `artifacts/tactical-damage-summary_subtask9`
- `artifacts/tactical-damage-summary_subtask7-remediation`
- Tactical reporting, dialog, fire-flow, mine-flow, weapon, vehicle, and tactical test changes in the reviewed commit range

Overall feature completeness:
- The feature is close to plan-complete: the reporting model exists, weapon and vehicle resolution emit structured data, the shared dialog is wired in, immediate electrical-fire and mine reports are integrated, defensive/offensive fire reports are integrated, and tactical tests were added.
- The delivered feature is not safe to treat as complete because the aggregation layer can overstate hull damage for attack-driven reports. That is a core correctness failure for the player-visible summary dialog.

Findings

BLOCKING
- Hull damage can be counted twice in the ship rollup for weapon and mine attacks. `buildTacticalCombatReportSummary()` adds `attack.hullDamage` to `hullDamageTaken` and then adds `event.hullDamage` again for each internal event ([include/tactical/FTacticalCombatReport.h](/home/tstephen/repos/SSW/include/tactical/FTacticalCombatReport.h#L397), [include/tactical/FTacticalCombatReport.h](/home/tstephen/repos/SSW/include/tactical/FTacticalCombatReport.h#L416)). The attack builders populate both fields from the same resolution payload ([src/tactical/FBattleDisplay.cpp](/home/tstephen/repos/SSW/src/tactical/FBattleDisplay.cpp#L70), [src/tactical/FBattleDisplay.cpp](/home/tstephen/repos/SSW/src/tactical/FBattleDisplay.cpp#L79), [src/tactical/FBattleBoard.cpp](/home/tstephen/repos/SSW/src/tactical/FBattleBoard.cpp#L49), [src/tactical/FBattleBoard.cpp](/home/tstephen/repos/SSW/src/tactical/FBattleBoard.cpp#L56)). At feature level this means the dialog can report more hull damage than the combat system actually applied, which directly violates the plan’s requirement to show what damage each ship took during the report window.

WARNING
- The tactical tests missed the duplicate-count path because the aggregation assertions only use synthetic attack records without hull-damage internal events ([tests/tactical/FTacticalCombatReportTest.cpp](/home/tstephen/repos/SSW/tests/tactical/FTacticalCombatReportTest.cpp#L235), [tests/tactical/FTacticalCombatReportTest.cpp](/home/tstephen/repos/SSW/tests/tactical/FTacticalCombatReportTest.cpp#L266)). The fire-flow and mine-flow tests are mostly sequencing/source-inspection checks, so this regression could ship even though the artifact verdicts are green. The plan explicitly called for deterministic coverage of result capture and aggregation behavior, and this gap leaves the main user-visible summary correctness under-tested.

NOTE
- Subtask coverage and verifier outcomes were generally strong after the mine-flow remediation, and the remaining issues I found are concentrated in the final aggregation layer rather than missing subtask integration.

Missed functionality and edge cases:
- Attack-driven summaries need a single canonical source of hull-damage accounting so one hit contributes exactly once to the target ship’s displayed hull total.
- Automated coverage is missing for the realistic case where an attack report contains both aggregate hull damage and per-effect internal events derived from the same shot.

Follow-up feature requests for planning:
- Update tactical combat summary aggregation so attack-level hull damage and per-effect event data cannot double-count the same applied hull damage in player-facing ship rollups for weapon fire and mine damage.
- Add deterministic tactical tests that build summaries from real attack-result shaped data, including hull-damage effects inside `internalEvents`, and assert the displayed hull totals remain equal to the actual applied damage.

Final outcome:
- FAIL
