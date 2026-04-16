## Documenter Report

### Files Updated
- **`AGENTS.md`** — Tightened the shared GUI-test guidance so parent-backed tactical damage-summary coverage now documents the `showModalWithAction(...)` path and owning-top-level-parent-only fallback during brief wxGTK geometry settling.
- **`doc/DesignNotes.md`** — Updated the tactical damage-summary live-test notes to describe the final direct-dialog stabilization: the test now uses `showModalWithAction(...)`, keeps the parent-centered placement assertion, pins the parent frame to a deterministic starting position, and gives the close-button action enough time to finish without racing the harness fallback.

### Summary
Updated the repository testing notes to match the shipped subtask-3 live-test remediation: the centering check remains parent-focused, rejects generic display-centered acceptance for parent-backed launches, and now documents the explicit modal-action harness path used to stabilize the direct `TacticalDamageSummaryGUI` test. Documenter handoff artifacts were refreshed for verifier review against the passing tester evidence.

### Commit Message
`docs: capture final damage summary test stabilization`
