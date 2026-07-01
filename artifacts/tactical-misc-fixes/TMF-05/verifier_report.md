Verifier Report

Scope reviewed:
- TMF-05: End-of-move single facing change via Turn Left / Turn Right action buttons. Implementer commit 0bf0d9e; Tester commit f292357; Documenter commit d2b86fc. Changes span FTacticalGame (model API — canApplyEndOfMoveTurnLeft/Right, applyEndOfMoveTurn, FTacticalTurnData fields), FBattleDisplay (Turn panel UI), FBattleScreen (delegation), WXTacticalUI (dialog tracking for TMF-03). Also includes TMF-01 through TMF-04 changes upstream. Tests: FTacticalEndOfMoveTurnTest (17 behavioral model tests), TacticalGuiLiveTest (3 GUI behavioral tests for TMF-05, plus TMF-02/03/04 tests). Documentation: AGENTS.md, doc/UsersGuide.md, doc/test-contracts.md updated.

Acceptance criteria / plan reference:
- plans/tactical-misc-fixes-plan.md § TMF-05 acceptance criteria (plan recovered from main repo; worktree predates plan merge). Also: AGENTS.md behavioral testing policy.

Convention files considered:
- AGENTS.md
- CLAUDE.md

Findings

BLOCKING
- None

WARNING
- None

NOTE
- tests/tactical/FTacticalEndOfMoveTurnTest.cpp - No explicit test for re-selecting a ship mid-pending-turn
  The acceptance criterion states "re-selecting the active ship before Movement Done re-presents the Turn controls and the ship's current pending facing". No test explicitly clicks the ship's own hex while a pending turn is active to confirm FTacticalTurnData state persists through selectShipFromHex. The behavior is correct by construction (selectShipFromHex does not clear pendingEndOfMoveFacing), but a dedicated test would strengthen confidence. Coverage gap, not a defect.

- tests/gui/TacticalGuiLiveTest.cpp:testBattleScreenShowModalContainsGtkWindowSetModal - TMF-02 source-contract-only coverage for gtk_window_set_modal call in ShowModal
  This test (for TMF-02, not TMF-05) is source-contract-only. AGENTS.md policy requires behavioral tests for behavioral criteria; however, FBattleScreen::ShowModal() cannot be exercised behaviorally under xvfb (its custom modal event loop blocks the test runner). Two behavioral tests cover the observable ACs for TMF-02. This was reviewed and accepted in TMF-02's verifier cycle; noted here for completeness.

- src/tactical/FBattleDisplay.cpp:drawMoveShip (~line 1143) - Turn panel shown in PH_MOVE regardless of ship selection when space permits
  The turn button panel is shown whenever a PH_MOVE draw occurs and there is sufficient horizontal space, even when no ship is selected. Both buttons are disabled in that case (canApplyEndOfMoveTurnLeft/Right return false when m_curShip==NULL). The acceptance criteria says buttons "appear when a ship is selected"; in practice the disabled-button pattern is standard UX and the model guards prevent any incorrect state change. Presentation detail, not a defect.

Test sufficiency assessment:
- SUFFICIENT. The 17 FTacticalEndOfMoveTurnTest behavioral model tests cover all core acceptance criteria: button enable/disable conditions (MR=0, min-move not satisfied, MR_TURN flag on end hex), heading change immediacy and correctness, pending state fields, one-hexside-at-a-time clamping, reverse undo, finalize commit, hex-click cancel, and speed-0 case. Three GUI behavioral tests cover panel visibility, button enable-state wiring, and click-to-model routing. One minor gap: no explicit re-selection-while-pending test (NOTE above). All TMF-05 tests are genuine behavioral tests per AGENTS.md policy.

Documentation accuracy assessment:
- ACCURATE. doc/UsersGuide.md documents the new Turn Left/Right buttons correctly including reversibility, MR consumption, commitment at Movement Done, hex-click cancel, and speed-0 extension. AGENTS.md correctly describes FTacticalTurnData fields, FBattleDisplay turn panel ownership, FBattleScreen delegation, and FTacticalEndOfMoveTurnTest registration. doc/test-contracts.md has a TMF-05 entry cataloging all 17 behavioral model tests and 3 GUI tests. Doxygen headers are updated on all new/edited functions. doc/rules/tactical_operations_manual.md is unchanged (empty diff confirmed).

Artifacts written:
- artifacts/tactical-misc-fixes/TMF-05/verifier_report.md
- artifacts/tactical-misc-fixes/TMF-05/verifier_result.json

Verdict:
- PASS
