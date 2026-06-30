# Implementer Report

## SMC-06 — Model: capture seeker movement path

Status: success

Task summary:
Added `std::vector<FPoint> movementPath` to `FTacticalSeekerMissileState` and populated it in `resolveActiveSeekersForMovingPlayer()`. Non-moving seekers have path cleared; active moving-player seekers have path cleared and rebuilt each resolution (start hex pushed first, each greedy step appended). Added `getSeekerMissiles()` const delegation through `FBattleScreen`. No wx types; persistence unaffected.

Changed files:
- `include/tactical/FTacticalGame.h` — added `std::vector<FPoint> movementPath` field to `FTacticalSeekerMissileState` struct; updated Doxygen for the struct and `resolveActiveSeekersForMovingPlayer()`
- `src/tactical/FTacticalGame.cpp` — in `resolveActiveSeekersForMovingPlayer()`: clear path for non-moving seekers, clear+initialize path with start hex for active seekers before greedy loop, append each step hex inside the loop
- `include/tactical/FBattleScreen.h` — added `getSeekerMissiles()` const accessor declaration
- `src/tactical/FBattleScreen.cpp` — added `getSeekerMissiles()` implementation delegating to `m_tacticalGame->getSeekerMissiles()`

Implementation/code commit hash:
- 3ef30447605db04fc639fd66c3ad6bcfccd80886

Validation commands run:
- `make -C tests tactical-tests`
- `tests/tactical/TacticalTests`

Validation outcome:
- PASS — 191 tests passed, 0 failures

Implementation context:
`movementPath` is populated inside `resolveActiveSeekersForMovingPlayer()` in `src/tactical/FTacticalGame.cpp`. The field is declared in `FTacticalSeekerMissileState` in `include/tactical/FTacticalGame.h`. `FBattleScreen::getSeekerMissiles()` delegates to `m_tacticalGame->getSeekerMissiles()` so renderers can read `movementPath` without direct `FTacticalGame` access. Path semantics: (1) cleared for all seekers at resolution start, (2) active moving-player seekers get start hex pushed before the movement loop begins, (3) each greedy step hex appended after heading/hex update inside the movement loop, (4) contacted or expired seekers are removed from `m_seekerMissiles` entirely. The existing `testMovementPathPopulatedByResolveActiveSeekers` test in `FTacticalSeekerMovementTest` already exercises the full SMC-06 runtime contract. The `FTacticalBattleBoardRendererDelegationTest::testDrawSeekerPathsIsCalledInMovePhaseWithCyanPen` source-contract test reads seeker records via `m_parent->getSeekerMissiles()` and filters to active seekers with `movementPath.size() >= 2`.

Expected validation failures carried forward:
- None
