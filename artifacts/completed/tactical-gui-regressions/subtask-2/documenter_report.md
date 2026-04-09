## Documenter Report

### Files Updated
- **`AGENTS.md`** — Updated repository guidance so tactical GUI coverage now explicitly records empty/no-detail `WXTacticalUI::showDamageSummary(...)` modal-close coverage and the default affirmative close-path contract without a manual `EndModal(...)` callback.
- **`doc/DesignNotes.md`** — Documented the wxGTK-safe tactical damage-summary close-path hardening and the regression coverage added for populated, no-detail, and empty runtime summary flows.

### Summary
Documented the tactical damage-summary modal hardening in the existing repository guidance and design notes, including the removal of the manual close callback, the safe default affirmative close behavior, and the new runtime/source regression coverage for empty and no-detail summaries.

### Commit Message
`Document tactical damage summary close-path hardening`
