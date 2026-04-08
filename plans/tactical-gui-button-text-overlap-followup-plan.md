# Requested agent: `planner`

- Repository-local definition found: no
- Shared definition found: yes
- Definition path used: `/home/tstephen/repos/agents/agents/planner.md`
- Precedence decision: shared `planner` definition applies because this repository does not define a repository-local `planner` override

## Feature Restatement

Correct the remaining tactical GUI layout regression where the movement and related completion buttons are now horizontally clear of the zoom control but still vertically overlap the painted instruction text. The button row must be positioned completely below the multi-line prompt text, and regression coverage must fail if that spacing collapses again.

## Confirmed Repository Facts

1. `src/tactical/FBattleDisplay.cpp` paints the tactical prompts manually with `dc.DrawText(...)` at fixed Y positions near the top of the panel.
2. The move, defensive-fire, offensive-fire, and mine-placement completion buttons are now sizer-managed controls created in `FBattleDisplay` and shown during the relevant draw paths.
3. The current live GUI regression in `tests/gui/TacticalGuiLiveTest.cpp` only asserts that the action button is to the right of the zoom rectangle and has a positive Y position.
4. The current live GUI regression does not assert that the action button sits below the lowest line of the instruction text.
5. `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp` already contains source-contract checks that the action-button show/hide paths call `Layout()` after visibility changes.

## Assumptions

1. The correct fix is to reserve enough vertical layout space for the prompt text before the action-button row, rather than relying on a larger incidental top margin.
2. The same vertical-spacing rule should apply to all tactical action-button phases that draw multi-line prompt text near the top of `FBattleDisplay`.
3. Headless GUI validation remains `cd tests/gui && make && xvfb-run -a ./GuiTests`.

## Likely Files To Modify

1. `src/tactical/FBattleDisplay.cpp`
2. `include/tactical/FBattleDisplay.h` if a prompt-height helper or layout seam is introduced
3. `tests/gui/TacticalGuiLiveTest.cpp`
4. `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`
5. `tests/tactical/TacticalTests.cpp` only if a new tactical fixture registration is required
6. `tests/gui/GuiTests.cpp` only if a new GUI fixture registration is required

## Overall Documentation Impact

No user-facing or design-document update is expected. This is a tactical GUI regression remediation with code-and-test impact only unless implementation uncovers a reusable layout constraint worth documenting in an existing artifact.

## Subtasks

### Subtask 1: Reserve prompt-text vertical space before the tactical action-button row

**Scope**

Adjust `FBattleDisplay` layout so the completion-button row is positioned below the full height of the instruction text for move, defensive-fire, offensive-fire, and mine-placement states. Preserve the existing sizer-based control ownership while making the text-to-button vertical spacing explicit rather than incidental.

**Acceptance Criteria**

1. In movement, defensive-fire, offensive-fire, and mine-placement states, the relevant completion button appears below the lowest line of the instruction text with visible vertical separation.
2. The action-button row remains to the right of the zoom control and no longer overlaps either the zoom graphic or any line of prompt text.
3. The implementation expresses the required prompt-to-button spacing in a maintainable way, such as a dedicated spacer/helper/offset contract, rather than depending on an accidental `Layout()` result alone.
4. Existing post-`Show()` / post-`Hide()` relayout behavior remains intact.
5. Regression coverage fails if a future change repositions the button row back into the prompt-text area.

**Documentation Impact**

No documentation update expected.

### Subtask 2: Strengthen tactical GUI regression coverage to check text overlap directly

**Scope**

Extend the live GUI regression and, if needed, the source-contract tactical regression so the tests verify the action button is below the full prompt-text block, not merely below `y=0` or outside the zoom rectangle.

**Acceptance Criteria**

1. `tests/gui/TacticalGuiLiveTest.cpp` computes or otherwise defines the prompt-text occupied region for the covered tactical phases and asserts that the action button does not intersect it.
2. The live GUI regression covers at least movement, defensive-fire, offensive-fire, and mine-placement button placement under the tightened rule.
3. If implementation introduces a helper or constant for prompt-text spacing, tactical/source-contract coverage locks in that contract so future refactors cannot silently remove it.
4. Existing layout assertions for the speed spin control and action-button relayout behavior remain valid.

**Documentation Impact**

No documentation update expected.

## Dependency Ordering

1. Subtask 1 should run first.
   Reason: the implementation needs a concrete prompt-spacing contract before the tests can assert the final geometry precisely.
2. Subtask 2 should run second.
   Reason: the strengthened tests should reflect the final chosen spacing seam and avoid rework while the layout contract is still in flux.

Parallelization note: these subtasks should be treated as sequential because they share `FBattleDisplay` layout behavior and the same tactical GUI regression surfaces.

## Implementer Agent Prompts

### Implementer Prompt: Subtask 1

You are the implementer agent.

Allowed files:
- `src/tactical/FBattleDisplay.cpp`
- `include/tactical/FBattleDisplay.h`

Task:
- Fix the remaining tactical action-button layout regression so the movement, defensive-fire, offensive-fire, and mine-placement completion buttons are laid out completely below the painted instruction text.
- Preserve the current sizer-based control ownership and the existing relayout-after-visibility-change behavior.
- Make the prompt-text-to-button vertical separation explicit in the implementation so the button row no longer depends on incidental panel layout.

Acceptance criteria:
- In each covered tactical phase, the relevant completion button is rendered below the lowest line of prompt text with no overlap.
- The action-button row remains to the right of the zoom control.
- The chosen implementation exposes a stable spacing contract that tests can verify.
- Existing `Layout()` calls after button `Show()` / `Hide()` remain present where required.

Validation guidance:
- Smallest likely source-level validation: `cd tests/tactical && make && ./TacticalTests`
- Required live GUI validation: `cd tests/gui && make && xvfb-run -a ./GuiTests`

Expected Tester file locations:
- `tests/gui/TacticalGuiLiveTest.cpp`
- `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`

Artifact directory guidance:
- If no shared artifact directory is provided by orchestration context, default to `artifacts/tactical-gui-button-text-overlap/subtask-1`.

Execution instruction:
- Begin implementation immediately using the files above if no blocking repository issue is discovered. Do not stop after preflight restatement alone.

Completion gate:
- Do not report success unless all required artifacts exist and all changes are committed.

### Implementer Prompt: Subtask 2

You are the implementer agent.

Allowed files:
- `tests/gui/TacticalGuiLiveTest.cpp`
- `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`
- `tests/tactical/TacticalTests.cpp` only if a new tactical fixture registration is required
- `tests/gui/GuiTests.cpp` only if a new GUI fixture registration is required
- `src/tactical/FBattleDisplay.cpp`
- `include/tactical/FBattleDisplay.h`

Task:
- Strengthen the tactical GUI regression coverage so it proves the action buttons sit below the entire instruction-text block rather than merely clearing the zoom rectangle.
- Update or add source-contract assertions if the layout fix introduces a helper, constant, or dedicated spacing seam that should remain stable.

Acceptance criteria:
- The live GUI test explicitly fails if the completion button intersects the instruction-text region for movement, defensive-fire, offensive-fire, or mine-placement states.
- Any new implementation seam introduced for prompt spacing is covered by a tactical/source-level regression.
- Existing speed-control and action-button relayout regressions continue to pass.

Validation guidance:
- Smallest likely source-level validation: `cd tests/tactical && make && ./TacticalTests`
- Required live GUI validation: `cd tests/gui && make && xvfb-run -a ./GuiTests`

Expected Tester file locations:
- `tests/gui/TacticalGuiLiveTest.cpp`
- `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`

Artifact directory guidance:
- If no shared artifact directory is provided by orchestration context, default to `artifacts/tactical-gui-button-text-overlap/subtask-2`.

Execution instruction:
- Begin implementation immediately using the files above if no blocking repository issue is discovered. Do not stop after preflight restatement alone.

Completion gate:
- Do not report success unless all required artifacts exist and all changes are committed.

## Output Artifact

`/home/tstephen/repos/SSW/plans/tactical-gui-button-text-overlap-followup-plan.md`
