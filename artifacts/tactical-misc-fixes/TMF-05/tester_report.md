# Tester Report — TMF-05: End-of-Move Single Facing Change

## Summary

All behavioral coverage requirements for TMF-05 are implemented and passing. No regressions introduced.

## Coverage Delivered

### Model-side behavioral tests (`tests/tactical/FTacticalEndOfMoveTurnTest`)

17 CppUnit tests covering the full canApply/apply/finalize/clear lifecycle:

| Test | What It Asserts |
|---|---|
| `testCanApplyTurnBothDirectionsWhenMRAvailableAndMinMoveSatisfied` | Both directions enabled when MR>0, speed=0 (minMove<=0) |
| `testCannotApplyTurnWhenMRIsZero` | Returns false for both directions when ship MR=0 |
| `testCannotApplyTurnBeforeMinimumMoveIsSatisfied` | Blocked when nMoved < speed - ADF (speed=5, 0 hexes moved) |
| `testCannotApplyTurnWhenPathEndHexHasMRTurnFlag` | Blocked when path endpoint already has MR_TURN flag |
| `testApplyLeftTurnSetsHeadingImmediatelyForVisualFeedback` | applyEndOfMoveTurn(+1) changes ship heading immediately |
| `testApplyRightTurnSetsHeadingImmediatelyForVisualFeedback` | applyEndOfMoveTurn(-1) changes ship heading immediately |
| `testPendingTurnRecordsOriginAndNewFacing` | pendingEndOfMoveFacing and endOfMoveOriginFacing both set correctly |
| `testOnlyReverseDirectionEnabledAfterLeftTurnApplied` | After left turn pending: only right enabled |
| `testOnlyReverseDirectionEnabledAfterRightTurnApplied` | After right turn pending: only left enabled |
| `testApplyingReverseWhenLeftPendingRestoresOriginHeading` | Reversing left turn restores original heading |
| `testApplyingReverseWhenRightPendingRestoresOriginHeading` | Reversing right turn restores original heading |
| `testReverseClearsPendingFacingFields` | Both pending fields reset to -1 after reverse |
| `testSecondSameDirectionTurnIsBlockedWhenPendingExists` | Same direction blocked while pending exists |
| `testFinalizeMovementStateCommitsPendingFacing` | finalizeMovementState() commits pending to curHeading/finalHeading |
| `testFinalizeWithNoPendingUsesCurrentHeading` | finalizeMovementState() with no pending preserves heading unchanged |
| `testHexClickWhilePendingTurnClearsAndRestoresOriginHeading` | handleMoveHexSelection clears pending and restores origin heading before hex |
| `testSpeedZeroDecelerationShipCanUseEndOfMoveTurn` | Speed=0 ship with any nMoved>=0 satisfies minMove gate |

### GUI behavioral tests (`tests/gui/TacticalGuiLiveTest`)

3 CppUnit tests added to `TacticalGuiLiveTest`:

| Test | What It Asserts |
|---|---|
| `testTurnButtonPanelHiddenInNonMovePhase` | Panel hidden after draw() in PH_DEFENSE_FIRE and PH_ATTACK_FIRE |
| `testTurnButtonPanelShownAndEnableStateReflectsModelInMovePhase` | Both buttons enabled initially; only reverse enabled after pending turn; both re-enabled after reverse |
| `testTurnButtonClickAppliesEndOfMoveTurnToModel` | Click Turn Left sets pendingEndOfMoveFacing != -1 and changes ship heading by +1 mod 6 |

## Build Results

- Tactical tests: **244 tests, 0 failures, 0 errors** (clean)
- GUI tests: **63 tests, 8 failures, 0 errors** — all 8 failures are pre-existing (not introduced by this work)

## Pre-Existing Failures (unchanged)

1. `StrategicGuiLiveTest::testStrategicDialogsUseStaticBoxChildParents`
2. `StrategicGuiLiveTest::testWXStrategicUISourceGuardsRuntimeAndPreservesParentlessFlow`
3. `StrategicGuiLiveTest::testRemediatedStrategicDialogsUseFirstShowSizingContract`
4. `TacticalGuiLiveTest::testTacticalActionButtonsStayBelowPromptReservationAcrossPhases`
5. `TacticalGuiLiveTest::testSeekerActivationPanelSourceContracts`
6. `TacticalGuiLiveTest::testSetupPlacementSourceRowsAndOrdnanceColorContracts`
7. `TacticalGuiLiveTest::testBattleDisplayNarrowWidthStacksShipStatsBelowButtons`
8. `TacticalGuiLiveTest::testICMSelectionDialogInteractionFinalizesAssignedCountsAndAmmo`

## Files Modified/Created

- `tests/tactical/FTacticalEndOfMoveTurnTest.h` (new)
- `tests/tactical/FTacticalEndOfMoveTurnTest.cpp` (new)
- `tests/tactical/TacticalTests.cpp` (registered new test suite)
- `tests/SSWTests.cpp` (registered new test suite)
- `tests/gui/TacticalGuiLiveTest.h` (3 new test declarations + doxygen)
- `tests/gui/TacticalGuiLiveTest.cpp` (3 new test methods + 5 peer accessors + `makeBattleScreenInMovePhase` helper)

## Tester

claude-sonnet-4-6 (medium), Jun 30, 2026
