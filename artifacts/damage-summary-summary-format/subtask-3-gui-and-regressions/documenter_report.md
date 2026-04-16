## Documenter Report

### Files Updated
- **`AGENTS.md`** — Clarified that GUI and tactical regressions now lock the multi-line tactical damage-summary contract, including explicit `ADF (-N)` / `MR (-N)` bullets and removal of the legacy `effects:` wording.
- **`doc/DesignNotes.md`** — Recorded the shipped `<Ship Name>:` plus ` - ...` summary contract and the regression coverage that preserves grouped weapon/defense bullets, hit details, and empty-state behavior.

### Summary
Updated existing contributor and design documentation to match the implemented Subtask 3 tactical damage-summary formatting and regression coverage. The docs now explicitly describe the multi-line per-ship summary contract, exact ADF/MR point-loss bullets, grouped weapon/defense-hit abbreviations, and the GUI/tactical regression surface that keeps the dialog's hit-detail and empty-state behavior intact.

### Commit Message
`docs: capture damage summary regression contract`
