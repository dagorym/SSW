## Documenter Report

### Files Updated
- **`doc/DesignNotes.md`** — Documented the labeled ship-rollup and conditional hit-detail sections, refreshed the live/tactical regression notes, and updated the GUI validation result count.

### Summary
Updated the tactical design notes to reflect that `TacticalDamageSummaryGUI` now renders a labeled `Ship Damage Summary` section, conditionally appends `Hit Details` only when `showHitDetails` is enabled and populated, preserves the existing empty-state path, and is covered by both tactical regression checks and the parent-backed `WXTacticalUI::showDamageSummary(...)` smoke flow.

### Commit Message
`Document tactical damage summary hit detail dialog`
