## Documenter Report

### Files Updated
- **`AGENTS.md`** — Added repository guidance that tactical coverage now includes the extracted destroyed-ship cleanup lifecycle seam and documented that the seam preserves the wx/model ownership split.
- **`doc/DesignNotes.md`** — Recorded the final Milestone 8 warning-fix validation for `runDestroyedShipCleanupLifecycle(...)`, including the new regression coverage scope and the tester-validated `OK (72 tests)` result.

### Summary
Updated the repository guidance and design notes so they accurately describe the extracted `FBattleScreen` destroyed-ship cleanup lifecycle seam, the direct tactical regression coverage added for selection clearing/redraw/bookkeeping/winner ordering, and the validated 72-test tactical suite outcome.

### Commit Message
`Document cleanup lifecycle coverage`
