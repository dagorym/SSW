## Documenter Report

### Files Updated
- **`doc/DesignNotes.md`** — Refreshed the lower-panel validation totals so the documented split-fit vs stacked fallback contract now matches the currently validated branch results (`OK (153 tests)` for the tactical runner and `OK (36 tests)` for the GUI runner).

### Summary
Refreshed the existing tactical lower-panel documentation so the recorded validation totals stay aligned with the tested branch state. The shipped docs already capture the actual split-fit vs stacked fallback behavior, the button-row-based stacked placement, and the preserved `FBattleScreen::onSize(...) -> reflowLowerPanelLayout() -> applyLayoutPolicy()` seam; this pass synchronizes the Design Notes validation bookkeeping with the expanded tester coverage.

### Assumptions
- Plan context: `plans/tactical-ship-stats-lower-panel-layout-fixes-plan.md` is the governing story plan for `tactical-ship-stats-lower-panel-layout-fixes/subtask-1`; its `Documentation Impact` said no update was expected, but the implemented/tested diff introduced a reusable documented layout policy worth capturing.
- Comparison base: compare the tester/documenter-stage branch against the shared `layout-update` base at commit `59994de` to review the combined implementation, tester coverage, and documentation impact for this subtask.
- Existing `doc/UsersGuide.md` coverage for the lower-panel split/stacked behavior remained accurate on this branch, so only the stale Design Notes validation totals needed a follow-up documentation edit in this pass.

### Commit Message
`Refresh lower-panel layout validation totals`
