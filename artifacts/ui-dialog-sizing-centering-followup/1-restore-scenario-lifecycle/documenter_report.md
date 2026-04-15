## Documenter Report

### Files Updated
- **`doc/DesignNotes.md`** — Documented the restored `ScenarioDialog` predefined-scenario modal lifecycle, the unchanged stack-owned `FBattleScreen bb;` launch contract, and the aligned tactical/live GUI regression coverage.

### Summary
Updated the BattleSim launch-path notes to reflect the shipped `ScenarioDialog` hide → `bb.ShowModal()` → `Show()` behavior and the tests that now lock that contract in place.

### Commit Message
`Document ScenarioDialog modal lifecycle`
