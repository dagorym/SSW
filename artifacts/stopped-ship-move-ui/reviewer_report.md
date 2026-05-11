Reviewer Report

Feature plan reviewed:
- `plans/stopped-ship-move-ui-plan.md`

Review scope:
- Feature-level read-only review except reviewer artifacts
- Reviewed delivered branch state on `stopped-ship-free-rotation-plan`
- Evaluated the combined implementation, tests, documentation, and verifier outputs for `artifacts/stopped-ship-move-ui/subtask-1`

Inputs reviewed:
- Plan: `plans/stopped-ship-move-ui-plan.md`
- Implementation/tests/docs:
  - `include/tactical/FTacticalGame.h`
  - `src/tactical/FTacticalGame.cpp`
  - `src/tactical/FBattleBoard.cpp`
  - `src/tactical/FBattleDisplay.cpp`
  - `include/tactical/FBattleScreen.h`
  - `src/tactical/FBattleScreen.cpp`
  - `tests/tactical/FTacticalMoveRouteSelectionTest.h`
  - `tests/tactical/FTacticalMoveRouteSelectionTest.cpp`
  - `tests/tactical/FTacticalGameMechanicsTest.cpp`
  - `tests/tactical/FTacticalModelSelectionHexClickSurfaceTest.h`
  - `tests/tactical/FTacticalModelSelectionHexClickSurfaceTest.cpp`
  - `doc/DesignNotes.md`
- Subtask artifacts:
  - `artifacts/stopped-ship-move-ui/subtask-1/implementer_report.md`
  - `artifacts/stopped-ship-move-ui/subtask-1/implementer_result.json`
  - `artifacts/stopped-ship-move-ui/subtask-1/tester_prompt.txt`
  - `artifacts/stopped-ship-move-ui/subtask-1/tester_report.md`
  - `artifacts/stopped-ship-move-ui/subtask-1/tester_result.json`
  - `artifacts/stopped-ship-move-ui/subtask-1/documenter_prompt.txt`
  - `artifacts/stopped-ship-move-ui/subtask-1/documenter_report.md`
  - `artifacts/stopped-ship-move-ui/subtask-1/documenter_result.json`
  - `artifacts/stopped-ship-move-ui/subtask-1/verifier_prompt.txt`
  - `artifacts/stopped-ship-move-ui/subtask-1/verifier_report.md`
  - `artifacts/stopped-ship-move-ui/subtask-1/verifier_result.json`
- Commits reviewed:
  - Implementer: `46210048ff634bcc73406585dfdc9175452bbcbb`
  - Tester: `89cc3b3bcfa6d6bd8b226be8de45edcc17492b6d`
  - Documenter: `d67040567f8413cac2166b22464e69aac7d35f26`
  - Verifier: `6f70546db67ce4e7b9c599606196ac8c12684d27`

Validation performed:
- `cd tests && make tactical-tests && ./tactical/TacticalTests` → `OK (141 tests)`
- Repository diff inspection confirmed the delivered change set since the plan commit `30a5bd4` only touches `FTacticalGame`, tactical tests, `doc/DesignNotes.md`, and artifacts; it does not include the planned `FBattleBoard` / `FBattleDisplay` / `FBattleScreen` UI work.

Overall feature completeness:
- FAIL-level incomplete against `plans/stopped-ship-move-ui-plan.md`.
- The delivered work completes the additive model surface from `SSW-SSI-001`, but the feature-level plan also requires preview-click resolution and tactical UI rendering/prompt updates from `SSW-SSI-002` and `SSW-SSI-003`.
- Ordinary non-stopped movement semantics remain intact, and stopped ships with `MR == 0` still do not gain new facing options.

Findings

BLOCKING
- The planned stopped-ship preview click behavior is not implemented. `FTacticalGame` exposes preview metadata, but runtime move selection still only resolves stopped-ship facing through the old adjacent-hex path in `src/tactical/FTacticalGame.cpp:1109-1121`; the new reverse-lookup accessor is not used by runtime code, so clicking a non-adjacent preview hex cannot choose an implied facing and continue normal movement selection as required by `plans/stopped-ship-move-ui-plan.md:65-76`.
- The planned UI rendering/forwarding work is not implemented. `FBattleBoard` still renders only `getMovementHexes()`, `getLeftTurnHexes()`, and `getRightTurnHexes()` in `src/tactical/FBattleBoard.cpp:288-295`, and `FBattleScreen` still forwards only those legacy route buckets in `include/tactical/FBattleScreen.h:205-218` / `src/tactical/FBattleScreen.cpp:509-518`. As delivered, selecting a stopped mover does not immediately show the forward route preview from every legal starting facing on the board.
- The delivered prompt text still documents the pre-feature interaction. `src/tactical/FBattleDisplay.cpp:448-450` tells the player to “Select an adjacent hex to choose facing,” which directly conflicts with the approved UI acceptance that preview selection should start from visible route previews rather than a hidden adjacent-hex discovery step.

WARNING
- The verifier PASS is narrower than the governing review scope. The verifier artifacts assess the additive preview-route model/data surface and documentation only, and they cite `plans/stopped-ship-free-rotation-plan.md` rather than the governing `plans/stopped-ship-move-ui-plan.md`, so that PASS does not establish feature-level completion for the UI plan.
- The added regression coverage is similarly scoped to the model surface. It validates preview metadata exposure and the legacy adjacent-hex stopped-ship facing flow, but it does not exercise the feature-plan behaviors that a clicked preview route hex should imply the chosen facing or that the tactical UI should render all legal start-route previews immediately.

NOTE
- `doc/DesignNotes.md` now accurately documents the additive preview-route model surface without overstating rules changes, but the same file also still contains earlier stopped-ship UI notes describing the adjacent-hex-first interaction. That mixed state is consistent with the delivered code, and it further indicates the feature stopped at the model layer instead of shipping the full UI flow in the governing plan.

Missed functionality / edge cases:
- Immediate board-visible preview rendering for every legal stopped-ship starting facing is still missing.
- Preview-route click resolution from any highlighted preview hex is still missing.
- Prompt text and any UI-facing assertions for the new interaction model are still missing.
- No gap was found in the `MR == 0` restriction or in ordinary non-stopped movement semantics.

Follow-up feature requests for planning:
- Implement the remaining `stopped-ship-move-ui` feature work from `plans/stopped-ship-move-ui-plan.md` by wiring `FTacticalGame` preview-route metadata through `FBattleScreen` into `FBattleBoard`, rendering all legal stopped-ship preview routes immediately on selection, and updating move-phase prompt text to match the preview-first interaction.
- Extend stopped-ship move selection so clicking any highlighted preview-route hex resolves the implied starting facing and continues through the normal movement-selection flow, then add regression coverage for preview-click resolution and the updated tactical UI behavior.

Final outcome:
- FAIL
