## Documenter Report

### Files Updated
- **`doc/DesignNotes.md`** — Documented that tactical report events preserve numeric `amount`/`previousValue`/`newValue` fields and that ship-summary rollups emit exact aggregated `ADF (-N)` and `MR (-N)` bullets instead of label-only echoes.

### Summary
Updated the tactical design notes so the documented combat-report summary contract matches Subtask 2's implemented behavior: structured event data now carries numeric point-loss fields, and the ship-summary rollup uses those values to produce exact ADF/MR loss bullets alongside existing hull, weapon, defense, and other effect bullets.

### Commit Message
`docs: document ADF/MR summary rollups`
