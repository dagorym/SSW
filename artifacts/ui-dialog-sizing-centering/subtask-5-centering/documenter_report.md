## Documenter Report

### Files Updated
- **`AGENTS.md`** — Added repository guidance for deterministic frame/dialog centering and noted the live GUI coverage that locks the placement policy.
- **`doc/DesignNotes.md`** — Documented the strategic, tactical, and BattleSim centering behavior and refreshed the latest GUI validation result for this story.

### Summary
Updated the repository guidance and design notes to match the implemented centering policy: top-level frames center on screen, parent-backed dialogs center on their parent, parentless modal fallbacks use screen centering, and the existing live GUI suites cover the representative BattleSim, strategic, and tactical launch paths tied to this remediation.

### Commit Message
`Document dialog centering policy`
