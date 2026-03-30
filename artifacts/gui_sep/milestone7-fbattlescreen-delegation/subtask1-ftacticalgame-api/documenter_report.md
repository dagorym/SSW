## Documenter Report

### Files Updated
- **`AGENTS.md`** — Updated repository guidance to describe `FTacticalGame` as the additive Milestones 5-7 tactical state-owner surface, including the new delegation-facing API categories and the `installUI(ITacticalUI*)` / `getUI()` seam.
- **`doc/DesignNotes.md`** — Added the Subtask 1 architecture summary covering the expanded `FTacticalGame` API categories, the additive wx-free UI seam, and why this unblocks Milestone 7 `FBattleScreen` delegation while preserving current runtime behavior.

### Summary
Updated the internal architecture documentation for Milestone 7 Subtask 1 so it reflects the implemented `FTacticalGame` API expansion from commit `5489c3b`. The docs now summarize the new state/control, setup/scenario, ship/weapon selection, movement-helper, and tactical-report access surfaces; explicitly call out the additive `installUI(ITacticalUI*)` / `getUI()` seam; and keep the wording aligned with the existing additive-boundary framing that the live tactical wx runtime still remains on `FBattleScreen`, `FBattleBoard`, and `FBattleDisplay`.

### Commit Message
`Document FTacticalGame delegation API`
