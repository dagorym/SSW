## Documenter Report

### Files Updated
- **`AGENTS.md`** — Updated the GUI test summary so it explicitly records that rebuilt `ICMSelectionGUI` spinners now keep `wxSP_ARROW_KEYS | wxSP_VERTICAL` under both runtime style-bit checks and a source-token regression lock.

### Summary
Refined repository guidance to match the implemented and tested ICM spinner change. `AGENTS.md` already covered the general vertical-spinner convention, so this pass limited the documentation update to the GUI test summary that now enforces the rebuilt `ICMSelectionGUI` style contract in addition to the existing in-client geometry checks.

### Commit Message
`Document ICM spinner regression coverage`
