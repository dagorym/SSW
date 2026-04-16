# UI Dialog Sizing and Centering Follow-Up Plan

## Feature Restatement
Repair the blocking BattleSim modal-lifecycle regression introduced during the UI dialog sizing/centering cleanup, and complete the requested follow-up by revalidating the BattleSim tactical launch path while preserving the new sizing and centering behavior.

## Confirmed Repository Facts
- The reviewer report at `/home/tstephen/repos/SSW/artifacts/ui-dialog-sizing-centering/reviewer_report.md` identifies one blocking regression: `ScenarioDialog` no longer preserves the existing hide/show lifecycle around the four tactical scenario launches.
- The current `ScenarioDialog` implementation in `/home/tstephen/repos/SSW/src/battleSim/ScenarioDialog.cpp` still launches `FBattleScreen bb;` on the stack in each scenario handler, but each handler now calls only `bb.ShowModal();` and no longer contains surrounding `Hide();` / `Show();` calls.
- The tactical regression suite explicitly locks the existing BattleSim modal contract in `/home/tstephen/repos/SSW/tests/tactical/FTacticalBattleScreenElectricalFireTest.cpp` by asserting four `Hide();`, four `Show();`, and four `bb.ShowModal();` occurrences in `ScenarioDialog.cpp`.
- `ScenarioEditorGUI` still preserves this supported pattern in `/home/tstephen/repos/SSW/src/battleSim/ScenarioEditorGUI.cpp` by calling `Hide();`, then `bb.ShowModal();`, then its post-battle finalization path.
- The BattleSim live GUI fixture in `/home/tstephen/repos/SSW/tests/gui/BattleSimGuiLiveTest.cpp` already covers BattleSim launch-path lifecycle behavior and first-show sizing contracts for `LocalGameDialog`, `ScenarioDialog`, and `ScenarioEditorGUI`.
- The current BattleSim launch-flow documentation in `/home/tstephen/repos/SSW/doc/DesignNotes.md` describes the modal `FBattleScreen` launch chain and says the BattleSim tests preserve lifecycle coverage.
- The original sizing/centering feature plan in `/home/tstephen/repos/SSW/plans/ui-dialog-sizing-centering-plan.md` Subtask 3 explicitly required preserving existing modal ownership/lifecycle behavior while remediating sizing issues.

## Assumptions
- No broader BattleSim launch redesign is desired for this follow-up; the user selected restoration of the existing `Hide(); ... bb.ShowModal(); ... Show();` contract.
- The only required production change is in `ScenarioDialog.cpp` unless revalidation exposes a second issue.
- Documentation changes may be unnecessary if the shipped documentation already reflects the intended contract, but the follow-up must review and update any stale wording or artifact notes if implementation or validation evidence requires it.

## Files To Modify
- Likely production files:
  - `/home/tstephen/repos/SSW/src/battleSim/ScenarioDialog.cpp`
- Likely test files:
  - `/home/tstephen/repos/SSW/tests/gui/BattleSimGuiLiveTest.cpp`
  - `/home/tstephen/repos/SSW/tests/tactical/FTacticalBattleScreenElectricalFireTest.cpp`
- Likely documentation files to review and update only if needed:
  - `/home/tstephen/repos/SSW/doc/DesignNotes.md`
  - `/home/tstephen/repos/SSW/AGENTS.md`
- Likely artifact outputs to refresh during orchestration:
  - `/home/tstephen/repos/SSW/artifacts/ui-dialog-sizing-centering/`

## Overall Documentation Impact
- Review `doc/DesignNotes.md` to ensure the documented BattleSim modal-launch contract still matches the restored implementation and validation story.
- Review `AGENTS.md` only if its BattleSim GUI coverage summary needs adjustment to reflect any strengthened lifecycle assertions.
- Refresh follow-up artifact reports under `artifacts/ui-dialog-sizing-centering/` if orchestration for this story records tester/documenter/verifier outputs there.

## Subtasks

### Subtask 1: Restore the `ScenarioDialog` modal hide/show lifecycle around tactical launches
Scope:
- Update the four predefined-scenario handlers in `ScenarioDialog.cpp` to restore the supported parent-dialog lifecycle around `FBattleScreen` launches.
- Preserve the current sizing and centering constructor behavior added by the previous feature.
- Keep the caller-owned stack `FBattleScreen` contract unchanged.

Likely files:
- `/home/tstephen/repos/SSW/src/battleSim/ScenarioDialog.cpp`
- `/home/tstephen/repos/SSW/tests/tactical/FTacticalBattleScreenElectricalFireTest.cpp`
- `/home/tstephen/repos/SSW/tests/gui/BattleSimGuiLiveTest.cpp`

Acceptance Criteria:
- Each of the four `ScenarioDialog` scenario handlers hides the dialog before `bb.ShowModal()` and restores the dialog afterward with `Show()`.
- `ScenarioDialog` continues to use stack-owned `FBattleScreen bb;` launch sites rather than heap allocation or non-modal launch paths.
- The existing first-show sizing and centering contract in `ScenarioDialog` remains intact.
- Tactical regression coverage continues to assert the supported BattleSim modal lifecycle contract and matches the restored implementation.
- BattleSim live GUI coverage still exercises scenario launch lifecycle behavior without introducing orphaned top-level windows.

Documentation Impact:
- No standalone documentation update is expected from the code restoration alone unless a test or review note now describes the lifecycle more explicitly.

### Subtask 2: Revalidate the BattleSim tactical launch path and align any affected docs or artifacts
Scope:
- Run the reviewer-requested validation for the restored launch path using the tactical and GUI suites named in the report.
- Update any affected documentation or follow-up artifacts only if the restored behavior or stronger validation evidence changes what is currently recorded.
- Keep this subtask focused on closing the reviewer-requested follow-up, not broadening into unrelated BattleSim GUI changes.

Likely files:
- `/home/tstephen/repos/SSW/tests/tactical/FTacticalBattleScreenElectricalFireTest.cpp`
- `/home/tstephen/repos/SSW/tests/gui/BattleSimGuiLiveTest.cpp`
- `/home/tstephen/repos/SSW/doc/DesignNotes.md`
- `/home/tstephen/repos/SSW/AGENTS.md`
- `/home/tstephen/repos/SSW/artifacts/ui-dialog-sizing-centering/` (artifact reports only)

Acceptance Criteria:
- The follow-up validation includes `cd tests/tactical && make && ./TacticalTests`.
- The follow-up validation includes `cd tests/gui && make && ./GuiTests`, or the closest equivalent command supported by the environment if display tooling constraints require an explicitly documented fallback.
- Validation results confirm the restored `ScenarioDialog` lifecycle no longer fails `FTacticalBattleScreenElectricalFireTest::testBattleSimLaunchPathUsesModalDialogFlowIntoBattleScreen`.
- Any documentation touched by this story accurately describes the shipped BattleSim modal-launch and validation contract after the fix.
- Any refreshed follow-up artifacts clearly record what was run, the result, and any remaining non-blocking limitations without overstating coverage.

Documentation Impact:
- `doc/DesignNotes.md` may need a narrow wording update if the follow-up records stronger or more explicit lifecycle-validation language than the current text.
- `AGENTS.md` should only change if the GUI/tactical test coverage summary needs to mention the restored BattleSim lifecycle assertion more precisely.
- Artifact reports under `artifacts/ui-dialog-sizing-centering/` are expected to be updated by downstream workflow stages for this follow-up.

## Dependency Ordering
1. Subtask 1 first.
   Reason: the code contract has to be restored before the requested tactical and GUI revalidation can confirm the reviewer finding is closed.
2. Subtask 2 second.
   Reason: validation and any resulting documentation/artifact alignment depend on the restored launch behavior being in place.

Parallelization guidance:
- Keep these subtasks sequential. They share the same BattleSim launch-path contract, and Subtask 2 is explicitly validating the outcome of Subtask 1 rather than implementing an independent surface.

## Implementer Agent Prompts

### Implementer Prompt: Subtask 1
You are the implementer agent

Allowed files:
- `/home/tstephen/repos/SSW/src/battleSim/ScenarioDialog.cpp`
- `/home/tstephen/repos/SSW/tests/tactical/FTacticalBattleScreenElectricalFireTest.cpp`
- `/home/tstephen/repos/SSW/tests/gui/BattleSimGuiLiveTest.cpp`

Task:
- Restore the supported BattleSim modal lifecycle in the four predefined-scenario handlers in `ScenarioDialog.cpp`.
- Preserve the existing caller-owned stack `FBattleScreen bb;` pattern and the sizing/centering constructor changes already shipped for `ScenarioDialog`.
- Update regression coverage only as needed to keep the tactical and live GUI tests aligned with the restored behavior.

Acceptance criteria:
- All four `ScenarioDialog` tactical launch handlers hide the dialog before `bb.ShowModal()` and show it again afterward.
- The implementation keeps `FBattleScreen bb;` on the stack and does not introduce `new FBattleScreen(...)`, `bb.Show()`, or other ownership-pattern changes.
- `ScenarioDialog` still retains its current first-show sizing contract (`Fit(...)` plus `SetMinSize(GetBestSize())`) and centering behavior.
- The tactical launch-path regression in `FTacticalBattleScreenElectricalFireTest` remains aligned with the restored modal contract.
- Any GUI lifecycle assertion changes remain scoped to this BattleSim launch path and do not broaden the story.

Validation guidance:
- Smallest relevant validation command: `cd tests/tactical && make && ./TacticalTests`
- If a narrower iteration command is used first, treat the full tactical module run above as the required acceptance validation.

Expected Tester file locations:
- `/home/tstephen/repos/SSW/tests/tactical/FTacticalBattleScreenElectricalFireTest.cpp`
- `/home/tstephen/repos/SSW/tests/gui/BattleSimGuiLiveTest.cpp`

Artifact directory guidance:
- Default to `/home/tstephen/repos/SSW/artifacts/ui-dialog-sizing-centering/followup-modal-lifecycle` unless orchestration provides a shared artifact directory.

Execution instruction:
- Begin implementation immediately when the prompt contains the required blocking inputs, and do not stop after preflight restatement alone.

Completion gate:
- Do not report success unless all required artifacts exist and all changes are committed.

### Implementer Prompt: Subtask 2
You are the implementer agent

Allowed files:
- `/home/tstephen/repos/SSW/tests/tactical/FTacticalBattleScreenElectricalFireTest.cpp`
- `/home/tstephen/repos/SSW/tests/gui/BattleSimGuiLiveTest.cpp`
- `/home/tstephen/repos/SSW/doc/DesignNotes.md`
- `/home/tstephen/repos/SSW/AGENTS.md`
- `/home/tstephen/repos/SSW/artifacts/ui-dialog-sizing-centering/`

Task:
- Revalidate the restored BattleSim launch path with the tactical and GUI suites requested by the reviewer report.
- If the restored behavior requires wording cleanup in `DesignNotes.md` or `AGENTS.md`, make only the minimal documentation changes needed to keep the repo’s documented contract accurate.
- Record the validation outcome in the story artifacts without expanding the scope into unrelated GUI cleanup.

Acceptance criteria:
- The follow-up runs `cd tests/tactical && make && ./TacticalTests`.
- The follow-up runs `cd tests/gui && make && ./GuiTests`, or clearly documents the closest supported fallback command when environment display constraints prevent the canonical command.
- Validation confirms that the BattleSim modal-launch lifecycle regression is fixed.
- Any documentation edits remain narrowly scoped to the restored launch contract or the exact validation story.
- Follow-up artifacts capture commands run, results observed, and any remaining non-blocking environment limitations accurately.

Validation guidance:
- Required validation commands:
  - `cd tests/tactical && make && ./TacticalTests`
  - `cd tests/gui && make && ./GuiTests`
- If GUI execution requires a display-specific fallback in the actual environment, record that fallback explicitly as an assumption and include the observed result.

Expected Tester file locations:
- `/home/tstephen/repos/SSW/tests/tactical/FTacticalBattleScreenElectricalFireTest.cpp`
- `/home/tstephen/repos/SSW/tests/gui/BattleSimGuiLiveTest.cpp`

Artifact directory guidance:
- Default to `/home/tstephen/repos/SSW/artifacts/ui-dialog-sizing-centering/followup-modal-lifecycle` unless orchestration provides a shared artifact directory.

Execution instruction:
- Begin implementation immediately when the prompt contains the required blocking inputs, and do not stop after preflight restatement alone.

Completion gate:
- Do not report success unless all required artifacts exist and all changes are committed.

## Output Markdown File
- `/home/tstephen/repos/SSW/plans/ui-dialog-sizing-centering-followup-plan.md`
