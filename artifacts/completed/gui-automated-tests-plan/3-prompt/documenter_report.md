## Documenter Report

### Files Updated
- **`AGENTS.md`** — Added repository guidance for live GUI test coverage, including the `tests/gui/*` scope and the deterministic `WXGuiTestHarness::showModalWithAction(...)` modal-driving seam.
- **`doc/DesignNotes.md`** — Expanded the strategic GUI regression notes to cover fleet-setup, transfer, combat-selection, and battle-results live dialog assertions, and updated the validated GUI suite result to `OK (10 tests)`.

### Summary
Updated the existing testing guidance and design notes to match the implemented strategic wx live-dialog coverage, the harness seam used to drive modal interactions deterministically, and the tester-validated headless `GuiTests` runs. Wrote the verifier handoff artifacts in `artifacts/gui-automated-tests-plan/3-prompt` after the documentation commit.

### Commit Message
`docs: document strategic GUI live dialog coverage`
