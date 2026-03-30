# Milestone 7 Plan: Refactor `FBattleScreen` to Delegate to `FTacticalGame`

## Feature Restatement

Milestone 7 moves the active tactical runtime off `FBattleScreen`'s duplicated mechanics state and onto the additive tactical model introduced in Milestone 5. The goal is not to refactor rendering yet; it is to make `FBattleScreen` the wx dialog shell that owns `FBattleBoard`, `FBattleDisplay`, and a live `FTacticalGame` / `WXTacticalUI` pair, while preserving the current external `FBattleScreen` API for callers and for Milestone 8 follow-on work.

## Planner Agent Activation

- Requested agent: `planner`
- Repository-local definition found: no
- Shared definition found: yes
- Active definition path: `/home/tstephen/repos/agents/agents/planner.md`
- Precedence decision: shared `planner` definition from `~/repos/agents` applies because no repository-local `planner` definition was found
- Workflow obligations being followed:
  - produce an implementation-ready, ordered plan without writing code
  - identify likely files and affected components from repository evidence
  - define observable acceptance criteria and documentation impact per subtask
  - include one implementer prompt per subtask
  - write the completed plan to a unique markdown file under `plans`

## Confirmed Repository Facts

- [`include/tactical/FBattleScreen.h`](/home/tstephen/repos/SSW/include/tactical/FBattleScreen.h) still owns tactical runtime state directly, including `m_state`, `m_phase`, `m_activePlayer`, `m_movingPlayer`, `m_curShip`, `m_curWeapon`, fleet lists, ICM data, and `m_tacticalReport`.
- [`src/tactical/FBattleScreen.cpp`](/home/tstephen/repos/SSW/src/tactical/FBattleScreen.cpp) still implements `setupFleets`, `setState`, `setPhase`, report lifecycle helpers, `fireICM`, `clearDestroyedShips`, `applyFireDamage`, and winner/close behavior locally.
- [`include/tactical/FTacticalGame.h`](/home/tstephen/repos/SSW/include/tactical/FTacticalGame.h) already owns matching tactical state and mechanics helpers for `setupFleets`, `setState`, `setPhase`, movement-reset/finalize helpers, tactical report lifecycle, `fireICM`, `fireAllWeapons`, `clearDestroyedShips`, and winner tracking.
- [`include/gui/WXTacticalUI.h`](/home/tstephen/repos/SSW/include/gui/WXTacticalUI.h) and [`src/gui/WXTacticalUI.cpp`](/home/tstephen/repos/SSW/src/gui/WXTacticalUI.cpp) exist from Milestone 6, but the current runtime wiring tests explicitly assert that `FBattleScreen` does not yet mention `WXTacticalUI`.
- [`src/tactical/FBattleBoard.cpp`](/home/tstephen/repos/SSW/src/tactical/FBattleBoard.cpp) and [`src/tactical/FBattleDisplay.cpp`](/home/tstephen/repos/SSW/src/tactical/FBattleDisplay.cpp) still call `FBattleScreen` getters and setters extensively, so Milestone 7 must preserve the `FBattleScreen` API surface even if the storage moves behind delegation.
- [`tests/tactical/FTacticalBattleScreenReportContextTest.cpp`](/home/tstephen/repos/SSW/tests/tactical/FTacticalBattleScreenReportContextTest.cpp), [`tests/tactical/FTacticalBattleScreenElectricalFireTest.cpp`](/home/tstephen/repos/SSW/tests/tactical/FTacticalBattleScreenElectricalFireTest.cpp), and [`tests/tactical/WXTacticalUIAdapterTest.cpp`](/home/tstephen/repos/SSW/tests/tactical/WXTacticalUIAdapterTest.cpp) currently encode pre-Milestone-7 behavior and will need to be updated.

## Assumptions

- Milestones 5 and 6 are the intended base state, so `FTacticalGame` may be extended to expose missing getters/setters needed by `FBattleScreen` delegation, but Milestone 7 should not pull `FBattleBoard` or `FBattleDisplay` rendering/input logic into the model yet.
- `FBattleScreen` should continue to own dialog lifetime and close semantics; `FTacticalGame` may notify the UI adapter about winner/message/report events, but `FBattleScreen` should remain responsible for ending the modal session.
- Because the active tactical widgets still compile against `FBattleScreen`, preserving method names on `FBattleScreen` is lower risk than trying to rewrite `FBattleBoard` and `FBattleDisplay` in this milestone.

## Files To Modify

- [`include/tactical/FBattleScreen.h`](/home/tstephen/repos/SSW/include/tactical/FBattleScreen.h)
- [`src/tactical/FBattleScreen.cpp`](/home/tstephen/repos/SSW/src/tactical/FBattleScreen.cpp)
- [`include/tactical/FTacticalGame.h`](/home/tstephen/repos/SSW/include/tactical/FTacticalGame.h)
- [`src/tactical/FTacticalGame.cpp`](/home/tstephen/repos/SSW/src/tactical/FTacticalGame.cpp)
- Likely dependency touch-ups in [`src/tactical/Makefile`](/home/tstephen/repos/SSW/src/tactical/Makefile) if the new includes require dependency updates
- Tactical regression coverage under [`tests/tactical`](/home/tstephen/repos/SSW/tests/tactical), likely including:
  - [`tests/tactical/FTacticalBattleScreenReportContextTest.cpp`](/home/tstephen/repos/SSW/tests/tactical/FTacticalBattleScreenReportContextTest.cpp)
  - [`tests/tactical/FTacticalBattleScreenElectricalFireTest.cpp`](/home/tstephen/repos/SSW/tests/tactical/FTacticalBattleScreenElectricalFireTest.cpp)
  - a new Milestone 7 delegation-focused test pair such as [`tests/tactical/FTacticalBattleScreenDelegationTest.h`](/home/tstephen/repos/SSW/tests/tactical/FTacticalBattleScreenDelegationTest.h) and [`tests/tactical/FTacticalBattleScreenDelegationTest.cpp`](/home/tstephen/repos/SSW/tests/tactical/FTacticalBattleScreenDelegationTest.cpp)
  - [`tests/tactical/Makefile`](/home/tstephen/repos/SSW/tests/tactical/Makefile)
  - [`tests/tactical/TacticalTests.cpp`](/home/tstephen/repos/SSW/tests/tactical/TacticalTests.cpp)

## Overall Documentation Impact

- [`plans/ui-model-separation-design.md`](/home/tstephen/repos/SSW/plans/ui-model-separation-design.md) remains the canonical milestone design and should not need structural changes unless implementation discovers a mismatch in the intended delegation seam.
- Internal architecture notes and any milestone tracking docs should note that `WXTacticalUI` becomes part of the live tactical runtime in Milestone 7 and that `FBattleScreen` no longer owns the canonical tactical mechanics state.
- No end-user documentation update is expected.

## Subtasks

### Subtask 1: Extend `FTacticalGame` to expose the delegation surface `FBattleScreen` needs

**Description**

Close the API gap between the additive model and the current `FBattleScreen` public surface. `FTacticalGame` already owns most of the required state, but Milestone 7 needs explicit delegation points for setup state, selection state, player-turn flags, report lifecycle, UI installation, and any scenario metadata still consumed indirectly by `FBattleBoard` or `FBattleDisplay`.

**Acceptance Criteria**

- `FTacticalGame` exposes explicit public methods for all tactical state `FBattleScreen` still needs to forward, rather than requiring `FBattleScreen` to keep duplicate storage.
- The model provides a safe way to install or replace the active `ITacticalUI` pointer from `FBattleScreen`.
- Any tactical state still queried through `FBattleScreen` by `FBattleBoard` or `FBattleDisplay` has a model-backed accessor.
- The model remains wx-free after the API expansion.

**Documentation Impact**

- No direct documentation update expected for this subtask.

### Subtask 2: Rewire `FBattleScreen` ownership and lifecycle around `FTacticalGame` and `WXTacticalUI`

**Description**

Change `FBattleScreen` from a state owner into a delegating wx shell. The constructor should create the tactical model and wx tactical adapter, connect them, and preserve existing board/display child creation and close-path behavior. The destructor should own cleanup for the new runtime objects without reintroducing leaks or double-deletes.

**Acceptance Criteria**

- `FBattleScreen` owns `FTacticalGame*` and `WXTacticalUI*` members.
- Construction wires the model to the wx adapter before runtime tactical flows use UI callbacks.
- Existing `FBattleBoard` and `FBattleDisplay` construction remains intact.
- `FBattleScreen` teardown cleans up the delegated runtime objects safely.
- The guarded modal close path remains in `FBattleScreen`.

**Documentation Impact**

- Internal architecture notes should mention that `FBattleScreen` is now a dialog shell around the tactical model rather than the model owner.

### Subtask 3: Forward `FBattleScreen` state, phase, and combat/report operations to the model

**Description**

Replace `FBattleScreen`'s duplicated mechanics logic with thin forwarding methods. Public getters/setters used by `FBattleBoard` and `FBattleDisplay` should forward to `FTacticalGame`, and local implementations of setup, phase transitions, report lifecycle, fire damage, ICM flow, destroyed-ship cleanup, and winner detection should either forward directly or wrap only the remaining dialog-lifetime concerns that must stay on the wx shell.

**Acceptance Criteria**

- `FBattleScreen` no longer stores duplicate copies of the tactical mechanics state that now lives in `FTacticalGame`.
- `getState`, `setState`, `getPhase`, `setPhase`, `getShip`, `setShip`, `getWeapon`, `setWeapon`, ship-list accessors, move-complete accessors, and player-turn accessors all forward to the model.
- `setupFleets`, report helpers, electrical-fire flow, ICM flow, and destroyed-ship cleanup use the model-owned state and mechanics paths.
- Any redraws still triggered from `FBattleScreen` happen through the existing wx surfaces without restoring model/view coupling in `FTacticalGame`.
- `FBattleBoard` and `FBattleDisplay` can continue compiling against `FBattleScreen` without Milestone 8 changes.

**Documentation Impact**

- Milestone tracking notes should state that the runtime tactical state handoff to `FTacticalGame` is complete, but rendering/input extraction remains Milestone 8 work.

### Subtask 4: Replace pre-delegation tactical tests with Milestone 7 delegation coverage

**Description**

Update the tactical test suite so it verifies the new delegated architecture instead of pinning the old direct-ownership implementation. Existing source-inspection tests should be revised where they currently require `FBattleScreen` to own report/dialog logic directly, and new checks should assert that `FBattleScreen` now wires `WXTacticalUI`, delegates state to `FTacticalGame`, and keeps only the remaining wx-shell responsibilities.

**Acceptance Criteria**

- Tests no longer fail solely because `FBattleScreen` mentions `WXTacticalUI` or delegates to `FTacticalGame`.
- Tactical tests verify that `FBattleScreen` owns a tactical model and forwards key state/report methods to it.
- Tests still protect the guarded close-path behavior on `FBattleScreen`.
- Tactical test build wiring includes the new or updated Milestone 7 tests.
- The updated suite validates the intended Milestone 7 architecture without requiring Milestone 8 rendering extraction.

**Documentation Impact**

- No documentation update expected unless the team maintains a separate tactical test inventory.

## Dependency Ordering

1. Subtask 1 must land first because `FBattleScreen` delegation depends on a complete model-facing API.
2. Subtask 2 depends on Subtask 1 because constructor wiring needs the model API and UI installation seam.
3. Subtask 3 depends on Subtasks 1-2 because forwarding cannot replace local state until the delegated runtime objects exist.
4. Subtask 4 depends on the final delegated shape from Subtasks 1-3 so the tests assert the target architecture instead of an intermediate transition.

## Parallelization Notes

- Test scaffolding for Subtask 4 can begin after the intended delegated `FBattleScreen` surface is fixed, but final assertions should wait until Subtask 3 settles the forwarding shape.
- Any `src/tactical/Makefile` dependency maintenance can happen alongside Subtask 2 or Subtask 3 once include ownership is clear.

## Implementer Agent Prompts

### Prompt 1

You are the implementer agent

Allowed files:
- `/home/tstephen/repos/SSW/include/tactical/FTacticalGame.h`
- `/home/tstephen/repos/SSW/src/tactical/FTacticalGame.cpp`

Task:
- Extend `FTacticalGame` so it can serve as the canonical tactical state owner for Milestone 7.
- Add the public accessors, mutators, and UI-installation seam needed for `FBattleScreen` to delegate its current tactical state API without keeping duplicate storage.
- Keep the model wx-free and aligned with the existing additive tactical architecture.

Acceptance criteria:
- `FTacticalGame` exposes the state and report APIs `FBattleScreen` needs to forward.
- The model has a safe way to receive the active `ITacticalUI`.
- No wx headers or wx types are introduced into the tactical model.

### Prompt 2

You are the implementer agent

Allowed files:
- `/home/tstephen/repos/SSW/include/tactical/FBattleScreen.h`
- `/home/tstephen/repos/SSW/src/tactical/FBattleScreen.cpp`
- `/home/tstephen/repos/SSW/src/tactical/Makefile`

Task:
- Rework `FBattleScreen` into a wx dialog shell that owns `FTacticalGame` and `WXTacticalUI`.
- Wire the constructor and destructor for safe ownership.
- Preserve the existing child widget creation and guarded close-path behavior.
- Update `src/tactical/Makefile` only if dependency lines need to reflect the new includes.

Acceptance criteria:
- `FBattleScreen` owns and initializes `FTacticalGame*` and `WXTacticalUI*`.
- Delegated runtime objects are cleaned up safely.
- Existing modal close behavior remains guarded in `FBattleScreen`.
- Any Makefile change is limited to dependency maintenance needed for the new compile surface.

### Prompt 3

You are the implementer agent

Allowed files:
- `/home/tstephen/repos/SSW/include/tactical/FBattleScreen.h`
- `/home/tstephen/repos/SSW/src/tactical/FBattleScreen.cpp`
- `/home/tstephen/repos/SSW/include/tactical/FTacticalGame.h`
- `/home/tstephen/repos/SSW/src/tactical/FTacticalGame.cpp`

Task:
- Replace `FBattleScreen`'s duplicated tactical mechanics logic with forwarding to `FTacticalGame`.
- Forward the public state API used by `FBattleBoard` and `FBattleDisplay`.
- Route setup, phase changes, tactical report lifecycle, electrical-fire handling, ICM handling, and destroyed-ship cleanup through the model while leaving only wx-shell concerns on `FBattleScreen`.

Acceptance criteria:
- `FBattleScreen` no longer keeps duplicate canonical copies of tactical mechanics state that now lives in `FTacticalGame`.
- Key getters/setters and combat/report helpers forward to the model.
- `FBattleBoard` and `FBattleDisplay` continue compiling against `FBattleScreen` without Milestone 8 rewrites.

### Prompt 4

You are the implementer agent

Allowed files:
- `/home/tstephen/repos/SSW/tests/tactical/FTacticalBattleScreenReportContextTest.h`
- `/home/tstephen/repos/SSW/tests/tactical/FTacticalBattleScreenReportContextTest.cpp`
- `/home/tstephen/repos/SSW/tests/tactical/FTacticalBattleScreenElectricalFireTest.h`
- `/home/tstephen/repos/SSW/tests/tactical/FTacticalBattleScreenElectricalFireTest.cpp`
- `/home/tstephen/repos/SSW/tests/tactical/FTacticalBattleScreenDelegationTest.h`
- `/home/tstephen/repos/SSW/tests/tactical/FTacticalBattleScreenDelegationTest.cpp`
- `/home/tstephen/repos/SSW/tests/tactical/Makefile`
- `/home/tstephen/repos/SSW/tests/tactical/TacticalTests.cpp`

Task:
- Update the tactical regression suite for Milestone 7.
- Remove pre-delegation assumptions that `FBattleScreen` must not reference `WXTacticalUI` or `FTacticalGame`.
- Add focused coverage that `FBattleScreen` now delegates runtime state to the tactical model while retaining guarded close behavior.

Acceptance criteria:
- Tactical tests validate the Milestone 7 delegated architecture rather than the Milestone 6 additive-only state.
- The guarded `FBattleScreen` close path remains covered.
- The tactical test runner builds and includes the Milestone 7 delegation tests.

## Output Artifact Path

- Plan file: `/home/tstephen/repos/SSW/plans/milestone7-fbattlescreen-delegation-plan.md`
- Supporting artifacts directory: `/home/tstephen/repos/SSW/artifacts/gui_sep/milestone7-fbattlescreen-delegation`
