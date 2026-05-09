## Documenter Report

### Files Updated
- `doc/UsersGuide.md` — documented that a pending station placement can be relocated to another adjacent hex before orbital direction finalization, and that only the latest valid hex is kept.

### Summary
Reviewed `plans/station-placement-duplication-plan.md`, the implementer/tester diff, `doc/UsersGuide.md`, and `doc/DesignNotes.md`. Repository evidence showed the user guide already documents tactical station setup, so the minimal accurate update was to extend that existing setup paragraph rather than add release notes or a new document. The update now matches the shipped `FTacticalGame::placeStation(...)` behavior and the added tactical regressions covering relocation, invalid clicks, and orbit initialization from the final selected hex.

### Validation
- `cd tests && make tactical-tests && ./tactical/TacticalTests`
- Result: `OK (129 tests)`

### Assumptions
- Used `station-placement-implementer-20260508` as the immediate comparison base for tester/documenter-stage review, with `master` only as broader repository context.
- No `AGENTS.md` or `.myteam` guidance update was required because the shipped change only affects tactical station setup behavior in the user-facing guide.

### Commit Message
`docs: describe station setup relocation`
