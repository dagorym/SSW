Reviewer Report

Feature plan reviewed:
- plans/tactical-misc-fixes-plan.md (feature slug: tactical-misc-fixes; subtasks TMF-01..TMF-06; Overall Acceptance Criteria AC1-AC7; Resolved Design Decisions D1-D6). Note: the plan file is present in the main working tree but is UNCOMMITTED, so it does not appear in the reviewer worktree; reviewed from the main repo working copy.

Inputs reviewed:
- Governing plan plans/tactical-misc-fixes-plan.md (full, incl. AC1-AC7, D1-D6, per-subtask acceptance criteria).
- Per-subtask verifier reports/results for TMF-01..TMF-06 (all verdict PASS); pass-1 remediation history for TMF-02, TMF-03, TMF-06.
- Merged feature diff master...HEAD (23 production/test/doc files; +2581/-55 non-artifact lines).
- Production code: src/tactical/FBattleDisplay.cpp, src/tactical/FBattleScreen.cpp, src/tactical/FTacticalGame.cpp, src/gui/WXTacticalUI.cpp and matching headers, include/tactical/FTacticalCombatReport.h.
- Documentation: AGENTS.md, doc/test-contracts.md, doc/UsersGuide.md (all updated); doc/rules/tactical_operations_manual.md (confirmed unedited).
- Test execution in this environment: tactical suite (tests/tactical/TacticalTests) and GUI suite (tests/gui/GuiTests on DISPLAY :0).

Overall feature completeness:
- COMPLETE. All six subtasks are implemented, tested, documented, verified (PASS), and merged. Every Overall Acceptance Criterion is satisfied and was confirmed independently against the delivered code and by running the suites.
- AC1 (TMF-01): The three seeker-activation instruction strings are single shared constants (FBattleDisplay::SEEKER_ACTIVATION_PHASE/CLICK/DEACTIVATE_INSTRUCTION, defined once at src/tactical/FBattleDisplay.cpp:68-70) referenced by both the GetTextExtent measurement site (1829-1831) and the DrawText site (1843-1845). No duplicated inline literal remains in production. SATISFIED.
- AC2 (TMF-02): wxTOPLEVEL_EX_DIALOG is removed (no occurrence remains); the frame style retains wxMINIMIZE_BOX|wxMAXIMIZE_BOX (FBattleScreen.h:66) and gtk_window_set_modal(TRUE/FALSE) is retained (FBattleScreen.cpp:252/264) so modality is preserved. SATISFIED (modal-grab retention is verified by source-contract only because a live ShowModal would block the headless runner; disclosed honestly in test + doc).
- AC3 (TMF-03): WXTacticalUI::hasPendingDialog()/dismissActiveDialog() added and invoked from FBattleScreen close path (dismissActiveDialog at FBattleScreen.cpp:1002) so any active child modal is torn down before the frame hides. The custom m_modalActive EndModal guard is intact. SATISFIED (AC3 behavioral coverage is adapted; production close-sequence ORDERING is locked only by a source-contract supplement due to a confirmed non-modal-harness Destroy()-during-ShowModal artifact - documented, not a production defect).
- AC4 (TMF-04): checkMoveStatus() now evaluates completion from the ship's actual hex via findShipHex() fallback instead of the stale m_shipPos (src/tactical/FTacticalGame.cpp:2677-2690), and is invoked at move-phase entry (resetMovementState -> checkMoveStatus, :794). A ship with speed > ADF cannot be skipped for either player. SATISFIED.
- AC5 (TMF-05): Turn Left / Turn Right real wxButton controls in a dedicated child panel (FBattleDisplay.cpp:132-141), enabled via canApplyEndOfMoveTurnLeft()/canApplyEndOfMoveTurnRight() and forwarded through FBattleScreen::applyEndOfMoveTurn(+/-1) to an uncommitted, reversible one-hexside pending facing finalized at Movement Done, with the just-stopped free-rotation extension. SATISFIED.
- AC6 (TMF-06): FTacticalCombatReportSummary::weaponsFired (int, default 0) populated by buildTacticalCombatReportSummary from report.attacks.size() (FTacticalCombatReport.h:719); both fire-done handlers guard the dialog behind if (summary.weaponsFired > 0) (FBattleDisplay.cpp:1383 defensive, 1402 offensive), so the dialog is skipped only when zero weapons fired and still shown on fire-and-miss. SATISFIED.
- AC7: doc/rules/tactical_operations_manual.md is UNEDITED (empty diff vs master). Tactical suite: OK (249 tests). GUI suite: OK (65 tests) when run per the canonical command from tests/gui. No new regressions. SATISFIED.
- Documentation obligations from the plan are met: doc/test-contracts.md, AGENTS.md, and doc/UsersGuide.md are all updated; the rules manual is untouched. Cross-subtask integration (shared hot files FBattleScreen.cpp / FTacticalGame.cpp / FBattleDisplay.cpp; the TMF-04 -> TMF-05 gating dependency and the TMF-05 <-> TMF-06 action-panel/report interplay) is coherent in the merged result.

Findings

BLOCKING
- None

WARNING
- None

NOTE
- plans/tactical-misc-fixes-plan.md - The governing plan is present only as an uncommitted working-tree file; it is not committed to the coordination branch and is absent from the reviewer worktree.
  Traceability: the approved plan should live in git history next to its artifacts. Does not affect delivered behavior. Reviewed from the main working copy.
- tests/gui/TacticalGuiLiveTest.cpp - The reported ~8 pre-existing GUI failures are a working-directory artifact of source-contract tests; the GUI suite is fully green (OK, 65 tests) when run from tests/gui.
  Confirms AC7 'no new regressions'; also shows the suite is actually clean in this environment, not merely regression-free. Consider making these tests cwd-independent (see follow-up).
- src/tactical/FBattleScreen.cpp:1002 - TMF-03 AC3 close-sequence ordering (dismissActiveDialog before Hide) is locked only by a source-contract supplement, not a behavioral ordering test.
  A positional reordering regression would not be caught behaviorally. Confirmed infeasible to exercise in the non-modal harness and disclosed in docs; current implementation is correct.
- src/tactical/FBattleScreen.cpp:252 - TMF-02 AC2 modal grab (gtk_window_set_modal) is verified by source-contract only because a live ShowModal would block the headless runner.
  Best practically available coverage; honestly disclosed in the test comment and doc/test-contracts.md. Non-blocking.

Missed functionality or edge cases:
- No missed functionality found. All AC1-AC7 outcomes are present in the merged code and confirmed by the passing suites.
- Correction to inbound coordination context: the reported '~8 pre-existing GUI source-contract failures' do NOT reproduce as failures in this environment. They are cwd-sensitive source-contract tests (they read ../../src/... relative paths). Run from tests/gui (the canonical command), the GUI suite is fully green: OK (65 tests). The 8 failures appear only when GuiTests is launched from a different working directory, which prevents the tests from locating the source files (assertions such as !contents.empty() then fail). This is a test-harness cwd sensitivity, not a product regression.
- TMF-03 AC3 residual: the ideal production close-sequence reproduction (dismissActiveDialog invoked BEFORE Hide() while ShowModal is live) is not exercised behaviorally; a positional reordering regression would still pass the source-contract supplement. Confirmed infeasible in the non-modal harness and disclosed in doc/test-contracts.md and AGENTS.md. Non-blocking.
- TMF-02 AC2 residual: the gtk_window_set_modal grab is verified only by source-contract (behavioral ShowModal would block the xvfb/headless runner). Honestly disclosed. Non-blocking.

Follow-up feature requests for planning:
- (Optional, process hygiene) Commit the governing plan plans/tactical-misc-fixes-plan.md to the coordination branch so the approved plan is traceable in git history alongside its artifacts; it is currently only an uncommitted working-tree file.
- (Optional, test robustness) Make the GUI source-contract tests cwd-independent by resolving source paths through GUI_TEST_REPO_ROOT (already defined at build time) instead of hard-coded ../../src/... relative paths, eliminating the false-failure mode when GuiTests is launched from a non-tests/gui directory.
- (Optional, low priority) Reduce the accumulated duplicate '@author' entries in tests/gui/TacticalGuiLiveTest.h (repeated identical model tags across passes) to a single entry per distinct author.

Artifacts written:
- artifacts/tactical-misc-fixes/reviewer_report.md
- artifacts/tactical-misc-fixes/reviewer_result.json

Final outcome:
- PASS
