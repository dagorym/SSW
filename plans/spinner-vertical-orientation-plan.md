# Spinner Vertical Orientation Plan

## Request Restatement
Change all value-selection spinner inputs that currently render with horizontal plus/minus buttons so they use vertical spinner controls instead, and cover the affected repository surfaces with regression checks.

## Confirmed Repository Facts
- Production `wxSpinCtrl` construction appears in exactly two code paths:
- `src/tactical/FBattleDisplay.cpp` creates the tactical speed control with `wxSP_ARROW_KEYS`.
- `src/gui/ICMSelectionGUI.cpp` creates the per-ship ICM allocation controls with `wxSP_ARROW_KEYS`.
- Existing regression coverage already touches both surfaces:
- `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp` source-inspects the `FBattleDisplay` constructor.
- `tests/gui/TacticalGuiLiveTest.cpp` exercises the tactical speed control and the rebuilt ICM allocation controls.
- `AGENTS.md` already documents `wxSpinCtrl` sizing/layout conventions on wxGTK, so any new explicit spinner-orientation convention likely belongs there as well.
- Local wxWidgets 3.3.1 headers available in this environment define `wxSP_VERTICAL` for spin controls/spin buttons, so the most likely remediation is to make the style explicit rather than replacing the controls wholesale.

## Likely Files To Modify
- `src/tactical/FBattleDisplay.cpp`
- `src/gui/ICMSelectionGUI.cpp`
- `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`
- `tests/gui/TacticalGuiLiveTest.cpp`
- `AGENTS.md`

## Overall Documentation Impact
- Update `AGENTS.md` to extend the existing wxGTK `wxSpinCtrl` guidance from sizing-only to sizing plus explicit vertical-arrow orientation for value-selection inputs.
- No end-user gameplay documentation is likely affected unless implementation reveals screenshots or UI guidance outside current developer-facing docs.

## Subtasks

### Subtask 1: Audit and remediate tactical speed spinner orientation
Scope:
- Update the tactical speed `wxSpinCtrl` in `FBattleDisplay` so its style explicitly requests vertical spinner arrows while preserving the existing sizer-managed sizing/layout contract.

Acceptance Criteria:
- `src/tactical/FBattleDisplay.cpp` no longer constructs the tactical speed control with only `wxSP_ARROW_KEYS`; the style explicitly includes the vertical spinner orientation.
- The existing sizing/layout safeguards remain intact, including `wxDefaultPosition`, `wxDefaultSize`, and `SetMinSize(GetBestSize())`.
- Tactical source-level regression coverage is updated so future changes cannot silently drop the explicit vertical style from the speed control.

Documentation Impact:
- No subtask-specific user docs expected.
- Developer-facing convention text may need to mention the tactical speed spinner as one of the controls that must remain explicitly vertical on wxGTK.

Implementer Prompt:
You are the implementer agent.

Allowed files:
- `src/tactical/FBattleDisplay.cpp`
- `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`

Task:
- Update the tactical speed spinner construction in `FBattleDisplay` to explicitly request vertical spinner arrows and preserve the current sizer-managed layout/sizing behavior.
- Extend the tactical regression coverage so it asserts the constructor keeps the explicit vertical style in addition to the current layout contract.

Acceptance criteria:
- The tactical speed control style explicitly includes vertical spinner orientation.
- Existing layout safeguards for the speed control remain in place.
- Tactical regression coverage fails if the constructor drops the explicit vertical style in the future.

Validation guidance:
- Smallest relevant validation command: `cd tests/tactical && make && ./TacticalTests`
- If tactical-only validation is insufficient because the assertions live in the combined runner, fallback assumption: `cd tests && make && ./SSWTests`

Tester guidance:
- Expected test location: `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`

Artifact guidance:
- If orchestration does not provide an artifact directory, default to `artifacts/spinner-vertical-orientation/subtask-1`.

Execution instruction:
- Begin implementation immediately if the repository state contains the files above and no blocking ambiguity remains; do not stop after preflight restatement alone.

Completion gate:
- Do not report success unless all required artifacts exist and all changes are committed.

### Subtask 2: Audit and remediate ICM allocation spinner orientation
Scope:
- Update the dynamic ICM allocation `wxSpinCtrl` instances in `ICMSelectionGUI` so rebuilt value-selection inputs explicitly request vertical spinner arrows without regressing the dialog’s post-rebuild fitting behavior.

Acceptance Criteria:
- `src/gui/ICMSelectionGUI.cpp` no longer constructs the dynamic ICM allocation controls with only `wxSP_ARROW_KEYS`; the style explicitly includes the vertical spinner orientation.
- The dynamic control rebuild path still re-fits the dialog and preserves valid in-client geometry for the rebuilt controls.
- GUI regression coverage is updated so future changes cannot silently revert the ICM allocation controls to non-explicit orientation styling.

Documentation Impact:
- No subtask-specific end-user documentation expected.
- Developer-facing convention text may need to mention dynamically rebuilt dialog spin controls, not only the tactical speed control.

Implementer Prompt:
You are the implementer agent.

Allowed files:
- `src/gui/ICMSelectionGUI.cpp`
- `tests/gui/TacticalGuiLiveTest.cpp`

Task:
- Update the dynamic ICM allocation spinner construction in `ICMSelectionGUI` to explicitly request vertical spinner arrows while preserving the current rebuild, fit, and modal-finalization behavior.
- Extend the GUI regression coverage so it proves the rebuilt controls retain explicit vertical style along with their current positive-geometry and in-client visibility guarantees.

Acceptance criteria:
- Each rebuilt ICM allocation spinner explicitly includes vertical spinner orientation in its style.
- The dialog rebuild flow still fits correctly and keeps the controls within the client area.
- GUI regression coverage fails if the ICM spinner style loses the explicit vertical orientation in the future.

Validation guidance:
- Smallest relevant validation command: `cd tests/gui && make && xvfb-run -a ./GuiTests`
- Fallback assumption when headless GUI execution is unavailable: `cd tests && make gui-tests`

Tester guidance:
- Expected test location: `tests/gui/TacticalGuiLiveTest.cpp`

Artifact guidance:
- If orchestration does not provide an artifact directory, default to `artifacts/spinner-vertical-orientation/subtask-2`.

Execution instruction:
- Begin implementation immediately if the repository state contains the files above and no blocking ambiguity remains; do not stop after preflight restatement alone.

Completion gate:
- Do not report success unless all required artifacts exist and all changes are committed.

### Subtask 3: Document the explicit vertical-spinner convention
Scope:
- Update repository guidance so future wxGTK/value-selection spinner work keeps explicit vertical arrow styling alongside the existing size/layout expectations.

Acceptance Criteria:
- `AGENTS.md` documents that value-selection `wxSpinCtrl` usage should explicitly request vertical spinner orientation where these controls are used.
- The documentation update remains consistent with the already-documented sizing/layout guidance and existing GUI/tactical regression summary language.

Documentation Impact:
- This subtask is itself the documentation deliverable.

Implementer Prompt:
You are the implementer agent.

Allowed files:
- `AGENTS.md`

Task:
- Extend the repository’s existing wxGTK `wxSpinCtrl` guidance to note that value-selection spinner controls should explicitly request vertical orientation, and adjust the test-summary text only as needed to reflect any new regression assertions added by the implementation subtasks.

Acceptance criteria:
- `AGENTS.md` records the explicit vertical-spinner convention in a location consistent with the current wxGTK spin-control guidance.
- Any updated test-summary language remains accurate to the implemented regression coverage.

Validation guidance:
- Documentation-only validation assumption: inspect the updated `AGENTS.md` alongside the changed tests to ensure the guidance matches repository behavior.

Tester guidance:
- Expected review surfaces: `AGENTS.md`, `tests/gui/TacticalGuiLiveTest.cpp`, `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`

Artifact guidance:
- If orchestration does not provide an artifact directory, default to `artifacts/spinner-vertical-orientation/subtask-3`.

Execution instruction:
- Begin implementation immediately if the repository state contains the files above and no blocking ambiguity remains; do not stop after preflight restatement alone.

Completion gate:
- Do not report success unless all required artifacts exist and all changes are committed.

## Dependency Ordering
1. Subtask 1 first.
   Tactical speed control is isolated and low-risk; it establishes the preferred explicit style pattern and the tactical source-contract assertion.
2. Subtask 2 second.
   The ICM dialog uses a different rebuild path and live GUI coverage, so it should follow after the style pattern is settled.
3. Subtask 3 last.
   Documentation should reflect the final implemented convention and regression scope, so it should trail the code/test changes.

## Parallelization Notes
- Subtasks 1 and 2 both change spinner-orientation behavior but touch different production files and different primary tests; they could run in parallel if coordination on the exact style convention is established up front.
- Subtask 3 should remain sequential after the code/test subtasks because it depends on the final implemented behavior and final regression scope.

## Output Artifact Path
- `plans/spinner-vertical-orientation-plan.md`
