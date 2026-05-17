## Documenter Report

### Files Updated
- **`doc/UsersGuide.md`** — Documented the tactical lower-panel runtime behavior so the selected-ship stats remain split to the right only when the full stats block fits, and otherwise stack below the action-button row without clipping or overlap.
- **`doc/DesignNotes.md`** — Recorded the shipped content-based sizing seam in `FBattleDisplay`, including measured ship-stats fit checks, stacked placement from the button-row bottom edge, preserved `FBattleScreen::onSize(...) -> reflowLowerPanelLayout() -> applyLayoutPolicy()` flow, and the paired tactical/gui regression coverage.

### Summary
Documented the validated tactical lower-panel split/stacked ship-stats layout fix in the existing user and design notes. The updates now describe the actual shipped behavior: measured ship-stats sizing decides whether split mode can stay active, narrow widths stack the stats block below the action controls, and the existing resize/reflow seam remains the path that updates lower-panel height.

### Assumptions
- Plan context: `plans/tactical-ship-stats-lower-panel-layout-fixes-plan.md` is the governing story plan for `tactical-ship-stats-lower-panel-layout-fixes/subtask-1`; its `Documentation Impact` said no update was expected, but the implemented/tested diff introduced a reusable documented layout policy worth capturing.
- Comparison base: compare the tester/documenter-stage branch against the shared `layout-update` base at commit `59994de` to review the combined implementation, tester coverage, and documentation impact for this subtask.

### Commit Message
`Document tactical lower-panel layout fix`
