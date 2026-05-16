# Stopped Ship Free Rotation Plan

## Feature Restatement
Fix tactical movement so a ship that ended the prior turn at a complete stop can start its next movement phase by choosing any legal facing before it moves, and can also spend the turn rotating in place without changing hexes. Preserve the existing tactical route-selection flow and keep the change aligned with the movement rules in `doc/rules/tactical_operations_manual.md`.

## Confirmed Repository Facts
- The movement rules in `doc/rules/tactical_operations_manual.md` state that a ship must move into the hex it is facing, may turn after entering a hex, and if its speed is zero through the entire turn it may rotate during its movement phase to face any hexside.
- The Advanced Game damage rules in `doc/rules/tactical_operations_manual.md` also state that a ship with `MR` reduced to `0` cannot turn or change facing, even if it stops in a single hex. The stopped-ship fix therefore must not bypass `MR == 0` steering limits.
- Tactical move selection is model-owned in `src/tactical/FTacticalGame.cpp`, primarily through `FTacticalGame::selectShipFromHex(...)`, `setInitialRoute()`, `computeRemainingMoves(...)`, `handleMoveHexSelection(...)`, `finalizeMovementState()`, and `checkMoveStatus()`.
- Current route generation only exposes three highlight buckets from the current heading: forward (`m_movementHexes`), left (`m_leftHexes`), and right (`m_rightHexes`).
- Selecting a moving-side ship during `PH_MOVE` immediately enables route drawing through `FTacticalGame::selectShipFromHex(...)`, which sets `m_drawRoute = true` and calls `setInitialRoute()`.
- Route rendering on the board is currently driven by `src/tactical/FBattleBoard.cpp`, which draws only the forward, left-turn, and right-turn highlight vectors returned by the tactical model.
- Movement finalization currently persists `nMoved` as the ship's new speed and `curHeading` as the ship's new heading in `FTacticalGame::finalizeMovementState()`.
- Existing tactical regression coverage already exercises route-selection and source-contract movement seams in:
  - `tests/tactical/FTacticalMoveRouteSelectionTest.cpp`
  - `tests/tactical/FTacticalGameMechanicsTest.cpp`
  - `tests/tactical/FTacticalBattleBoardRendererDelegationTest.cpp`
  - `tests/tactical/FTacticalModelSelectionHexClickSurfaceTest.cpp`

## Assumptions And Resolved Design Direction
- Scope is limited to tactical ship movement in battle mode. Strategic movement and station orbit rules are out of scope except where existing tactical shared seams must remain compatible.
- The most conservative UX is to extend the existing move-route interaction rather than add a new dialog or a separate movement button.
- For ships that begin the turn at `speed == 0` and still have legal facing-change authority, the model should expose a stopped-ship facing-selection state before any displacement is committed.
- The preferred no-move completion path is to reuse the existing move-complete flow after a facing choice is made, so a player can rotate in place and then finish movement without adding traveled hexes.
- The fix must preserve the current restriction that ships with `MR == 0` cannot rotate freely just because their speed is zero.

## Files To Modify
- Likely production files for the model subtask:
  - `src/tactical/FTacticalGame.cpp`
  - `include/tactical/FTacticalGame.h` if small additive state/accessor declarations are needed
- Likely production files for the tactical interaction/rendering subtask:
  - `src/tactical/FBattleBoard.cpp`
  - `src/tactical/FBattleDisplay.cpp`
  - `src/tactical/FBattleScreen.cpp` only if refresh or completion wiring needs a narrow adjustment
  - corresponding headers under `include/tactical/` only if additive declarations are required
- Likely tester follow-up files:
  - `tests/tactical/FTacticalMoveRouteSelectionTest.cpp`
  - `tests/tactical/FTacticalGameMechanicsTest.cpp`
  - `tests/tactical/FTacticalBattleBoardRendererDelegationTest.cpp`
  - `tests/tactical/FTacticalModelSelectionHexClickSurfaceTest.cpp`
  - a new stopped-ship tactical regression fixture under `tests/tactical/` only if extending the existing route-selection fixture would become unclear

## Overall Documentation Impact
No documentation update is expected. This work should bring the implementation back into alignment with the existing tactical rules text rather than introduce a new rules concept. Downstream documentation review should only confirm that no project documentation incorrectly describes the current buggy stopped-ship behavior.

## Subtasks

### SSW-ZSR-001: Add Model-Side Stopped-Ship Facing Selection And Zero-Move Finalization
Extend the tactical model so a ship that begins its movement step at `speed == 0` can choose any legal starting facing before displacement is committed, can then either move from that chosen facing or complete the turn with zero displacement, and still respects steering-loss restrictions.

Acceptance Criteria:
- When a moving-side ship begins its turn at `speed == 0` and has legal facing-change authority, the tactical model exposes all legal starting facings instead of only projecting movement from the ship's current heading.
- Choosing a stopped-ship facing updates the model's pending movement heading without immediately forcing displacement into an adjacent hex.
- After a stopped ship chooses a new facing and finishes movement without traveling, `finalizeMovementState()` preserves `speed == 0`, commits the chosen final heading, and marks the ship as moved for the turn.
- After a stopped ship chooses a new facing and then moves, the first traveled hex can be in the chosen direction rather than only along the prior-turn heading.
- Ships with `MR == 0` or other existing steering restrictions do not gain free facing changes from this fix.
- Existing non-stopped movement routing and turn-count behavior remain unchanged.

Documentation Impact:
No documentation update expected; this is a model-correctness fix against existing rules.

### SSW-ZSR-002: Surface Stopped-Ship Facing Choice Through Existing Tactical Move UI
Update tactical battle interaction and rendering so the stopped-ship model behavior is visible and usable through the existing board/display flow, including a clear path to rotate in place and end movement without inventing a separate modal workflow.

Acceptance Criteria:
- Selecting a stopped ship during `PH_MOVE` shows legal stopped-ship facing choices on the tactical board instead of only the existing forward/left/right route projections.
- After a facing is selected, the player can either continue into route selection for actual movement or complete movement with zero traveled hexes through the existing move-completion flow.
- Board rendering and prompt text remain coherent for both normal movement and the stopped-ship special case.
- Existing move-route rendering for ships that begin the turn with nonzero speed remains unchanged.
- Selection, refresh, and move-completion wiring still progress cleanly into the normal post-move phase flow after a rotate-in-place turn.

Documentation Impact:
No documentation update expected; downstream review should only verify that any inline prompt text remains accurate.

## Dependency Ordering
1. `SSW-ZSR-001` must land first because the board/display layer needs a stable model contract for stopped-ship facing selection and zero-move finalization.
2. `SSW-ZSR-002` depends on `SSW-ZSR-001` and should adapt the existing tactical interaction/rendering flow to the new model behavior.

Parallelization Guidance:
No parallelization. Both subtasks share the same movement-state contract and would otherwise overlap on `FTacticalGame`-owned interaction semantics.

## Implementer Agent Prompts

### Implementer Prompt: SSW-ZSR-001
Your role is 'implementer'. Your task is as follows:

Allowed files:
- `src/tactical/FTacticalGame.cpp`
- `include/tactical/FTacticalGame.h` only if small additive state or accessor declarations are required to keep the stopped-ship movement contract explicit

Implement the tactical model change needed for stopped ships. A ship that begins its movement step at `speed == 0` should be able to choose any legal facing before displacement is committed, then either move from that chosen facing or finish the turn with zero displacement. Keep the change tightly scoped to the model-owned movement/routing seam and preserve the existing restriction that ships with `MR == 0` cannot change facing even when stopped.

Acceptance criteria:
- A moving-side ship that starts the turn at `speed == 0` and retains legal maneuver authority can choose any legal starting facing.
- Facing selection for a stopped ship updates pending movement orientation without immediately consuming displacement.
- Completing movement after a facing-only turn keeps the ship in the same hex, preserves `speed == 0`, and commits the selected final heading.
- Moving after the facing choice allows the ship's first traveled hex to follow the selected facing instead of the previous-turn heading.
- Existing non-stopped routing behavior and `MR == 0` steering restrictions are preserved.

Validation guidance:
- Smallest likely validation scope after implementation: `cd tests && make tactical-tests && ./tactical/TacticalTests`
- If local iteration favors a narrower command, treat `tests/tactical` as the required validation area and record the exact command used as an assumption.

Tester test-file location guidance:
- Expected tester-owned coverage is most likely in:
  - `tests/tactical/FTacticalMoveRouteSelectionTest.cpp`
  - `tests/tactical/FTacticalGameMechanicsTest.cpp`
  - `tests/tactical/FTacticalModelSelectionHexClickSurfaceTest.cpp`
  - a new stopped-ship tactical fixture under `tests/tactical/` only if extending the existing route-selection fixture would be unreasonably unclear

Artifact directory guidance:
- If orchestration context does not provide a path, use repository-root-relative `artifacts/stopped-ship-free-rotation/subtask-1`

Execution instruction:
- Begin implementation immediately when the prompt contains the required blocking inputs, and do not stop after preflight restatement alone.

Completion gate:
- Do not report success unless all required artifacts exist and all changes are committed.

### Implementer Prompt: SSW-ZSR-002
Your role is 'implementer'. Your task is as follows:

Allowed files:
- `src/tactical/FBattleBoard.cpp`
- `src/tactical/FBattleDisplay.cpp`
- `src/tactical/FBattleScreen.cpp` only if a narrow refresh or completion-flow change is needed
- `include/tactical/FBattleDisplay.h` only if an additive declaration is required
- `include/tactical/FBattleScreen.h` only if an additive declaration is required

Implement the tactical board/display integration for the stopped-ship movement fix. Reuse the existing move-selection and move-completion flow so that stopped ships can visibly choose a facing, optionally rotate in place, and still progress through the normal move phase without introducing a separate modal UX.

Acceptance criteria:
- Selecting a stopped ship in `PH_MOVE` visibly exposes the legal facing choices required by the model contract from `SSW-ZSR-001`.
- After the player selects a facing, the UI supports both continuing into actual movement and ending movement with zero displacement through the existing completion flow.
- Prompt text and board highlights remain clear for both the stopped-ship case and the existing nonzero-speed movement case.
- Existing route rendering for ships that begin the turn with nonzero speed is unchanged.
- The move-completion path still transitions cleanly into the normal post-move phase flow after a rotate-in-place turn.

Validation guidance:
- Smallest likely validation scope after implementation: `cd tests && make tactical-tests && ./tactical/TacticalTests`
- If a GUI-backed validation command is needed for confidence, keep it secondary and note the exact command used as an assumption rather than expanding the required scope by default.

Tester test-file location guidance:
- Expected tester-owned coverage is most likely in:
  - `tests/tactical/FTacticalBattleBoardRendererDelegationTest.cpp`
  - `tests/tactical/FTacticalMoveRouteSelectionTest.cpp`
  - `tests/tactical/FTacticalGameMechanicsTest.cpp`
  - `tests/tactical/FTacticalModelSelectionHexClickSurfaceTest.cpp`

Artifact directory guidance:
- If orchestration context does not provide a path, use repository-root-relative `artifacts/stopped-ship-free-rotation/subtask-2`

Execution instruction:
- Begin implementation immediately when the prompt contains the required blocking inputs, and do not stop after preflight restatement alone.

Completion gate:
- Do not report success unless all required artifacts exist and all changes are committed.

## Output Artifact Path
- `plans/stopped-ship-free-rotation-plan.md`
