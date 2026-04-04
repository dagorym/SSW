# Milestone 8 Plan: FBattleDisplay and FBattleBoard Rendering/Input Separation

## Feature Restatement
Convert `FBattleDisplay` and `FBattleBoard` into wx-only rendering and input-translation classes for tactical combat. After this milestone, tactical mechanics state and decisions should live in `FTacticalGame`, while the wx classes render model state and translate UI events into model API calls.

## Confirmed Repository Facts
- The milestone design reference is in `/home/tstephen/repos/SSW/plans/ui-model-separation-design.md`.
- `FBattleScreen` already owns `FTacticalGame * m_tacticalGame` and delegates significant tactical state to it.
- `FBattleDisplay` still contains `fireAllWeapons()`, weapon-selection logic, defense-selection logic, setup/mine-placement state transitions, and direct `FBattleScreen` state mutation.
- `FBattleBoard` still owns duplicated tactical model state: `m_hexData`, `m_turnInfo`, movement highlight state, targeting ranges, mine state, and movement/firing resolution logic.
- `FTacticalGame` already owns duplicated tactical state including `m_hexData`, `m_turnInfo`, movement highlight/range state, mine state, and `fireAllWeapons()`.
- `FTacticalGame` does not currently expose Milestone 8 target APIs named in the prompt such as `selectWeapon()`, `selectDefense()`, or `handleHexClick(FPoint)`.
- Existing tactical tests are mostly source-inspection tests under `/home/tstephen/repos/SSW/tests/tactical`, including coverage for `FBattleScreen` delegation and legacy `FBattleDisplay::fireAllWeapons()` behavior.

## Assumptions
- Milestones 1-7 are complete and the Milestone 7 tactical delegation surface is the intended baseline.
- It is acceptable for Milestone 8 to add new non-wx methods to `FTacticalGame` where the prompt requires UI classes to delegate to model APIs that do not yet exist.
- Because `FBattleBoard` must become a pure renderer/hit-tester, movement-path computation, target validation, setup placement, and mine-resolution routines should move into `FTacticalGame` even when not called out individually in the prompt.
- Verification will include tactical module tests plus a manual full tactical combat run; if automated GUI execution is not practical in the sandbox, that remains a manual acceptance step on the feature branch.

## Files To Modify
- `/home/tstephen/repos/SSW/include/tactical/FTacticalGame.h`
- `/home/tstephen/repos/SSW/src/tactical/FTacticalGame.cpp`
- `/home/tstephen/repos/SSW/include/tactical/FBattleDisplay.h`
- `/home/tstephen/repos/SSW/src/tactical/FBattleDisplay.cpp`
- `/home/tstephen/repos/SSW/include/tactical/FBattleBoard.h`
- `/home/tstephen/repos/SSW/src/tactical/FBattleBoard.cpp`
- `/home/tstephen/repos/SSW/include/tactical/FBattleScreen.h`
- `/home/tstephen/repos/SSW/src/tactical/FBattleScreen.cpp`
- `/home/tstephen/repos/SSW/tests/tactical/Makefile`
- `/home/tstephen/repos/SSW/tests/tactical/TacticalTests.cpp`
- Likely new tactical tests under `/home/tstephen/repos/SSW/tests/tactical/`
- Possibly `/home/tstephen/repos/SSW/tests/SSWTests.cpp` if new tactical tests must also be registered in the top-level runner

## Overall Documentation Impact
- Review `/home/tstephen/repos/SSW/plans/ui-model-separation-design.md` milestone notes for accuracy after implementation, especially Milestone 8 language about `FBattleDisplay` and `FBattleBoard` becoming pure rendering/input-translation classes.
- If the repository tracks milestone progress elsewhere, update notes to state that tactical board state now lives exclusively in `FTacticalGame`.
- No user-facing gameplay documentation change is expected unless the tactical interaction flow changes visibly.

## Subtasks

### 1. Expand FTacticalGame into the canonical tactical board interaction API
Description:
Add the model-side methods and read accessors required for `FBattleDisplay` and `FBattleBoard` to stop owning game logic. This includes setup-placement actions, movement/targeting selection, weapon and defense selection, mine placement, fire-phase progression helpers, and hex-click handling.

Acceptance Criteria:
- `FTacticalGame` exposes explicit non-wx APIs for the interactions the prompt names or implies, including weapon selection, defense selection, and board hex-click handling.
- `FTacticalGame` exposes read accessors for render-relevant tactical state currently duplicated in wx classes, including ship occupancy/positions, planet/station placement, route highlights, target-range highlights, mine state, and current selection state.
- Tactical state transitions previously computed inside `FBattleDisplay` or `FBattleBoard` are executable from `FTacticalGame` without requiring wx types.
- `FTacticalGame` remains free of wx includes.
- Existing Milestone 7 delegation behavior in `FBattleScreen` is preserved or extended rather than bypassed.

Documentation Impact:
- Update milestone design notes if the final model API names differ from the prompt’s suggested names.
- No end-user documentation update expected.

### 2. Refactor FBattleDisplay into a pure tactical HUD renderer and event translator
Description:
Change `FBattleDisplay` so rendering pulls all tactical state from `FTacticalGame`, and event handlers/buttons only translate wx events into model method calls or `FBattleScreen` redraw/close orchestration. Remove its remaining mechanics logic, especially `fireAllWeapons()`.

Acceptance Criteria:
- `FBattleDisplay::draw()` and `FBattleDisplay::onPaint()` read tactical state from `FTacticalGame` via `FBattleScreen` or an injected reference, not from locally owned gameplay state.
- `FBattleDisplay::fireAllWeapons()` is removed.
- Weapon selection handling calls model APIs such as `FTacticalGame::selectWeapon(...)` rather than directly mutating weapons through UI logic.
- Defense selection handling calls model APIs such as `FTacticalGame::selectDefense(...)` rather than directly mutating the ship from the wx class.
- Button callbacks for move completion, defensive fire completion, offensive fire completion, setup progression, and mine placement completion no longer contain battle-resolution logic beyond invoking model transitions and requesting redraw.
- Any display-local caches that remain are limited to view concerns such as clickable wx rectangles for the current frame.

Documentation Impact:
- Milestone 8 notes should reflect that `FBattleDisplay` is now a HUD/view-only class.
- No user guide update expected.

### 3. Refactor FBattleBoard into a pure renderer and hex hit-tester
Description:
Move the remaining tactical board mechanics out of `FBattleBoard`. Keep wx-specific grid drawing, pixel-to-hex hit testing, scrolling, and drawing helpers in the class, but make the model authoritative for occupancy, pathing, target ranges, mine state, and click semantics.

Acceptance Criteria:
- `FBattleBoard::draw()` renders ships, planet/station state, route highlights, weapon ranges, targets, and mined hexes by reading from `FTacticalGame` rather than member-owned tactical state.
- `FBattleBoard::onLeftUp()` performs hit-testing only and forwards the selected `FPoint` hex to a model method such as `FTacticalGame::handleHexClick(FPoint)`.
- `hexData` and `turnData` typedefs and related owner members are removed from `FBattleBoard.h`.
- Members such as `m_hexData`, `m_turnInfo`, route highlight sets, target-range sets, mine state, and ship-position mechanics state are removed from `FBattleBoard` unless a member is strictly display geometry only.
- Non-rendering tactical methods in `FBattleBoard.cpp` are removed or reduced to wx-only helpers; mechanics logic resides in `FTacticalGame`.
- `FBattleBoard` still provides correct map scaling, center computation, scrolling, and pixel-to-hex conversion for the UI.

Documentation Impact:
- Milestone 8 notes should state that `FBattleBoard` retains only rendering geometry and hex hit-testing responsibilities.
- No user-facing documentation update expected.

### 4. Rewire FBattleScreen and tactical tests around the new ownership boundary
Description:
Adjust `FBattleScreen` forwarding where needed so the display and board talk to `FTacticalGame` consistently, then replace obsolete source-inspection tests with assertions that reflect the new Milestone 8 architecture and cover the migrated API surface.

Acceptance Criteria:
- `FBattleScreen` exposes any missing forwarding/accessor needed by `FBattleDisplay` and `FBattleBoard` to reach the tactical model cleanly.
- Tactical tests no longer assert the presence of `FBattleDisplay::fireAllWeapons()` or other logic that Milestone 8 intentionally removes.
- New or updated tactical tests cover the expected Milestone 8 architecture, including model-owned selection/click APIs and removal of duplicated board state from `FBattleBoard`.
- Test registration and makefiles are updated so new tests build in both `tests/tactical` and the aggregate test runner as required by the repository conventions.

Documentation Impact:
- No external documentation update expected.
- Internal test comments should reflect the Milestone 8 acceptance criteria instead of Milestone 7 transitional behavior.

### 5. Build and perform end-to-end tactical validation
Description:
Run the tactical test suite and a full tactical combat session to confirm the board is fully functional after the refactor, despite the milestone’s temporary instability warning.

Acceptance Criteria:
- Relevant tactical tests build and pass.
- The repository still builds successfully for the modified tactical/UI surface.
- A full tactical combat can be started, played through movement, defensive fire, offensive fire, mine interactions where applicable, and battle completion without breaking the board.
- Any manual-only verification steps are documented in the implementation handoff.

Documentation Impact:
- Record the manual validation steps/results in the implementation notes or artifact for the milestone branch.
- No permanent documentation update expected unless new runtime caveats are discovered.

## Dependency Ordering
1. Subtask 1 must complete first because `FBattleDisplay` and `FBattleBoard` cannot become pure translators/renderers until the model exposes the needed APIs and state accessors.
2. Subtasks 2 and 3 can proceed in parallel after Subtask 1 if ownership boundaries are agreed and the `FTacticalGame` API is stable.
3. Subtask 4 depends on Subtasks 2 and 3 because forwarding/tests should reflect the final UI/model split.
4. Subtask 5 depends on Subtasks 2-4.

## Parallelization Notes
- After the `FTacticalGame` API is established, `FBattleDisplay` work and `FBattleBoard` work are good parallel candidates if each implementer owns a disjoint file set.
- Test rewrites can begin once the target architecture is stable, but final verification should wait until both UI classes are refactored.

## Implementer Agent Prompts

### Subtask 1 Prompt
You are the implementer agent.

Allowed files:
- `/home/tstephen/repos/SSW/include/tactical/FTacticalGame.h`
- `/home/tstephen/repos/SSW/src/tactical/FTacticalGame.cpp`
- `/home/tstephen/repos/SSW/include/tactical/FBattleScreen.h`
- `/home/tstephen/repos/SSW/src/tactical/FBattleScreen.cpp`

Task:
Add the non-wx tactical model APIs needed for Milestone 8 so `FBattleDisplay` and `FBattleBoard` can stop owning tactical mechanics. Introduce model-side selection, defense/weapon activation, hex-click handling, setup placement, movement/firing progression, and render-state accessors as needed, while keeping `FTacticalGame` free of wx dependencies. Update `FBattleScreen` only as needed to expose/forward the tactical model cleanly.

Acceptance criteria:
- `FTacticalGame` exposes the interaction APIs needed by Milestone 8, including weapon selection, defense selection, and board hex-click handling.
- `FTacticalGame` exposes read accessors for occupancy, selection, route/range highlights, mine state, and setup state needed by the wx renderers.
- Tactical mechanics previously trapped in wx classes can execute through the model without wx types.
- `FTacticalGame` remains non-wx and compiles cleanly.
- `FBattleScreen` forwards to the model cleanly without reintroducing duplicated mechanics ownership.

### Subtask 2 Prompt
You are the implementer agent.

Allowed files:
- `/home/tstephen/repos/SSW/include/tactical/FBattleDisplay.h`
- `/home/tstephen/repos/SSW/src/tactical/FBattleDisplay.cpp`
- `/home/tstephen/repos/SSW/include/tactical/FBattleScreen.h`
- `/home/tstephen/repos/SSW/src/tactical/FBattleScreen.cpp`
- `/home/tstephen/repos/SSW/include/tactical/FTacticalGame.h`

Task:
Refactor `FBattleDisplay` into a pure tactical HUD renderer and wx event translator. Remove its local battle-resolution logic, eliminate `fireAllWeapons()`, and make selection/setup/fire event handlers call into `FTacticalGame` APIs instead of mutating tactical mechanics directly.

Acceptance criteria:
- `FBattleDisplay::draw()` and `onPaint()` read tactical state from `FTacticalGame`.
- `FBattleDisplay::fireAllWeapons()` is removed.
- Weapon selection delegates to `FTacticalGame::selectWeapon(...)` or the final equivalent model API.
- Defense selection delegates to `FTacticalGame::selectDefense(...)` or the final equivalent model API.
- Event handlers and button callbacks do not contain tactical battle logic beyond translating wx input into model calls and requesting redraw.
- Setup and mine-placement display behavior remains operational.

### Subtask 3 Prompt
You are the implementer agent.

Allowed files:
- `/home/tstephen/repos/SSW/include/tactical/FBattleBoard.h`
- `/home/tstephen/repos/SSW/src/tactical/FBattleBoard.cpp`
- `/home/tstephen/repos/SSW/include/tactical/FBattleScreen.h`
- `/home/tstephen/repos/SSW/src/tactical/FBattleScreen.cpp`
- `/home/tstephen/repos/SSW/include/tactical/FTacticalGame.h`

Task:
Refactor `FBattleBoard` into a pure renderer and hex hit-tester. Remove duplicated tactical state ownership from the wx class, render from `FTacticalGame`, and reduce `onLeftUp()` to hit-testing plus forwarding the clicked hex to a model API.

Acceptance criteria:
- `FBattleBoard::draw()` renders from `FTacticalGame` state.
- `FBattleBoard::onLeftUp()` only computes the clicked hex and forwards it to `FTacticalGame::handleHexClick(FPoint)` or the final equivalent model API.
- `hexData`, `turnData`, `m_hexData`, `m_turnInfo`, and other duplicated mechanics state are removed from `FBattleBoard`.
- `FBattleBoard` keeps only geometry, rendering, scrolling, and hit-test responsibilities.
- Tactical board behavior still supports setup, movement, targeting, mines, and combat rendering after the migration.

### Subtask 4 Prompt
You are the implementer agent.

Allowed files:
- `/home/tstephen/repos/SSW/tests/tactical/Makefile`
- `/home/tstephen/repos/SSW/tests/tactical/TacticalTests.cpp`
- `/home/tstephen/repos/SSW/tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`
- `/home/tstephen/repos/SSW/tests/tactical/FTacticalBattleScreenDelegationTest.cpp`
- New or updated files under `/home/tstephen/repos/SSW/tests/tactical/`
- `/home/tstephen/repos/SSW/tests/SSWTests.cpp`
- `/home/tstephen/repos/SSW/include/tactical/FBattleDisplay.h`
- `/home/tstephen/repos/SSW/src/tactical/FBattleDisplay.cpp`
- `/home/tstephen/repos/SSW/include/tactical/FBattleBoard.h`
- `/home/tstephen/repos/SSW/src/tactical/FBattleBoard.cpp`
- `/home/tstephen/repos/SSW/include/tactical/FTacticalGame.h`
- `/home/tstephen/repos/SSW/src/tactical/FTacticalGame.cpp`

Task:
Update the tactical tests and any required `FBattleScreen` forwarding after the Milestone 8 refactor. Remove assertions that depend on transitional logic still living in `FBattleDisplay` or `FBattleBoard`, and replace them with tests that validate the new model-owned API surface and removed duplicated board state.

Acceptance criteria:
- Tactical tests compile and reflect the Milestone 8 architecture.
- Tests no longer require `FBattleDisplay::fireAllWeapons()` to exist.
- Tests cover the new selection/hex-click delegation surface and the removal of duplicated board state from `FBattleBoard`.
- Test registration and makefiles are updated so the suite builds under repository conventions.

### Subtask 5 Prompt
You are the implementer agent.

Allowed files:
- `/home/tstephen/repos/SSW/include/tactical/FTacticalGame.h`
- `/home/tstephen/repos/SSW/src/tactical/FTacticalGame.cpp`
- `/home/tstephen/repos/SSW/include/tactical/FBattleDisplay.h`
- `/home/tstephen/repos/SSW/src/tactical/FBattleDisplay.cpp`
- `/home/tstephen/repos/SSW/include/tactical/FBattleBoard.h`
- `/home/tstephen/repos/SSW/src/tactical/FBattleBoard.cpp`
- `/home/tstephen/repos/SSW/include/tactical/FBattleScreen.h`
- `/home/tstephen/repos/SSW/src/tactical/FBattleScreen.cpp`
- `/home/tstephen/repos/SSW/tests/tactical/Makefile`
- `/home/tstephen/repos/SSW/tests/tactical/TacticalTests.cpp`
- New or updated files under `/home/tstephen/repos/SSW/tests/tactical/`
- `/home/tstephen/repos/SSW/tests/SSWTests.cpp`

Task:
Build and validate the Milestone 8 tactical board refactor end to end. Run the relevant test targets and perform a full tactical combat validation so the milestone finishes with the board fully operational.

Acceptance criteria:
- The modified tactical/UI code builds successfully.
- Relevant tactical automated tests pass.
- A full tactical combat run exercises movement, defensive fire, offensive fire, and battle completion without board breakage.
- Any manual verification steps or known gaps are documented clearly in the implementation notes.

## Output Artifact Path
- `/home/tstephen/repos/SSW/plans/milestone8-battle-display-board-rendering-plan.md`
