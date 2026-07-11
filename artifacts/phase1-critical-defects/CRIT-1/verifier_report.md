# CRIT-1 Verifier Report

## Verdict: PASS

`FGame::cleanUpShips()` was independently reviewed, rebuilt, and re-tested from a
clean state in the verifier worktree
(`/home/tstephen/repos/worktrees/coord-phase1-critical-defects-CRIT-1-verifier-20260710`,
branch `coord-phase1-critical-defects-CRIT-1-verifier-20260710`). All acceptance
criteria for CRIT-1 are met.

## Scope reviewed

Full diff `coord-phase1-critical-defects...HEAD`:
- `src/strategic/FGame.cpp` -- `cleanUpShips()` rewrite (Implementer)
- `include/strategic/FGame.h` -- Doxygen block update on `cleanUpShips()` (Implementer)
- `tests/strategic/FGameTest.cpp`, `tests/strategic/FGameTest.h` -- 3 new behavioral
  tests + `readLossCounters()` helper (Tester)
- `doc/synthesized-roadmap.md` -- traceability-only annotation marking defect C1
  resolved (Documenter)

## Code review findings

`cleanUpShips()` (`src/strategic/FGame.cpp:1171-1246`) now:
- Takes `FleetList &fList = (*pItr)->getFleetList();` -- a live reference
  (`FPlayer::getFleetList()` returns `FleetList &`, confirmed in
  `include/strategic/FPlayer.h:144`), not a by-value copy.
- Takes `const VehicleList &sList = fleet->getShipList();` -- also live
  (`FFleet::getShipList() const` returns `const VehicleList &`, confirmed in
  `include/strategic/FFleet.h:316`).
- Collects destroyed-ship IDs in a first pass (no mutation during iteration,
  avoiding the classic erase-while-iterating skip bug), then removes each via
  `FFleet::removeShip(id)` (confirmed in `src/strategic/FFleet.cpp:71-80`,
  which erases from the real `m_ships` and returns the removed pointer, or
  `NULL` if already gone -- the `if (ship == NULL) continue;` guard prevents
  double-recording/double-counting).
- Records each destroyed ship on the owning player via
  `FPlayer::addDestroyedShip()` and increments loss counters exactly once per
  destroyed ship, matching the original counter logic (UPF/Sathar branch,
  Fighter/HvCruiser/AssaultCarrier type checks) but now against real removed
  pointers rather than re-scanned copy contents.
- Surviving ships (`getHP() > 0`) are `reload()`ed in the same first pass.
- Fleets left with an empty ship list are collected into `emptiedFleets` and,
  after the fleet-scan loop completes (so `fList` is never mutated while being
  iterated), removed via `FPlayer::removeFleet(fleetID)` (confirmed in
  `src/strategic/FPlayer.cpp:162-171`, erases from the real `m_fleets`) and
  `FSystem::removeFleet(fleetID)` (confirmed in `src/strategic/FSystem.cpp:41-50`,
  erases from the real `m_fleets`), then `delete fleet;` -- correct, since
  after both removals no other container holds a reference to the fleet
  object, and ships already moved to `m_destroyed` on the player are unaffected
  (no double-free).

The Doxygen block on `cleanUpShips()` in `include/strategic/FGame.h` accurately
describes the live-container behavior, the destroyed-ID-first-pass rationale,
and fleet removal/deletion ordering; author list correctly appends
`Claude Sonnet 5 (medium)` without removing `Tom Stephens`; Last Modified date
updated.

## Test review findings

`tests/strategic/FGameTest.cpp` adds three behavioral tests plus a
`readLossCounters()` helper that parses the real `FGame::save()` byte stream
(fixed-layout fields in the exact order `save()`/`load()` use) rather than
inspecting source text -- a genuine behavioral read since `FGame` exposes no
public getters for the private loss counters. All three tests drive
`cleanUpShips()` only through the public `FGame::endUPFTurn()` path (never
calling the private method directly), using a real co-located opposing Sathar
fleet against the real "Task Force Prenglar"/"Task Force Cassidine" UPF
fleets, with a ship's HP forced to 0 via `FVehicle::setHP(0)`:
- `testCleanUpShipsRemovesDestroyedShipAndReloadsSurvivor` -- asserts the
  destroyed ship's ID is absent from `raiders->getShipList()`, the survivor's
  ID is present, and the survivor's torpedo ammo (deliberately depleted to 0
  beforehand) is back to its max (2) after cleanup, proving `reload()` ran.
- `testCleanUpShipsRemovesEmptiedFleetFromPlayerAndSystem` -- asserts a
  fleet's ID is present in both `FPlayer::getFleetList()` and
  `FSystem::getFleetList()` before cleanup and absent from both afterward.
- `testCleanUpShipsUpdatesLossCountersWithoutInflation` -- asserts a destroyed
  Sathar Destroyer increments `m_lostSatharShips`/`m_lostTendaySathar` by
  exactly 1 (with `m_lostHC`/`m_lostAC`/`m_lostTendayUPF` unaffected), and
  that a second turn-processing pass with no new destruction does not further
  inflate the counters.

## Independent rebuild/re-test (this verification run)

1. `make all_clean` (repo root) -- exit 0.
2. `make` (repo root) -- full clean build of all module libraries, `SSW`,
   `BattleSim`, and the top-level test suites (`SSWTests`, `GuiTests`,
   `TacticalTests`) -- exit 0.
3. `./SSWTests` run with cwd = `tests/` -- **OK (201 tests)**, 0 failures,
   0 errors.
4. `doc/rules/tactical_operations_manual.md` -- confirmed zero diff against
   `coord-phase1-critical-defects` (non-negotiable constraint respected).
5. `doc/synthesized-roadmap.md` diff -- confirmed the only change is a single
   table-row annotation marking C1 resolved with a traceability pointer to
   this artifact directory; no other content changed; documentation-only,
   zero behavioral effect.

## Independent negative-control (fails-against-unfixed) reconfirmation

Per AGENTS.md's mandatory behavioral-verification requirement, independently
(not relying solely on the Tester's prior report) reverted
`src/strategic/FGame.cpp`'s `cleanUpShips()` fleet-processing loop in place to
the exact pre-fix by-value-copy version (from commit `43fc466f~1`), leaving
the rest of the file, the test files, and the Doxygen header untouched:

1. Rebuilt `src/strategic` and `tests/` (`make -C src/strategic`, then
   `make -C tests`) -- exit 0 (compiles cleanly either way, confirming this is
   a behavioral defect, not a type error).
2. Ran `./SSWTests` (cwd = `tests/`) -- **4 failures** out of 201 run:
   - `FrontierTests::FGameTest::testCleanUpShipsRemovesDestroyedShipAndReloadsSurvivor` -- FAILED (`ships.size() == 1` assertion failed)
   - `FrontierTests::FGameTest::testCleanUpShipsRemovesEmptiedFleetFromPlayerAndSystem` -- FAILED (`!foundInPlayerAfter` assertion failed)
   - `FrontierTests::FGameTest::testCleanUpShipsUpdatesLossCountersWithoutInflation` -- FAILED (`lostSatharAfter2 == lostSatharAfter1` assertion failed, confirming re-scan inflation)
   - `FrontierTests::FFleetTest::testDecTransitTime` -- FAILED (a cascading
     side effect of the unfixed bug's failure to prune the corrupted/leftover
     test fleets from the shared `FGame` singleton state across tests in the
     same binary; see Notes below -- not one of the 3 target cases, and not
     part of the CRIT-1 diff surface)
3. All three target CRIT-1 test cases failed exactly as expected against
   unfixed code, confirming they are genuine behavioral checks that catch the
   regression.
4. Restored the fix via `git checkout -- src/strategic/FGame.cpp` (verified
   `git status` clean / diff-free against the committed HEAD version
   immediately after).
5. Rebuilt `src/strategic` and `tests/` again -- exit 0.
6. Re-ran `./SSWTests` -- **OK (201 tests)**, confirming the fix restores all
   three target cases to passing.

## Notes (non-blocking)

- **WARNING (pre-existing, out of scope):** `FrontierTests::FFleetTest::testDecTransitTime`
  is flaky independent of CRIT-1. It exercises `FFleet::decTransitTime()`,
  which calls `irand(100) > getRJChance()` to decide jump success -- a
  genuinely randomized branch (`FGame`'s constructor seeds the global RNG with
  `srand(time(NULL))`). Across three consecutive full `./SSWTests` runs during
  this verification (fix restored, no further code changes), this test failed
  once and passed twice, with no other test's outcome changing. `FFleetTest.cpp`
  and `src/strategic/FFleet.cpp` are not part of the CRIT-1 diff surface
  (`git diff coord-phase1-critical-defects...HEAD` touches only
  `FGame.cpp`/`FGame.h`/`FGameTest.cpp`/`FGameTest.h`/`doc/synthesized-roadmap.md`).
  This is a pre-existing test-suite flakiness issue unrelated to this
  subtask's scope and does not block CRIT-1 acceptance; it is noted here for
  visibility and potential separate follow-up (e.g., seeding the RNG
  deterministically in tests, or asserting on a range rather than an exact
  jump-success outcome).

## Acceptance criteria checklist

- [x] A ship with HP <= 0 is absent from its owning `FFleet::getShipList()`
      after cleanup and appears exactly once in the owning `FPlayer`'s
      destroyed record.
- [x] Loss counters increase by exactly the count of matching destroyed ships
      per `cleanUpShips()` call (no inflation from re-scanning).
- [x] A fleet emptied by cleanup is absent from both its `FPlayer` fleet list
      and its `FSystem` fleet list afterward.
- [x] Surviving ships remain in their fleet and have been `reload()`ed.
- [x] Repo builds; existing suites still pass (201 tests OK).
- [x] Behavioral coverage independently reconfirmed to fail against unfixed
      code and pass against the fix.
- [x] `doc/rules/tactical_operations_manual.md` untouched.
- [x] `doc/synthesized-roadmap.md` change confirmed documentation-only with
      zero behavioral effect.
- [x] Working tree clean; all changes committed.
