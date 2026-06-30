# Documenter Report — SMF-05

## Status
PASS

## Documentation changes
The Doxygen comment on `FBattleDisplay::drawSeekerActivation` in `include/tactical/FBattleDisplay.h` was updated by the implementer to reflect:
- The "Activated seekers" block is now positioned below `getActionButtonRowBottom()`
- Data source is `getActiveSeekersByMovingPlayerThisPhase()` (not `getActiveSeekersByMovingPlayer()`)

No additional documentation changes required. The in-code comments in `src/tactical/FBattleDisplay.cpp` include clear inline rationale for both changes (SMF-05 references).

## Artifacts committed
Documenter report only (no code changes needed).
