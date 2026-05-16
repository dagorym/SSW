# Implementer Report — SSW-ZSR-001

## Scope and constraints
- Role: implementer
- Subtask: SSW-ZSR-001
- Allowed files: `src/tactical/FTacticalGame.cpp` (header unchanged)
- Artifact directory: `artifacts/stopped-ship-free-rotation-plan/subtask-001`

## Preflight summary
- Goal: allow ships that begin movement at `speed == 0` to choose facing before displacement, while preserving normal routing and `MR == 0` steering restrictions.
- Validation command selected from planner guidance:
  - `cd tests && make tactical-tests && ./tactical/TacticalTests`
- Tester location guidance:
  - `tests/tactical/FTacticalMoveRouteSelectionTest.cpp`
  - `tests/tactical/FTacticalGameMechanicsTest.cpp`
  - `tests/tactical/FTacticalModelSelectionHexClickSurfaceTest.cpp`

## Implementation completed
Modified file:
- `src/tactical/FTacticalGame.cpp`

Key model changes:
1. Added stopped-ship free-rotation eligibility checks (`speed == 0`, `nMoved == 0`, single-point route, `MR > 0`).
2. Added stopped-ship pre-move facing option generation in movement routing (`computeRemainingMoves`) without changing displacement state.
3. Updated move-hex selection handling so adjacent-hex facing choices for eligible stopped ships update `startHeading` / `curHeading` / `finalHeading` and keep displacement at zero.
4. Preserved standard move behavior for forward movement clicks and non-stopped routing behavior.
5. Preserved `MR == 0` behavior by requiring `ship->getMR() > 0` for stopped-ship facing changes.

## Acceptance criteria mapping
- Moving-side ship starting at `speed == 0` with maneuver authority can choose legal facing:
  - Implemented via stopped-ship turn-option generation and adjacent-facing selection handling.
- Facing selection updates pending orientation without consuming displacement:
  - `nMoved` remains `0`; route resets to only the current hex.
- Facing-only turn completes in same hex with speed `0` and selected heading committed:
  - Movement finalize path commits heading from turn data while speed remains derived from `nMoved`.
- Moving after facing choice uses selected facing for first traveled hex:
  - Recomputed forward movement uses updated `curHeading`.
- Existing non-stopped behavior and `MR == 0` restrictions preserved:
  - Normal movement route behavior unchanged for non-stopped ships; stopped facing path gated on `MR > 0`.

## Validation
Baseline before edits:
- `cd tests && make tactical-tests && ./tactical/TacticalTests` → PASS (`OK (129 tests)`)

After implementation:
- `cd tests && make tactical-tests && ./tactical/TacticalTests` → PASS (`OK (129 tests)`)

## Commits
- Implementation/code commit: `20bb6ab5dee8fe7c48fb7c29b7e2397dcd709f19`
