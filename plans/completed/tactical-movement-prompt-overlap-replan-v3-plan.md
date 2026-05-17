# Tactical Movement Prompt Overlap Replan V3

Follow-on plan for the missing responsive-layout behavior after implementing [plans/tactical-movement-prompt-overlap-replan-v2.md](/home/tstephen/repos/SSW/plans/tactical-movement-prompt-overlap-replan-v2.md).

## Feature Restatement

Complete the tactical lower-panel responsive layout so the selected-ship move prompt can return from stacked mode to side-by-side mode when width becomes sufficient again, and so width-driven extra wrapping in the left prompt panel increases lower-panel height enough to keep the final wrapped text line clear of the `Movement Done` button instead of allowing overlap.

## Confirmed Repository Facts

1. `FBattleDisplay::ensureLowerPanelLayoutState(int panelWidth, int panelHeight)` currently keeps stacked mode whenever `m_lowerPanelLayoutState.shipStatsTop >= stackedTop`, which allows stacked mode to persist even after width expands enough for the right split again in [src/tactical/FBattleDisplay.cpp](/home/tstephen/repos/SSW/src/tactical/FBattleDisplay.cpp:253).
2. The current requested lower-panel height still starts from `ACTION_PROMPT_MAX_LINES` instead of the actual wrapped prompt block consumed by the active geometry in [src/tactical/FBattleDisplay.cpp](/home/tstephen/repos/SSW/src/tactical/FBattleDisplay.cpp:285).
3. `FBattleScreen::onSize(...)` already reflows the display before reapplying the screen-level layout policy in [src/tactical/FBattleScreen.cpp](/home/tstephen/repos/SSW/src/tactical/FBattleScreen.cpp:215).
4. Existing live/runtime regression coverage for this area is already concentrated in [tests/gui/TacticalGuiLiveTest.cpp](/home/tstephen/repos/SSW/tests/gui/TacticalGuiLiveTest.cpp:767) and source-contract coverage is in [tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp](/home/tstephen/repos/SSW/tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp:525).

## Assumptions

1. The intended behavior is to prefer the side-by-side split whenever current width and current prompt/button clearance both allow it; stacked mode remains a fallback, not a sticky mode.
2. The overlap reported by the user is a product-layout defect in the active lower-panel sizing logic, not a test-only gap.
3. No documentation changes are expected unless downstream review finds existing docs that explicitly describe this responsive tactical HUD behavior.

## Likely Files To Modify

Implementation files:

1. `include/tactical/FBattleDisplay.h`
2. `src/tactical/FBattleDisplay.cpp`
3. `include/tactical/FBattleScreen.h` if a small screen/display sizing seam adjustment is needed
4. `src/tactical/FBattleScreen.cpp` if a small screen/display sizing seam adjustment is needed

Expected Tester coverage locations:

1. `tests/gui/TacticalGuiLiveTest.cpp`
2. `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`

Possible documentation touchpoints for the downstream Documenter:

1. `doc/UsersGuide.md`
2. `doc/DesignNotes.md`

## Documentation Impact

Probably none. This is a responsive-layout correction inside the existing tactical HUD behavior, not a new feature. Downstream documentation work should remain no-op unless repository docs already promise a conflicting tactical layout rule. `doc/rules/tactical_operations_manual.md` remains protected and must not be edited.

## Subtasks

### Subtask TMPR-1: Make lower-panel split selection reversible under resize

**Scope**

Refine the shared lower-panel layout-state logic so stacked mode is not one-way. The display should keep the current mode while it remains valid, but when width grows enough to support the right-split prompt/stats arrangement again, the layout should return to side-by-side mode on the next resize/reflow instead of remaining stacked until some unrelated state change occurs.

**Acceptance Criteria**

1. The lower-panel layout state still persists across phases and repaints while the current mode remains valid.
2. A width-constrained resize can still force stacked mode when the right split is genuinely invalid.
3. After entering stacked mode, widening the window enough to satisfy the right-split constraints causes the lower panel to return to side-by-side mode during normal resize/reflow.
4. The side-by-side re-entry happens because geometry became valid again, not because of a phase change or unrelated redraw.

**Documentation Impact**

No documentation update expected.

### Subtask TMPR-2: Base lower-panel height on actual wrapped prompt clearance in the active layout

**Scope**

Update the lower-panel sizing logic so the requested display height reflects the final wrapped prompt block for the current prompt width and chosen layout mode, including enough vertical clearance above the `Movement Done` button in the left prompt panel. When width pressure causes extra wrapping, the lower panel should request the small additional height needed to keep the prompt block and button separated; when width pressure is removed, that extra requested height should shrink back toward the baseline instead of staying inflated.

**Acceptance Criteria**

1. In selected-ship move mode, the requested lower-panel height is derived from the final wrapped prompt geometry for the active width, not from the fixed three-line baseline alone.
2. If narrowing width causes the prompt text to wrap deeper, the lower panel requests enough extra height to keep the full wrapped prompt block clear of the `Movement Done` button.
3. The last wrapped line in the left prompt panel does not overlap the button band when the panel could instead grow vertically within the existing screen-level layout policy.
4. If width later increases and fewer wrapped lines are needed, the lower-panel requested height contracts back toward the baseline instead of remaining unnecessarily tall.
5. The existing `60%` map-height floor and stacked-fallback behavior from V2 remain intact.

**Documentation Impact**

No documentation update expected.

## Dependency Ordering

1. `TMPR-1` first.
   Reason: reversible mode selection is the geometry-state rule that `TMPR-2` should size against.
2. `TMPR-2` second.
   Reason: height growth/shrink behavior depends on the final active layout mode and prompt width.

Parallelization note: no safe parallel split is recommended. Both tasks overlap in the same lower-panel layout state and sizing code.

## Implementer Prompts

### Implementer Prompt: TMPR-1

Your role is 'implementer'. Your task is as follows:

Allowed files:
- `include/tactical/FBattleDisplay.h`
- `src/tactical/FBattleDisplay.cpp`

Task:
- Refine the shared lower-panel layout-state logic so stacked mode is a reversible fallback rather than a sticky terminal state.
- Preserve the current mode while it remains valid for the current geometry.
- When width grows enough to support the right-split prompt/stats arrangement again, switch the layout back to side-by-side during normal resize/reflow.
- Keep this change localized to tactical lower-panel layout-state selection; do not alter tactical rules or phase flow.

Implementation-outcome acceptance criteria:
- The lower-panel layout state still persists across phases and ordinary redraws while valid.
- Narrow width can still force stacked mode when needed.
- Widening after a forced stacked state returns the layout to side-by-side when the right split becomes valid again.
- Re-entry to side-by-side does not depend on a phase transition.

Validation guidance:
- Build-focused validation: `cd tests && make tactical-tests && ./tactical/TacticalTests`
- GUI regression validation: `cd tests/gui && make && xvfb-run -a ./GuiTests`

Tester test-file location guidance:
- `tests/gui/TacticalGuiLiveTest.cpp`
- `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`

Artifact directory guidance:
- Reuse `artifacts/tactical-movement-prompt-overlap-replan-v2/` with a coordinator-assigned repository-root-relative child directory unless the coordinator explicitly redirects the restart.

Execution instruction:
- Begin implementation immediately using the files above if no blocking repository issue is discovered. Do not stop after preflight restatement alone.

Completion gate:
- Do not report success unless all required artifacts exist and all changes are committed.

### Implementer Prompt: TMPR-2

Your role is 'implementer'. Your task is as follows:

Allowed files:
- `include/tactical/FBattleDisplay.h`
- `src/tactical/FBattleDisplay.cpp`
- `include/tactical/FBattleScreen.h` if needed for a small sizing seam adjustment
- `src/tactical/FBattleScreen.cpp` if needed for a small sizing seam adjustment

Task:
- Update the lower-panel sizing logic so requested height is based on the final wrapped selected-ship move prompt geometry in the active layout, including enough clearance above the `Movement Done` button in the left prompt panel.
- When horizontal space shrinks and prompt wrapping deepens, request only the additional height needed to avoid prompt/button overlap while staying inside the existing screen-level layout policy.
- When horizontal space later grows, shrink the extra requested height back toward the baseline if the wrapped prompt block no longer needs it.
- Preserve the V2 `60%` map-height floor and stacked-fallback contract.

Implementation-outcome acceptance criteria:
- Requested lower-panel height in selected-ship move mode reflects actual wrapped prompt geometry for the active width.
- Width-driven extra wrapping can increase lower-panel height enough to prevent overlap with the `Movement Done` button.
- The last wrapped prompt line does not overlap the button band when the lower panel can instead grow vertically.
- Removing width pressure lets the extra requested height shrink back toward baseline.
- The existing map-floor and fallback-layout behavior remain intact.

Validation guidance:
- Build-focused validation: `cd tests && make tactical-tests && ./tactical/TacticalTests`
- GUI regression validation: `cd tests/gui && make && xvfb-run -a ./GuiTests`
- During self-check, reason specifically about both transitions: narrow enough to need extra height, then wide enough to reduce that height and restore side-by-side mode when valid.

Tester test-file location guidance:
- `tests/gui/TacticalGuiLiveTest.cpp`
- `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`

Artifact directory guidance:
- Reuse `artifacts/tactical-movement-prompt-overlap-replan-v2/` with a coordinator-assigned repository-root-relative child directory unless the coordinator explicitly redirects the restart.

Execution instruction:
- Begin implementation immediately using the files above if no blocking repository issue is discovered. Do not stop after preflight restatement alone.

Completion gate:
- Do not report success unless all required artifacts exist and all changes are committed.

## Output Artifact Path

`plans/tactical-movement-prompt-overlap-replan-v3-plan.md`
