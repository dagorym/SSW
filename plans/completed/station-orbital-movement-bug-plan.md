# Station Orbital Movement Bug Fix Plan

## Problem Statement

Space stations are not staying in orbit around planets during tactical combat — they continue in a straight line instead of following the orbital path around the planet.

## Root Cause

**Primary Bug — Wrong heading applied in `finalizeMove()`**

In `FBattleBoard::finalizeMove()` (`src/tactical/FBattleBoard.cpp`, line ~842):

```cpp
(*itr)->setHeading(m_turnInfo[id].curHeading);  // BUG: uses curHeading for ALL ships
```

In `resetMoveData()`, when setting up a station's turn, the code computes a `finalHeading` that applies the orbital (gravity) turn:

```cpp
int turnDir = getPlanetTurnDirection(nextHex, d.curHeading);
d.finalHeading = turnShip(d.curHeading, turnDir);  // Correct orbital heading computed here...
```

But `finalizeMove()` never reads `finalHeading` — it always uses `curHeading`. The station's heading is never updated after each orbital move, so every turn it moves one hex in the same direction: a straight line.

**Secondary Bug — Station speed incorrectly overwritten**

Also in `finalizeMove()` (line ~841):

```cpp
(*itr)->setSpeed(m_turnInfo[id].nMoved);  // BUG: sets station speed from 0 to 1
```

For stations, `nMoved` is pre-set to `1` in `resetMoveData()` (the pre-applied orbital move). This overwrites the station's correct speed of `0`, and also triggers `changedSpeed = true` every turn, which can cause erroneous masking screen decrements.

## Files to Modify

| File | Change Needed |
|------|---------------|
| `src/tactical/FBattleBoard.cpp` | Fix `finalizeMove()`: use `finalHeading` for stations; preserve station speed |
| `tests/tactical/FTacticalStationOrbitalMovementTest.h` | New test file — station heading and position updates |
| `tests/tactical/FTacticalStationOrbitalMovementTest.cpp` | New test file — implementation |
| `tests/tactical/Makefile` | Add new test object to build |
| `tests/SSWTests.cpp` | Register new test suite |

## Subtasks

### Subtask 1 — Fix `finalizeMove()`: apply `finalHeading` to stations

**File:** `src/tactical/FBattleBoard.cpp`

In `finalizeMove()`, after the existing `isStation` detection block (where the station position is updated), use `finalHeading` instead of `curHeading` when calling `setHeading()` for stations.

The current code pattern is:
```cpp
(*itr)->setSpeed(m_turnInfo[id].nMoved);
(*itr)->setHeading(m_turnInfo[id].curHeading);
```

The fix must:
1. Detect when the vehicle is a station (check type == "ArmedStation", "FortifiedStation", or "Fortress")
2. For stations: call `setHeading(m_turnInfo[id].finalHeading)` instead of `curHeading`
3. For non-stations: retain existing `setHeading(m_turnInfo[id].curHeading)` behaviour

The `isStation` bool is already determined later in the method. The detection logic should either be hoisted earlier, or the heading/speed corrections applied in the existing `isStation` block.

**Acceptance Criteria:**
- After `finalizeMove()`, a station's `getHeading()` returns `m_turnInfo[id].finalHeading`
- After `finalizeMove()`, a non-station ship's `getHeading()` returns `m_turnInfo[id].curHeading` (unchanged)
- No regression in ship movement behaviour

---

### Subtask 2 — Fix `finalizeMove()`: preserve station speed

**File:** `src/tactical/FBattleBoard.cpp`

In `finalizeMove()`, the unconditional `setSpeed(nMoved)` must not corrupt the station's speed.

The fix must:
1. For stations: skip `setSpeed()` call entirely (stations always have speed 0), OR restore the station's original speed (0) after the call
2. The `changedSpeed` flag must also be correctly computed — for stations it should always be `false` (speed has not changed)
3. For non-stations: retain existing `setSpeed(nMoved)` behaviour

**Acceptance Criteria:**
- After `finalizeMove()`, a station's `getSpeed()` returns `0`
- After `finalizeMove()`, `changedSpeed` is `false` for stations
- The masking screen decrement is not triggered for stations due to a spurious speed change

---

### Subtask 3 — Write unit tests for station orbital movement

**Files:**
- `tests/tactical/FTacticalStationOrbitalMovementTest.h`
- `tests/tactical/FTacticalStationOrbitalMovementTest.cpp`

Create a `FTacticalStationOrbitalMovementTest` class in `namespace FrontierTests` using CppUnit. The tests must exercise the orbital heading and speed update logic directly, covering:

1. **`testStationHeadingUpdatedAfterOrbit`** — After one simulated orbital move (calling the equivalent of `finalizeMove()` logic), the station's heading equals `finalHeading` (not `curHeading`).
2. **`testStationSpeedPreservedAfterOrbit`** — After `finalizeMove()`, the station's speed is `0`.
3. **`testNonStationHeadingUnchangedByFix`** — A regular ship's heading still comes from `curHeading` after `finalizeMove()`.

Use the same CppUnit patterns as other tests in `tests/tactical/` (e.g., `FTacticalAttackResultTest`).

**Acceptance Criteria:**
- All three tests compile and pass
- Tests live in `namespace FrontierTests`
- Class named `FTacticalStationOrbitalMovementTest`
- Header uses `#ifndef _FTACTICALSTATIONORBITALMOVEMENTTEST_H_` guard

---

### Subtask 4 — Register new tests in build and runner

**Files:**
- `tests/tactical/Makefile`
- `tests/SSWTests.cpp`

1. Add `FTacticalStationOrbitalMovementTest.o` to the objects list in `tests/tactical/Makefile`
2. Add `#include "tactical/FTacticalStationOrbitalMovementTest.h"` in `tests/SSWTests.cpp`
3. Add `runner.addTest(FrontierTests::FTacticalStationOrbitalMovementTest::suite())` in `tests/SSWTests.cpp`

**Acceptance Criteria:**
- `cd tests && make` compiles without errors
- `./SSWTests` runs and includes the new tests in its output
- No existing tests are broken

---

## Dependency Ordering

```
Subtask 1 (fix heading)  ──┐
                            ├──► Subtask 3 (write tests) ──► Subtask 4 (register tests)
Subtask 2 (fix speed)   ──┘
```

- Subtasks 1 and 2 are independent of each other and can be implemented in parallel (both in the same method, same file)
- Subtask 3 depends on Subtasks 1 and 2 being complete so tests can verify the fixes
- Subtask 4 depends on Subtask 3 being complete

---

## Implementer Agent Prompts

---

### Prompt for Subtask 1 — Fix station heading in `finalizeMove()`

You are the implementer agent.

**Files you are allowed to change:**
- `src/tactical/FBattleBoard.cpp`

**Task:**
Fix `FBattleBoard::finalizeMove()` so that when a station finishes its orbital move, its heading is updated to `m_turnInfo[id].finalHeading` rather than `m_turnInfo[id].curHeading`.

The bug is on this line in `finalizeMove()`:
```cpp
(*itr)->setHeading(m_turnInfo[id].curHeading);
```

The `turnData` struct (defined in `include/tactical/FBattleBoard.h`) has both `curHeading` and `finalHeading` fields. For stations, `resetMoveData()` computes the correct orbital heading into `finalHeading` via `turnShip(d.curHeading, turnDir)`. The `finalHeading` is never applied because `finalizeMove()` always uses `curHeading`.

The station type check pattern already exists later in the method:
```cpp
if ((*itr)->getType()=="ArmedStation" || (*itr)->getType()=="FortifiedStation" || (*itr)->getType()=="Fortress"){
```

Use this same condition (or hoist the `isStation` bool earlier) to select `finalHeading` for stations and `curHeading` for all other vehicles.

Do not change any other logic in this method.

**Acceptance Criteria:**
- After `finalizeMove()`, a station's `getHeading()` returns `m_turnInfo[id].finalHeading`
- After `finalizeMove()`, a non-station ship's `getHeading()` returns `m_turnInfo[id].curHeading` (unchanged)
- The existing `isStation` bool (used for position and masking screen logic later in the method) must still be set correctly

---

### Prompt for Subtask 2 — Fix station speed in `finalizeMove()`

You are the implementer agent.

**Files you are allowed to change:**
- `src/tactical/FBattleBoard.cpp`

**Task:**
Fix `FBattleBoard::finalizeMove()` so that a station's speed is not overwritten from `0` to `1` each turn.

The bug is on these two lines in `finalizeMove()`:
```cpp
bool changedSpeed = false;
if (m_turnInfo[id].nMoved != (*itr)->getSpeed()){
    changedSpeed = true;
}
(*itr)->setSpeed(m_turnInfo[id].nMoved);
```

For stations, `resetMoveData()` pre-sets `nMoved = 1` (the orbital pre-move). This causes `setSpeed(1)` to be called on a station that should always have speed `0`. It also causes `changedSpeed = true` every single turn (because `nMoved=1` != station speed `0`), potentially triggering erroneous masking screen decrements.

The fix: when the vehicle is a station, skip the `setSpeed()` call and force `changedSpeed = false`. Use the same type-check pattern already present later in the method:
```cpp
if ((*itr)->getType()=="ArmedStation" || (*itr)->getType()=="FortifiedStation" || (*itr)->getType()=="Fortress")
```

Alternatively, hoist the `isStation` bool before these lines and branch on it.

Do not change any other logic in this method.

**Acceptance Criteria:**
- After `finalizeMove()`, a station's `getSpeed()` returns `0`
- `changedSpeed` is `false` for stations (no masking screen decrement triggered by speed change)
- Non-station ship speed and `changedSpeed` behaviour is unchanged

---

### Prompt for Subtask 3 — Write unit tests for station orbital movement

You are the implementer agent.

**Files you are allowed to change:**
- `tests/tactical/FTacticalStationOrbitalMovementTest.h` (create new)
- `tests/tactical/FTacticalStationOrbitalMovementTest.cpp` (create new)

**Task:**
Create a CppUnit test class `FTacticalStationOrbitalMovementTest` in `namespace FrontierTests` that verifies the orbital movement fixes from Subtasks 1 and 2.

Follow the naming and structural conventions of existing tests in `tests/tactical/` such as `FTacticalAttackResultTest`.

The header file must:
- Use include guard `#ifndef _FTACTICALSTATIONORBITALMOVEMENTTEST_H_`
- Use Doxygen header block with `@file`, `@brief`, `@author`, `@date`
- Declare the class in `namespace FrontierTests`
- Declare these three test methods: `testStationHeadingUpdatedAfterOrbit`, `testStationSpeedPreservedAfterOrbit`, `testNonStationHeadingUnchangedByFix`
- Use `CPPUNIT_TEST_SUITE`, `CPPUNIT_TEST`, `CPPUNIT_TEST_SUITE_END` macros

The test implementation must verify:
1. **`testStationHeadingUpdatedAfterOrbit`**: After an orbital move, station heading equals `finalHeading` (not `curHeading` from before the turn)
2. **`testStationSpeedPreservedAfterOrbit`**: After an orbital move, station speed remains `0`
3. **`testNonStationHeadingUnchangedByFix`**: A regular ship (e.g., `FAssaultScout`) heading is set from `curHeading` after `finalizeMove()`, confirming no regression

Use `CPPUNIT_ASSERT_EQUAL` for all assertions.

**Acceptance Criteria:**
- All three tests compile and pass with the Subtask 1 and 2 fixes applied
- Tests fail (as expected) against the unfixed code (to confirm they are meaningful)
- Class is in `namespace FrontierTests`
- Header uses correct include guard

---

### Prompt for Subtask 4 — Register new tests in build and runner

You are the implementer agent.

**Files you are allowed to change:**
- `tests/tactical/Makefile`
- `tests/SSWTests.cpp`

**Task:**
Register the new `FTacticalStationOrbitalMovementTest` test class so it is compiled and run as part of the test suite.

In `tests/tactical/Makefile`:
- Add `FTacticalStationOrbitalMovementTest.o` to the list of object files (follow the pattern of existing entries)

In `tests/SSWTests.cpp`:
- Add `#include "tactical/FTacticalStationOrbitalMovementTest.h"` alongside the other tactical test includes
- Add `runner.addTest(FrontierTests::FTacticalStationOrbitalMovementTest::suite());` alongside the other `runner.addTest(...)` calls

**Acceptance Criteria:**
- `cd tests && make` compiles without errors or warnings related to the new files
- `./SSWTests` runs and the three new orbital movement tests appear in output
- All previously passing tests still pass
