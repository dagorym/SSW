# Tactical Movement Prompt Overlap Plan

## Feature Restatement

Fix the tactical movement-phase HUD so the movement instructions no longer overlap the selected ship data block after initial setup. Update the third movement instruction line to clearer wording, and implement layout behavior that supports the longer copy by wrapping and/or shifting the ship stats block when space allows.

## Confirmed Repository Facts

1. `src/tactical/FBattleDisplay.cpp` paints movement instructions directly with `dc.DrawText(...)` inside `FBattleDisplay::drawMoveShip(wxDC &dc)`.
2. The current normal movement third line is `Press Movement Done when all ships finish movement.`.
3. `FBattleDisplay::drawCurrentShipStats(wxDC & dc)` uses a fixed `lMargin = 300` for the ship data block.
4. Movement prompt vertical placement is already routed through the `ACTION_PROMPT_*` constants and `getActionPromptLineY(...)` helper in [include/tactical/FBattleDisplay.h](/home/tstephen/repos/SSW/include/tactical/FBattleDisplay.h:67).
5. Tactical source-contract coverage currently locks the old movement prompt wording in [tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp](/home/tstephen/repos/SSW/tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp:417).
6. Live tactical GUI geometry coverage already checks action-button prompt spacing in [tests/gui/TacticalGuiLiveTest.cpp](/home/tstephen/repos/SSW/tests/gui/TacticalGuiLiveTest.cpp:625), but it does not verify the horizontal relationship between movement prompt text and the ship stats block.

## Assumptions

1. The overlap happens in the normal move-phase selected-ship state, not in the stopped-ship facing-selection copy path.
2. The preferred implementation is responsive rather than purely static: wrap the longer movement copy in constrained widths, and shift the ship stats block right only when the client width leaves enough room to do so cleanly.
3. The tactical panel already has enough ownership over both prompt drawing and stats drawing that this can remain localized to `FBattleDisplay` without changing broader tactical model behavior.
4. Headless validation remains `cd tests/gui && make && xvfb-run -a ./GuiTests`, with focused tactical validation via `cd tests && make tactical-tests && ./tactical/TacticalTests`.

## Unresolved Decisions

No blocking design decisions remain. The user explicitly allowed wrapping, horizontal movement of the ship data block, or a combination of both, so implementation can choose the narrowest responsive layout seam that satisfies the acceptance criteria below.

## Likely Files To Modify

1. `src/tactical/FBattleDisplay.cpp`
2. `include/tactical/FBattleDisplay.h`

Expected Tester coverage locations:

1. `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`
2. `tests/gui/TacticalGuiLiveTest.cpp`

## Documentation Impact

No documentation update is expected. This is a tactical GUI layout and wording correction. The protected tactical manual at `doc/rules/tactical_operations_manual.md` must not change.

## Subtasks

### Subtask TMP-1: Implement responsive movement prompt and ship-stats separation

**Scope**

Adjust `FBattleDisplay` so the normal movement-phase instructions and the selected ship stats block do not overlap. The implementation should support the longer revised third instruction line by introducing a maintainable prompt-layout seam that can wrap movement text within an available width and, where practical, compute a wider left margin for the ship stats block on larger windows.

**Acceptance Criteria**

1. In the normal movement-phase state with a selected moving ship, the movement prompt text does not overlap the ship data block.
2. The third movement instruction line is updated to the clearer wording: `Press the 'Movement Done' button when all ships have been assigned their movement instructions.` or an equivalent final string that preserves that meaning and includes the quoted button label.
3. The longer third instruction line is rendered in a way that remains readable in limited horizontal space, with wrapping or equivalent constrained layout rather than unchecked single-line overflow.
4. When the tactical panel is wide enough, the implementation may move the ship stats block right to preserve cleaner separation, but it must not rely solely on a fixed hard-coded margin that recreates the overlap on narrower widths.
5. The change remains localized to tactical display/layout code and does not alter tactical game rules or phase flow.
6. Any new prompt-layout or stats-position helper introduced for this behavior is explicit and maintainable enough for regression coverage to lock down.

**Documentation Impact**

No documentation update expected.

## Dependency Ordering

1. `TMP-1` runs first and is sufficient for this feature.
   Reason: the layout and wording change are tightly coupled in the same drawing surface, and splitting them further would create artificial coordination overhead.

Parallelization note: no safe parallel decomposition is recommended. The prompt drawing, width calculation, wrapping behavior, and ship-stats positioning all overlap in `FBattleDisplay`.

## Implementer Agent Prompts

### Implementer Prompt: TMP-1

Your role is 'implementer'. Your task is as follows:

Allowed files:
- `src/tactical/FBattleDisplay.cpp`
- `include/tactical/FBattleDisplay.h`

Task:
- Fix the tactical movement-phase display so the movement instructions no longer overlap the selected ship data block after setup.
- Update the normal movement third instruction line to read more clearly, using the intent `Press the 'Movement Done' button when all ships have been assigned their movement instructions.`.
- Implement the layout responsively inside `FBattleDisplay`: wrap the longer prompt text within the available prompt area and, if useful when the client width allows it, compute a larger left margin for the ship stats block so both regions stay visually separate.
- Keep the change localized to tactical display/layout behavior. Do not alter tactical rules, phase sequencing, or non-movement prompt behavior unless required by the shared layout seam.

Implementation-outcome acceptance criteria:
- In the normal movement-phase selected-ship state, the movement prompt and ship stats block do not overlap.
- The revised third instruction line is present with the intended meaning and quoted `Movement Done` label.
- The longer instruction remains readable in narrower widths through wrapping or equivalent constrained rendering instead of overflowing into the ship stats area.
- If a dynamic ship-stats margin is introduced, it is derived from available space or prompt width constraints rather than replacing one fixed overlap-prone constant with another fixed constant.
- Any new helper or constants added for prompt wrapping or ship-stats positioning are explicit enough for downstream regression coverage.

Validation guidance:
- Source-focused tactical validation: `cd tests && make tactical-tests && ./tactical/TacticalTests`
- Required live GUI validation: `cd tests/gui && make && xvfb-run -a ./GuiTests`

Tester test-file location guidance:
- `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`
- `tests/gui/TacticalGuiLiveTest.cpp`

Artifact directory guidance:
- If no shared artifact directory is supplied by orchestration context, default to `artifacts/tactical-movement-prompt-overlap/tmp-1`.

Execution instruction:
- Begin implementation immediately using the files above if no blocking repository issue is discovered. Do not stop after preflight restatement alone.

Completion gate:
- Do not report success unless all required artifacts exist and all changes are committed.

## Output Artifact

`plans/tactical-movement-prompt-overlap-plan.md`
