## Documenter Report

### Files Updated
- **`doc/DesignNotes.md`** — Documented that raw `FTacticalReportEvent` data now preserves structured weapon-hit identity fields across attack-generated and immediate damage-resolution paths.

### Summary
Updated the tactical design notes so the documented `FTacticalCombatReport` lifecycle matches the shipped structured event payload: weapon-hit metadata now survives both mapping paths, repeated weapon-hit events remain separate raw entries, and non-weapon effects still share the same report flow.

### Commit Message
`docs: describe structured tactical weapon-hit report fields`
