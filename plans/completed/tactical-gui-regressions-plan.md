# Requested agent: `planner`

- Repository-local definition found: no
- Shared definition found: yes
- Definition path used: `/home/tstephen/repos/agents/agents/planner.md`
- Precedence decision: shared `planner` definition applies because this repository does not define a repository-local `planner` override

## Feature Restatement

Diagnose and correct two tactical GUI regressions introduced by recent changes:

1. The fire-phase and movement completion buttons are no longer laid out correctly and can appear on top of the zoom graphic and prompt text.
2. Closing the tactical damage summary dialog can segfault, with the user-observed repro so far being a no-damage/no-detail result.

The fix plan must include regression tests that cover the affected runtime paths.

## Confirmed Repository Facts

1. `src/tactical/FBattleDisplay.cpp` now creates the speed/action controls under sizers in the constructor.
2. `drawGetSpeed()` calls `Layout()` after showing the speed controls, but `drawMoveShip()`, `drawDefensiveFire()`, `drawAttackFire()`, and `drawPlaceMines()` show action buttons without calling `Layout()`.
3. The corresponding hide paths in `onMoveDone()`, `onDefensiveFireDone()`, `onOffensiveFireDone()`, and `onMinePlacementDone()` also omit `Layout()`.
4. `src/gui/TacticalDamageSummaryGUI.cpp` gives the close button `wxID_OK`, sets it as the affirmative/default item, and also binds a custom click handler that unconditionally calls `EndModal(wxID_OK)`.
5. Existing GUI coverage in `tests/gui/TacticalGuiLiveTest.cpp` checks the speed spin control layout and direct `TacticalDamageSummaryGUI` close behavior, but it does not verify fire-phase action-button placement after those buttons are shown.
6. Existing GUI coverage also does not drive the empty/no-detail damage summary through the real `WXTacticalUI::showDamageSummary(...)` path that the tactical runtime uses.

## Likely Root Causes

1. **Button layout regression**
   - The action buttons were moved under sizers, but the fire/movement/mine-placement code paths that show and hide those buttons do not relayout the panel afterward.
   - On wxGTK, showing a sizer-managed child without a layout pass can leave it at the default origin until the next explicit layout, which matches the user-reported overlap with the zoom image and left-side prompt text.
2. **Damage summary close crash**
   - The close button is configured as a normal affirmative `wxID_OK` dialog button and also bound to a custom handler that manually calls `EndModal(...)`.
   - That combination creates a plausible double-close / non-modal `EndModal(...)` path on button activation, especially when the default dialog handling and the explicit handler both participate in closing the dialog.

## Assumptions

1. The segfault is caused by the dialog close path itself rather than the destroyed-ship cleanup that runs immediately afterward. If reproduction during implementation contradicts that, Subtask 2 should adjust the fix while preserving the same acceptance criteria and test intent.
2. `xvfb-run` remains the expected headless wrapper for GUI regression validation in this repository.

## Likely Files To Modify

1. `src/tactical/FBattleDisplay.cpp`
2. `include/tactical/FBattleDisplay.h` if a small relayout/helper seam is introduced
3. `src/gui/TacticalDamageSummaryGUI.cpp`
4. `include/gui/TacticalDamageSummaryGUI.h` if the close handler contract changes
5. `tests/gui/TacticalGuiLiveTest.cpp`
6. `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`
7. `tests/tactical/FTacticalDamageSummaryGUITest.cpp`

## Overall Documentation Impact

No user-facing or design-document update is currently expected. This looks like code-and-tests-only regression remediation unless implementation uncovers a workflow note worth capturing in an existing tactical GUI artifact.

## Subtasks

### Subtask 1: Restore sizer-driven layout for tactical action buttons

**Scope**

Correct the runtime show/hide flow for the movement, defensive-fire, offensive-fire, and mine-placement completion buttons so that the sizer layout is recomputed whenever those controls become visible or hidden.

**Acceptance Criteria**

1. Entering `PH_MOVE`, `PH_DEFENSE_FIRE`, and `PH_ATTACK_FIRE` causes the relevant completion button to appear to the right of the zoom area rather than at the panel origin.
2. The affected button no longer overlaps the zoom graphic or the left-side instructional text in the live tactical window.
3. The show/hide code path for mine-placement completion follows the same relayout discipline so the control row stays consistent across tactical phases.
4. Tactical regression coverage fails if action buttons are shown without the required relayout after the sizer conversion.
5. Existing speed-control layout coverage remains intact.

**Documentation Impact**

No documentation update expected.

### Subtask 2: Harden the tactical damage summary close path and cover empty/no-detail runtime cases

**Scope**

Remove the unsafe close behavior from `TacticalDamageSummaryGUI` so the dialog closes through a single, wx-correct path, then add regression coverage that exercises both the direct dialog and the runtime `WXTacticalUI` modal flow for empty/no-detail summaries.

**Acceptance Criteria**

1. Closing the tactical damage summary no longer segfaults for an empty/no-detail report.
2. The dialog close path uses a single modal-close mechanism that does not rely on an unconditional manual `EndModal(...)` alongside the default affirmative-button behavior.
3. GUI regression coverage drives an empty or no-detail summary through `WXTacticalUI::showDamageSummary(...)` and verifies clean modal dismissal.
4. Tactical/source regression coverage locks in the corrected close-path contract so a future double-close pattern is caught during test review.
5. Existing direct dialog assertions for title, context, summary text, and empty-state behavior continue to pass.

**Documentation Impact**

No documentation update expected.

## Dependency Ordering

1. Subtask 1 should run first.
   Reason: it is a contained tactical-display fix and its GUI test additions will likely share `tests/gui/TacticalGuiLiveTest.cpp`.
2. Subtask 2 should run second.
   Reason: it also touches `tests/gui/TacticalGuiLiveTest.cpp`, so sequencing avoids test-file conflicts and keeps the crash fix isolated after the layout regression is stabilized.

Parallelization note: these subtasks are not good parallel candidates because both likely update the same tactical GUI live test fixture and nearby tactical regression files.

## Implementer Agent Prompts

### Implementer Prompt: Subtask 1

You are the implementer agent.

Allowed files:
- `src/tactical/FBattleDisplay.cpp`
- `include/tactical/FBattleDisplay.h`
- `tests/gui/TacticalGuiLiveTest.cpp`
- `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`
- `tests/tactical/TacticalTests.cpp` only if a new tactical fixture registration is required
- `tests/gui/GuiTests.cpp` only if a new GUI fixture registration is required

Task:
- Fix the tactical action-button layout regression caused by showing/hiding sizer-managed completion buttons without a relayout.
- Keep the existing sizer-based approach.
- Add regression coverage that proves the movement/defensive/offensive completion buttons are positioned away from the origin/zoom area after they become visible.
- Extend coverage to the mine-placement completion button if the same helper or pattern is used there.

Acceptance criteria:
- Entering movement, defensive-fire, and offensive-fire display states shows the correct completion button in a sizer-laid-out position rather than at `(0,0)` or overlapping the zoom image/prompt text.
- The relevant show/hide code path performs the necessary relayout after visibility changes.
- Tests fail if a future change removes the post-show/post-hide relayout behavior for these buttons.
- Existing spin-control layout assertions still pass.

Validation guidance:
- Smallest likely source-level validation: `cd tests/tactical && make && ./TacticalTests`
- Required live GUI validation: `cd tests/gui && make && xvfb-run -a ./GuiTests`

Expected Tester file locations:
- `tests/gui/TacticalGuiLiveTest.cpp`
- `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`

Artifact directory guidance:
- If no shared artifact directory is provided by orchestration context, default to `artifacts/tactical-gui-regressions/subtask-1`.

Execution instruction:
- Begin implementation immediately using the files above if no blocking repository issue is discovered. Do not stop after preflight restatement alone.

Completion gate:
- Do not report success unless all required artifacts exist and all changes are committed.

### Implementer Prompt: Subtask 2

You are the implementer agent.

Allowed files:
- `src/gui/TacticalDamageSummaryGUI.cpp`
- `include/gui/TacticalDamageSummaryGUI.h`
- `src/gui/WXTacticalUI.cpp` if the runtime dialog invocation needs a minimal adjustment
- `tests/gui/TacticalGuiLiveTest.cpp`
- `tests/tactical/FTacticalDamageSummaryGUITest.cpp`
- `tests/tactical/WXTacticalUIAdapterTest.cpp` if source-contract coverage needs updating
- `tests/tactical/TacticalTests.cpp` only if a new tactical fixture registration is required
- `tests/gui/GuiTests.cpp` only if a new GUI fixture registration is required

Task:
- Fix the tactical damage summary dialog close path so it no longer risks a double-close or invalid `EndModal(...)` call.
- Reproduce the user-observed empty/no-detail case in regression coverage through the real `WXTacticalUI::showDamageSummary(...)` modal path, not only the direct dialog constructor path.
- Preserve the current title/context/summary rendering behavior.

Acceptance criteria:
- Closing an empty/no-detail tactical damage summary no longer crashes.
- The corrected implementation uses one clear dialog-close path that is safe for modal dialog usage on wxGTK.
- GUI tests cover at least one empty/no-detail `WXTacticalUI::showDamageSummary(...)` path and verify clean modal return.
- Tactical/source tests lock in the corrected close-path contract so the prior unsafe pattern is caught if reintroduced.
- Existing direct dialog content assertions remain valid.

Validation guidance:
- Smallest likely source-level validation: `cd tests/tactical && make && ./TacticalTests`
- Required live GUI validation: `cd tests/gui && make && xvfb-run -a ./GuiTests`

Expected Tester file locations:
- `tests/gui/TacticalGuiLiveTest.cpp`
- `tests/tactical/FTacticalDamageSummaryGUITest.cpp`
- `tests/tactical/WXTacticalUIAdapterTest.cpp` if updated

Artifact directory guidance:
- If no shared artifact directory is provided by orchestration context, default to `artifacts/tactical-gui-regressions/subtask-2`.

Execution instruction:
- Begin implementation immediately using the files above if no blocking repository issue is discovered. Do not stop after preflight restatement alone.

Completion gate:
- Do not report success unless all required artifacts exist and all changes are committed.

## Output Artifact

`/home/tstephen/repos/SSW/plans/tactical-gui-regressions-plan.md`
