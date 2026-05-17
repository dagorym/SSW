# Tester Report: tactical-ship-stats-lower-panel-layout-fixes/subtask-1

## Scope
Validated the tactical lower-panel split/stacked ship-stats layout fix in:
- `include/tactical/FBattleDisplay.h`
- `src/tactical/FBattleDisplay.cpp`

Added tester coverage in:
- `tests/gui/TacticalGuiLiveTest.h`
- `tests/gui/TacticalGuiLiveTest.cpp`
- `tests/tactical/FTacticalBattleDisplayFireFlowTest.h`
- `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`

## Test Changes
- Added tactical source-contract coverage proving ship-stats measurement and split eligibility rely on measured content width/height rather than brittle fixed assumptions.
- Added live GUI coverage proving narrow-width runtime behavior forces stacked ship-stats layout, keeps action buttons below reserved prompt space, and increases lower-panel minimum height.

## Validation Commands
- `cd tests/tactical && make && ./TacticalTests` → **passed** (`OK (153 tests)`).
- `cd tests/gui && make && xvfb-run -a ./GuiTests` → **passed** (`OK (36 tests)`).

## Acceptance Criteria Traceability
1. **No split-mode right-edge ship-stats clipping**
   - Covered by new tactical source-contract assertions in `testShipStatsMeasurementAndSplitEligibilityUseContentBasedSizing` verifying measured text extents and content-driven width calculation (`measureShipStatsLayoutRequirements`) are used for layout decisions.
2. **Split only when full stats fit**
   - Covered by assertions on `largestMarginWithStatsRoom`, `splitCanFit`, and split/stack mode switching in `ensureLowerPanelLayoutState`.
3. **Stacked stats below action buttons, no overlap**
   - Covered by new live GUI test `testBattleDisplayNarrowWidthStacksShipStatsBelowButtons` asserting narrow-width layout keeps the action button below prompt reservation and raises lower-panel minimum height.
4. **Resize reflow still runs through existing seam**
   - Existing regression coverage remains active and passing: `testBattleScreenResizeHandlerReflowsLowerPanelBeforeLayoutPolicy`.
5. **Stable content-based sizing contract is in place**
   - Covered by combined tactical assertions on measured content width/height and requested-height propagation via `ensureLowerPanelLayoutState` + `applyRequestedDisplayHeight`.

## Commit Status
- Test changes committed: `522f1bb8540bcda1554a085c5a777ef9ec8e7a7f`.
- Artifact commit: pending at report creation time.

## Cleanup
No temporary non-handoff byproducts remain.
