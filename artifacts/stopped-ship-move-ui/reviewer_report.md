Reviewer Report

Feature plan reviewed:
- `plans/stopped-ship-move-ui-plan.md`

Review scope:
- Final feature-level review of the merged `stopped-ship-move-ui` delivery on coordination branch `stopped-ship-free-rotation-plan`
- Reviewed implementation, tests, documentation, verifier outputs, and shared handoff artifacts across subtasks 1-3
- Read-only review except for these reviewer artifacts

Inputs reviewed:
- Governing plan:
  - `plans/stopped-ship-move-ui-plan.md`
- Implementation surface:
  - `include/tactical/FTacticalGame.h`
  - `src/tactical/FTacticalGame.cpp`
  - `src/tactical/FBattleBoard.cpp`
  - `src/tactical/FBattleDisplay.cpp`
  - `src/tactical/FBattleScreen.cpp`
- Tactical tests:
  - `tests/tactical/FTacticalMoveRouteSelectionTest.h`
  - `tests/tactical/FTacticalMoveRouteSelectionTest.cpp`
  - `tests/tactical/FTacticalGameMechanicsTest.cpp`
  - `tests/tactical/FTacticalModelSelectionHexClickSurfaceTest.h`
  - `tests/tactical/FTacticalModelSelectionHexClickSurfaceTest.cpp`
  - `tests/tactical/FTacticalBattleBoardRendererDelegationTest.cpp`
  - `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`
  - `tests/tactical/FTacticalBattleScreenDelegationTest.cpp`
- Documentation:
  - `doc/DesignNotes.md`
  - `doc/UsersGuide.md`
- Reviewed subtask artifact sets under:
  - `artifacts/stopped-ship-move-ui/subtask-1`
  - `artifacts/stopped-ship-move-ui/subtask-2`
  - `artifacts/stopped-ship-move-ui/subtask-3`
- Reviewed commit outputs called out for this feature:
  - Implementer: `46210048ff634bcc73406585dfdc9175452bbcbb`
  - Tester: `89cc3b3bcfa6d6bd8b226be8de45edcc17492b6d`, `1533578fdb3f38ead4a90595e732a143fed8ac66`
  - Documenter: `d67040567f8413cac2166b22464e69aac7d35f26`, `9e60ef4b686fa0bb4f7448489eca02cdf349c55f`, `c500fdfd8bf28ecab0b924f19a391b0e781334bc`
  - Verifier: `6f70546db67ce4e7b9c599606196ac8c12684d27`, `dcbe160987eec15f951dbaf392d5ced8d914e8bf`, `dab06cea29183fad750c6a2d7ad6e1f9c685b846`

Validation performed:
- `cd tests && make tactical-tests && ./tactical/TacticalTests` → `OK (143 tests)`

Overall feature completeness:
- The delivered feature is **not complete** against the governing plan.
- The additive model surface, preview-click resolution, MR==0 guard, and most regression coverage are in place.
- However, the shipped UI does **not** show the current-heading forward preview as one of the immediately visible legal starting facings for a stopped ship, which is required by the plan's feature restatement and acceptance criteria.

Findings

BLOCKING
- The stopped-ship preview surface omits the ship's current heading, so the UI does not actually show previews for **every legal starting facing**. The plan requires that selecting a stopped mover "immediately shows the forward route preview from every legal starting facing" (`plans/stopped-ship-move-ui-plan.md:4`, `:55-58`, `:81-84`). But `FTacticalGame::rebuildStoppedShipPreviewRoutes()` explicitly skips `heading == turnData->curHeading` (`src/tactical/FTacticalGame.cpp:1038-1041`), `FTacticalMoveRouteSelectionTest` locks that behavior by expecting exactly five preview routes and requiring every preview heading to differ from the original heading (`tests/tactical/FTacticalMoveRouteSelectionTest.cpp:384-401`), and `FBattleBoard::drawRoute()` renders only `getStoppedShipPreviewRoutes()` in the stopped-ship preview state (`src/tactical/FBattleBoard.cpp:292-301`). The model still keeps the current-heading path in `m_movementHexes`, and `handleMoveHexSelection()` can still consume those hexes (`src/tactical/FTacticalGame.cpp:1187-1234`), but that path is not rendered in the preview-only branch, leaving a legal move direction undiscoverable in the UI.

WARNING
- The verifier PASS results are not sufficient at the feature level because the regression suite and documentation have converged on the same incomplete interpretation. The test suite now treats five alternate-heading previews as correct (`tests/tactical/FTacticalMoveRouteSelectionTest.cpp:384-401`), so it would not catch the missing current-heading preview required by the plan.
- Documentation inherits the same gap. `doc/UsersGuide.md:382` says the highlighted preview routes represent the legal facings available to a stopped ship, but the implementation does not highlight the current-heading forward option in the preview state.

NOTE
- No separate gaps were confirmed for preview-click continuation, additive/non-wx model boundaries, or the `MR == 0` restriction. Those parts of the delivery match the plan.
- `doc/rules/tactical_operations_manual.md` was not modified.
- Tactical validation passed even though the feature-level acceptance gap remains, which reinforces that this is a scope/completeness miss rather than a build or test-breakage issue.

Missed functionality / edge cases:
- Missing current-heading preview for a stopped ship's initial selection state.
- Because the stopped-ship preview renderer shows only alternate-heading preview routes, the straight-ahead move from the ship's existing facing is still legal in the model but may be invisible to the player until they already know where to click.

Follow-up feature requests for planning:
- Update the stopped-ship preview generation and PH_MOVE renderer so the immediately visible preview state includes the current-heading forward route alongside the alternate-facing previews, and extend regression coverage/documentation to assert all legal starting facings are shown rather than only five alternates.

Final outcome:
- FAIL
