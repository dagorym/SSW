## Documenter Report

### Files Updated
- **`AGENTS.md`** — Updated tactical test inventory and contributor guidance to describe `FTacticalGameMechanicsTest` coverage plus the additive mechanics responsibilities now owned by `FTacticalGame.cpp`.
- **`.github/copilot-instructions.md`** — Kept repository-wide guidance in sync with `AGENTS.md` for the Milestone 5 additive `FTacticalGame` scope and new tactical mechanics test coverage.
- **`doc/DesignNotes.md`** — Recorded that Milestone 5 adds `FTacticalGame` as an additive non-wx tactical mechanics container while the wx battle flow remains the active runtime path.

### Summary
Documented Milestone 5 Subtask 2 as an additive tactical-model step: `FTacticalGame` now owns mechanics-side setup/reset, movement bookkeeping, tactical report lifecycle, `fireAllWeapons()` aggregation, and winner/combat-end helpers without rewiring `FBattleScreen`, `FBattleBoard`, or `FBattleDisplay`. Repository guidance and internal design notes now reflect the validated implementation and the new tactical mechanics test coverage.

### Commit Message
`docs: document FTacticalGame additive mechanics scope`
