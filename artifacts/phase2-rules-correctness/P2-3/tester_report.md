# Tester Report — P2-3 (Strategic loss-counting S2/S3 in FGame::cleanUpShips)

## Status: PASS

## Scope

Author behavioral tests locking in the P2-3 strategic loss-counting fixes in
`FGame::cleanUpShips()`:

- **S2** (retreat condition 3, "40 ships, including fighters"):
  `m_lostSatharShips` counts EVERY destroyed Sathar ship, including fighters
  (the old `getType() != "Fighter"` exclusion guarding this counter was
  removed).
- **S3** (retreat condition 5, "Fighters and Militia ships are not counted"):
  `m_lostTendaySathar` / `m_lostTendayUPF` increment ONLY when the destroyed
  ship is neither a fighter nor a member of a militia fleet
  (`fleet->isMilitia()`). Exclusion applies to BOTH sides.

Conditions 1/2 (`m_lostAC`/`m_lostHC`), removal/ownership logic, and
`save()`/`load()` were intentionally not changed and are not asserted anew.

## Implementation under test

- Predicate added by implementer (commit `d6a6702d`):
  `bool countsTowardTenday = (ship->getType() != "Fighter") && !fleet->isMilitia();`
  gating both tenday counters on both sides; `m_lostSatharShips++` now
  unconditional for destroyed Sathar ships.
- Files: `src/strategic/FGame.cpp`, `include/strategic/FGame.h`.

## Test approach (behavioral)

All new tests construct real game state (real `FGame`, real UPF/Sathar
`FPlayer`, real `FSystem`, real `FFleet`, real ships from `createShip(...)`),
set ships to HP 0, and drive the **private** `cleanUpShips()` /
`checkForVictory()` through the **public** turn path
`endUPFTurn() -> checkForCombat() -> cleanUpShips()` (and `checkForVictory()`).

Private counters are read behaviorally, not by source inspection:

- `readLossCounters(...)` (existing helper) parses the real bytes emitted by
  `FGame::save()` — a behavioral read of production-emitted state.
- `RetreatConditionMockUI` (existing mock, installed via the `FGame::create(ui)`
  seam) supplies a fixed retreat condition and records the production
  `notifyVictory(int)` callback that `checkForVictory()` itself makes when it
  decides the UPF have won.

New helper `addShipLossFleet(owner, sys, name, shipType, doomed, survivor,
militia)` builds a real per-side fleet (optionally flagged militia via
`FFleet::setMilitia(true, ...)`), of a given ship type, with `doomed` ships at
HP 0 and `survivor` ships at full health, registered with both the `FPlayer`
and the `FSystem`.

## New tests (9) added to `tests/strategic/FGameTest`

| Test | Criterion | Assertion |
|------|-----------|-----------|
| `testCleanUpShipsCountsSatharFightersTowardLostSatharShips` | S2 | 3 destroyed Sathar Fighters (non-militia) -> `m_lostSatharShips` +3 |
| `testCleanUpShipsExcludesSatharFightersFromTendaySatharCounter` | S3 fighters, Sathar | 2 destroyed Sathar Fighters -> `m_lostTendaySathar` unchanged (cross-check: `m_lostSatharShips` +2) |
| `testCleanUpShipsExcludesUPFFightersFromTendayUPFCounter` | S3 fighters, UPF | 2 destroyed UPF Fighters -> `m_lostTendayUPF` unchanged |
| `testCleanUpShipsExcludesSatharMilitiaShipsFromTendaySatharCounter` | S3 militia, Sathar | 2 destroyed non-fighter militia ships -> `m_lostTendaySathar` unchanged (cross-check: `m_lostSatharShips` +2) |
| `testCleanUpShipsExcludesUPFMilitiaShipsFromTendayUPFCounter` | S3 militia, UPF | 2 destroyed non-fighter militia ships -> `m_lostTendayUPF` unchanged |
| `testCleanUpShipsCountsNonFighterNonMilitiaSatharShipsTowardTendaySatharCounter` | S3 positive, Sathar | 3 destroyed non-fighter/non-militia Destroyers -> `m_lostTendaySathar` +3 |
| `testCleanUpShipsCountsNonFighterNonMilitiaUPFShipsTowardTendayUPFCounter` | S3 positive, UPF | 3 destroyed non-fighter/non-militia Destroyers -> `m_lostTendayUPF` +3 |
| `testCheckForVictoryCondition5FilteredSatharLossesExceedUPFDespiteFighterAndMilitiaNoise` | condition-5 boundary (positive) | filtered Sathar 2 > UPF 1 (plus fighter/militia noise) at tenday boundary -> `notifyVictory(1)` |
| `testCheckForVictoryCondition5NoVictoryWhenFilteredSatharLossesDoNotExceedUPFDespiteFighterAndMilitiaNoise` | condition-5 boundary (negative/discriminating) | filtered Sathar 3 == UPF 3 while raw Sathar 12 > UPF 3 -> NO victory (proves filtering, not raw counts, feeds the check) |

All 9 registered in the `FGameTest` `CPPUNIT_TEST_SUITE` in `FGameTest.h`.

## Commands executed

- `cd tests && make` — clean build (exit 0).
- `cd tests && ./SSWTests` (cwd = `tests/`) — **OK (226 tests)** with the fix in
  place (was 217 before; +9 new).

## Anti-regression (red/green) verification

Temporarily reverted the `src/strategic/FGame.cpp` counting block to the
pre-fix logic (fighters excluded from `m_lostSatharShips`; both tenday counters
incremented unconditionally), rebuilt, and ran the suite. Result: **6 of the 9
new tests failed red**, exactly the discriminating assertions:

1. `testCleanUpShipsCountsSatharFightersTowardLostSatharShips` — expected 3, actual 0.
2. `testCleanUpShipsExcludesSatharFightersFromTendaySatharCounter` — expected 0, actual 2.
3. `testCleanUpShipsExcludesUPFFightersFromTendayUPFCounter` — expected 0, actual 2.
4. `testCleanUpShipsExcludesSatharMilitiaShipsFromTendaySatharCounter` — expected 0, actual 2.
5. `testCleanUpShipsExcludesUPFMilitiaShipsFromTendayUPFCounter` — expected 0, actual 2.
6. `testCheckForVictoryCondition5NoVictoryWhenFilteredSatharLossesDoNotExceedUPFDespiteFighterAndMilitiaNoise` — expected 0 victory calls, actual 1.

The 3 non-failing new tests are intentionally non-discriminating guards: the
two "non-fighter/non-militia ships ARE counted" tests assert behavior that is
unchanged by the fix (they pass both pre- and post-fix, guarding against an
over-broad fix), and the positive condition-5 win case passes both because its
raw-count comparison also yields a win; the discriminating condition-5 proof is
the negative case (#6 above), which correctly goes red.

The source revert was used only to confirm red and was fully restored
(`git checkout src/strategic/FGame.cpp`); **no source revert was committed**.
Rebuilt and reran with the fix: **OK (226 tests)**.

## Files changed by Tester

- `tests/strategic/FGameTest.h`
- `tests/strategic/FGameTest.cpp`

## Files changed by Implementer (for documentation review)

- `src/strategic/FGame.cpp`
- `include/strategic/FGame.h`

## Test commit

- `0523299854a205135c4ea59e444d610a399568d0`

## Cleanup

No temporary non-handoff byproducts remain in the worktree. The
`readLossCounters()` helper removes its own `.tmp` scratch file. The scratch
backup of `FGame.cpp` used for the red check lives outside the repo (session
scratchpad) and is not part of the worktree.
