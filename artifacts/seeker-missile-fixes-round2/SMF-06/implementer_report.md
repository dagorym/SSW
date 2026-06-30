# Implementer Report — SMF-06

## Status
success

## Task summary
SMF-06: Keep impacting seeker visible until after damage report.

## Changed files
- include/tactical/FTacticalGame.h (Doxygen updates for resolveActiveSeekersForMovingPlayer, applyMovementSeekerDamage)
- src/tactical/FTacticalGame.cpp (keep contacting seekers in nextStates, add requestRedraw() before dialogs)
- include/tactical/FBattleBoard.h (Doxygen update for drawSeekerPaths)
- src/tactical/FBattleBoard.cpp (extend drawSeekerPaths gate to PH_SEEKER_ACTIVATION)
- tests/tactical/FTacticalSeekerMovementTest.cpp (updated seeker count assertions)
- tests/tactical/FTacticalBattleBoardRendererDelegationTest.cpp (updated phase gate assertion)

## Validation outcome
PASS: 198/198 tactical tests pass. GUI tests: 9 pre-existing failures unchanged.

## Implementation/code commit hash
bdae93c

## Key implementation details
1. resolveActiveSeekersForMovingPlayer: contacting seekers are now pushed to nextStates (two call sites: same-hex contact and movement-step contact), so they stay in m_seekerMissiles with movementPath intact until applyMovementSeekerDamage removes them after dialogs.
2. applyMovementSeekerDamage: calls m_ui->requestRedraw() before resolvePendingSeekerDetonationDamage.
3. beginSeekerActivationPhase / completeSeekerActivationPhase: calls m_ui->requestRedraw() before resolvePendingSeekerDetonationDamage.
4. FBattleBoard::draw: drawSeekerPaths now called for PH_MOVE || PH_SEEKER_ACTIVATION.

## Expected validation failures carried forward
None
