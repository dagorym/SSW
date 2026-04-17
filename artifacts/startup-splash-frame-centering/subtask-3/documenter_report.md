## Documenter Report

### Files Updated
- **`AGENTS.md`** — Expanded repository guidance so the GUI test inventory now documents direct startup-seam coverage for both SSW and BattleSim, including centered splash/frame assertions and the `wxSTAY_ON_TOP` observable used for splash-above-frame behavior.
- **`doc/DesignNotes.md`** — Updated the startup centering notes to record that both live GUI fixtures now call `createStartupSplashAndFrame(...)` directly and assert simultaneous visibility, display centering, and splash-on-top behavior.

### Summary
Documented the new startup-path GUI regression scope for both applications in contributor guidance and design notes. No user-facing documentation change was needed because the shipped startup behavior was already described; this pass records the new regression coverage that now protects that behavior.

### Commit Message
`docs: note startup seam GUI coverage`
