# Tactical Movement Prompt Overlap Replan V2

Supersedes `plans/tactical-movement-prompt-overlap-replan.md`.

## Feature Restatement

Modernize the tactical battle-screen layout so the default dialog opens at `1200x900`, the map remains the dominant interaction surface, and the lower tactical HUD grows taller only when width pressure makes that necessary. The selected-ship movement reminder must remain readable without overlapping the selected ship stats block or the `Movement Done` button area, and the ship-stats region must not jump around between phases unless the layout is forced to change by size constraints.

## Locked Design Decisions

These decisions are fixed for implementation and are not left to the implementer:

1. The default `FBattleScreen` size changes from `750x550` to `1200x900`.
2. The map keeps at least `60%` of the dialog client height.
3. The lower `FBattleDisplay` region starts from the current `120px` baseline and stays there whenever possible.
4. The bottom HUD grows taller only when the current width makes the baseline layout insufficient.
5. The side-by-side prompt/stats split is preserved as long as it still fits cleanly.
6. If extra HUD height is needed, the HUD grows only as much as necessary while preserving the `60%` minimum map-height floor.
7. If the required HUD height would violate the `60%` map floor, the selected-ship layout switches to a stacked fallback.
8. Once a left/right split is chosen for the ship-stats region, that split stays stable across phases and resize/repaint cycles until constraints force it to change.
9. There is no fixed prompt-line cap; layout decisions are geometry-driven.
10. Resize responsiveness is live while the dialog is open.
11. Because the first attempt failed by not proving the selected-ship constrained-width runtime path, the plan includes explicit implementation work to make that layout state deterministic enough for downstream live GUI validation.

## Failure Analysis From The First Attempt

1. The original plan treated the fix as prompt wrapping plus local margin adjustment inside `FBattleDisplay`, but the real issue spans both `FBattleDisplay` and the parent `FBattleScreen` height-allocation policy.
2. The failed implementation reserved more prompt lines, but it still assumed the selected-ship move-phase layout should stay in the same narrow top strip beside the stats block at the legacy default geometry.
3. The verifier correctly found that on the legacy default size the wrapped selected-ship reminder still overran the reserved button area.
4. The failed live GUI checks never drove the selected-ship movement state under width pressure, so they validated only the simpler no-ship movement prompt.
5. The new plan therefore needs explicit screen-level sizing policy, explicit fallback behavior, and an implementation seam that makes the selected-ship constrained-width path testable at runtime.

## Confirmed Repository Facts

1. The default battle-screen size is still `wxSize( 750,550 )` in [include/tactical/FBattleScreen.h](/home/tstephen/repos/SSW/include/tactical/FBattleScreen.h:44).
2. `FBattleScreen` currently uses a two-row `wxFlexGridSizer` and marks only row `0` growable in [src/tactical/FBattleScreen.cpp](/home/tstephen/repos/SSW/src/tactical/FBattleScreen.cpp:112), which biases resize flexibility toward the map row.
3. `FBattleDisplay` still sets `SetMinSize( wxSize( -1,120 ) )` in [src/tactical/FBattleDisplay.cpp](/home/tstephen/repos/SSW/src/tactical/FBattleDisplay.cpp:36).
4. The clean branch still draws move-phase prompts directly in `FBattleDisplay::drawMoveShip(wxDC &dc)` and the selected ship stats from a fixed left margin in `FBattleDisplay::drawCurrentShipStats(wxDC & dc)` in [src/tactical/FBattleDisplay.cpp](/home/tstephen/repos/SSW/src/tactical/FBattleDisplay.cpp:424) and [src/tactical/FBattleDisplay.cpp](/home/tstephen/repos/SSW/src/tactical/FBattleDisplay.cpp:467).
5. The current live tactical GUI tests that exercise action-button and prompt geometry are in [tests/gui/TacticalGuiLiveTest.cpp](/home/tstephen/repos/SSW/tests/gui/TacticalGuiLiveTest.cpp:586) and currently do not prove the selected-ship constrained-width move path.
6. The source-contract tactical checks for this area are in [tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp](/home/tstephen/repos/SSW/tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp:377).
7. wxWidgets 3.3.2 provides `wxTextWrapper::Wrap(wxWindow*, const wxString&, int)` in `/home/tstephen/wxWidgets/wxWidgets-3.3.2/include/wx/textwrapper.h` and its implementation in `/home/tstephen/wxWidgets/wxWidgets-3.3.2/src/common/stattextcmn.cpp` derives wrapped lines from the actual window font.

## Assumptions

1. The dialog may still be resized below the new `1200x900` default, so smaller-width constrained behavior remains acceptance-critical even though it is no longer the default opening size.
2. `FBattleDisplay` can own the lower-panel layout state and sizing calculations, while `FBattleScreen` owns the screen-level split between the map row and display row.
3. Documentation updates remain small and limited to user/design docs if they currently describe tactical GUI layout or visible prompt wording.

## Likely Files To Modify

Implementation files:

1. `include/tactical/FBattleScreen.h`
2. `src/tactical/FBattleScreen.cpp`
3. `include/tactical/FBattleDisplay.h`
4. `src/tactical/FBattleDisplay.cpp`

Expected Tester coverage locations:

1. `tests/gui/TacticalGuiLiveTest.cpp`
2. `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`

Possible documentation touchpoints for the downstream Documenter:

1. `doc/UsersGuide.md`
2. `doc/DesignNotes.md`

## Documentation Impact

Likely small but non-zero. The new default battle-screen size is user-visible, and the movement reminder wording/layout contract may warrant small updates in `doc/UsersGuide.md` or `doc/DesignNotes.md` if those docs currently describe tactical screen behavior. `doc/rules/tactical_operations_manual.md` remains protected and must not be edited.

## Subtasks

### Subtask TMP-V2-1: Update battle-screen defaults and parent row-allocation policy

**Scope**

Change the tactical battle-screen default size to `1200x900` and replace the current parent sizer policy with one that can preserve a `120px` lower HUD when possible, but can also let the lower row claim additional height when `FBattleDisplay` requests it. This task establishes the `60%` minimum map-height floor and screen-level ownership of height negotiation.

**Acceptance Criteria**

1. `FBattleScreen` defaults to `1200x900`.
2. The parent layout has an explicit policy that starts the lower HUD at `120px` when that fits.
3. The parent layout can grant additional height to the lower HUD when `FBattleDisplay` requests it.
4. The map row retains at least `60%` of the dialog client height.
5. The screen-level height-allocation policy is explicit enough that downstream work does not have to infer when the lower panel may grow.

**Documentation Impact**

Likely small. Documenter should review whether user/design docs mention the default tactical battle-screen size or tactical screen composition.

### Subtask TMP-V2-2: Introduce a persistent bottom-panel layout state shared across phases

**Scope**

Add a stable lower-panel layout model in `FBattleDisplay` that owns the chosen prompt/stats split, current layout mode, and the requested display height. This state must survive phase changes and ordinary redraws so the ship-stats region does not jump around between phases unless a resize or geometry constraint forces recalculation.

**Acceptance Criteria**

1. `FBattleDisplay` has an explicit shared layout state rather than recomputing unrelated prompt/stats placement ad hoc per phase.
2. The chosen side-by-side split for the ship-stats region remains stable across tactical phases when the dialog size remains compatible with that split.
3. The split/layout state changes only when a resize or geometry constraint makes the existing state invalid.
4. The lower panel can communicate a requested height to the parent layout based on the current layout state.
5. The new shared layout state remains tactical-display-only and does not alter tactical game rules or phase flow.

**Documentation Impact**

No documentation update expected unless the implementation changes the visible tactical GUI behavior enough to warrant brief design-note coverage.

### Subtask TMP-V2-3: Implement geometry-driven move-phase prompt behavior with conservative HUD growth and stacked fallback

**Scope**

Implement the selected-ship move-phase behavior on top of the new shared layout state: keep the HUD at `120px` and preserve side-by-side prompt/stats when possible, grow the lower HUD only when width pressure makes that necessary, and switch to stacked fallback only if the required HUD height would violate the `60%` minimum map-height floor. Use wxWidgets-backed wrapping and final rendered prompt geometry, not a count-only approximation.

**Acceptance Criteria**

1. The selected-ship move-phase reminder keeps the revised quoted `Movement Done` wording.
2. At the new default `1200x900` size, the selected-ship move-phase prompt, ship stats, and `Movement Done` button band render cleanly without overlap while keeping the lower HUD at `120px` if that is sufficient.
3. Under smaller-width constrained sizes, the lower HUD grows only when the baseline height is insufficient.
4. The extra HUD growth is the minimum needed to satisfy the current side-by-side layout while preserving the `60%` map-height floor.
5. If satisfying the side-by-side layout would violate the `60%` map-height floor, the layout switches to the stacked fallback and remains non-overlapping.
6. Prompt reservation is derived from the final rendered prompt block for the chosen layout mode.
7. Stopped-ship-facing, no-ship movement, and other action-prompt phases remain readable and functionally correct under the shared layout contract.

**Documentation Impact**

Likely small. Documenter should review whether the visible reminder wording or tactical GUI behavior merits a small doc update.

### Subtask TMP-V2-4: Add deterministic resize/reflow implementation support for runtime selected-ship validation

**Scope**

Add the implementation-side resize/reflow seam needed so the selected-ship constrained-width move path can be driven and observed deterministically in live GUI validation. This is not a standalone testing task; it is product-code support to ensure the responsive behavior recomputes immediately and predictably during an open-dialog resize.

**Acceptance Criteria**

1. Open-dialog resize events trigger immediate tactical lower-panel reflow rather than relying on incidental repaint behavior.
2. The lower-panel layout state recalculates predictably when width/height changes require it.
3. The selected-ship constrained-width move path is deterministic enough that downstream live GUI tests can drive it and verify the resulting geometry.
4. The implementation support added for this purpose stays localized to tactical screen/display layout behavior and does not add unrelated model coupling.

**Documentation Impact**

No documentation update expected.

## Dependency Ordering

1. `TMP-V2-1` first.
   Reason: the new default size and parent row-allocation policy establish the screen-level constraints for all later layout work.
2. `TMP-V2-2` second.
   Reason: the shared lower-panel layout state must exist before the move-phase behavior and cross-phase split stability can be implemented cleanly.
3. `TMP-V2-3` third.
   Reason: the geometry-driven move-phase behavior depends on both the parent sizing policy and the persistent layout state.
4. `TMP-V2-4` fourth.
   Reason: the resize/reflow seam should be completed after the layout behavior exists, so it reflects the final layout decisions rather than a transitional contract.

Parallelization note: no safe parallel decomposition is recommended. These tasks overlap in the same tactical screen/display surfaces and build directly on each other.

## Implementer Agent Prompts

### Implementer Prompt: TMP-V2-1

Your role is 'implementer'. Your task is as follows:

Allowed files:
- `include/tactical/FBattleScreen.h`
- `src/tactical/FBattleScreen.cpp`

Task:
- Change the default `FBattleScreen` size from `750x550` to `1200x900`.
- Rework the parent tactical screen layout policy so the lower `FBattleDisplay` row starts at `120px` when possible, but the parent can grant it additional height when the display requests that.
- Encode the `60%` minimum map-height floor at the screen-layout level so downstream tasks do not have to infer it.
- Keep the tactical map as the dominant region whenever the lower panel does not need extra height.

Implementation-outcome acceptance criteria:
- `FBattleScreen` defaults to `1200x900`.
- The parent layout has an explicit height-allocation policy that starts the lower panel at `120px` when possible.
- The parent layout can honor a larger lower-panel request when needed.
- The map keeps at least `60%` of client height.
- No tactical rules or battle-state behavior are changed.

Validation guidance:
- Build-focused validation: `cd tests && make tactical-tests && ./tactical/TacticalTests`
- GUI regression validation: `cd tests/gui && make && xvfb-run -a ./GuiTests`

Tester test-file location guidance:
- `tests/gui/TacticalGuiLiveTest.cpp`
- `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`

Artifact directory guidance:
- Reuse `artifacts/tactical-movement-prompt-overlap-plan/tmp-1` unless the coordinator assigns a different repository-root-relative restart directory.

Execution instruction:
- Begin implementation immediately using the files above if no blocking repository issue is discovered. Do not stop after preflight restatement alone.

Completion gate:
- Do not report success unless all required artifacts exist and all changes are committed.

### Implementer Prompt: TMP-V2-2

Your role is 'implementer'. Your task is as follows:

Allowed files:
- `include/tactical/FBattleDisplay.h`
- `src/tactical/FBattleDisplay.cpp`

Task:
- Introduce a persistent lower-panel layout state in `FBattleDisplay` that owns the chosen prompt/stats split, current layout mode, and requested display height.
- Make that layout state shared across tactical phases so the ship-stats region does not jump around between phases when the current dialog size still supports the existing split.
- Ensure the layout state changes only when resize or geometry constraints make the current state invalid.

Implementation-outcome acceptance criteria:
- `FBattleDisplay` has an explicit shared layout state for prompt/stats positioning and requested height.
- The ship-stats split remains stable across phases when size constraints do not force change.
- The display can communicate a requested height based on its current layout state.
- The change remains localized to tactical display/layout behavior.

Validation guidance:
- Build-focused validation: `cd tests && make tactical-tests && ./tactical/TacticalTests`
- GUI regression validation: `cd tests/gui && make && xvfb-run -a ./GuiTests`

Tester test-file location guidance:
- `tests/gui/TacticalGuiLiveTest.cpp`
- `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`

Artifact directory guidance:
- Reuse `artifacts/tactical-movement-prompt-overlap-plan/tmp-1` unless the coordinator assigns a different repository-root-relative restart directory.

Execution instruction:
- Begin implementation immediately using the files above if no blocking repository issue is discovered. Do not stop after preflight restatement alone.

Completion gate:
- Do not report success unless all required artifacts exist and all changes are committed.

### Implementer Prompt: TMP-V2-3

Your role is 'implementer'. Your task is as follows:

Allowed files:
- `include/tactical/FBattleDisplay.h`
- `src/tactical/FBattleDisplay.cpp`

Task:
- Implement the selected-ship move-phase prompt behavior on top of the shared lower-panel layout state.
- Keep the lower HUD at `120px` and preserve side-by-side prompt/stats layout whenever that remains sufficient.
- Use wxWidgets-backed wrapping and final rendered prompt geometry for the selected-ship reminder; do not rely on a count-only approximation that can drift from the actual rendering.
- Grow the lower HUD only when width pressure makes the baseline height insufficient.
- Preserve the `60%` minimum map-height floor while doing so.
- If the side-by-side layout would require more height than the map-floor rule allows, switch to the stacked fallback as the last resort.
- Keep the chosen split stable across phases unless constraints force a change.
- Preserve the revised quoted `Movement Done` wording.

Implementation-outcome acceptance criteria:
- At `1200x900`, the selected-ship move-phase prompt, stats block, and `Movement Done` button band do not overlap and the lower HUD remains at `120px` if that is sufficient.
- Under narrower sizes, the lower HUD grows only when needed.
- The HUD growth is the minimum needed to keep the chosen side-by-side layout valid.
- The `60%` map-height floor is respected.
- The stacked fallback is used only when required by constraints.
- Prompt reservation is derived from final rendered geometry for the chosen layout mode.
- Other prompt states remain readable and functionally correct.

Validation guidance:
- Build-focused validation: `cd tests && make tactical-tests && ./tactical/TacticalTests`
- GUI regression validation: `cd tests/gui && make && xvfb-run -a ./GuiTests`
- During self-check, reason specifically about both the new default `1200x900` size and a narrower width that forces pressure on the selected-ship move-phase path.

Tester test-file location guidance:
- `tests/gui/TacticalGuiLiveTest.cpp`
- `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`

Artifact directory guidance:
- Reuse `artifacts/tactical-movement-prompt-overlap-plan/tmp-1` unless the coordinator assigns a different repository-root-relative restart directory.

Execution instruction:
- Begin implementation immediately using the files above if no blocking repository issue is discovered. Do not stop after preflight restatement alone.

Completion gate:
- Do not report success unless all required artifacts exist and all changes are committed.

### Implementer Prompt: TMP-V2-4

Your role is 'implementer'. Your task is as follows:

Allowed files:
- `include/tactical/FBattleScreen.h`
- `src/tactical/FBattleScreen.cpp`
- `include/tactical/FBattleDisplay.h`
- `src/tactical/FBattleDisplay.cpp`

Task:
- Add the implementation-side resize/reflow support needed so the tactical lower-panel layout recomputes immediately and predictably while an open `FBattleScreen` is being resized.
- Ensure the selected-ship constrained-width move path can be driven deterministically enough for downstream live GUI validation to observe the final geometry.
- Keep this work focused on product-code resize/reflow behavior; do not turn it into a tests-only change.

Implementation-outcome acceptance criteria:
- Open-dialog resize triggers immediate lower-panel reflow.
- The lower-panel layout state recalculates predictably when size changes require it.
- The selected-ship constrained-width move path is deterministic enough for downstream live GUI tests to target.
- The change stays localized to tactical layout behavior and does not alter tactical rules.

Validation guidance:
- Build-focused validation: `cd tests && make tactical-tests && ./tactical/TacticalTests`
- GUI regression validation: `cd tests/gui && make && xvfb-run -a ./GuiTests`

Tester test-file location guidance:
- `tests/gui/TacticalGuiLiveTest.cpp`
- `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`

Artifact directory guidance:
- Reuse `artifacts/tactical-movement-prompt-overlap-plan/tmp-1` unless the coordinator assigns a different repository-root-relative restart directory.

Execution instruction:
- Begin implementation immediately using the files above if no blocking repository issue is discovered. Do not stop after preflight restatement alone.

Completion gate:
- Do not report success unless all required artifacts exist and all changes are committed.

## Output Artifact

`plans/tactical-movement-prompt-overlap-replan-v2.md`
