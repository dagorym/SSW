# Implementer Report — Milestone 8 Remediation Subtask 1

## Summary
Established and documented an explicit destroyed-ship cleanup lifecycle contract for delegated fire resolution across `FTacticalGame`, `FBattleScreen`, and `FBattleDisplay`.

## Workflow / Plan Steps Completed
1. Confirmed required agent definitions and worktree context (`AGENTS.md`, `implementer.yaml`, branch/cwd).
2. Reviewed current fire cleanup flow in model/screen/display files.
3. Added explicit contract documentation and seam APIs in allowed files.
4. Validated with tactical build/tests.
5. Committed implementation changes.

## Cleanup Lifecycle Contract (Agreed Sequence)
1. `FBattleDisplay::{onDefensiveFireDone,onOffensiveFireDone}` invokes `m_parent->resolveCurrentFirePhase()`.
2. `FTacticalGame::resolveCurrentFirePhase()` delegates to `fireAllWeapons()`.
3. `FTacticalGame::fireAllWeapons()` runs model fire resolution, then calls `clearDestroyedShips()` to remove destroyed ships from model-owned state and capture IDs in `m_lastDestroyedShipIDs`.
4. `FBattleDisplay` then calls `m_parent->clearDestroyedShips()`.
5. `FBattleScreen::clearDestroyedShips()` is the wx-side cleanup seam; it consumes `getLastDestroyedShipIDs()`, triggers wx redraw/cleanup orchestration, handles winner notification path, then clears bookkeeping exactly once via `clearLastDestroyedShipIDs()`.

This preserves model-owned fire resolution and wx-owned cleanup orchestration, while keeping destroyed IDs available until screen-side consumption.

## Files Changed
- `include/tactical/FTacticalGame.h`
- `src/tactical/FTacticalGame.cpp`
- `src/tactical/FBattleScreen.cpp`
- `src/tactical/FBattleDisplay.cpp`

## Validation Commands Run
1. `make -C src/tactical && cd tests/tactical && make && ./TacticalTests` (baseline) — **PASS**
2. `make -C src/tactical && cd tests/tactical && make && ./TacticalTests` (after initial edits) — **FAIL**
   - Failure: source-assertion test expected legacy line `const int liveShips = m_tacticalGame->clearDestroyedShips();` in `FBattleScreen::clearDestroyedShips()`.
3. `make -C src/tactical && cd tests/tactical && make && ./TacticalTests` (after adjustment) — **PASS**

## Implementation Commit
- `042c3f458b74266802c0f482722053c5305d093f`
