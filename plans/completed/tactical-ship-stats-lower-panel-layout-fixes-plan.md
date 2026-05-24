# Requested agent: `planner`

- Repository-local definition found: no
- Shared definition found: yes
- Definition path used: `/home/tstephen/repos/agents/agents/planner.md`
- Precedence decision: shared `planner` definition applies because this repository does not define a repository-local `planner` override

## Feature Restatement

Correct the remaining tactical lower-panel layout regressions in `FBattleDisplay` / `FBattleScreen` so that:
1. right-aligned ship statistics remain fully visible for larger ships with longer weapon lists, and
2. the stacked lower-panel layout reserves enough vertical space that the action button row never overlaps the ship-data block.

## Confirmed Repository Facts

1. `src/tactical/FBattleDisplay.cpp` currently computes lower-panel mode and placement through `ensureLowerPanelLayoutState(...)`, with `LOWER_PANEL_LAYOUT_RIGHT_SPLIT` and `LOWER_PANEL_LAYOUT_STACKED` persisted in `m_lowerPanelLayoutState`.
2. The current split-mode width policy is driven by fixed constants, especially `SHIP_STATS_MIN_WIDTH = 320` and `ACTION_PROMPT_MIN_WIDTH = 240`, and the active left margin is derived from `largestMarginWithStatsRoom = panelWidth - SHIP_STATS_MIN_WIDTH - BORDER`.
3. Ship stats are painted manually in `drawCurrentShipStats(wxDC & dc)` rather than being laid out by wx sizers, so width and height requirements must be computed explicitly before drawing.
4. The current stacked-mode top position is derived from `stackedTop = promptBottomY + ACTION_PROMPT_BUTTON_GAP`, while button controls are still placed by the panel sizer and the display min-height is updated through `requestedDisplayHeight`.
5. `FBattleScreen::onSize(...)` already calls `m_display->reflowLowerPanelLayout()` before `applyLayoutPolicy()`, so the resize seam for deterministic lower-panel recomputation already exists.
6. Existing regression coverage already targets this area in `tests/gui/TacticalGuiLiveTest.cpp` and `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`.

## Assumptions

1. The intended behavior is to keep the ship stats right-aligned when split mode has enough width, but to compute the actual width requirement from rendered ship-stat content rather than relying on the current fixed 320-pixel minimum.
2. When split mode cannot fully display the ship-stat content, the panel should switch to stacked mode instead of clipping the weapon list.
3. The stacked-mode overlap is a lower-panel geometry/reflow bug, not a product decision to allow the button row to intrude into the ship-data area.
4. Validation can remain focused on tactical source-level coverage plus live GUI coverage: `cd tests/tactical && make && ./TacticalTests` and `cd tests/gui && make && xvfb-run -a ./GuiTests`.

## Likely Files To Modify

1. `include/tactical/FBattleDisplay.h`
2. `src/tactical/FBattleDisplay.cpp`
3. `src/tactical/FBattleScreen.cpp`
4. `tests/gui/TacticalGuiLiveTest.cpp`
5. `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`

## Unresolved Design Decisions

No material user-facing design decisions remain. The issues describe incorrect layout behavior rather than competing valid UX directions, so planning can proceed directly to decomposition.

## Overall Documentation Impact

No user-facing or design-document update is expected. This is a tactical GUI regression fix with code-and-test impact only unless implementation uncovers a reusable layout policy that the repository already documents elsewhere.

## Subtasks

### Subtask 1: Recompute tactical ship-stats width and stacked-mode vertical requirements from actual lower-panel content

**Scope**

Update the tactical lower-panel layout policy so split mode only remains active when the ship-stats block can be fully displayed, including larger ships with longer weapon lists. The implementation should also correct stacked-mode geometry so the ship-data block and action-button row receive non-overlapping vertical allocations after resize and phase changes.

**Acceptance Criteria**

1. In split mode, a selected large ship such as a heavy cruiser or battleship can display its full ship-stats content, including the complete visible weapons list, without clipping at the right edge of the display panel.
2. The lower panel keeps the ship-stats block on the right side when there is enough width, but falls back to stacked mode when the actual ship-stats width requirement would otherwise exceed the available split allocation.
3. In stacked mode, the ship-data block is positioned fully below the prompt/button region with no overlap between the top action area and the painted ship statistics.
4. Resize and phase-change reflow continues to route through the existing `FBattleScreen` / `FBattleDisplay` layout seams and updates the display min-height to match the active lower-panel mode.
5. The implementation expresses ship-stats width and stacked-mode height requirements through stable lower-panel layout calculations rather than a hard-coded incidental margin that only fits some ship classes.

**Documentation Impact**

No documentation update expected.

### Subtask 2: Strengthen tactical regression coverage for large-ship width fit and stacked-mode non-overlap

**Scope**

Extend the existing tactical live GUI and tactical source-contract regressions so they prove the corrected lower-panel behavior: large ship-stat content remains fully visible in split mode when space allows, and stacked-mode button placement no longer intrudes into the ship-data block.

**Acceptance Criteria**

1. `tests/gui/TacticalGuiLiveTest.cpp` adds or updates live assertions that fail if a narrow/stacked lower-panel layout places the action button region on top of the ship-data area.
2. Live tactical GUI coverage includes at least one large-ship scenario whose lower-panel geometry would have previously clipped the ship-stat content, and the regression fails if the split-mode width calculation regresses.
3. `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp` locks in the intended lower-panel layout seam without depending on obsolete fixed-width assumptions that would block the corrected implementation.
4. Existing lower-panel runtime-policy checks remain meaningful after the regression update and continue guarding the `FBattleScreen` resize/reflow path.

**Documentation Impact**

No documentation update expected.

## Dependency Ordering

1. Subtask 1 should run first.
   Reason: the runtime layout contract must exist before the regression can be tightened around the corrected geometry.
2. Subtask 2 should run second.
   Reason: the regression should encode the final lower-panel policy, not the current broken fixed-width behavior.

Parallelization note: these subtasks should be treated as sequential because they overlap in `FBattleDisplay` lower-panel policy and the same tactical GUI regression surfaces.

## Implementer Prompts

### Implementer Prompt: Subtask 1

Your role is 'implementer'. Your task is as follows:

Allowed files:
- `include/tactical/FBattleDisplay.h`
- `src/tactical/FBattleDisplay.cpp`
- `src/tactical/FBattleScreen.cpp`

Task:
- Fix the tactical lower-panel runtime layout regression so the ship-stats block computes its real width/height requirements and remains fully visible for larger ships.
- Preserve the current right-aligned split layout when enough width exists, but switch to stacked mode instead of clipping when the actual ship-stats content will not fit.
- Correct stacked-mode lower-panel reflow so the action button row and the painted ship-data block occupy separate vertical regions after resize and phase changes.

Acceptance criteria:
- Large-ship stat content, including long weapon lists, is no longer cut off at the right edge when split mode is active.
- Split mode remains right-aligned only when the available width can display the full ship-stats block.
- Stacked mode places the ship-data block fully below the action-button area with no overlap.
- `FBattleScreen` resize handling still recomputes the lower-panel layout deterministically through the existing display reflow seam.
- The implementation replaces brittle fixed-layout assumptions with a stable lower-panel sizing contract that later tests can verify.

Validation guidance:
- Smallest likely source-level validation: `cd tests/tactical && make && ./TacticalTests`
- Required live GUI validation: `cd tests/gui && make && xvfb-run -a ./GuiTests`

Expected Tester file locations:
- `tests/gui/TacticalGuiLiveTest.cpp`
- `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`

Artifact directory guidance:
- If no shared artifact directory is provided by orchestration context, default to `artifacts/tactical-ship-stats-lower-panel-layout-fixes/subtask-1`.

Execution instruction:
- Begin implementation immediately using the files above if no blocking repository issue is discovered. Do not stop after preflight restatement alone.

Completion gate:
- Do not report success unless all required artifacts exist and all changes are committed.

### Implementer Prompt: Subtask 2

Your role is 'implementer'. Your task is as follows:

Allowed files:
- `tests/gui/TacticalGuiLiveTest.cpp`
- `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`
- `include/tactical/FBattleDisplay.h`
- `src/tactical/FBattleDisplay.cpp`
- `src/tactical/FBattleScreen.cpp`

Task:
- Strengthen tactical regression coverage so it proves both corrected lower-panel behaviors: large ship-stat content stays fully visible when split mode is allowed, and stacked-mode action-button placement does not overlap the ship-data block.
- Update source-contract assertions as needed so they lock in the intended lower-panel sizing and reflow seams without preserving the current broken fixed-width policy.

Acceptance criteria:
- The live GUI regression fails if stacked-mode geometry allows the top action area to overlap the ship-data block.
- The live GUI regression includes a large-ship lower-panel scenario that would fail again if split-mode width computation regressed back to clipping.
- Source-contract tactical coverage reflects the corrected lower-panel contract and no longer enforces obsolete fixed-width implementation details.
- Existing resize/reflow runtime checks continue to guard the tactical screen layout policy.

Validation guidance:
- Smallest likely source-level validation: `cd tests/tactical && make && ./TacticalTests`
- Required live GUI validation: `cd tests/gui && make && xvfb-run -a ./GuiTests`

Expected Tester file locations:
- `tests/gui/TacticalGuiLiveTest.cpp`
- `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`

Artifact directory guidance:
- If no shared artifact directory is provided by orchestration context, default to `artifacts/tactical-ship-stats-lower-panel-layout-fixes/subtask-2`.

Execution instruction:
- Begin implementation immediately using the files above if no blocking repository issue is discovered. Do not stop after preflight restatement alone.

Completion gate:
- Do not report success unless all required artifacts exist and all changes are committed.

## Output Artifact Path

`plans/tactical-ship-stats-lower-panel-layout-fixes-plan.md`
