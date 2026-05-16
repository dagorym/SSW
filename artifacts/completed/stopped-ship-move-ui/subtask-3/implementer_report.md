# Implementer Report — stopped-ship-move-ui/subtask-3

## Scope and constraints
- Role: implementer
- Allowed files: `src/tactical/FBattleBoard.cpp`, `src/tactical/FBattleDisplay.cpp`, `src/tactical/FBattleScreen.cpp`, `include/tactical/FBattleScreen.h`
- Shared artifact directory: `artifacts/stopped-ship-move-ui/subtask-3`

## Preflight summary
- Goal: surface stopped-ship move preview routes for all legal starting facings in PH_MOVE, keep nonzero-speed rendering unchanged, and keep `FBattleScreen` additive-forwarding only.
- Validation command: `cd tests && make tactical-tests && ./tactical/TacticalTests`
- Tester target files (guidance):
  - `tests/tactical/FTacticalBattleBoardRendererDelegationTest.cpp`
  - `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`
  - `tests/tactical/FTacticalMoveRouteSelectionTest.cpp`
  - `tests/tactical/FTacticalGameMechanicsTest.cpp`

## Implementation completed
Modified files:
- `src/tactical/FBattleBoard.cpp`
- `src/tactical/FBattleDisplay.cpp`
- `src/tactical/FBattleScreen.cpp`
- `include/tactical/FBattleScreen.h`

Key changes:
1. Added additive `FBattleScreen` forwarders for stopped-ship preview accessors:
   - `getStoppedShipPreviewRoutes()`
   - `getStoppedShipPreviewHeadingsForHex(const FPoint &)`
2. Updated `FBattleBoard::drawRoute(...)`:
   - In PH_MOVE for selected moving-side ships, when stopped-ship free-rotation conditions apply (`speed==0`, `nMoved==0`, `MR>0`), render all preview routes from `getStoppedShipPreviewRoutes()`.
   - Otherwise preserve existing rendering path using movement/left/right hex lists with moved-count offset.
3. Updated move prompt copy in `FBattleDisplay::drawMoveShip(...)` to describe preview-route-first interaction and explicitly mention rotate-in-place as adjacent-hex + Movement Done.

## Acceptance criteria mapping
- PH_MOVE stopped-ship selection highlights all legal start-route previews:
  - Implemented via preview-route loop in `FBattleBoard::drawRoute(...)` under stopped-ship selection guard.
- Board stays coherent after preview commitment and normal route continuation:
  - Rendering automatically falls back to normal movement/left/right route lists when `nMoved > 0`.
- Move prompt text reflects preview model, no hidden adjacent discovery dependency:
  - Prompt now calls out highlighted preview routes explicitly.
- Nonzero-speed route rendering unchanged:
  - Existing movement/left/right route drawing path remains unchanged in non-stopped flow.
- FBattleScreen API growth additive and forwarding-only:
  - Added two const accessors forwarding directly to `FTacticalGame`.

## Validation
Baseline before edits:
- `cd tests && make tactical-tests && ./tactical/TacticalTests` → PASS (`OK (143 tests)`)

Post-change:
- `cd tests && make tactical-tests && ./tactical/TacticalTests` → FAIL (1 expected source-string failure):
  - `FTacticalBattleDisplayFireFlowTest::testMovePromptDifferentiatesStoppedFacingNormalAndNoShipCases`
  - Failure reason: test still asserts legacy stopped-ship prompt strings (`"Select an adjacent hex to choose facing."`) while implementation now uses preview-route prompt wording required by this subtask.

## Commits
- Implementation/code commit: `a1d42ccf68ef16dbfe77ce0df53ec5cb896ce10d`
