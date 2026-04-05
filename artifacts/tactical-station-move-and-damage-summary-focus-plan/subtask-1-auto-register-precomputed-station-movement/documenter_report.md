## Documenter Report

### Files Updated
- **`doc/DesignNotes.md`** — Updated the tactical mechanics notes to document that `resetMovementState()` now rechecks move completion after rebuilding turn data, so pre-seeded station orbital movement can auto-complete `PH_MOVE` on entry and the mechanics assertion path stays aligned with `checkMoveStatus()`.

### Summary
Documented the tactical movement-phase bookkeeping change in the existing design notes, including the movement-phase-entry auto-complete behavior for pre-seeded station orbits and the matching mechanics-test assertion path.

### Commit Message
`Document station movement auto-completion`
