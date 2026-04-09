## Documenter Report

### Files Updated
- **`doc/DesignNotes.md`** — Corrected the tactical damage-summary design notes so they no longer overstate current player-facing defense-effect formatting; the doc now distinguishes preserved structured defense metadata from the still label/detail-driven ship-summary wording.

### Summary
Updated the tactical damage-summary design notes to match the implemented behavior: `FTacticalReportEvent` preserves defense metadata in raw report events, but the current player-facing ship-summary rollup still relies on existing label/detail text for defense-effect wording.

### Commit Message
`Clarify tactical defense summary wording`
