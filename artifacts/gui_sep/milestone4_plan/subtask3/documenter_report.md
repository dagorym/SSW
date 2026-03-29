## Documenter Report

### Files Updated
- **`artifacts/gui_sep/milestone4_plan/subtask3/documenter_report.md`** — Records the Subtask 3 documentation handoff, including the GUI-owned coordinate conversion and end-turn hit-testing boundary.
- **`artifacts/gui_sep/milestone4_plan/subtask3/documenter_result.json`** — Stores the machine-readable documentation result for Subtask 3, including the known out-of-scope Subtask 4 test failures.
- **`artifacts/gui_sep/milestone4_plan/subtask3/verifier_prompt.txt`** — Hands off the implemented GUI event-routing changes, tester evidence, and documentation scope to the Verifier.

### Summary
Reviewed Milestone 4 Subtask 3 against `plans/milestone4-fgame-rendering-extraction-plan.md`, the tester handoff artifacts, and the shipped diff from `gui_sep` to this worktree. The implementation now keeps pixel-space math and end-turn hit testing in `FMainFrame`: `onLeftDClick(wxMouseEvent&)` creates a `wxClientDC` for `m_drawingPanel`, derives scale through `WXMapDisplay::getScale(dc)`, converts event pixels to logical map coordinates, and then calls `FGame::handleMapClick(double, double)`, while `onLeftUp(wxMouseEvent&)` owns the end-turn hit test and calls `FGame::processEndTurn()` to preserve the existing menu-toggle return-code contract. `onNew`, `onOpen`, and `onPlaceNova` now call `Refresh()` where the removed `m_game->draw()` path would otherwise have left stale UI state. No repository documentation file outside the shared artifact directory was updated because this subtask changes an internal GUI/model boundary already captured in the milestone design materials, with no user-facing workflow change. Root `make -j2` passes, and the only remaining `SSWTests` failures are the two known `FGameHeaderDependencyTest` assertions reserved for Subtask 4 rather than regressions in Subtask 3.

### Commit Message
`M4S3: add documenter handoff artifacts`
