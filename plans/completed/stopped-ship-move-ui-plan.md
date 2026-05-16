# Stopped Ship Move UI Plan

## Feature Restatement
Update the tactical move UI for stopped ships so selecting a stopped mover immediately shows the forward route preview from every legal starting facing, rather than only the route from the ship's current heading. After the player clicks a hex on one of those previews, the existing movement-selection flow should continue normally from the implied chosen facing.

## Confirmed Repository Facts
- The stopped-ship movement rule fix has already been implemented on this branch and documented in `plans/stopped-ship-free-rotation-plan.md`.
- The tactical model already recognizes the stopped-ship special case through `canUseStoppedShipFreeRotation(...)` in `src/tactical/FTacticalGame.cpp`.
- Current stopped-ship interaction already allows a hidden adjacent-hex facing pick in `FTacticalGame::handleMoveHexSelection(...)`, which updates `startHeading`, `curHeading`, `finalHeading`, and a one-point path before recalculating movement options.
- Current move highlighting is still exposed through only three route buckets on `FTacticalGame` and `FBattleScreen`: `getMovementHexes()`, `getLeftTurnHexes()`, and `getRightTurnHexes()`.
- `FTacticalGame::setInitialRoute()` still seeds stopped-ship selection with only the current-heading forward path in `m_movementHexes`; it does not build multi-heading preview data.
- `FBattleBoard::drawRoute(...)` renders only those three highlight buckets, so the board cannot currently advertise all legal stopped-ship start directions.
- `FBattleDisplay::drawMoveShip(...)` already contains stopped-ship-specific prompt text: "Select an adjacent hex to choose facing." and "Then move along a route, or press Movement Done to rotate in place."
- Move-phase click handling in `FTacticalGame::handleHexClick(...)` first tries `handleMoveHexSelection(hex)` when `m_drawRoute` is active, and only falls back to `selectShipFromHex(hex)` if no active movement target matched.
- Existing tactical regression coverage already touches the relevant seams in:
  - `tests/tactical/FTacticalMoveRouteSelectionTest.cpp`
  - `tests/tactical/FTacticalGameMechanicsTest.cpp`
  - `tests/tactical/FTacticalBattleBoardRendererDelegationTest.cpp`
  - `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`
  - `tests/tactical/FTacticalModelSelectionHexClickSurfaceTest.cpp`

## Assumptions And Design Direction
- Scope is limited to the tactical battle move UI. Strategic movement, setup rotation UX, and rule-document edits are out of scope.
- The desired UX should remain board-first and non-modal; no new dialog or button flow is needed.
- The cleanest implementation path is to add an explicit model-owned "stopped ship preview route" surface rather than overloading the existing `movement/left/right` buckets with ambiguous mixed semantics.
- Clicking any highlighted preview hex for a stopped ship should resolve the implied initial facing and then continue through the existing route-selection flow, instead of requiring a separate preliminary click on an adjacent hex.
- Ships that are stopped but have `MR == 0` must continue to behave like ordinary non-rotating movers and must not receive the multi-heading preview.
- No user-facing documentation update is expected because this work aligns the UI with already-existing movement rules and with the already-implemented stopped-ship rule fix.

## Likely Files To Modify
- Model and movement-selection seam:
  - `src/tactical/FTacticalGame.cpp`
  - `include/tactical/FTacticalGame.h`
- Tactical screen forwarding seam:
  - `src/tactical/FBattleScreen.cpp`
  - `include/tactical/FBattleScreen.h`
- Tactical board/display rendering:
  - `src/tactical/FBattleBoard.cpp`
  - `src/tactical/FBattleDisplay.cpp`
- Likely tester follow-up files:
  - `tests/tactical/FTacticalMoveRouteSelectionTest.cpp`
  - `tests/tactical/FTacticalGameMechanicsTest.cpp`
  - `tests/tactical/FTacticalBattleBoardRendererDelegationTest.cpp`
  - `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`
  - `tests/tactical/FTacticalModelSelectionHexClickSurfaceTest.cpp`

## Documentation Impact
No documentation update is expected. Downstream documentation review should only confirm that no project documentation or inline prompt text still implies the old "adjacent hex first" discovery-based workflow once the UI has been updated.

## Subtasks

### SSW-SSI-001: Add Explicit Stopped-Ship Preview Route Data To The Tactical Model
Extend `FTacticalGame` so a stopped ship with legal maneuver authority exposes the full set of selectable start-route previews needed by the UI, instead of only exposing the current-heading forward route plus ad hoc adjacent-facing behavior.

Acceptance Criteria:
- When a moving-side ship begins its move at `speed == 0`, `nMoved == 0`, and `MR > 0`, the tactical model can expose preview data for every legal starting facing.
- Each preview includes enough information for the UI to highlight the route path and for click handling to determine which implied initial facing was chosen.
- Existing `getMovementHexes()`, `getLeftTurnHexes()`, and `getRightTurnHexes()` behavior for non-stopped movement remains unchanged.
- Stopped ships with `MR == 0` do not receive the new multi-facing preview data.
- The model contract remains non-wx and additive, consistent with the existing tactical module boundary rules.

Documentation Impact:
No documentation update expected.

### SSW-SSI-002: Resolve Stopped-Ship Preview Clicks Into Normal Movement Selection
Update move-phase click handling so clicking any hex on a stopped-ship preview route chooses the implied facing and then continues through the normal route-selection mechanics without requiring an undiscoverable adjacent-hex intermediary click.

Acceptance Criteria:
- Selecting a stopped ship and clicking a highlighted preview hex commits the correct pending starting facing before extending the route.
- If the clicked preview hex is the first hex on a candidate route, the first moved hex in the ship path matches that preview direction.
- After the first preview-based selection is resolved, subsequent route trimming, extension, and completion continue through the existing movement flow.
- Rotate-in-place completion still works for stopped ships after a facing has been chosen.
- Non-stopped movement click behavior remains unchanged.

Documentation Impact:
No documentation update expected.

### SSW-SSI-003: Render And Prompt The New Stopped-Ship Preview State In The Tactical UI
Update the tactical board and move-phase prompt rendering so the stopped-ship preview state is visible and self-explanatory, using the new model contract from `SSW-SSI-001` and the click semantics from `SSW-SSI-002`.

Acceptance Criteria:
- Selecting a stopped ship in `PH_MOVE` visibly highlights the available route previews for every legal starting facing.
- The board rendering differentiates the stopped-ship preview state cleanly enough that the player can infer the available movement options without first discovering an adjacent-hex click.
- Move-phase prompt text matches the new interaction model and no longer instructs the player to begin with an adjacent-hex click if that is no longer the intended workflow.
- Existing route rendering for ships that begin the phase with nonzero speed remains unchanged.
- Tactical screen forwarding remains a narrow delegation seam and only grows additively if the new model accessors must be forwarded to the board.

Documentation Impact:
No documentation update expected beyond validating any inline move prompt text.

## Dependency Ordering
1. `SSW-SSI-001` must land first because the board cannot render or interpret full stopped-ship route previews until the tactical model exposes them explicitly.
2. `SSW-SSI-002` depends on `SSW-SSI-001` because click handling needs the new preview-route contract to map a clicked hex back to an implied starting facing and route.
3. `SSW-SSI-003` depends on both earlier subtasks because rendering and prompt text should describe the final model and interaction behavior, not an intermediate state.

Parallelization Guidance:
No parallelization. All three subtasks overlap on the same stopped-ship movement contract and should be implemented serially to avoid conflicting assumptions in `FTacticalGame` and its tactical UI forwarders.

## Implementer Agent Prompts

### Implementer Prompt: SSW-SSI-001
Your role is 'implementer'. Your task is as follows:

Allowed files:
- `src/tactical/FTacticalGame.cpp`
- `include/tactical/FTacticalGame.h`

Implement an additive model-owned preview-route surface for stopped-ship movement selection. The current branch already supports stopped-ship free rotation after an adjacent-hex facing pick, but the UI now needs explicit preview data for every legal starting facing as soon as the ship is selected. Keep the contract non-wx and preserve the existing behavior for ordinary movement and for stopped ships with `MR == 0`.

Acceptance criteria:
- A stopped ship with `speed == 0`, `nMoved == 0`, and legal maneuver authority exposes preview-route data for each legal starting facing.
- The preview data is sufficient for callers to both render the candidate paths and identify which initial facing a clicked preview hex belongs to.
- Existing `movement/left/right` route accessors for non-stopped movement remain valid and unchanged in semantics.
- The change does not grant new facing options to ships with `MR == 0`.

Validation guidance:
- Smallest likely validation scope after implementation: `cd tests && make tactical-tests && ./tactical/TacticalTests`
- If local iteration favors narrower execution, treat `tests/tactical` as the minimum required validation area and record the exact command used as an assumption.

Tester test-file location guidance:
- Expected tester-owned coverage is most likely in:
  - `tests/tactical/FTacticalMoveRouteSelectionTest.cpp`
  - `tests/tactical/FTacticalGameMechanicsTest.cpp`
  - `tests/tactical/FTacticalModelSelectionHexClickSurfaceTest.cpp`

Artifact directory guidance:
- If orchestration context does not provide a path, use repository-root-relative `artifacts/stopped-ship-move-ui/subtask-1`

Execution instruction:
- Begin implementation immediately when the prompt contains the required blocking inputs, and do not stop after preflight restatement alone.

Completion gate:
- Do not report success unless all required artifacts exist and all changes are committed.

### Implementer Prompt: SSW-SSI-002
Your role is 'implementer'. Your task is as follows:

Allowed files:
- `src/tactical/FTacticalGame.cpp`
- `include/tactical/FTacticalGame.h` only if a small additive declaration is needed to support preview-click resolution

Implement the stopped-ship move-click behavior so a click on any highlighted preview route hex resolves the implied starting facing and proceeds through the existing movement route-selection flow. The current hidden adjacent-hex facing workflow should no longer be required for the intended UI path.

Acceptance criteria:
- Clicking a stopped-ship preview route hex selects the correct implied initial facing before adding movement to the path.
- The ship's first moved hex follows the selected preview direction.
- Existing route trimming and follow-on move extension continue to work after a preview-based first selection.
- Rotate-in-place completion still works once the player has chosen a facing without displacement.
- Non-stopped move selection behavior remains unchanged.

Validation guidance:
- Smallest likely validation scope after implementation: `cd tests && make tactical-tests && ./tactical/TacticalTests`
- If local iteration uses a narrower command, keep `tests/tactical` as the required validation area and record the exact command used as an assumption.

Tester test-file location guidance:
- Expected tester-owned coverage is most likely in:
  - `tests/tactical/FTacticalMoveRouteSelectionTest.cpp`
  - `tests/tactical/FTacticalGameMechanicsTest.cpp`
  - `tests/tactical/FTacticalModelSelectionHexClickSurfaceTest.cpp`

Artifact directory guidance:
- If orchestration context does not provide a path, use repository-root-relative `artifacts/stopped-ship-move-ui/subtask-2`

Execution instruction:
- Begin implementation immediately when the prompt contains the required blocking inputs, and do not stop after preflight restatement alone.

Completion gate:
- Do not report success unless all required artifacts exist and all changes are committed.

### Implementer Prompt: SSW-SSI-003
Your role is 'implementer'. Your task is as follows:

Allowed files:
- `src/tactical/FBattleBoard.cpp`
- `src/tactical/FBattleDisplay.cpp`
- `src/tactical/FBattleScreen.cpp`
- `include/tactical/FBattleScreen.h`

Implement the stopped-ship move UI rendering and forwarding changes needed to surface the new preview-route behavior. The player should be able to select a stopped ship and immediately see the route previews for every legal starting facing, with prompt text that matches the new interaction model. Keep the tactical screen as a narrow forwarder over the model contract.

Acceptance criteria:
- In `PH_MOVE`, selecting a stopped ship visibly highlights all legal start-route previews instead of only the current-heading route.
- The board rendering stays coherent once the player commits to one preview and continues normal route selection.
- Move-phase prompt text describes the new preview-based interaction accurately and no longer depends on a hidden adjacent-hex discovery step.
- Ordinary nonzero-speed route rendering remains unchanged.
- Any `FBattleScreen` API growth is additive and limited to forwarding the new model accessors required by the renderer.

Validation guidance:
- Smallest likely validation scope after implementation: `cd tests && make tactical-tests && ./tactical/TacticalTests`
- If a GUI-backed validation command is used for additional confidence, keep it secondary and record the exact command used as an assumption instead of expanding the default required scope.

Tester test-file location guidance:
- Expected tester-owned coverage is most likely in:
  - `tests/tactical/FTacticalBattleBoardRendererDelegationTest.cpp`
  - `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`
  - `tests/tactical/FTacticalMoveRouteSelectionTest.cpp`
  - `tests/tactical/FTacticalGameMechanicsTest.cpp`

Artifact directory guidance:
- If orchestration context does not provide a path, use repository-root-relative `artifacts/stopped-ship-move-ui/subtask-3`

Execution instruction:
- Begin implementation immediately when the prompt contains the required blocking inputs, and do not stop after preflight restatement alone.

Completion gate:
- Do not report success unless all required artifacts exist and all changes are committed.

## Output Artifact Path
- `plans/stopped-ship-move-ui-plan.md`
