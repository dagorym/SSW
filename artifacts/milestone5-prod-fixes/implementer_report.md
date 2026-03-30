Subtask: m5-prod-fixes
Worktree: /home/tstephen/worktrees/gui_sep_ms5-implementer-20260329
Branch: gui_sep_ms5-implementer-20260329

## Summary
Implemented the production-code fixes in `FTacticalGame` for Milestone 5 subtasks 1 and 2:
- destroyed-ship cleanup now removes dead ship IDs from side ship list, model hex occupancy (`m_hexData`), and per-turn bookkeeping (`m_turnInfo`)
- ICM collection now queues only actionable interception opportunities and always stores a non-null `vehicles` pointer

## Modified files
- `include/tactical/FTacticalGame.h`
- `src/tactical/FTacticalGame.cpp`

## Implementation details
### Destroyed ship model-state cleanup
Added internal helper paths on `FTacticalGame`:
- `removeShipFromHexOccupancy(unsigned int shipID)`
- `removeShipFromTurnInfo(unsigned int shipID)`
- `removeShipFromModelState(unsigned int shipID)`

`clearDestroyedShips()` now calls `removeShipFromModelState((*itr)->getID())` before erasing each dead ship from the side ship list.

### ICM collection filtering and non-null vehicles
Added internal helper paths on `FTacticalGame`:
- `findHexOccupantsForShip(unsigned int shipID)` (+ const overload)
- `hasUsableICMDefenderInHex(const VehicleList&, unsigned int defendingSideID) const`

`fireICM()` behavior now:
1. clears previous `m_ICMData`
2. skips weapons with no target or zero ICM modifier
3. finds target ship hex occupants by scanning `m_hexData`
4. requires at least one same-side occupant with usable ICM (has ICM defense, ammo > 0, power system intact, defense not damaged)
5. only then enqueues `ICMData` with `vehicles` set to the found hex occupant list (never NULL)

Winner bookkeeping behavior in `clearDestroyedShips()` is preserved.

## Validation run (from assigned worktree)
1. Baseline before changes:
   - `cd /home/tstephen/worktrees/gui_sep_ms5-implementer-20260329/tests/tactical && make -s && ./TacticalTests`
   - Result: `OK (63 tests)`
2. Post-change validation:
   - `cd /home/tstephen/worktrees/gui_sep_ms5-implementer-20260329/tests/tactical && make -s && ./TacticalTests`
   - Result: `OK (63 tests)`
3. Final re-run after cleanup/format pass:
   - `cd /home/tstephen/worktrees/gui_sep_ms5-implementer-20260329/tests/tactical && make -s && ./TacticalTests`
   - Result: `OK (63 tests)`

## Implementation commit
`475cd7e057b600cc7e5f396e05b5004c9b380d59`
