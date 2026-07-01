# TMF-04 Tester Report

## Task

Validate the TMF-04 fix: the Movement Done gating bug in `FTacticalGame::checkMoveStatus()` that
allowed the defending player to click "Movement Done" at move-phase entry without any ships having
moved, even when ships had `speed > ADF` and were required to move a minimum of `speed - ADF` hexes.

## Acceptance Criteria Status

| # | Criterion | Status |
|---|-----------|--------|
| 1 | At move-phase entry, before any hex-click, a ship with current `speed > ADF` keeps `isMoveComplete() == false` for BOTH players | PASS |
| 2 | A defending ship with speed 10 / ADF 1 cannot complete its move phase until it has moved at least 9 hexes | PASS |
| 3 | "Movement Done" enables once every must-move ship has satisfied its minimum move; ships that may legitimately stop (speed <= ADF) do not block completion | PASS |
| 4 | No regression to the attacker's existing correct gating | PASS — 226 pre-existing tests pass unchanged |

## Test Authored

**Test method:** `FTacticalGameMechanicsTest::testDefenderMovePhaseGatingRejectsCompletionBeforeMinimumMove`

**Files modified:**
- `tests/tactical/FTacticalGameMechanicsTest.h` — added declaration + Doxygen header
- `tests/tactical/FTacticalGameMechanicsTest.cpp` — added implementation (tail of file)

## Test Design and Fail/Pass Proof

The key challenge was setting up a scenario where the pre-fix code (using `m_shipPos` as fallback)
produces the wrong answer while the post-fix code (using `findShipHex()`) produces the correct answer.

**Setup:**
- Minelayer (defender) placed at (10, 10) with heading 3 — `speed=10`, `ADF=1`, `minMove=9`
- AssaultScout (attacker) placed LAST at (54, 10) with heading 3

**Why this triggers the pre-fix bug:**

`placeShip(FPoint(54, 10))` is the last call that sets `m_shipPos = (54, 10)`. When the defender's
move phase starts via `setPhase(PH_MOVE)` → `beginMovePhase()` → `resetMovementState()` →
`checkMoveStatus()`, the Minelayer's path is empty (just initialized by `resetTurnInfoForCurrentMover()`).

Pre-fix code: `pos = m_shipPos = (54, 10)`. The Minelayer's `curHeading = 3`. `findNextHex((54, 10), 3)` returns `(55, 10)`. The bounds check `55 < 55` fails. `finished` stays `true`. `setMoveComplete(true)` — WRONG.

Post-fix code: `findShipHex(minelayer_ID, pos)` returns `(10, 10)` (actual board position). `findNextHex((10, 10), 3)` returns `(11, 10)`. Bounds check `11 < 55` passes. `finished = false`. `setMoveComplete(false)` — CORRECT.

**Heading geometry verification:**

`computeHexAngle((54, 10), (55, 10))`:
- sx = 1 + 2×54 + (10%2) = 109 (y=10 is even)
- dx = 1 + 2×55 + 0 = 111
- angle = atan2(0, 109-111) = atan2(0, -2) = 180°
- ang = floor(180 + 30) = 210; heading = 210/60 = **3** ✓

Same formula confirms `computeHeading((10,10), (11,10)) = 3` — a valid in-bounds hex.

## Behavioral Assertions

1. `CPPUNIT_ASSERT_MESSAGE("... isMoveComplete() must be false ...", !game.isMoveComplete())`
   — Fails on pre-fix code (returns `true`), passes on post-fix code (returns `false`).

2. After clicking the 9th hex in `getMovementHexes()` (index 8), `handleHexClick` sets `nMoved=9`.
   `checkMoveStatus()`: `9 < 9` is false → `finished=true` → `setMoveComplete(true)`.
   `CPPUNIT_ASSERT_MESSAGE("... isMoveComplete() must be true ...", game.isMoveComplete())`
   — Passes on both pre-fix and post-fix code (tests correct positive case).

## Test Execution Results

Build: clean (no warnings or errors)
Test run: `tests/tactical/TacticalTests` — **227/227 PASS** (226 pre-existing + 1 new)

Output:
```
OK (227 tests)
```

## Validation Commands

```bash
make -C tests tactical-tests
tests/tactical/TacticalTests
```
