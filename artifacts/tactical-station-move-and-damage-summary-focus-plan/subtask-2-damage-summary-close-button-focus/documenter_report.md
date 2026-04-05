## Documenter Report

### Files Updated
- **`AGENTS.md`** — Clarified that the live `TacticalDamageSummaryGUI` regression uses a simulated Enter keypress to validate the Close button's default-focus affirmative dismissal path.

### Summary
Updated repository guidance so the GUI testing notes match the remediated second-pass coverage: the damage summary dialog still opens with Close focused/default, and the live regression now proves dismissal through an actual simulated Enter keypress rather than synthetic button-click dispatch.

### Commit Message
`Clarify tactical damage summary Enter-key coverage`
