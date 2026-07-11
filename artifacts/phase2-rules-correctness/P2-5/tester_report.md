# Tester Report — P2-5 (Free `FPlayer::m_destroyed` + ownership contract; deferred-tasks doc)

Status: **PASS** (with documented pre-existing failures; 0 new failures)

## Testing scope

Validate reviewer follow-up **F2**: `FPlayer` becomes the sole owner of ships
in `m_destroyed` and frees them in `~FPlayer()`, without double-deleting a ship
still held by a surviving fleet; the ownership contract is documented;
`doc/deferred-tasks.md` is created (H2, F2-serialization); `save()/load()` is
untouched and `m_destroyed` remains unserialized.

Implementation surface under test:
- `src/strategic/FPlayer.cpp` — `~FPlayer()` now deletes every `FVehicle*` in
  `m_destroyed` and clears the vector (third cleanup block mirroring
  `m_fleets`/`m_unattached`).
- `include/strategic/FPlayer.h` — Doxygen ownership contract on `~FPlayer()`,
  `addDestroyedShip()`, and the `m_destroyed` member.
- `doc/deferred-tasks.md` — new deferred-items list (H2, F2-serialization).

## Acceptance criteria → verification

| # | Acceptance criterion | Verified by | Result |
|---|----------------------|-------------|--------|
| 1 | An `FPlayer` owning ships in `m_destroyed` frees them on destruction — each destructed exactly once | `testDestroyedShipFreedExactlyOnceOnPlayerDestruction` (behavioral) | PASS |
| 1b | No double-delete of a ship still held by a surviving fleet | `testSurvivingFleetShipNotDoubleDeletedByDestroyedList` (behavioral) | PASS |
| 2 | Surviving (non-destroyed) ships/fleets still freed via existing paths | `testSurvivingFleetShipNotDoubleDeletedByDestroyedList` asserts the sibling in the surviving fleet is destructed exactly once | PASS |
| 3 | Ownership contract documented on `m_destroyed`/`addDestroyedShip()` | Reviewed `include/strategic/FPlayer.h` (implementer change) | PASS (present) |
| 4 | `~FPlayer()` Doxygen header updated | Reviewed `include/strategic/FPlayer.h` | PASS (present) |
| 5 | `save()/load()` NOT touched; `m_destroyed` NOT serialized | Reviewed `src/strategic/FPlayer.cpp` `save()`/`load()`; unchanged, no `m_destroyed` reference | PASS |
| 6 | `doc/deferred-tasks.md` created with H2 + F2-serialization (description, origin, why deferred, target phase) | Reviewed `doc/deferred-tasks.md` | PASS (both entries present with all four fields) |
| 7 | Repo builds clean; existing suites still pass (no new failures) | `make` (root) exit 0; `cd tests && make && ./SSWTests` | PASS |

Note: criteria 3–6 are documentation/serialization-shape criteria owned by the
Implementer. They are confirmed present by inspection. The two runtime
behaviors (criteria 1, 1b, 2) are covered by mandatory behavioral tests below.

## Behavioral tests added

File: `tests/strategic/FPlayerTest.{h,cpp}` (added to the existing
`CPPUNIT_TEST_SUITE`; registered via the existing
`CPPUNIT_TEST_SUITE_REGISTRATION`). A file-local `CountingVehicle : FVehicle`
records each instance's `this` pointer into a shared static vector on
destruction, letting each test assert exactly which pointers were freed and how
many times — proving both "freed exactly once" and "no double-delete" without
relying on undefined double-free behavior.

1. `testDestroyedShipFreedExactlyOnceOnPlayerDestruction` — a `CountingVehicle`
   handed to `addDestroyedShip()` on a scoped local `FPlayer` is destructed
   exactly once (size == 1, and the recorded pointer matches) when the player
   goes out of scope.
2. `testSurvivingFleetShipNotDoubleDeletedByDestroyedList` — two
   `CountingVehicle`s are added to a fleet; one is removed via
   `FFleet::removeShip()` (which erases without deleting) and handed to
   `addDestroyedShip()`, the other stays in the fleet. Both the fleet and the
   destroyed ship are owned by a scoped local `FPlayer`. On player destruction,
   exactly 2 destructions occur, each of `survivor` and `destroyedShip` exactly
   once — proving no leak, no aliasing/double-delete.

## Red/green verification (mandatory behavioral proof)

- **RED (unfixed destructor):** Temporarily swapped in `master`'s
  `src/strategic/FPlayer.cpp` and `include/strategic/FPlayer.h` (the pre-F2
  destructor with no `m_destroyed` delete block), rebuilt, and ran the suite.
  Both new tests failed:
  - `testDestroyedShipFreedExactlyOnceOnPlayerDestruction`: Expected 1, Actual 0
    (destroyed ship never freed — the leak).
  - `testSurvivingFleetShipNotDoubleDeletedByDestroyedList`: Expected 2, Actual 1
    (only the surviving fleet ship freed; the destroyed ship leaked).
- **GREEN (fixed destructor):** Restored the committed implementer files
  (byte-identical — `git diff` empty), did a clean root build + tests build +
  suite run. Both new tests pass; total suite 219 run, 9 failures, 0 errors.
- No source revert was committed; only the two test files are the tester's
  change.

## Commands executed

- `make` (repo root) — clean build, exit 0.
- `make -C tests` — tests build, exit 0.
- `./SSWTests` (cwd `tests/`) — **Run: 219, Failures: 9, Errors: 0**.

## Result summary

- Total suite: 219 run, 9 failures, 0 errors.
- New tests: 2 added, 2 passing.
- All 9 failures are `FrontierTests::FGameHeaderDependencyTest::*` — pre-existing
  source-inspection tests that cannot resolve readable candidate file paths in
  this worktree layout ("Expected at least one candidate file path to be
  readable"). They fail identically on the unmodified baseline and are unrelated
  to P2-5.
- `FrontierTests::FFleetTest::testDecTransitTime` (documented pre-existing RNG
  flake, targeted by P2-7) did not fail in the recorded runs.
- **0 new failures introduced by P2-5.**

## Files changed by Implementer (for downstream inspection)

- `src/strategic/FPlayer.cpp`
- `include/strategic/FPlayer.h`
- `doc/deferred-tasks.md` (new)
- `include/core/FGameConfig.h` (incidental: `@date Last Modified` line only)

## Files changed by Tester

- `tests/strategic/FPlayerTest.cpp`
- `tests/strategic/FPlayerTest.h`

## Test commit

- `93e01370fb27cd4216e65fe855cdd998a694e47b`

## Cleanup

- Temporary scratch copies of `FPlayer.{cpp,h}` (master/fixed) used for the
  red/green swap live only under the session scratchpad, not in the worktree.
  The worktree contains no leftover byproducts; only the two committed test
  files and the artifact files remain.
