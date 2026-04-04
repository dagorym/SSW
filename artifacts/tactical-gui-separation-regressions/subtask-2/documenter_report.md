# Documenter Report — Subtask 2 Move-Route Selection Regression

## Files Updated
- `doc/DesignNotes.md`
- `artifacts/tactical-gui-separation-regressions/subtask-2/documenter_report.md`
- `artifacts/tactical-gui-separation-regressions/subtask-2/documenter_result.json`
- `artifacts/tactical-gui-separation-regressions/subtask-2/verifier_prompt.txt`

## Summary
- Activated the shared `documenter` agent definition from `/home/tstephen/repos/agents/agents/documenter.yaml` after confirming no repository-local override existed and re-read `/home/tstephen/repos/SSW/AGENTS.md` before substantive work.
- Reviewed `plans/tactical-gui-separation-regressions-plan.md` as a documentation-impact hint, then verified the actual shipped scope from the `gui_sep...gui_sep-tester-20260404` diff and the tester artifacts.
- Updated `doc/DesignNotes.md` to record the restored exact-click move-route selection contract, the runtime `FTacticalMoveRouteSelectionTest` coverage, heading/move-counter invariants, and the fact that no `FBattleBoard` renderer adjustment was required.
- Reused tester validation evidence for the documented behavior: `cd tests/tactical && make && ./TacticalTests` → `OK (75 tests)` and the standalone `FTacticalMoveRouteSelectionTest` fixture run → `OK (3 tests)`.
- Documentation commit hash: `97a23491987a5ab4963e037218332d6cb3904b8a`.

## Commit Message
- Documentation commit: `docs: record move-route regression coverage`
- Artifact commit: `docs: add subtask 2 documenter artifacts`
