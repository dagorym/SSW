## Documenter Report

### Files Updated
- **`doc/DesignNotes.md`** — Clarified that the tactical damage-summary live regression still enforces parent-relative centering, now waits for the modal to exist before checking placement, and only tolerates owning-top-level-parent fallback during the brief wxGTK geometry-settle window.

### Summary
Updated the design notes to match the shipped subtask-3 live-test remediation: the centering check remains parent-focused, rejects generic display-centered acceptance for parent-backed launches, and documents the modal-settle timing guard added to deflake wxGTK runs. Documenter handoff artifacts were refreshed for verifier review against the passing tester evidence.

### Commit Message
`docs: capture damage summary centering validation`
