# SMC-02 Implementer Report

## Status

success

## Task

Rework the seeker activation screen so board clicks activate seekers and the lower status window lists and deactivates all activated seekers, and make activations visible on the board. Builds on the SMC-01 model APIs.

## Branch and Commit

- Branch: `seeker-fix`
- Implementation commit: `04a9cdf`

## Changed Files

- `src/tactical/FBattleBoard.cpp`
- `src/tactical/FBattleDisplay.cpp`
- `tests/gui/TacticalGuiLiveTest.cpp`
- `tests/tactical/FTacticalBattleBoardRendererDelegationTest.cpp`
- `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`

## Implementation Summary

### 1. FBattleDisplay::drawSeekerActivation (src/tactical/FBattleDisplay.cpp)

Replaced the selected-stack inactive list with a list of all activated seekers for the moving player. The method now:
- Updates instruction text to: "Click a seeker stack on the board to activate one seeker." and "Click a row below to deactivate an activated seeker."
- Calls `m_parent->getActiveSeekersByMovingPlayer()` to get all activated seekers for the current player.
- Renders each as a "Deactivate seeker #N (heading H, allowance A)" clickable row, storing hit regions and IDs.
- Shows "No seekers activated yet." when none are activated.

### 2. Activation-phase board clicks (src/tactical/FBattleBoard.cpp:onLeftUp)

Changed PH_SEEKER_ACTIVATION board clicks to call `m_parent->activateInactiveSeekerAtHex(hex)` followed by `m_parent->reDraw()` instead of `selectSeekerActivationHex`. This activates the lowest-seekerID inactive seeker owned by the moving player at the clicked hex.

### 3. FBattleDisplay::checkSeekerActivationSelection (src/tactical/FBattleDisplay.cpp)

Row clicks now call `m_parent->deactivateActiveSeekerByID(m_seekerActivationSeekerIDs[i])` to deactivate the selected activated seeker. Added the missing `m_parent->reDraw()` call after the deactivation.

### 4. FBattleBoard::drawSeekerMissiles (src/tactical/FBattleBoard.cpp)

For `PH_SEEKER_ACTIVATION`, now renders BOTH inactive stacks (so they remain clickable for activation) AND active seekers for the moving player (so activations are visible on the board with heading-based rotation).

### 5. FBattleScreen delegation

All three model APIs (`deactivateActiveSeekerByID`, `activateInactiveSeekerAtHex`, `getActiveSeekersByMovingPlayer`) were already delegated through `FBattleScreen` from SMC-01. No new delegation methods needed.

## Test Updates

Source-inspection regression tests were updated to lock the new flow contracts (as required consequences of the approved behavior changes):
- `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`: updated assertions for `checkSeekerActivationSelection` (deactivate instead of activate, reDraw present) and `drawSeekerActivation` (new instruction text, new accessor, new row label).
- `tests/tactical/FTacticalBattleBoardRendererDelegationTest.cpp`: updated `testOnLeftUpRoutesActivationPhaseThroughSeekerSelection` to assert `activateInactiveSeekerAtHex` and `reDraw` instead of `selectSeekerActivationHex`.
- `tests/gui/TacticalGuiLiveTest.cpp`: updated `testSeekerActivationPanelSourceContracts` to match the new instruction text, deactivate method, and board-click method.

## Validation Results

- **TacticalTests**: 189/189 pass (no regressions).
- **GuiTests**: 43 run, 8 failures — all 8 were pre-existing before these changes. No new failures introduced.

## Artifacts Written

- `artifacts/seeker-missile-ui-cleanup/smc-02/implementer_report.md`
- `artifacts/seeker-missile-ui-cleanup/smc-02/tester_prompt.txt`
- `artifacts/seeker-missile-ui-cleanup/smc-02/implementer_result.json`
