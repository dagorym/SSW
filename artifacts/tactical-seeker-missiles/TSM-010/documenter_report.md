# Documenter Report — TSM-010: Active Seeker Path Contact in completeMovePhase

## Task Summary

Update project documentation to reflect the TSM-010 implementation that
integrates active seeker contact into normal ship movement finalization so that
when a ship's path enters a hex containing an active seeker owned by the opposing
player, seeker detonation is resolved before mine damage.

## Scope

- Implementation files inspected:
  - `include/tactical/FTacticalGame.h`
  - `src/tactical/FTacticalGame.cpp`
- Documentation files updated:
  - `doc/DesignNotes.md`
  - `AGENTS.md`
  - `include/tactical/FTacticalGame.h` (Doxygen correction)

## Documentation Changes

### doc/DesignNotes.md

Added a TSM-010 paragraph immediately after the TSM-009 paragraph in the
`Combat Phases` / seeker narrative section. The new paragraph describes:

- `completeMovePhase()` clears `m_pendingSeekerContactOutcomes` at the start of
  finalization.
- The new `checkForActiveSeekersOnPath(FVehicle*)` is called per moving ship
  alongside `checkForMines()`; it appends one `FTacticalSeekerContactOutcome`
  per path hex with a qualifying active opposing seeker, skipping inactive
  seekers via an explicit `active` flag guard.
- `applyMovementSeekerDamage()` resolves collected contacts through the TSM-009
  `resolvePendingSeekerDetonationDamage()` seam and removes detonated seekers
  from `m_seekerMissiles` exactly once.
- Seeker contact damage resolves before `applyMineDamage()` and before the phase
  advances to `PH_DEFENSE_FIRE`.

### AGENTS.md (tactical test section)

Extended the tactical test bullet to note that `FTacticalMineDamageFlowTest`
and `FTacticalSeekerMovementTest` now cover TSM-010 movement-path seeker contact
behavior. Specifically documented:

- Source-inspection checks that `completeMovePhase()` clears pending outcomes at
  the start, calls `checkForActiveSeekersOnPath` per moving ship before mine
  checking, and sequences `applyMovementSeekerDamage()` before `applyMineDamage()`
  and the defensive-fire phase transition.
- Source-inspection check that `checkForActiveSeekersOnPath` guards inactive
  seekers with an explicit `!active` early-continue.
- Source-inspection check that `applyMovementSeekerDamage` removes each detonated
  seeker exactly once.
- Runtime checks in `FTacticalSeekerMovementTest` that inactive seekers generate
  no contact outcomes, active seekers generate one outcome and are removed from
  the model, and bystander seekers not on the path survive.

### include/tactical/FTacticalGame.h (Doxygen correction)

Corrected the `applyMovementSeekerDamage` Doxygen comment. The previous
description claimed the method "Clears any outcomes left over from pre-movement
activation", which was inaccurate — that clearing is done by `completeMovePhase()`
before path scanning begins. The corrected description accurately states that
`applyMovementSeekerDamage()` collects detonating seeker IDs from pending
outcomes, calls `resolvePendingSeekerDetonationDamage()` when a UI is installed
(or clears directly when no UI is present), then removes each detonated seeker
exactly once. The caller's responsibility for the pre-move clear is now explicitly
stated.

The `checkForActiveSeekersOnPath` Doxygen was already accurate and needed no
changes.

## Assumptions

- Artifact directory `artifacts/tactical-seeker-missiles/TSM-010/` reused from
  tester handoff.
- Base branch for diff comparison: master.
- `doc/rules/tactical_operations_manual.md` was not modified per repository
  non-volatile artifact policy.

## Documentation Commit

Documentation commit hash: `5b248a7`

## Test Results

No new tests were added or modified by the Documenter. The full test suite
status from the Tester agent: **OK (187 tests)**, all passing.
