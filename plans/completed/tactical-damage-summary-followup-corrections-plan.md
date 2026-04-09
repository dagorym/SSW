# Tactical Damage Summary Follow-Up Corrections Plan

## Feature Restatement

Correct the tactical damage summary contract in three places:

1. align the redundant-hit-note suppression with the actual emitted note text `Attack hit target` (lowercase `target`);
2. render defensive hit rollups with defense abbreviations such as `MS` and `PS` instead of long names such as `Masking Screen` and `Proton Screen`; and
3. repair the stale design-note section identified by the reviewer so repository documentation describes the shipped summary contract consistently.

## Confirmed Repository Facts

- [`src/weapons/FWeapon.cpp`](/home/tstephen/repos/SSW/src/weapons/FWeapon.cpp) currently emits the hit note `Attack hit target` when an attack lands.
- [`include/tactical/FTacticalCombatReport.h`](/home/tstephen/repos/SSW/include/tactical/FTacticalCombatReport.h) still treats only `Attack hit Target` as the redundant placeholder note when building hit-detail lines.
- The tactical damage summary ship-rollup formatting in [`include/tactical/FTacticalCombatReport.h`](/home/tstephen/repos/SSW/include/tactical/FTacticalCombatReport.h) currently uses `damagedDefenseName`, producing output like `Defense Hit: Masking Screen`.
- Defense model types already expose abbreviated names through [`include/defenses/FDefense.h`](/home/tstephen/repos/SSW/include/defenses/FDefense.h) and concrete defense classes already define abbreviations such as `MS` and `PS`.
- Existing tactical regression coverage already asserts the current summary strings in [`tests/tactical/FTacticalCombatReportTest.cpp`](/home/tstephen/repos/SSW/tests/tactical/FTacticalCombatReportTest.cpp) and [`tests/tactical/FTacticalDamageSummaryGUITest.cpp`](/home/tstephen/repos/SSW/tests/tactical/FTacticalDamageSummaryGUITest.cpp).
- The reviewer flagged a stale earlier design-note section in [`doc/DesignNotes.md`](/home/tstephen/repos/SSW/doc/DesignNotes.md) because it still says defense wording relies on label/detail text even though the later section documents structured defense-name rollups.

## Assumptions

- This follow-up should preserve the existing emitted attack note text in [`src/weapons/FWeapon.cpp`](/home/tstephen/repos/SSW/src/weapons/FWeapon.cpp) and fix the suppression logic to match it, rather than normalizing the weapon layer to a different capitalization.
- Defense abbreviations for the summary should come from structured defense identity already carried on report events, not from brittle string shortening of long names.
- No GUI layout or dialog changes are required; the GUI continues to render summary strings produced by the tactical report builder.

## Files To Modify

- [`include/tactical/FTacticalCombatReport.h`](/home/tstephen/repos/SSW/include/tactical/FTacticalCombatReport.h)
- [`tests/tactical/FTacticalCombatReportTest.cpp`](/home/tstephen/repos/SSW/tests/tactical/FTacticalCombatReportTest.cpp)
- [`tests/tactical/FTacticalDamageSummaryGUITest.cpp`](/home/tstephen/repos/SSW/tests/tactical/FTacticalDamageSummaryGUITest.cpp)
- [`doc/DesignNotes.md`](/home/tstephen/repos/SSW/doc/DesignNotes.md)

Possible-only-if-needed:

- [`src/weapons/FWeapon.cpp`](/home/tstephen/repos/SSW/src/weapons/FWeapon.cpp) if the implementer decides the repository-wide note source should also be normalized, though the current evidence suggests a report-side fix is sufficient.

## Overall Documentation Impact

Update [`doc/DesignNotes.md`](/home/tstephen/repos/SSW/doc/DesignNotes.md) so both the earlier tactical-report metadata section and the later tactical-damage-summary section describe the same final player-facing contract: defensive hit rollups use abbreviations, and the redundant lowercase `Attack hit target` note is suppressed from hit-detail output.

## Subtasks

### Subtask 1: Correct redundant hit-note suppression to the actual emitted note text

Adjust the hit-detail formatting contract so the report builder suppresses the real placeholder note text currently emitted by weapon fire, and tighten regressions so the case-sensitive note mismatch cannot recur.

#### Acceptance Criteria

- Hit-detail formatting suppresses the actual redundant note `Attack hit target`.
- Tactical regressions assert against the lowercase emitted note text rather than the obsolete `Attack hit Target` variant.
- Meaningful non-placeholder notes still remain visible in hit-detail output.
- No other hit-detail wording regresses.

#### Documentation Impact

Update the design-note wording that describes hit-detail note suppression so it references the actual lowercase placeholder text and does not document the obsolete capitalization.

### Subtask 2: Change defensive ship-rollup entries to abbreviations

Rework defensive-hit rollup formatting so the player-facing ship summary uses defense abbreviations derived from structured defense identity, producing entries such as `Defense Hit: MS` and `Defense Hit: PS` instead of long-form names.

#### Acceptance Criteria

- Ship rollups format defensive hits with abbreviations, preserving order and duplicates where applicable.
- The abbreviation source is tied to structured defense identity, not ad hoc string replacement of long names.
- Existing weapon-hit abbreviation formatting remains unchanged.
- Tactical regressions cover at least `MS` and `PS` summary output, plus a defense-only rollup case.

#### Documentation Impact

Update design-note text that currently describes named defense rollups so it instead documents abbreviation-based defensive hit output.

### Subtask 3: Repair the stale tactical-report design notes called out by review

Clean up the inconsistent sections in [`doc/DesignNotes.md`](/home/tstephen/repos/SSW/doc/DesignNotes.md) so the earlier metadata section and later damage-summary section both describe the same implemented contract after Subtasks 1 and 2 land.

#### Acceptance Criteria

- The earlier tactical-report metadata section no longer says the current player-facing defense wording still relies on label/detail text.
- The later tactical-damage-summary section matches the final implemented contract for defense abbreviations and redundant-hit-note suppression.
- Documentation no longer contains conflicting descriptions of the same tactical damage summary behavior.

#### Documentation Impact

This subtask is itself the documentation repair. No additional external documentation is expected beyond [`doc/DesignNotes.md`](/home/tstephen/repos/SSW/doc/DesignNotes.md).

## Dependency Ordering

1. Subtask 1
2. Subtask 2
3. Subtask 3

Parallelization guidance:

- Keep the work sequential.
- Subtasks 1 and 2 both touch [`include/tactical/FTacticalCombatReport.h`](/home/tstephen/repos/SSW/include/tactical/FTacticalCombatReport.h) and the same tactical regression files.
- Subtask 3 should follow the final runtime contract so the repaired design notes document shipped behavior rather than an intermediate state.

## Implementer Agent Prompts

### Implementer Prompt: Subtask 1

You are the implementer agent.

Allowed files:

- `include/tactical/FTacticalCombatReport.h`
- `tests/tactical/FTacticalCombatReportTest.cpp`
- `tests/tactical/FTacticalDamageSummaryGUITest.cpp`
- `doc/DesignNotes.md` only if a small wording correction is needed in the same change

Task:

Fix the tactical hit-detail redundant-note suppression so it matches the actual emitted placeholder text `Attack hit target` from weapon fire. Update the tactical regressions so they assert the correct lowercase contract and still prove that meaningful non-placeholder notes survive.

Acceptance criteria:

- `isRedundantHitDetailNote(...)` or its equivalent suppresses `Attack hit target`.
- Tactical report and GUI-facing summary tests no longer rely on the obsolete `Attack hit Target` spelling.
- Meaningful note text still appears in hit-detail output.
- No unrelated hit-detail summary wording changes are introduced.

Validation:

- `cd tests/tactical && make && ./TacticalTests`

Expected Tester file locations:

- `tests/tactical/FTacticalCombatReportTest.cpp`
- `tests/tactical/FTacticalDamageSummaryGUITest.cpp`

Artifact directory:

- Default to `artifacts/tactical-damage-summary-followup-corrections/subtask-1` if no shared artifact path is provided by orchestration context.

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
- `doc/DesignNotes.md` only if tightly coupled wording needs to be updated alongside the behavior

Task:

Change tactical damage summary defensive-hit rollups from long-form defense names to defense abbreviations such as `MS` and `PS`. Use structured defense identity already present on report events as the source of truth, and refresh tactical regressions to lock in abbreviation-based rollups without disturbing existing weapon-hit abbreviation behavior.

Acceptance criteria:

- Defensive-hit ship rollups render abbreviations instead of long names.
- The implementation derives abbreviations from structured defense identity rather than brittle string trimming.
- Weapon-hit rollups remain unchanged.
- Tactical regressions cover abbreviation output for multiple defense types, including `MS` and `PS`.

Validation:

- `cd tests/tactical && make && ./TacticalTests`

Expected Tester file locations:

- `tests/tactical/FTacticalCombatReportTest.cpp`
- `tests/tactical/FTacticalDamageSummaryGUITest.cpp`

Artifact directory:

- Default to `artifacts/tactical-damage-summary-followup-corrections/subtask-2` if no shared artifact path is provided by orchestration context.

Execution instruction:

- Begin implementation immediately if no blocking inputs are missing. Do not stop after a preflight restatement.

Completion gate:

- Do not report success unless all required artifacts exist and all changes are committed.

### Implementer Prompt: Subtask 3

You are the implementer agent.

Allowed files:

- `doc/DesignNotes.md`

Task:

Repair the stale and conflicting tactical damage summary documentation identified in the reviewer report. Update both the earlier tactical-report metadata section and the later tactical-damage-summary section so they consistently describe the final shipped contract after the follow-up corrections: defense abbreviations in ship rollups and suppression of the redundant lowercase `Attack hit target` note in hit-detail output.

Acceptance criteria:

- The stale earlier design-note wording about defense rollups relying on label/detail text is removed or corrected.
- The later design-note wording matches the final abbreviation-based defense rollup and lowercase-note suppression contract.
- [`artifacts/tactical-damage-summary-followup/reviewer_report.md`](/home/tstephen/repos/SSW/artifacts/tactical-damage-summary-followup/reviewer_report.md) is fully addressed in the updated design notes.

Validation:

- No dedicated build command is required beyond verifying the edited markdown content is internally consistent.
- If repository workflow expects a broader confidence check, assume `cd tests/tactical && make && ./TacticalTests` remains the relevant validation scope because the documented contract is tied to those regressions.

Expected Tester file locations:

- No new dedicated tester file is expected for this documentation-only follow-up unless orchestration requires a documentation verification artifact.

Artifact directory:

- Default to `artifacts/tactical-damage-summary-followup-corrections/subtask-3` if no shared artifact path is provided by orchestration context.

Execution instruction:

- Begin implementation immediately if no blocking inputs are missing. Do not stop after a preflight restatement.

Completion gate:

- Do not report success unless all required artifacts exist and all changes are committed.

## Output Artifact Path

- [`plans/tactical-damage-summary-followup-corrections-plan.md`](/home/tstephen/repos/SSW/plans/tactical-damage-summary-followup-corrections-plan.md)
