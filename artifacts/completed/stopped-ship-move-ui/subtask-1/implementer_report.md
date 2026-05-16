# Implementer Report — stopped-ship-move-ui/subtask-1

## Scope and constraints
- Role: implementer
- Allowed files: `src/tactical/FTacticalGame.cpp`, `include/tactical/FTacticalGame.h`
- Shared artifact directory: `artifacts/stopped-ship-move-ui/subtask-1`

## Preflight summary
- Goal: add a model-owned stopped-ship preview-route surface that exposes legal starting-facing previews immediately on ship selection.
- Preserve existing movement/left/right route accessor semantics for ordinary movement.
- Preserve the `MR == 0` stopped-ship restriction.
- Validation command (given guidance): `cd tests && make tactical-tests && ./tactical/TacticalTests`
- Tester target files (given guidance):
  - `tests/tactical/FTacticalMoveRouteSelectionTest.cpp`
  - `tests/tactical/FTacticalGameMechanicsTest.cpp`
  - `tests/tactical/FTacticalModelSelectionHexClickSurfaceTest.cpp`

## Implementation completed
Modified files:
- `include/tactical/FTacticalGame.h`
- `src/tactical/FTacticalGame.cpp`

Key changes:
1. Added additive preview-route model type `FTacticalMovePreviewRoute` and new getters:
   - `getStoppedShipPreviewRoutes()`
   - `getStoppedShipPreviewHeadingsForHex(const FPoint&)`
2. Added model-owned preview caches for stopped-ship legal starting facings and reverse lookup from preview hex to starting heading(s).
3. Added `rebuildStoppedShipPreviewRoutes()` and `clearStoppedShipPreviewRoutes()` lifecycle wiring.
4. Rebuild now occurs as route highlights update and on initial move-route setup; preview data is cleared whenever movement highlights are cleared or the selected ship is not an active mover.
5. Preview generation is gated by the existing stopped-ship free-rotation guard (`speed == 0`, `nMoved == 0`, path length 1, `MR > 0`), so no new facing options are granted to MR==0 ships.

## Acceptance criteria mapping
- Stopped ship with speed 0, nMoved 0, legal maneuver authority exposes preview data for each legal starting facing:
  - Implemented via per-heading preview route generation and cache rebuild on selection/setup.
- Preview data supports rendering and clicked-hex ownership:
  - Each preview route includes a facing anchor hex and route hex list; reverse hex->heading list accessor is provided.
- Existing left/right/forward route semantics unchanged for non-stopped movement:
  - Existing accessors and core route selection flow remain intact; new surface is additive.
- MR==0 does not gain new facing options:
  - Preview generation uses the existing `canUseStoppedShipFreeRotation(... && ship->getMR() > 0)` guard.

## Validation
Baseline before edits:
- `cd tests && make tactical-tests && ./tactical/TacticalTests` → PASS (`OK (138 tests)`)

Post-change:
- `cd tests && make tactical-tests && ./tactical/TacticalTests` → PASS (`OK (138 tests)`)

## Commits
- Implementation/code commit: `46210048ff634bcc73406585dfdc9175452bbcbb`
