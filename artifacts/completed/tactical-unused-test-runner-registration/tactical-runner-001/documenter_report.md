## Documenter Report

### Files Updated
- **`AGENTS.md`** — Documented that `cd tests && make tactical-tests && ./tactical/TacticalTests` now registers and executes the previously compiled-but-unregistered tactical suites, and that enabling them can expose downstream fixture/source expectation failures.

### Summary
Updated the repository testing guidance so the tactical standalone runner documentation now distinguishes the pre-fix state (the three suites were compiled into the binary but skipped because they were not registered) from the post-fix state (the runner now registers and executes `FTacticalCombatReportTest`, `FTacticalDamageSummaryGUITest`, and `WXTacticalUIAdapterTest`).

### Commit Message
`Document tactical runner registration coverage`
