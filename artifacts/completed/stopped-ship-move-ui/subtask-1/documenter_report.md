# Documenter Report

## Files Updated
- `doc/DesignNotes.md` — documented the additive stopped-ship preview-route metadata on `FTacticalGame`, including per-facing preview routes, reverse hex-to-heading lookup, preserved non-stopped route accessor semantics, and the existing `MR == 0` free-rotation guard.

## Summary
Reviewed `plans/stopped-ship-free-rotation-plan.md`, the implemented `FTacticalGame` surface, tester coverage, and the existing tactical design notes. Using the assumption that `3374ee7` is the parent implementer baseline for this documenter pass, I updated the existing stopped-ship movement section in `doc/DesignNotes.md` instead of creating a new document. The notes now explain that stopped-ship preview metadata is additive, remains gated by the existing zero-speed free-rotation rules, and gives callers enough data to render preview paths and map clicked preview hexes back to legal starting facings.

## Validation
- Documentation accuracy checked against:
  - `include/tactical/FTacticalGame.h`
  - `src/tactical/FTacticalGame.cpp`
  - `tests/tactical/FTacticalMoveRouteSelectionTest.cpp`
  - `tests/tactical/FTacticalGameMechanicsTest.cpp`
  - `tests/tactical/FTacticalModelSelectionHexClickSurfaceTest.cpp`
- Tester-provided command outcomes reviewed:
  - `cd tests && make tactical-tests && ./tactical/TacticalTests` → `OK (141 tests)` post-change
  - `cd tests/tactical && make && ./TacticalTests` → `OK (141 tests)`

## Assumptions
- `artifacts/stopped-ship-move-ui/subtask-1` is the shared repository-root-relative artifact directory for this pass because the tester handoff already populated it.
- `doc/DesignNotes.md` is the correct documentation target because this change extends the tactical model API/change log rather than introducing a new user-facing rules concept.
- No `AGENTS.md` or `.myteam` update was required because the shipped change does not alter bootstrap or repository-wide runtime guidance.

## Commit Message
`docs: note stopped-ship preview route metadata`
