## Documenter Report

### Files Updated
- **`doc/DesignNotes.md`** — Updated the tactical damage-summary design notes so the documented ship-summary rollup matches the shipped multiline contract: a `<Ship Name>:` header plus one bullet per grouped summary item, while preserving aggregated hull totals, grouped weapon/defense abbreviations, and grouped non-hull effect counts.

### Summary
Updated the existing tactical damage-summary design notes to reflect the implemented Subtask 1 behavior. The docs now state that `FTacticalCombatReportSummary` ship rollups are model-owned multiline summaries with a ship header and bullet lines, while still documenting the preserved grouping rules for hull totals, weapon/defense hit abbreviation lists, and repeated non-hull effects.

### Commit Message
`docs: document multiline damage summary rollups`
