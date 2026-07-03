Reviewer Report

Feature plan reviewed:
- plans/tactical-misc-fixes-round2-plan.md (feature slug: tactical-misc-fixes-round2)
- Governing scope: Overall Acceptance Criteria AC1-AC7, the Verification Policy (manual-verification gate for TMFR-01/TMFR-03 runtime behaviors), and each subtask's acceptance criteria.
- Reviewed against pre-work coordination-branch tip 7c205d7 on branch 'fixes'.

Inputs reviewed:
- Merged code diffs 7c205d7..HEAD: src/tactical/FBattleScreen.cpp, include/tactical/FBattleScreen.h, src/gui/WXTacticalUI.cpp, include/gui/WXTacticalUI.h, src/gui/SelectCombatGUI.cpp, src/tactical/FTacticalGame.cpp, src/tactical/FBattleDisplay.cpp, include/tactical/FBattleDisplay.h.
- Merged test diffs: tests/gui/TacticalGuiLiveTest.{cpp,h}, tests/gui/StrategicGuiLiveTest.cpp, tests/tactical/FTacticalEndOfMoveTurnTest.{cpp,h}, FTacticalGameMockUITest.{cpp,h}, WXTacticalUIAdapterTest.{cpp,h}, FTacticalBattleDisplayFireFlowTest.{cpp,h}.
- Documentation diffs: AGENTS.md, doc/test-contracts.md (TMF-02/TMF-05 updated; TMFR-01/TMFR-03 entries added), doc/UsersGuide.md.
- Per-subtask artifacts (implementer/tester/documenter/verifier reports + results) under artifacts/tactical-misc-fixes-round2/TMFR-01..TMFR-04/. All four Verifier verdicts: PASS (0 blocking, 0 warning; TMFR-01 and TMFR-04 each 1 NOTE).
- Non-negotiable constraint check: git diff 7c205d7..HEAD -- doc/rules/tactical_operations_manual.md produced no output (unedited).

Overall feature completeness:
- AC1 (minimize + preserved modality): SATISFIED at deliverable level. gtk_window_set_modal() grab removed from FBattleScreen::ShowModal(); modality now enforced by a wxWindowDisabler member (m_windowDisabler) disabling other top-levels for the battle's duration and restored in EndModal()/destructor. wxMINIMIZE_BOX remains in the style. The rendered minimize decoration is a runtime behavior explicitly DEFERRED to the required user manual verification pass.
- AC2 (title-bar X + Quit no-assert; dismissal stack): SATISFIED at deliverable level. Added a GTK delete-event bypass on the top-level widget mirroring the existing Quit activate bypass so the X reaches closeBattleScreen() while a child dialog is modal. WXTacticalUI's single m_activeDialog pointer replaced by m_dialogStack (std::vector<wxDialog*>); dismissActiveDialog() walks rbegin() (innermost-first) calling EndModal on every live dialog. Automated coverage of the dismissal-stack/close logic present (behavioral single-dialog + empty-stack no-op tests, wxWindowDisabler restore test, close idempotency test; nested-two-dialog live case documented as harness-infeasible and locked with a source-contract supplement). Live mid-battle X close and Quit-no-assert-with-multiple-dialogs DEFERRED to manual pass.
- AC3 (turn enable rule): SATISFIED and behaviorally verified. canUseEndOfMoveTurn() now gates on turnData->path.countFlags(MR_TURN) >= (unsigned)ship->getMR() (i.e. enabled while < getMR()), matching the route-highlight rule; minimum-move and pendingEndOfMoveFacing guards retained. Behavioral tests include the exact minelayer MR=2 repro driven through handleHexClick() (real MR_TURN flagging), plus budget-boundary tests; the retired flag-on-end-hex test is documented.
- AC4 (wrapped-width placement): SATISFIED at deliverable level. New measureWrappedActionPromptWidth() drives instrW from wrapped line widths; the long instruction line split across detailPromptTwo/detailPromptThree; requestedDisplayHeight grows via applyRequestedDisplayHeight(). GUI behavioral tests assert panel shown at default 1200px, placement right of Movement Done and left of ship-info column, and height expansion. Visible-at-default-size runtime confirmation DEFERRED to manual pass.
- AC5 (caption): SATISFIED. A fixed 4-line 8pt caption ('If at the end of a ship's movement / you want to make a facing change / (and have remaining MR), use these / buttons to make a single final turn.') is drawn above the buttons; blockWidth sizing and panel-height growth prevent clipping. Verified by a pixel-level behavioral test plus a source-contract supplement locking the exact lines.
- AC6 (GUI test cwd-independence + @author dedup): SATISFIED. Hard-coded ../../src/... paths replaced by guiRepoFile()/repoFile() helpers resolving GUI_TEST_REPO_ROOT across TacticalGuiLiveTest.cpp and StrategicGuiLiveTest.cpp; grep confirms no remaining literal '../../src' in tests/gui. Duplicate @author tags collapsed. Verifier independently ran GuiTests from both tests/gui and the repo root: OK (71 tests) both.
- AC7 (manual unedited; suites pass): SATISFIED. tactical_operations_manual.md is unedited across the whole feature. The four verifiers independently reran the suites on this branch (TacticalTests 251-254, GuiTests 67-71, SSWTests 195-198), all OK; one pre-existing, unrelated non-deterministic BattleSim GUI flake was noted and is out of scope.
- Cross-subtask integration: consistent. TMFR-02's whole-path MR budget rule and TMFR-03's caption ('...and have remaining MR...') describe the same end-of-move turn feature and are mutually consistent; AGENTS.md and doc/test-contracts.md TMF-05 entries were updated to the corrected rule. TMFR-01's modality/close rework is coherently documented across AGENTS.md and the new doc/test-contracts.md TMFR-01 entry, including the DEFERRED-TO-MANUAL items. Working tree is clean; all changes committed.

Findings

BLOCKING
- None

WARNING
- None

NOTE
- doc/test-contracts.md / src/gui/WXTacticalUI (historical entries) - Pre-existing TMF-03 entry still describes the retired single m_activeDialog pointer and single-dialog dismissActiveDialog() semantics; this predates TMFR-01 and was flagged by the TMFR-01 verifier as out-of-scope staleness.
  The historical TMF-03 wording is now superseded by the m_dialogStack mechanism documented in the new TMFR-01 entry. It does not affect the accuracy of the current TMFR-01 documentation, but a future doc-hygiene pass could add a forward-pointer from TMF-03 to TMFR-01 to avoid confusion.
- Verification Policy (plan) / AC1, AC2, AC4 runtime portions - Four runtime behaviors remain unverified by automation by design: (a) effective minimize decoration, (b) live mid-battle title-bar X close with no assert, (c) File->Quit no-assert with more than one child dialog live, (d) Turn-panel visibility at the default window size.
  These are correctly and explicitly deferred to the plan-mandated user manual verification pass (documented in the TMFR-01 test-contracts entry and in-test comments), not gaps in delivery. The feature is not FULLY accepted until that manual pass is signed off; that gate is tracked by the Coordinator outside the automated chain. Per the review remit this deferral alone does not reduce the automated/deliverable verdict.
- tests/gui/TacticalGuiLiveTest.cpp / tests/tactical/WXTacticalUIAdapterTest.cpp - The genuinely-nested two-live-dialog dismissal path (the exact multi-dialog scenario that motivated the m_dialogStack change) is covered only by a single-dialog behavioral test plus a source-contract supplement locking the stack shape and rbegin() iteration, because a live nested ShowModal hangs the headless harness.
  This is an acceptable, plan-consistent limitation (the Verification Policy prohibits driving a live ShowModal loop), and the multi-dialog runtime behavior is part of the manual verification gate. Noted so the manual pass explicitly exercises closing the battle while two child dialogs have been live.

Missed functionality or edge cases:
- No missed in-scope functionality identified. Every plan subtask (TMFR-01..TMFR-04) and every Overall Acceptance Criterion (AC1-AC7) is addressed by merged code with corresponding automated behavioral coverage for all headless-testable behavior, plus the documented manual gate for the runtime-only behaviors.
- Edge cases confirmed covered: budget boundary at turns-used == MR (blocked) and < MR (allowed), the real-movement minelayer MR=2 repro, empty dialog stack no-op, wxWindowDisabler restore-on-destroy, close idempotency under duplicate close events, caption/button non-clipping via panel-height growth, and cwd-independent test path resolution from both launch directories.
- Edge case explicitly deferred (not a delivery gap): the true nested multi-dialog live close/Quit sequence and all window-manager decoration behaviors are covered by the required user manual verification pass rather than automation, consistent with the plan's Verification Policy.

Follow-up feature requests for planning:
- None required for delivery gaps. Non-blocking optional hygiene follow-up: in a future documentation-only pass, add a forward reference from the historical doc/test-contracts.md TMF-03 entry to the TMFR-01 entry so the retired single-m_activeDialog wording is not mistaken for current behavior.
- Process (not a code gap): the Coordinator must run and sign off the plan-mandated user manual verification pass covering (a) effective minimize button, (b) live mid-battle title-bar X close with no EndModal assert, (c) File->Quit no-assert with more than one child dialog live, and (d) Turn-panel visibility/placement at the default window size, before declaring tactical-misc-fixes-round2 fully accepted.

Artifacts written:
- artifacts/tactical-misc-fixes-round2/reviewer_report.md
- artifacts/tactical-misc-fixes-round2/reviewer_result.json

Final outcome:
- PASS
