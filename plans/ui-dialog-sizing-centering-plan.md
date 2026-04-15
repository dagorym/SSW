# UI Dialog Sizing and Centering Cleanup Plan

## Feature Restatement
Clean up wxWidgets UI regressions that currently cause GTK spin-button layout warnings, clipped BattleSim and ICM dialogs, likely similar sizing issues in other dialogs, and inconsistent top-level window/dialog placement across the application.

## Confirmed Repository Facts
- `src/tactical/FBattleDisplay.cpp` creates a tactical setup speed `wxSpinCtrl` with explicit size `wxSize(50, -1)` and hides/shows it dynamically.
- `src/gui/ICMSelectionGUI.cpp` dynamically creates per-ship `wxSpinCtrl` controls with explicit size `wxSize(50, -1)` and repeatedly `Fit()`s both the panel and dialog after rebuilding those controls.
- `include/battleSim/BattleSimFrame.h` still defaults the frame to `wxSize(335,200)`, while `src/battleSim/BattleSimFrame.cpp` tries to enforce a taller minimum height after layout.
- `src/battleSim/LocalGameDialog.cpp`, `src/battleSim/ScenarioDialog.cpp`, `src/battleSim/ScenarioEditorGUI.cpp`, and many strategic dialogs rely on `Layout()` plus `Fit(this)` but do not consistently center themselves.
- `src/gui/WXTacticalUI.cpp` and `src/gui/WXStrategicUI.cpp` launch many modal dialogs directly via `ShowModal()` without a shared centering pass.
- Existing live GUI coverage already exercises BattleSim launch paths, tactical dialogs, tactical spin control geometry, and modal launch discovery in `tests/gui/BattleSimGuiLiveTest.cpp` and `tests/gui/TacticalGuiLiveTest.cpp`.

## Assumptions
- The GTK warning is most likely caused by one or more spin controls being laid out at an invalid or effectively collapsed size during initial show/hover on wxGTK.
- The broader dialog sizing regressions are primarily wxGTK layout regressions from legacy wxFormBuilder-generated constructors rather than tactical model logic.
- Centering policy should be: frames centered on screen, dialogs centered on their parent window when parented, with screen-centering fallback only when no parent exists.

## Files To Modify
- Likely production files:
  - `src/tactical/FBattleDisplay.cpp`
  - `src/gui/ICMSelectionGUI.cpp`
  - `src/battleSim/BattleSimFrame.cpp`
  - `include/battleSim/BattleSimFrame.h`
  - `src/battleSim/LocalGameDialog.cpp`
  - `src/battleSim/ScenarioDialog.cpp`
  - `src/battleSim/ScenarioEditorGUI.cpp`
  - `src/gui/WXTacticalUI.cpp`
  - `src/gui/WXStrategicUI.cpp`
  - Likely one or more additional dialog constructors under `src/gui/*.cpp` that currently use only `Fit(this)` and no centering or minimum-size stabilization
- Likely test files:
  - `tests/gui/TacticalGuiLiveTest.cpp`
  - `tests/gui/BattleSimGuiLiveTest.cpp`
  - Possibly `tests/gui/StrategicGuiLiveTest.cpp`

## Overall Documentation Impact
- No user-facing rules documentation should change.
- Developer-facing documentation impact is likely limited to inline test intent/comments if new GUI regression coverage needs rationale for sizing/centering assertions.
- If a reusable GUI sizing/centering convention is introduced, short notes in existing GUI test comments may need updating to reflect the new contract.

## Subtasks

### Subtask 1: Stabilize spin-control layout paths that trigger wxGTK warnings
Scope:
- Audit all current `wxSpinCtrl` creation sites in active runtime GUI paths.
- Fix the tactical speed selector and ICM allocation controls so they always receive valid sizer-managed geometry on wxGTK and never depend on overly narrow hard-coded widths.
- Preserve the existing tactical button/prompt layout contract.

Likely files:
- `src/tactical/FBattleDisplay.cpp`
- `src/gui/ICMSelectionGUI.cpp`
- `tests/gui/TacticalGuiLiveTest.cpp`
- Possibly `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp` if source-contract assertions need updates

Acceptance Criteria:
- The tactical setup speed spinner is visible with positive width and height when shown through the existing battle-screen flow.
- The ICM dialog’s dynamically created spin controls are visible with positive width and height after selecting a weapon row.
- The implementation removes or replaces hard-coded spin-control sizing that is likely to collapse on wxGTK where repository evidence supports doing so.
- Existing tactical prompt/button layout behavior remains intact.
- Regression coverage explicitly exercises both runtime spin-control surfaces.

Documentation Impact:
- No standalone documentation update expected beyond brief test comments if needed.

### Subtask 2: Fix BattleSim launch-window sizing regression and verify close-path visibility
Scope:
- Correct the BattleSim frame’s initial/default sizing so all launch buttons, including `Quit`, are visible on first show.
- Ensure the frame opens with a size derived from its contents rather than a stale undersized default.
- Verify the close/quit affordance is reachable without manual resize.

Likely files:
- `include/battleSim/BattleSimFrame.h`
- `src/battleSim/BattleSimFrame.cpp`
- `tests/gui/BattleSimGuiLiveTest.cpp`

Acceptance Criteria:
- `BattleSimFrame` no longer depends on the legacy undersized default `335x200` geometry.
- On first show, the BattleSim launch window is large enough to display all three buttons without clipping.
- The `Quit` button is discoverable and visible in live GUI coverage.
- The frame remains centered on screen per the requested window-placement policy.

Documentation Impact:
- No documentation update expected.

### Subtask 3: Fix Battlesim dialog auto-sizing regressions for Local Game, Scenario, and ICM selection
Scope:
- Correct the constructor-time sizing behavior for the dialogs explicitly reported by the user and those directly adjacent in the same BattleSim flow.
- Ensure dialogs default to a size large enough to reveal their full content and action buttons without manual resize.
- Preserve existing modal ownership/lifecycle behavior.

Likely files:
- `src/battleSim/LocalGameDialog.cpp`
- `src/battleSim/ScenarioDialog.cpp`
- `src/battleSim/ScenarioEditorGUI.cpp`
- `src/gui/ICMSelectionGUI.cpp`
- `tests/gui/BattleSimGuiLiveTest.cpp`
- `tests/gui/TacticalGuiLiveTest.cpp`

Acceptance Criteria:
- `LocalGameDialog` shows all controls, including the bottom action button, without clipping.
- `ScenarioDialog` and `ScenarioEditorGUI` default to content-complete sizes on first show.
- `ICMSelectionGUI` defaults to a size that shows its grid, selection panel, dynamic controls, and Done button without clipping.
- Dialog sizing is stable after dynamic content refresh in `ICMSelectionGUI::selectWeapon(...)`.
- Existing modal launch-path tests continue to pass, with new assertions covering content visibility where practical.

Documentation Impact:
- No documentation update expected.

### Subtask 4: Audit and remediate other likely mis-sized dialogs/windows across GUI and strategic flows
Scope:
- Perform a targeted sweep of other wx dialogs that still rely on legacy `Fit(this)` without centering/min-size stabilization.
- Prioritize dialogs already exercised by the live harness and dialogs with significant list/grid content.
- Remediate only the surfaces confirmed or strongly indicated by inspection/test evidence, avoiding unnecessary churn.

Likely files:
- One or more of:
  - `src/gui/SelectCombatGUI.cpp`
  - `src/gui/SystemDialogGUI.cpp`
  - `src/gui/ViewFleetGUI.cpp`
  - `src/gui/BattleResultsGUI.cpp`
  - `src/gui/SatharFleetsGUI.cpp`
  - `src/gui/UPFUnattachedGUI.cpp`
  - `src/gui/TransferShipsGUI.cpp`
  - `src/gui/SelectJumpGUI.cpp`
  - `src/gui/CombatFleetsGUI.cpp`
  - `src/gui/SatharRetreatGUI.cpp`
  - `src/gui/SelectResolutionGUI.cpp`
  - `src/gui/CombatLocationGUI.cpp`
  - `src/gui/TwoPlanetsGUI.cpp`
- Likely test files:
  - `tests/gui/StrategicGuiLiveTest.cpp`
  - `tests/gui/BattleSimGuiLiveTest.cpp`

Acceptance Criteria:
- The audit identifies the subset of dialogs/windows in active use that need sizing remediation.
- Each remediated dialog/window gains a stable initial size large enough for its controls and buttons on first show.
- New or updated GUI regression tests cover the remediated dialogs that the harness can launch deterministically.
- The plan does not broaden into speculative changes for dialogs not supported by evidence.

Documentation Impact:
- No documentation update expected.

### Subtask 5: Normalize frame/dialog placement so windows open centered in consistent locations
Scope:
- Apply a consistent placement policy for top-level GUI surfaces.
- Ensure frames open centered on screen.
- Ensure dialogs open centered on their triggering parent, with fallback behavior for unparented dialogs or message dialogs launched through UI adapter seams.

Likely files:
- `src/battleSim/BattleSimFrame.cpp`
- `src/battleSim/LocalGameDialog.cpp`
- `src/battleSim/ScenarioDialog.cpp`
- `src/battleSim/ScenarioEditorGUI.cpp`
- `src/gui/ICMSelectionGUI.cpp`
- `src/gui/TacticalDamageSummaryGUI.cpp` if needed for consistency
- `src/gui/WXTacticalUI.cpp`
- `src/gui/WXStrategicUI.cpp`
- Likely additional dialog constructors in `src/gui/*.cpp`
- Likely test files:
  - `tests/gui/BattleSimGuiLiveTest.cpp`
  - `tests/gui/TacticalGuiLiveTest.cpp`
  - `tests/gui/StrategicGuiLiveTest.cpp`

Acceptance Criteria:
- `BattleSimFrame` opens centered on screen.
- Parent-backed dialogs in BattleSim, tactical, and strategic adapter flows open centered relative to their parent window.
- Parentless dialog cases use a deterministic screen-centering fallback.
- Live GUI coverage verifies centering behavior for representative frame and dialog launch paths.

Documentation Impact:
- No documentation update expected.

## Dependency Ordering
1. Subtask 1 first.
   Reason: the spin-control warning is a concrete wxGTK defect and affects the ICM dialog sizing work.
2. Subtask 2 second.
   Reason: the BattleSim frame’s undersized default is an isolated root-cause regression and a prerequisite for clean downstream launch-path geometry.
3. Subtask 3 third.
   Reason: it builds on the BattleSim frame fix and the spin-control stabilization work.
4. Subtask 4 fourth.
   Reason: the broader dialog audit should happen after the known regressions establish the preferred sizing pattern.
5. Subtask 5 last.
   Reason: placement normalization should be applied after constructor sizing contracts are stable to avoid mixing two kinds of GUI regressions in earlier subtasks.

Parallelization guidance:
- Subtasks 2 and 5 both touch `BattleSimFrame` and related launch paths, so keep them sequential.
- Subtasks 3 and 5 overlap on the same dialog constructors and modal adapters, so keep them sequential.
- Subtask 4 should remain after 2 and 3 so the audit uses the settled sizing pattern instead of duplicating exploratory churn.

## Implementer Agent Prompts

### Implementer Prompt: Subtask 1
You are the implementer agent

Allowed files:
- `/home/tstephen/repos/SSW/src/tactical/FBattleDisplay.cpp`
- `/home/tstephen/repos/SSW/src/gui/ICMSelectionGUI.cpp`
- `/home/tstephen/repos/SSW/tests/gui/TacticalGuiLiveTest.cpp`
- `/home/tstephen/repos/SSW/tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`

Task:
- Stabilize all active wxGTK spin-control runtime paths implicated by the reported `GtkSpinButton` warning.
- Focus on the tactical speed selector and the dynamically created ICM allocation controls.
- Prefer sizer-managed sizing/min-size behavior over narrow hard-coded spin-control widths when repository evidence supports that change.
- Keep the existing tactical action-button and prompt-spacing behavior intact.

Acceptance criteria:
- Tactical setup speed selection shows a visible spin control with positive width and height.
- ICM allocation rows show visible spin controls with positive width and height after row selection.
- The implementation materially reduces the likelihood of collapsed or invalid spin-control geometry on wxGTK.
- Existing tactical layout behavior outside the spin-control fixes remains stable.
- GUI regression coverage is updated to exercise both spin-control surfaces.

Validation guidance:
- Smallest likely validation command: `cd tests/gui && make && xvfb-run -a ./GuiTests`
- If a narrower tactical-only GUI target is easier during iteration, use the smallest available `GuiTests` scope supported by the repo as an explicit assumption.

Expected Tester file locations:
- `/home/tstephen/repos/SSW/tests/gui/TacticalGuiLiveTest.cpp`
- `/home/tstephen/repos/SSW/tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`

Artifact directory guidance:
- Default to `/home/tstephen/repos/SSW/artifacts/ui-dialog-sizing-centering/subtask-1-spin-controls` unless orchestration provides a shared artifact directory.

Execution instruction:
- Begin implementation immediately if no blocking inputs are missing; do not stop after preflight restatement alone.

Completion gate:
- Do not report success unless all required artifacts exist and all changes are committed.

### Implementer Prompt: Subtask 2
You are the implementer agent

Allowed files:
- `/home/tstephen/repos/SSW/include/battleSim/BattleSimFrame.h`
- `/home/tstephen/repos/SSW/src/battleSim/BattleSimFrame.cpp`
- `/home/tstephen/repos/SSW/tests/gui/BattleSimGuiLiveTest.cpp`

Task:
- Fix the BattleSim launch window sizing regression so all launch controls, especially `Quit`, are visible on first show.
- Remove reliance on the stale undersized default geometry and replace it with content-driven initial sizing/min-size behavior.
- Keep the existing launch flow intact.

Acceptance criteria:
- The default BattleSim frame geometry no longer starts undersized relative to its controls.
- The launch frame shows all three buttons without clipping on first display.
- Live GUI coverage asserts that the `Quit` button is visible/reachable.
- The frame is centered on screen.

Validation guidance:
- Smallest relevant validation command: `cd tests/gui && make && xvfb-run -a ./GuiTests`

Expected Tester file locations:
- `/home/tstephen/repos/SSW/tests/gui/BattleSimGuiLiveTest.cpp`

Artifact directory guidance:
- Default to `/home/tstephen/repos/SSW/artifacts/ui-dialog-sizing-centering/subtask-2-battlesim-frame` unless orchestration provides a shared artifact directory.

Execution instruction:
- Begin implementation immediately if no blocking inputs are missing; do not stop after preflight restatement alone.

Completion gate:
- Do not report success unless all required artifacts exist and all changes are committed.

### Implementer Prompt: Subtask 3
You are the implementer agent

Allowed files:
- `/home/tstephen/repos/SSW/src/battleSim/LocalGameDialog.cpp`
- `/home/tstephen/repos/SSW/src/battleSim/ScenarioDialog.cpp`
- `/home/tstephen/repos/SSW/src/battleSim/ScenarioEditorGUI.cpp`
- `/home/tstephen/repos/SSW/src/gui/ICMSelectionGUI.cpp`
- `/home/tstephen/repos/SSW/tests/gui/BattleSimGuiLiveTest.cpp`
- `/home/tstephen/repos/SSW/tests/gui/TacticalGuiLiveTest.cpp`

Task:
- Fix the reported dialog sizing regressions in the BattleSim flow and the ICM selection dialog.
- Ensure each dialog defaults to a content-complete size on first show and remains correctly sized after any dynamic ICM-content rebuild.
- Preserve current modal ownership and launch sequencing.

Acceptance criteria:
- `LocalGameDialog`, `ScenarioDialog`, `ScenarioEditorGUI`, and `ICMSelectionGUI` display all controls and action buttons without clipping by default.
- `ICMSelectionGUI` remains properly sized after `selectWeapon(...)` rebuilds its dynamic ship rows.
- Existing BattleSim modal launch paths continue to work.
- GUI regression coverage is updated for the affected dialogs where deterministic harness checks are practical.

Validation guidance:
- Smallest relevant validation command: `cd tests/gui && make && xvfb-run -a ./GuiTests`

Expected Tester file locations:
- `/home/tstephen/repos/SSW/tests/gui/BattleSimGuiLiveTest.cpp`
- `/home/tstephen/repos/SSW/tests/gui/TacticalGuiLiveTest.cpp`

Artifact directory guidance:
- Default to `/home/tstephen/repos/SSW/artifacts/ui-dialog-sizing-centering/subtask-3-known-dialogs` unless orchestration provides a shared artifact directory.

Execution instruction:
- Begin implementation immediately if no blocking inputs are missing; do not stop after preflight restatement alone.

Completion gate:
- Do not report success unless all required artifacts exist and all changes are committed.

### Implementer Prompt: Subtask 4
You are the implementer agent

Allowed files:
- `/home/tstephen/repos/SSW/src/gui/SelectCombatGUI.cpp`
- `/home/tstephen/repos/SSW/src/gui/SystemDialogGUI.cpp`
- `/home/tstephen/repos/SSW/src/gui/ViewFleetGUI.cpp`
- `/home/tstephen/repos/SSW/src/gui/BattleResultsGUI.cpp`
- `/home/tstephen/repos/SSW/src/gui/SatharFleetsGUI.cpp`
- `/home/tstephen/repos/SSW/src/gui/UPFUnattachedGUI.cpp`
- `/home/tstephen/repos/SSW/src/gui/TransferShipsGUI.cpp`
- `/home/tstephen/repos/SSW/src/gui/SelectJumpGUI.cpp`
- `/home/tstephen/repos/SSW/src/gui/CombatFleetsGUI.cpp`
- `/home/tstephen/repos/SSW/src/gui/SatharRetreatGUI.cpp`
- `/home/tstephen/repos/SSW/src/gui/SelectResolutionGUI.cpp`
- `/home/tstephen/repos/SSW/src/gui/CombatLocationGUI.cpp`
- `/home/tstephen/repos/SSW/src/gui/TwoPlanetsGUI.cpp`
- `/home/tstephen/repos/SSW/tests/gui/StrategicGuiLiveTest.cpp`
- `/home/tstephen/repos/SSW/tests/gui/BattleSimGuiLiveTest.cpp`

Task:
- Audit other likely mis-sized dialogs/windows in active strategic and BattleSim flows.
- Remediate only those with confirmed or strongly indicated initial sizing issues.
- Add focused regression coverage for the dialogs the harness can verify deterministically.

Acceptance criteria:
- The implementation documents, through code/test changes, which additional dialogs required remediation.
- Each remediated dialog opens large enough to show its controls and action buttons without clipping.
- The test coverage added or updated maps to the dialogs actually remediated.
- The subtask stays evidence-driven and does not broaden into speculative churn.

Validation guidance:
- Smallest relevant validation command: `cd tests/gui && make && xvfb-run -a ./GuiTests`

Expected Tester file locations:
- `/home/tstephen/repos/SSW/tests/gui/StrategicGuiLiveTest.cpp`
- `/home/tstephen/repos/SSW/tests/gui/BattleSimGuiLiveTest.cpp`

Artifact directory guidance:
- Default to `/home/tstephen/repos/SSW/artifacts/ui-dialog-sizing-centering/subtask-4-dialog-audit` unless orchestration provides a shared artifact directory.

Execution instruction:
- Begin implementation immediately if no blocking inputs are missing; do not stop after preflight restatement alone.

Completion gate:
- Do not report success unless all required artifacts exist and all changes are committed.

### Implementer Prompt: Subtask 5
You are the implementer agent

Allowed files:
- `/home/tstephen/repos/SSW/src/battleSim/BattleSimFrame.cpp`
- `/home/tstephen/repos/SSW/src/battleSim/LocalGameDialog.cpp`
- `/home/tstephen/repos/SSW/src/battleSim/ScenarioDialog.cpp`
- `/home/tstephen/repos/SSW/src/battleSim/ScenarioEditorGUI.cpp`
- `/home/tstephen/repos/SSW/src/gui/ICMSelectionGUI.cpp`
- `/home/tstephen/repos/SSW/src/gui/TacticalDamageSummaryGUI.cpp`
- `/home/tstephen/repos/SSW/src/gui/WXTacticalUI.cpp`
- `/home/tstephen/repos/SSW/src/gui/WXStrategicUI.cpp`
- `/home/tstephen/repos/SSW/src/gui/SelectCombatGUI.cpp`
- `/home/tstephen/repos/SSW/src/gui/SystemDialogGUI.cpp`
- `/home/tstephen/repos/SSW/src/gui/ViewFleetGUI.cpp`
- `/home/tstephen/repos/SSW/src/gui/BattleResultsGUI.cpp`
- `/home/tstephen/repos/SSW/src/gui/SatharFleetsGUI.cpp`
- `/home/tstephen/repos/SSW/src/gui/UPFUnattachedGUI.cpp`
- `/home/tstephen/repos/SSW/src/gui/TransferShipsGUI.cpp`
- `/home/tstephen/repos/SSW/src/gui/SelectJumpGUI.cpp`
- `/home/tstephen/repos/SSW/src/gui/CombatFleetsGUI.cpp`
- `/home/tstephen/repos/SSW/src/gui/SatharRetreatGUI.cpp`
- `/home/tstephen/repos/SSW/src/gui/SelectResolutionGUI.cpp`
- `/home/tstephen/repos/SSW/src/gui/CombatLocationGUI.cpp`
- `/home/tstephen/repos/SSW/src/gui/TwoPlanetsGUI.cpp`
- `/home/tstephen/repos/SSW/tests/gui/BattleSimGuiLiveTest.cpp`
- `/home/tstephen/repos/SSW/tests/gui/TacticalGuiLiveTest.cpp`
- `/home/tstephen/repos/SSW/tests/gui/StrategicGuiLiveTest.cpp`

Task:
- Normalize top-level placement so frames open centered on screen and dialogs open centered on their triggering parent window.
- Apply the behavior consistently across BattleSim, tactical adapter dialogs, and strategic adapter dialogs, using screen-centered fallback only when no parent exists.
- Add deterministic live GUI coverage for representative frame and dialog launch paths.

Acceptance criteria:
- Representative frames open centered on screen.
- Representative parent-backed dialogs open centered relative to their parent.
- Parentless modal cases use a deterministic screen-centered fallback.
- Live GUI regression tests verify the placement policy for at least one frame, one tactical dialog, and one strategic or BattleSim dialog.

Validation guidance:
- Smallest relevant validation command: `cd tests/gui && make && xvfb-run -a ./GuiTests`

Expected Tester file locations:
- `/home/tstephen/repos/SSW/tests/gui/BattleSimGuiLiveTest.cpp`
- `/home/tstephen/repos/SSW/tests/gui/TacticalGuiLiveTest.cpp`
- `/home/tstephen/repos/SSW/tests/gui/StrategicGuiLiveTest.cpp`

Artifact directory guidance:
- Default to `/home/tstephen/repos/SSW/artifacts/ui-dialog-sizing-centering/subtask-5-centering` unless orchestration provides a shared artifact directory.

Execution instruction:
- Begin implementation immediately if no blocking inputs are missing; do not stop after preflight restatement alone.

Completion gate:
- Do not report success unless all required artifacts exist and all changes are committed.

## Output Artifact Path
- `/home/tstephen/repos/SSW/plans/ui-dialog-sizing-centering-plan.md`
