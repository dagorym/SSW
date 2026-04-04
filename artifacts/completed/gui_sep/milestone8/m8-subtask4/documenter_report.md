## Documenter Report

### Files Updated
- **`doc/DesignNotes.md`** — Added Milestone 8 Subtask 4 validation notes covering the tactical test-suite shift to FTacticalGame selection/hex-click delegation, removal of transitional `FBattleDisplay::fireAllWeapons()` coupling, and registration of the new tactical model API surface test suite.

### Summary
Recorded that Milestone 8 Subtask 4 is test-validated with tactical coverage now anchored on the FTacticalGame-owned selection and hex-click delegation surface. The design notes now explain that the updated tactical tests verify FBattleScreen forwarding mutators, the new FTacticalModelSelectionHexClickSurfaceTest coverage, and the continued removal of duplicated FBattleBoard mechanics state from the tested architecture.

### Commit Message
`docs: record milestone 8 subtask 4 validation`
