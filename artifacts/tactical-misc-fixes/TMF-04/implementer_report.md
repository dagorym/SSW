# Implementer Report — TMF-04

**Status:** success

## Task Summary

Fixed the Movement Done gating bug in `FTacticalGame::checkMoveStatus()` so that the defending player (and any player) cannot complete their move phase before every ship with `speed > ADF` has moved the required minimum hexes (`speed - ADF`).

## Root Cause

`checkMoveStatus()` used `m_shipPos` as the position fallback when a ship's movement path had not yet been started (`path.getPathLength() <= 1`). `m_shipPos` tracks only the last selected ship and is stale — or holds the sentinel value `(-1,-1)` — at the start of any player's move phase. The defending player is especially affected because `m_shipPos` still holds the attacking player's previously selected hex when the defender's move phase begins.

`findNextHex((-1,-1), heading)` produces a negative-coordinate hex; the bounds check `(x >= 0 && x < 55 && y >= 0 && y < 39)` fails; `finished` is never set `false`; and `setMoveComplete(true)` fires immediately — enabling "Movement Done" before any ship has moved.

## Fix

In `src/tactical/FTacticalGame.cpp::checkMoveStatus()`, replaced the ternary:

```cpp
FPoint pos = (turnData->path.getPathLength() > 1) ? turnData->path.endPoint() : m_shipPos;
```

with an explicit branch:

```cpp
FPoint pos;
if (turnData->path.getPathLength() > 0) {
    pos = turnData->path.endPoint();
} else if (!findShipHex((*itr)->getID(), pos)) {
    // Ship not yet placed on the board (setup phase) — skip.
    continue;
}
```

When `path.getPathLength() > 0`, the path's end point is used (either the starting hex if just selected, or the latest moved hex). When the path is empty (phase entry, no ship selected yet), `findShipHex()` looks up the ship's actual board position. If `findShipHex()` returns false (ship not placed — setup phase), that ship is skipped via `continue`.

A Doxygen block was added to the function in `src/tactical/FTacticalGame.cpp`. A brief `///` comment was added to the declaration in `include/tactical/FTacticalGame.h`.

## Changed Files

- `include/tactical/FTacticalGame.h` — added `///` Doxygen comment to `checkMoveStatus()` declaration
- `src/tactical/FTacticalGame.cpp` — replaced `m_shipPos` fallback with `findShipHex()` in `checkMoveStatus()`; added Doxygen block

## Validation

- **Build:** `make` from repo root — clean, no warnings
- **Tactical tests:** `make tactical-tests && ./tactical/TacticalTests` — 226/226 passed

## Implementation/Code Commit Hash

`999293f6e38bbe4b8dbd76f1bc3983cdb628b0d5`

## Artifacts Written

- `artifacts/tactical-misc-fixes/TMF-04/implementer_report.md`
- `artifacts/tactical-misc-fixes/TMF-04/tester_prompt.txt`
- `artifacts/tactical-misc-fixes/TMF-04/implementer_result.json`
