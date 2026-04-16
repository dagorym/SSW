## Documenter Report

### Files Updated
- **`doc/DesignNotes.md`** — Clarified that both live `FTacticalGame.cpp` report-conversion seams now preserve `previousValue`, `newValue`, and `amount`, so runtime ship-summary rollups keep exact aggregated `ADF (-N)` and `MR (-N)` bullets.

### Summary
Updated the tactical design notes to match the shipped Subtask 2 remediation: `FTacticalReportEvent` numeric payload propagation is now documented at the runtime conversion-seam level, and the ship-summary rollup is documented as using those preserved fields for exact ADF/MR loss totals in live tactical damage summaries.

### Commit Message
`docs: note ADF/MR payload propagation seams`
