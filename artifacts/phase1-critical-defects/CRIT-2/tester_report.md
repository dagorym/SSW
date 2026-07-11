# CRIT-2 Tester Report

**Task:** CRIT-2 — Fix swapped Sathar retreat conditions 4/5 in `FGame::checkForVictory()`
**Role:** Tester
**Date:** Jul 11, 2026
**Stage branch:** `coord-phase1-critical-defects-CRIT-2-tester-20260711`
**Code under test commit:** `2828da4394af65e9766330feca9a42b92bd8bcbf` (implementer fix)

## Summary

Added behavioral (runtime) test coverage for the CRIT-2 fix to the existing
`FGameTest` fixture. All acceptance criteria are covered by tests that
construct real `FGame` state, drive the victory evaluation through the public
turn flow, and assert observed runtime outcomes. **No production-code test
seam was required** — the branches were driven entirely through the public
`FGame::endUPFTurn()` path using approaches already established in this
repository.

## Seam question — resolved without a production-code change

The Coordinator flagged that `checkForVictory()` is private and the relevant
counters/selector (`m_satharRetreat`, `m_round`, `m_stationsDestroyed`,
`m_lostTendaySathar`, `m_lostTendayUPF`) have no public setters/getters, and
asked whether an implementer-provided seam would be needed. **It was not.**
The Option-1 public-flow approach works end-to-end:

- **Selector (`m_satharRetreat`)** is set through the real `init(NULL)` path by
  installing a mock `IStrategicUI` whose `selectRetreatCondition()` returns the
  desired condition number (4 or 5). This is the exact production mechanism
  (`FGame::init()` calls `m_ui->selectRetreatCondition()`).
- **`checkForVictory()`** is reached through the public `FGame::endUPFTurn()`,
  which calls it and then increments `m_round`.
- **`m_round` / tenday boundary** is reached by calling `endUPFTurn()` the
  right number of times so the evaluation lands on `m_round % 10 == 0 &&
  m_round > 0`.
- **`m_stationsDestroyed`** is populated organically: real stations are set to
  `HP = 0` (via the public `FVehicle::setHP`), and a co-located Sathar fleet
  triggers `checkForCombat()` → `cleanUpShips()`, which increments the real
  counter exactly as in gameplay.
- **`m_lostTendaySathar` / `m_lostTendayUPF`** are populated the same organic
  way: Sathar / UPF ships set to `HP = 0` are counted by the real
  `cleanUpShips()` loss-counting code.
- **Outcome observation:** the only externally visible signal of a UPF win is
  the production code's own `notifyVictory(1)` call on the installed
  `IStrategicUI`. The mock records those calls — a behavioral read of the real
  evaluated outcome.
- **Counter-reset observation:** read back via the real `FGame::save()` byte
  stream (the `readLossCounters()` helper already used by the existing
  loss-counter test), a behavioral read of real emitted state, not a
  source inspection.

## Tests added (all in `tests/strategic/FGameTest.{h,cpp}`)

1. `testCheckForVictoryCondition4WinsWithStationsBelowThresholdAndResetsCounter`
   — case 4, 1 station destroyed (< 2): asserts `notifyVictory(1)` fired and
   `m_stationsDestroyed` reset to 0 at the boundary.
2. `testCheckForVictoryCondition4NoWinAtThresholdButCounterStillResets`
   — case 4, 2 stations destroyed (>= 2): asserts no `notifyVictory` and the
   station counter still resets to 0.
3. `testCheckForVictoryCondition4RegressionDoesNotAwardVictoryWhenStationsAtFiveDespiteShipLossImbalance`
   — **Planner-mandated regression:** case 4, 5 stations destroyed, 5 Sathar /
   0 UPF ship losses. Fixed code must NOT award UPF victory. **Fails against
   the swapped/pre-fix code** (which would evaluate `5 > 0` and win).
4. `testCheckForVictoryCondition5WinsWhenSatharLossesExceedUPFAndResetsCounters`
   — case 5, 3 Sathar / 1 UPF losses (3 > 1): asserts `notifyVictory(1)` and
   both tenday loss counters reset to 0.
5. `testCheckForVictoryCondition5NoWinWhenSatharLossesDoNotExceedUPFButCountersStillReset`
   — case 5, 1 Sathar / 2 UPF losses (1 !> 2): asserts no `notifyVictory` and
   both tenday counters still reset.
6. `testCheckForVictoryCondition5RegressionAwardsVictoryWhenStationsAtFiveDespiteBeingAboveThreshold`
   — **Planner-mandated symmetric regression:** case 5, 5 stations destroyed,
   5 Sathar / 0 UPF losses. Fixed code correctly awards UPF victory. **Fails
   against the swapped/pre-fix code** (which would evaluate `5 < 2` and miss
   the win).
7. `testShowRetreatConditionsTextMatchesEvaluatedCaseNumber`
   — asserts the displayed condition text for case 4 references the station
   condition and for case 5 references the ship-loss condition, tying the
   displayed text to the counters actually evaluated for each case number
   (acceptance criterion "the evaluated condition matches the condition text
   for the same case number").

## Regression proof (fails against unfixed code)

I temporarily re-swapped the case-4/case-5 bodies in
`src/strategic/FGame.cpp` (reproducing the pre-fix defect), rebuilt, and ran
the suite. Result — **6 of the 6 counter-driven tests failed** exactly as
required:

```
1) testCheckForVictoryCondition4WinsWithStationsBelowThresholdAndResetsCounter  Expected: 1  Actual: 0
2) testCheckForVictoryCondition4NoWinAtThresholdButCounterStillResets           Expected: 0  Actual: 2
3) testCheckForVictoryCondition4RegressionDoesNotAwardVictoryWhenStationsAtFiveDespiteShipLossImbalance  Expected: 0  Actual: 1
4) testCheckForVictoryCondition5WinsWhenSatharLossesExceedUPFAndResetsCounters  Expected: 0  Actual: 3
5) testCheckForVictoryCondition5NoWinWhenSatharLossesDoNotExceedUPFButCountersStillReset  Expected: 0  Actual: 1
6) testCheckForVictoryCondition5RegressionAwardsVictoryWhenStationsAtFiveDespiteBeingAboveThreshold  Expected: 1  Actual: 0
```

(The 7th text-matching test is invariant to the swap by design, since the
displayed text was never wrong — only the evaluation was.) I then restored the
implementer's fix; `git status` confirms `FGame.cpp` is back to the committed
fix state and only the two test files are modified.

## Validation runs (fixed code)

- `make` (repo root): clean build, exit 0.
- `cd tests && make`: clean build/link, exit 0.
- `./SSWTests` (cwd = `tests/`): **OK (217 tests)** — all passing. Confirmed
  green across 4 consecutive runs.

## Note: pre-existing flaky test (not introduced by this work)

During the swapped-code regression run, `FFleetTest::testDecTransitTime` also
failed once. This is a **pre-existing time-seeded flaky test unrelated to
CRIT-2**. Root cause: `FFleet::decTransitTime()` performs a risk-jump roll
`irand(100) > getRJChance()`; the test's fleet (speed 2, no battleship) has
`getRJChance() == 90`, so the jump fails ~9% of the time. `irand` uses the
global RNG seeded by `srand(time(NULL))` in the `FGame` constructor, so the
outcome depends on the wall-clock second the suite runs. The swapped run
happened to land on a failing second; it is not caused by the CRIT-2 fix or by
the added tests. With the fix restored the full suite passed cleanly on 4
consecutive runs. Flagging for coordinator awareness only — no action taken, as
it is out of CRIT-2 scope.

## Files changed

- `tests/strategic/FGameTest.h` — registered 7 tests; added method
  declarations; updated file-header author/date.
- `tests/strategic/FGameTest.cpp` — added `RetreatConditionMockUI` mock,
  `addCoLocatedSatharRaiders`/`destroyStationAt`/`advanceUPFTurns` helpers, and
  the 7 test bodies; updated file-header author/date.

No production code was modified by the Tester.
