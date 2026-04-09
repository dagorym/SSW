## Documenter Report

### Files Updated
- **`doc/DesignNotes.md`** — Documented the new tactical combat report hit-detail summary contract, default toggle behavior, hit-only detail emission, and preserved ship-rollup semantics.

### Summary
Updated the tactical design notes to capture the new `FTacticalCombatReportSummary` hit-detail section (`showHitDetails`, `hitDetails`, and player-readable outcome/display text), while noting that no-hit and immediate-only reports remain valid and existing per-ship rollups stay canonical. Validation remained `cd tests/tactical && make && ./TacticalTests` with `OK (88 tests)`.

### Commit Message
`Document tactical hit detail summaries`
