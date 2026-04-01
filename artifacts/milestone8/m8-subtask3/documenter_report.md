## Documenter Report

### Files Updated
- **`AGENTS.md`** — Updated contributor guidance to reflect that `FTacticalGame` now owns canonical tactical mechanics state and that `FBattleBoard` is narrowed to renderer/hit-testing responsibilities in Milestone 8 Subtask 3.
- **`doc/DesignNotes.md`** — Corrected tactical architecture notes so board rendering, click forwarding, model-owned occupancy/path state, and Subtask 3 validation match the implemented diff.

### Summary
Updated repository guidance and design notes to document the Milestone 8 Subtask 3 tactical refactor: `FBattleBoard` now renders from `FTacticalGame` state via `FBattleScreen` accessors, forwards hex clicks through `handleHexClick()`, and no longer owns duplicate mechanics state.

### Commit Message
`Document Milestone 8 board delegation`
