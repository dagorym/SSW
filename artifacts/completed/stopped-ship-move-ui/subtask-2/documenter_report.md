# Documenter Report

## Files Updated
- `doc/UsersGuide.md` — updated the tactical movement instructions so stopped ships at speed 0 can use highlighted preview routes directly, while preserving adjacent facing-only selection and rotate-in-place completion guidance.
- `doc/DesignNotes.md` — documented that `FTacticalGame::handleMoveHexSelection()` now infers a stopped ship's starting facing from non-adjacent preview-route clicks before continuing through the existing append/trim movement flow.

## Summary
Reviewed `plans/stopped-ship-move-ui-plan.md`, `plans/stopped-ship-free-rotation-plan.md`, the implemented `FTacticalGame` move-click change, the refreshed tactical regression coverage, and the existing tactical docs. Using the explicit tester handoff plus repository history, I treated `1533578fdb3f38ead4a90595e732a143fed8ac66^` as the tester-provided comparison-base assumption for the final handoff and `205c46a..HEAD` as the safer bounded review surface for the full implementer+tester+documenter context because the implementation commit (`3c2c2655a27e6afdbd8376e1f6cb8494d91beccc`) sits earlier in linear history. I updated the existing user-facing and design-note documentation instead of creating a new document so the shipped stopped-ship preview-route click behavior is described where tactical movement behavior and model semantics already live.

## Validation
- Documentation accuracy checked against:
  - `src/tactical/FTacticalGame.cpp`
  - `tests/tactical/FTacticalMoveRouteSelectionTest.h`
  - `tests/tactical/FTacticalMoveRouteSelectionTest.cpp`
  - `tests/tactical/FTacticalGameMechanicsTest.cpp`
  - `tests/tactical/FTacticalModelSelectionHexClickSurfaceTest.cpp`
- Tester-provided command outcomes reviewed:
  - `cd tests && make tactical-tests && ./tactical/TacticalTests` → baseline `OK (141 tests)`
  - `cd tests && make tactical-tests && ./tactical/TacticalTests` → updated `OK (143 tests)`
- Documenter rerun after documentation edits:
  - `cd tests && make tactical-tests && ./tactical/TacticalTests` → `OK (143 tests)`

## Assumptions
- `artifacts/stopped-ship-move-ui/subtask-2` is the shared repository-root-relative artifact directory because the tester handoff already populated it.
- `doc/UsersGuide.md` and `doc/DesignNotes.md` are the correct documentation targets because this change affects both player-visible tactical move selection behavior and the tactical model interaction notes.
- No `AGENTS.md` or `.myteam` update was required because the shipped change does not alter bootstrap or repository-wide runtime guidance.

## Commit Message
`docs: describe stopped-ship preview route clicks`
