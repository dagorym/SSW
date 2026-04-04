# Milestone 5 Tweak Plan: Restore `FTacticalGame` ICM Resolution and Destroyed-Ship Cleanup

## Feature Restatement

Starting from the current `FTacticalGame` additive model on this branch, implement two tactical-state corrections:

1. `FTacticalGame::fireICM()` must build `ICMData` entries that point at the defending ships in the target hex, and it must only enqueue weapons that actually have at least one friendly ship able to spend ICMs.
2. `FTacticalGame::clearDestroyedShips()` must remove destroyed ships from all model-owned tactical state, not just the side ship list, so dead ships do not remain in hex occupancy or per-turn bookkeeping.

The work remains additive. No wx tactical class should be rewired to delegate to `FTacticalGame` yet. The goal is to make the model-owned tactical state internally coherent so later milestones can rely on it safely.

## Planner Agent Activation

- Requested agent: `planner`
- Repository-local definition found: no
- Shared definition found: yes
- Active definition path: `/home/tstephen/repos/agents/agents/planner.md`
- Precedence decision: shared `planner` definition from `~/repos/agents` applies because no repository-local `planner` definition was found

## Confirmed Repository Facts

- [`include/tactical/FTacticalGame.h`](/home/tstephen/repos/SSW/include/tactical/FTacticalGame.h) currently exposes `fireICM()`, `clearDestroyedShips()`, `findTurnData()`, `m_ICMData`, `m_hexData[100][100]`, and `m_turnInfo`.
- [`src/tactical/FTacticalGame.cpp`](/home/tstephen/repos/SSW/src/tactical/FTacticalGame.cpp) currently implements `fireICM()` by creating `ICMData` records with `d->vehicles = NULL` and pushing them into `m_ICMData` when a weapon has a target and non-zero ICM modifier.
- [`src/tactical/FTacticalGame.cpp`](/home/tstephen/repos/SSW/src/tactical/FTacticalGame.cpp) currently implements `clearDestroyedShips()` by erasing dead ships from the active side ship list only; it does not remove those ship IDs from `m_hexData` or `m_turnInfo`.
- [`src/tactical/FBattleScreen.cpp`](/home/tstephen/repos/SSW/src/tactical/FBattleScreen.cpp) shows the intended ICM-selection behavior for the live wx flow: for each candidate weapon, locate the ships in the target hex, then only keep the record when at least one same-side ship in that hex has usable ICM ammunition and an undamaged power/ICM system.
- [`src/tactical/FBattleBoard.cpp`](/home/tstephen/repos/SSW/src/tactical/FBattleBoard.cpp) already has legacy helpers for finding the ships in a target hex and for removing a ship from board occupancy by ID.
- [`src/gui/ICMSelectionGUI.cpp`](/home/tstephen/repos/SSW/src/gui/ICMSelectionGUI.cpp) dereferences `d->vehicles` unconditionally when building the ICM allocation grid, so queued `ICMData` entries must carry a non-null vehicle list.
- [`include/ships/FVehicle.h`](/home/tstephen/repos/SSW/include/ships/FVehicle.h) defines `ICMData` as a `FWeapon * weapon` plus `VehicleList * vehicles`.
- [`tests/tactical/FTacticalGameMechanicsTest.cpp`](/home/tstephen/repos/SSW/tests/tactical/FTacticalGameMechanicsTest.cpp) currently validates `FTacticalGame` mostly through source-text inspection rather than stateful behavior.

## Assumptions

- `FTacticalGame` remains an additive model in Milestone 5; the active runtime path is still `FBattleScreen` + `FBattleBoard` + `FBattleDisplay`.
- The tweak should preserve the existing `ICMData` shape because GUI code already depends on it.
- It is acceptable to add internal helpers to `FTacticalGame` to search model hex occupancy and to remove ship IDs from model-owned state.
- Behavioral tests may use a small test harness subclass or equivalent exposure pattern to seed `m_hexData`, `m_turnInfo`, and `m_ICMData` for verification without changing production visibility more than necessary.

## Files To Modify

- [`include/tactical/FTacticalGame.h`](/home/tstephen/repos/SSW/include/tactical/FTacticalGame.h)
- [`src/tactical/FTacticalGame.cpp`](/home/tstephen/repos/SSW/src/tactical/FTacticalGame.cpp)
- Likely new or expanded test file under [`tests/tactical`](/home/tstephen/repos/SSW/tests/tactical)
- [`tests/tactical/Makefile`](/home/tstephen/repos/SSW/tests/tactical/Makefile)
- [`tests/tactical/TacticalTests.cpp`](/home/tstephen/repos/SSW/tests/tactical/TacticalTests.cpp)
- Likely [`tests/SSWTests.cpp`](/home/tstephen/repos/SSW/tests/SSWTests.cpp) if the team wants the new `FTacticalGame` behavior coverage in the top-level suite instead of tactical-only coverage

## Overall Documentation Impact

- No end-user documentation changes are expected.
- No milestone design rewrite is required.
- Internal code comments should explain any new model helper that mirrors legacy board behavior, especially if a helper exists to find target-hex occupants or to purge a ship from model-owned tactical state.

## Subtasks

### Subtask 1: Make destroyed-ship cleanup remove stale model state

**Description**

Extend `FTacticalGame` so destroyed ships are removed from every model-owned structure that can still reference them. Today `clearDestroyedShips()` trims only the side ship list. The tweak should add explicit model cleanup helpers that purge a dead ship ID from:

- the owning side ship list,
- every `m_hexData[c][r].ships` occupancy list,
- `m_turnInfo`,
- and any related model selection/bookkeeping that would otherwise keep a dangling reference to a destroyed ship.

The cleanup should remain additive and internal to `FTacticalGame`; no wx tactical caller changes are required.

**Acceptance Criteria**

- `FTacticalGame` has a dedicated internal path to remove a ship ID from model-owned hex occupancy.
- `FTacticalGame` has a dedicated internal path to erase the same ship ID from `m_turnInfo`.
- `clearDestroyedShips()` removes dead ships from the owning side ship list and also invokes the model cleanup path so no dead ship remains in `m_hexData` or `m_turnInfo`.
- Winner resolution still behaves correctly after cleanup: if the cleared side has no live ships left, `m_hasWinner` and `m_winnerID` are updated consistently.
- The implementation does not require `FBattleBoard.h` or any wx tactical type.

**Documentation Impact**

- Add a short code comment only if needed to state that the helper removes dead ships from model-owned tactical state, not from GUI state.

### Subtask 2: Restore `fireICM()` target-hex resolution and eligibility filtering

**Description**

Rework `FTacticalGame::fireICM()` so it builds usable `ICMData` entries for the same conceptual selection flow already used in the tactical GUI. For each live firing ship and each weapon:

1. Ignore weapons with no target or no ICM modifier.
2. Locate the current hex occupancy list for the targeted ship by searching `m_hexData`.
3. Skip the weapon if the target ship cannot be found in model hex occupancy.
4. Scan the target hex occupants and determine whether at least one ship on the target’s side has usable ICMs:
   - the ship has an ICM defense,
   - that defense still has ammo,
   - the ship’s power system is not damaged,
   - the ICM defense itself is not damaged.
5. Only if such a defender exists, enqueue an `ICMData` entry with `weapon` set to the firing weapon and `vehicles` set to the target hex’s `VehicleList`.

This makes `m_ICMData` safe for any later GUI consumer that expects a non-null vehicle list and a filtered set of real interception choices.

**Acceptance Criteria**

- `FTacticalGame` has an internal helper or equivalent implementation path that can find the `VehicleList` for a target ship by scanning `m_hexData`.
- `fireICM()` no longer pushes any `ICMData` record with `vehicles == NULL`.
- `fireICM()` only enqueues entries for weapons whose target hex contains at least one same-side ship with usable ICM capability.
- `fireICM()` continues to clear old ICM records before collecting new ones.
- The implementation stays within the existing `ICMData` contract used by tactical GUI code.

**Documentation Impact**

- Add a brief code comment if needed to document that `m_ICMData` stores only actionable interception opportunities and that `vehicles` must point at the target hex occupant list.

### Subtask 3: Add behavioral regression coverage for model ICM collection and cleanup

**Description**

Add focused `FTacticalGame` behavior tests that exercise the two fixes against seeded model state rather than only checking source text. A practical approach is to introduce a small tactical-test harness subclass that exposes the minimum state needed to seed `m_hexData`, `m_turnInfo`, and side ship lists. The tests should cover:

- destroyed ship removal from ship lists, hex occupancy, and turn bookkeeping,
- and `fireICM()` collection of only actionable ICM opportunities with a non-null `vehicles` list.

These tests should verify state transitions directly so later refactors do not silently regress the model’s internal consistency.

**Acceptance Criteria**

- The tactical test suite contains at least one behavior-level test for destroyed-ship cleanup and one for ICM collection.
- The cleanup test seeds a dead ship into side lists plus model occupancy/bookkeeping, calls `clearDestroyedShips()`, and verifies the ship ID is gone from all relevant structures.
- The ICM test seeds a target hex with a valid defending ship or escort that can spend ICMs, calls `fireICM()`, and verifies that `m_ICMData` contains a record with non-null `vehicles` pointing at the expected occupant list.
- The ICM test also covers the negative case where no ship in the target hex has usable ICMs and verifies that no record is queued.
- The relevant tactical test target builds and passes.

**Documentation Impact**

- No documentation update expected.

## Dependency Ordering

1. Subtask 1 should land first because stale hex and turn-data cleanup defines the model-consistency rules that the rest of the tweak relies on.
2. Subtask 2 depends on the settled model-state rules from Subtask 1 and should reuse the same ship-ID/hex-occupancy assumptions.
3. Subtask 3 should be implemented after the production helper names and behavior are stable, though test scaffolding can begin earlier.

## Parallelization Notes

- Subtasks 1 and 2 should stay with one owner because they both modify `FTacticalGame.h` and `FTacticalGame.cpp`.
- Test scaffolding for Subtask 3 can begin in parallel once the intended helper/API shape is agreed, but final assertions should wait for the production implementation to settle.

## Implementer Agent Prompts

### Prompt 1

You are the implementer agent

Allowed files:
- `/home/tstephen/repos/SSW/include/tactical/FTacticalGame.h`
- `/home/tstephen/repos/SSW/src/tactical/FTacticalGame.cpp`

Task:
- Extend `FTacticalGame` so destroyed ships are removed from all model-owned tactical state, not just the side ship list.
- Add internal helper logic to remove a ship ID from `m_hexData` occupancy and from `m_turnInfo`.
- Update `clearDestroyedShips()` to invoke that cleanup whenever it erases a dead ship.
- Preserve the current additive design and winner-resolution behavior.

Acceptance criteria:
- Dead ship IDs are removed from the side ship list, `m_hexData`, and `m_turnInfo`.
- `clearDestroyedShips()` still returns the surviving ship count for the cleared side.
- Winner bookkeeping remains correct when the cleared side is eliminated.
- No wx tactical header or runtime rewiring is introduced.

### Prompt 2

You are the implementer agent

Allowed files:
- `/home/tstephen/repos/SSW/include/tactical/FTacticalGame.h`
- `/home/tstephen/repos/SSW/src/tactical/FTacticalGame.cpp`

Task:
- Rework `FTacticalGame::fireICM()` so it collects only actionable interception opportunities.
- Add or reuse an internal helper that finds the target ship’s current hex occupant list by scanning `m_hexData`.
- For each candidate weapon, only enqueue `ICMData` when the target hex exists and contains at least one same-side ship with a usable ICM defense, remaining ammo, and working power/ICM systems.
- Ensure queued `ICMData` records carry a non-null `vehicles` pointer to the target hex’s `VehicleList`.

Acceptance criteria:
- `fireICM()` no longer pushes `ICMData` entries with `vehicles == NULL`.
- Weapons are queued only when an actual defending ship in the target hex can spend ICMs.
- `m_ICMData` is cleared before new collection starts.
- The implementation stays compatible with the existing `ICMData` struct and GUI expectations.

### Prompt 3

You are the implementer agent

Allowed files:
- `/home/tstephen/repos/SSW/tests/tactical`
- `/home/tstephen/repos/SSW/tests/tactical/Makefile`
- `/home/tstephen/repos/SSW/tests/tactical/TacticalTests.cpp`
- `/home/tstephen/repos/SSW/tests/SSWTests.cpp`

Task:
- Add behavior-level tactical regression tests for the two `FTacticalGame` tweaks.
- Use a focused test harness if needed to seed model-owned hex occupancy, turn data, side ship lists, and ICM records without expanding production visibility more than necessary.
- Verify both the positive and negative ICM collection paths and the destroyed-ship cleanup path.
- Register the tests in the appropriate tactical runners, and in `tests/SSWTests.cpp` only if the team wants the top-level suite to cover them as well.

Acceptance criteria:
- The tactical tests verify removal of dead ships from side lists, hex occupancy, and turn bookkeeping.
- The tactical tests verify `fireICM()` enqueues only actionable entries and that queued entries have non-null `vehicles`.
- A negative ICM case is covered.
- The relevant tactical test target builds and passes.

## Output Artifact Path

- Plan file: `/home/tstephen/repos/SSW/plans/milestone5-tweak-plan.md`
