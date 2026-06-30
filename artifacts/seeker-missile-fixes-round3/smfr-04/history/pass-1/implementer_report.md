# Implementer Report

Status: success

Task: SMFR-04 — Persist Seeker Speed Label From Activation To Impact Or Exhaustion

## Task Summary

Persist the seeker speed (move-count) label from activation until impact or movement exhaustion. The label previously disappeared between movement resolution passes and in non-movement phases because `drawSeekerMoveCountOverlay()` was called only during `PH_MOVE`/`PH_SEEKER_ACTIVATION`, and only for seekers with a recorded `movementPath` (size >= 2). Fixed by: (1) moving the overlay call outside the phase guard so it fires for all `BS_Battle` phases; (2) removing the `movementPath.size() < 2` filter in the overlay function, using `movementPath.size()-1` when a path is recorded or `movementAllowance` as fallback. No change to seeker movement, targeting, or damage behavior.

## Changed Files

- `include/tactical/FBattleBoard.h`
- `src/tactical/FBattleBoard.cpp`

## Validation Commands Run

- `make -C tests tactical-tests`
- `./tests/tactical/TacticalTests`
- `make -C tests gui-tests`

## Validation Outcome

PASS — 209 tactical tests pass, GUI module builds cleanly.

## Implementation/Code Commit Hash

5b48aef

## Artifacts Written

- `artifacts/seeker-missile-fixes-round3/smfr-04/implementer_report.md`
- `artifacts/seeker-missile-fixes-round3/smfr-04/tester_prompt.txt`
- `artifacts/seeker-missile-fixes-round3/smfr-04/implementer_result.json`

## Implementation Context

`drawSeekerMoveCountOverlay()` now iterates all active seekers (not just those with `movementPath.size() >= 2`). Count = `movementPath.size()-1` when a path exists, `movementAllowance` otherwise. The overlay is called unconditionally inside `BS_Battle` (drawSeekerPaths remains gated on `PH_MOVE`/`PH_SEEKER_ACTIVATION`). Doxygen updated in both header and source.

## Edge Cases

- Newly activated seeker (movementTurn=0, movementAllowance=0): label shows 0 until first resolution
- After first resolution: label shows movementAllowance (2 for turn 1)
- Multiple seekers in same hex: counts stacked vertically as before
- PH_ATTACK_FIRE: overlay now fires alongside the active-seeker icon draw in drawSeekerMissiles

## Expected Validation Failures Carried Forward

None.
