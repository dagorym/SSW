# Tactical GUI Separation Regressions Plan

## Feature Restatement

Fix three post-separation tactical regressions while keeping `FTacticalGame` as the canonical owner of movement and fire-range mechanics:

1. stations must keep orbiting instead of drifting away from the planet,
2. move-route clicks must land on the exact highlighted hex the user selects, and
3. forward-fire highlights and target validation must honor the ship's final orientation when the last movement step is a turn.

## Confirmed Repository Facts

- The live tactical movement and fire-range logic now runs through `src/tactical/FTacticalGame.cpp`, with `FBattleScreen` and `FBattleBoard` forwarding into model-owned state.
- `FTacticalGame::resetTurnInfoForCurrentMover()` currently seeds station movement with a gravity-turn flagged path step, but leaves `d.finalHeading = d.curHeading` and does not compute the orbital turn there.
- `FTacticalGame::handleMoveHexSelection()` uses `findHexInList(...)` to populate `moved`, and `findHexInList(...)` returns a zero-based index.
- `FTacticalGame::computeWeaponRange()` and `FTacticalGame::setIfValidTarget()` both walk `turnData->path` from `startHeading` and only update heading while a next path point exists, so the endpoint can miss the post-turn orientation.
- `tests/tactical/FTacticalStationOrbitalMovementTest.cpp` is source-inspection coverage, not behavior-level execution, and `tests/tactical/TacticalTests.cpp` does not currently register that fixture even though `tests/tactical/Makefile` still builds it.

## Assumptions

- The intended route-selection contract is exact-click semantics: clicking a highlighted hex selects that exact destination, and clicking an earlier hex on the current path trims the route back to that exact hex.
- Forward-fire highlight generation and target validation should use the same heading progression logic so the displayed legal targets and the accepted targets cannot diverge.
- Behavior-level tactical tests are the right regression guard for these bugs now that the tactical model owns the mechanics.

## Files To Modify

- `src/tactical/FTacticalGame.cpp`
- `tests/tactical/FTacticalStationOrbitalMovementTest.h`
- `tests/tactical/FTacticalStationOrbitalMovementTest.cpp`
- `tests/tactical/FTacticalMoveRouteSelectionTest.h` (new)
- `tests/tactical/FTacticalMoveRouteSelectionTest.cpp` (new)
- `tests/tactical/FTacticalForwardFireFinalOrientationTest.h` (new)
- `tests/tactical/FTacticalForwardFireFinalOrientationTest.cpp` (new)
- `tests/tactical/FTacticalGameMechanicsTest.cpp`
- `tests/tactical/TacticalTests.cpp`
- `tests/tactical/Makefile`
- `doc/DesignNotes.md` if the milestone-validation notes should record the added regression coverage
- `src/tactical/FBattleBoard.cpp` only if post-fix validation shows the renderer overlay still communicates a different endpoint than the model accepts

## Overall Documentation Impact

- No user-guide update is expected.
- Review `doc/DesignNotes.md` after implementation because the current tactical-model validation notes may need a short addendum describing the restored station orbit, exact route-selection coverage, and final-orientation forward-fire coverage.

## Subtasks

### 1. Restore station orbital turn computation in the model-owned movement path

Scope:
- Repair the station-specific setup/finalization flow in `FTacticalGame` so a station's one-step orbital move computes and carries the turned heading, instead of preserving the old heading across turns.
- Replace the current source-inspection-only station regression test with behavior-level tactical coverage that executes the model seam.

Acceptance criteria:
- A station starting adjacent to a planet remains adjacent after move completion and updates its heading to the orbital turn direction rather than repeating the same straight-line heading.
- Station movement still preserves the station speed contract and does not reintroduce the old speed-change masking-screen bug.
- Non-station movement behavior remains unchanged.
- The station regression test executes `FTacticalGame` runtime behavior rather than only searching source text.

Documentation Impact:
- No direct documentation update is expected from the production fix itself.
- If `doc/DesignNotes.md` currently implies this behavior is already covered, update that note to describe the new runtime regression coverage accurately.

### 2. Fix move-route selection indexing so clicked hexes and committed path endpoints match

Scope:
- Correct the move-selection bookkeeping in `FTacticalGame::handleMoveHexSelection()` so selecting a highlighted forward/left/right candidate appends the clicked endpoint itself, not the previous hex.
- Cover both forward extension and path-backtracking behavior with runtime tactical tests.
- Only touch `FBattleBoard` if the renderer still displays a route-selection contract that disagrees with the corrected model behavior.

Acceptance criteria:
- Clicking the first highlighted movement hex advances the selected path by one hex.
- Clicking any later highlighted movement hex ends the path on that exact clicked hex.
- Clicking an earlier hex already on the route trims the path back to that exact hex without requiring an extra backward click.
- Movement counters and heading bookkeeping remain consistent with the trimmed or extended path.
- If a renderer change is needed, the displayed route highlight contract matches the corrected model selection contract.

Documentation Impact:
- No direct documentation update is expected unless a renderer-side contract note in `doc/DesignNotes.md` needs to mention the exact-click route behavior.

### 3. Apply final-orientation heading semantics to forward-fire highlighting and target validation

Scope:
- Update the moving-ship forward-fire logic in `FTacticalGame::computeWeaponRange()` so each path position, especially the endpoint, uses the heading that is actually in effect after movement and turn resolution.
- Apply the same heading progression fix to `FTacticalGame::setIfValidTarget()` so targeting rules match the displayed highlights.
- Add runtime tactical coverage for a path whose last movement step is a turn.

Acceptance criteria:
- When the final movement step is a turn, the endpoint forward-fire highlight uses the post-turn orientation.
- The target-validation path accepts targets that are legal in the displayed final orientation and rejects targets that are only legal under the stale pre-turn heading.
- Straight-line movement and non-forward-firing weapon behavior remain unchanged.
- Range highlighting and target acceptance stay in sync for the moving-ship case.

Documentation Impact:
- No direct documentation update is expected unless `doc/DesignNotes.md` needs a short note that moving-ship forward-fire coverage now includes final-orientation behavior.

### 4. Repair tactical regression-fixture registration and update shared mechanics assertions

Scope:
- Register the updated and new tactical fixtures in the module test runner and Makefile.
- Update any stale source-inspection assertions in shared tactical mechanics tests so they reflect the corrected model contract rather than the regressed behavior.
- Review design-note coverage text only after the code and tests are settled.

Acceptance criteria:
- `tests/tactical/TacticalTests.cpp` registers the station-orbit fixture plus the new move-selection and forward-fire fixtures.
- `tests/tactical/Makefile` builds every new or renamed tactical regression fixture required by the plan.
- Shared tactical mechanics assertions no longer lock in the regressed station or heading semantics.
- Documentation changes, if any, are limited to accurate validation notes rather than architecture churn.

Documentation Impact:
- `doc/DesignNotes.md` may need a brief validation update if the new coverage meaningfully changes the milestone-status narrative.

## Dependency Ordering

1. Subtask 1 must land first because it restores the station-specific movement contract in `FTacticalGame` and establishes the runtime testing pattern for model-owned movement bugs.
2. Subtask 2 depends on the same movement/path bookkeeping surfaces in `FTacticalGame.cpp` and should follow Subtask 1 sequentially rather than in parallel.
3. Subtask 3 also changes `FTacticalGame.cpp`, specifically the movement-path heading progression used during fire-range evaluation, so it should follow Subtask 2 sequentially.
4. Subtask 4 should finish last because it depends on the final set of updated/new fixtures and any corrected shared assertions.

Parallelization guidance:
- Do not parallelize Subtasks 1-3. They all touch `src/tactical/FTacticalGame.cpp` and rely on agreeing on the same movement-path and heading semantics.
- Subtask 4 can begin only after the production/test file set is stable enough to avoid runner-registration churn.

## Implementer Agent Prompts

### Prompt 1

You are the implementer agent

Files you are allowed to change:
- `src/tactical/FTacticalGame.cpp`
- `tests/tactical/FTacticalStationOrbitalMovementTest.h`
- `tests/tactical/FTacticalStationOrbitalMovementTest.cpp`
- `tests/tactical/FTacticalGameMechanicsTest.cpp`

Task:
- Restore the model-owned station orbital-movement behavior in `FTacticalGame`.
- Ensure the station turn-info reset path computes the orbital turn heading instead of leaving `finalHeading` equal to `curHeading`.
- Keep the existing station speed-preservation behavior intact.
- Replace the current source-inspection-only station regression coverage with behavior-level tactical tests that execute the runtime movement seam.

Acceptance criteria:
- A station adjacent to a planet remains in orbit after move completion and does not drift straight away from the planet.
- The station heading after move completion reflects the orbital turn.
- Station speed preservation remains intact.
- Non-station movement semantics remain unchanged.
- The updated station regression test is runtime behavior coverage, not source-text inspection.

Do not report success unless all required artifacts exist and all changes are committed.

### Prompt 2

You are the implementer agent

Files you are allowed to change:
- `src/tactical/FTacticalGame.cpp`
- `src/tactical/FBattleBoard.cpp` only if needed to keep the renderer contract aligned with the corrected model behavior
- `tests/tactical/FTacticalMoveRouteSelectionTest.h`
- `tests/tactical/FTacticalMoveRouteSelectionTest.cpp`

Task:
- Fix the move-route selection off-by-one behavior in the model-owned move-selection flow.
- The clicked highlighted hex must become the actual route endpoint, both when extending the route and when trimming it backward.
- Add runtime tactical regression coverage for forward extension and backtracking selection behavior.
- Only change `FBattleBoard` if a renderer mismatch remains after the model fix.

Acceptance criteria:
- Clicking the first highlighted movement hex advances the selected path by exactly one hex.
- Clicking any later highlighted movement hex lands the route on that exact clicked hex.
- Clicking an earlier path hex trims the current route to that exact hex.
- Movement counters and heading state remain consistent after extension or trimming.
- If a renderer adjustment is necessary, the displayed selectable contract matches the corrected model behavior.

Do not report success unless all required artifacts exist and all changes are committed.

### Prompt 3

You are the implementer agent

Files you are allowed to change:
- `src/tactical/FTacticalGame.cpp`
- `tests/tactical/FTacticalForwardFireFinalOrientationTest.h`
- `tests/tactical/FTacticalForwardFireFinalOrientationTest.cpp`

Task:
- Fix moving-ship forward-fire range generation and target validation so both use the ship orientation that is actually in effect after each movement step, including a final turn on the endpoint.
- Keep the highlight and targeting logic on the same heading progression contract.
- Add runtime tactical regression coverage for a path whose last step is a turn.

Acceptance criteria:
- Endpoint forward-fire highlights use the final post-turn orientation when the last move is a turn.
- Target validation accepts and rejects targets consistently with the displayed final-orientation highlights.
- Straight-line movement behavior remains unchanged.
- Non-forward-firing weapon behavior remains unchanged.

Do not report success unless all required artifacts exist and all changes are committed.

### Prompt 4

You are the implementer agent

Files you are allowed to change:
- `tests/tactical/Makefile`
- `tests/tactical/TacticalTests.cpp`
- `tests/tactical/FTacticalGameMechanicsTest.cpp`
- `doc/DesignNotes.md`

Task:
- Register the updated/new tactical regression fixtures required by the first three subtasks.
- Update shared tactical mechanics assertions so they describe the corrected model contract instead of locking in regressed behavior.
- Review `doc/DesignNotes.md` and make only the minimal validation-note update needed, if any.

Acceptance criteria:
- The tactical module runner registers the updated station orbit fixture and the new move-selection and final-orientation fire fixtures.
- The tactical Makefile builds the required regression fixtures.
- Shared tactical assertions no longer encode the regressions being fixed.
- Any documentation update is limited to accurate validation notes for the restored regression coverage.

Do not report success unless all required artifacts exist and all changes are committed.

## Output Artifact

- Written plan file: `plans/tactical-gui-separation-regressions-plan.md`
