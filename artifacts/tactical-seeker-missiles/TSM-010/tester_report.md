# Tester Report — TSM-010: Active Seeker Path Contact in completeMovePhase

## Task Summary

Validate the TSM-010 implementation that integrates active seeker contact into
normal ship movement finalization so that when a ship's path enters a hex
containing an active seeker owned by the opposing player, seeker detonation is
resolved before mine damage.

## Scope

- Modified implementation files:
  - `include/tactical/FTacticalGame.h`
  - `src/tactical/FTacticalGame.cpp`

- Test files added/modified:
  - `tests/tactical/FTacticalMineDamageFlowTest.h` — 4 new tests added
  - `tests/tactical/FTacticalMineDamageFlowTest.cpp` — 4 new test implementations added
  - `tests/tactical/FTacticalSeekerMovementTest.h` — 2 new tests added
  - `tests/tactical/FTacticalSeekerMovementTest.cpp` — 2 new test implementations added, `TestableTacticalGame` extended

## Assumptions

- Artifact directory `artifacts/tactical-seeker-missiles/TSM-010/` reused from implementer handoff (provided explicitly).
- Smallest relevant test command: `cd tests && make tactical-tests && ./tactical/TacticalTests` (from AGENTS.md).
- `TestableTacticalGame` helper class may be extended with `using` declarations for protected methods per the existing pattern in the test file.

## Acceptance Criteria Validation

| # | Criterion | Status | Test(s) |
|---|-----------|--------|---------|
| 1 | Ship movement paths checked for active seeker hex contact during movement finalization | PASS | `testShipPathSeekerContactCheckedInCompleteMovePhase` |
| 2 | Inactive seeker hexes do not trigger contact or damage | PASS | `testInactiveSeekerNotTriggeredByPathContact` (source-inspection), `testInactiveSeekerIgnoredByPathContactCheck` (runtime) |
| 3 | Ship-triggered seeker damage resolves before mine damage in completeMovePhase() | PASS | `testSeekerDamageAppliedBeforeMineDamageInCompleteMovePhase` |
| 4 | Detonated seekers are removed exactly once | PASS | `testApplyMovementSeekerDamageDetonatesSeekersExactlyOnce` (source-inspection), `testSeekerRemovedFromModelAfterMovementContact` (runtime) |
| 5 | Destroyed ships do not leave stale occupancy, turn-info, or current selection state | PASS | Pre-existing: `testRuntimeMoveDonePathUsesCanonicalCompletionAndResolvesOffBoardOutcome` |
| 6 | Existing mine damage still applies once per movement completion after seeker-contact resolution | PASS | Pre-existing: `testFinalizeMoveAppliesMineDamageExactlyOncePerMovementResolution` + new: `testSeekerDamageAppliedBeforeMineDamageInCompleteMovePhase` |
| 7 | Phase progression to defensive fire remains correct after seeker contact and mine damage | PASS | Pre-existing: `testRuntimeMoveDonePathUsesCanonicalCompletionAndResolvesOffBoardOutcome` + new: `testSeekerDamageAppliedBeforeMineDamageInCompleteMovePhase` |

## Test Results

**Run command:** `cd tests && make tactical-tests && ./tactical/TacticalTests`

**Result:** OK (187 tests)

- Previous total: 181 tests (all passing)
- New tests added: 6
- New total: 187 tests, all passing
- No regressions

## New Tests Added

### FTacticalMineDamageFlowTest (4 new)

1. **testShipPathSeekerContactCheckedInCompleteMovePhase** — Source-inspects `completeMovePhase` to confirm `clearPendingSeekerContactOutcomes()` is called at the start and `checkForActiveSeekersOnPath(*itr)` is called per moving ship before mine checking. Verifies the clear happens before path checking so activation-phase leftovers do not double-count.

2. **testInactiveSeekerNotTriggeredByPathContact** — Source-inspects `checkForActiveSeekersOnPath` to confirm the `!seekerItr->active` early-continue guard, the same-owner skip, and the hex-coordinate match check are all present.

3. **testApplyMovementSeekerDamageDetonatesSeekersExactlyOnce** — Source-inspects `applyMovementSeekerDamage` to confirm: early-exit when no outcomes pending; seeker IDs collected before resolution; `resolvePendingSeekerDetonationDamage()` called when UI is installed; `clearPendingSeekerContactOutcomes()` called when no UI; `m_seekerMissiles.erase()` called exactly once per detonated seeker with a break to prevent double removal.

4. **testSeekerDamageAppliedBeforeMineDamageInCompleteMovePhase** — Source-inspects `completeMovePhase` ordering to confirm `applyMovementSeekerDamage()` appears exactly once, `applyMineDamage()` appears exactly once, seeker damage precedes mine damage, and mine damage precedes `setPhase(PH_DEFENSE_FIRE)`.

### FTacticalSeekerMovementTest (2 new)

5. **testInactiveSeekerIgnoredByPathContactCheck** — Runtime test: places an inactive opposing seeker on a ship's movement path, calls `checkForActiveSeekersOnPath`, verifies zero contact outcomes are generated, and verifies the seeker remains in the model.

6. **testSeekerRemovedFromModelAfterMovementContact** — Runtime test: places an active opposing seeker on a ship's movement path plus a bystander seeker off the path, calls `checkForActiveSeekersOnPath` to verify one contact outcome, then calls `applyMovementSeekerDamage` (no UI installed) to confirm the contacting seeker is removed and the bystander seeker survives.

## Commit Information

Test commit hash: `7d3fd26`

## Temporary Byproducts

None. No temporary files created outside the test and artifact directories.
