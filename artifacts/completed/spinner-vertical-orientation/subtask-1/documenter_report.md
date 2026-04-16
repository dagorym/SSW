## Documenter Report

### Files Updated
- **`AGENTS.md`** — Extended the wxGTK `wxSpinCtrl` contributor guidance to require explicit `wxSP_VERTICAL` for value-selection spinners and updated the tactical test summary to reflect the shipped source-level vertical-style regression check.

### Summary
Updated repository guidance to match the implemented tactical spinner change. `AGENTS.md` now documents that wxGTK value-selection `wxSpinCtrl` controls should keep the existing default-size plus best-size minimum-sizing contract and explicitly request vertical arrows, and the tactical test summary now reflects that `FBattleDisplay` source inspection enforces the vertical style token.

### Commit Message
`Document vertical spinner guidance`
