Reviewer Report

Feature plan reviewed:
- plans/seeker-missile-fixes-round4-plan.md (Seeker Missile Fixes Round 4; subtasks SMRIV-01..SMRIV-06; Overall Acceptance Criteria 1-10)

Inputs reviewed:
- Governing plan: plans/seeker-missile-fixes-round4-plan.md
- Subtask artifacts (implementer/tester/documenter/verifier reports+results): artifacts/seeker-missile-fixes-round4/SMRIV-01..SMRIV-06 (all six Verifier results PASS, no Security stage required)
- Production code: src/tactical/FBattleDisplay.cpp, include/tactical/FBattleDisplay.h (SMRIV-01..04); src/tactical/FTacticalGame.cpp, include/tactical/FTacticalGame.h (SMRIV-05, SMRIV-06)
- Tests: tests/tactical/FTacticalSeekerMovementTest.*, FTacticalMineDamageFlowTest.*, FTacticalPreGameOrdnanceTest.*, tests/gui/TacticalGuiLiveTest.*
- Docs: doc/test-contracts.md (SMRIV-01..06 entries), header Doxygen blocks; doc/rules/tactical_operations_manual.md confirmed unedited
- Executed merged-tree tests: tests/tactical/TacticalTests (225 pass) and tests/gui/GuiTests (run from tests/gui CWD)
- Executed pre-round-4 baseline (commit 529b615) GUI suite to attribute a failing GUI test

Overall feature completeness:
- COMPLETE. All 10 Overall Acceptance Criteria are satisfied by the merged sf2 tree.
- AC1-AC4 (HUD layout): SMRIV-01 mine-phase rows anchored to panel top / right of left column with two-line instruction text; SMRIV-02 seeker-phase side-by-side middle/right columns with two-line text; SMRIV-03 PH_ATTACK_FIRE recall list anchored to panel top right of Done. Live position/click-region coverage in TacticalGuiLiveTest passes (when run from the tests/gui CWD).
- AC5 (panel shrink at phase change): SMRIV-04 adds lastBattleState/lastBattlePhase to LowerPanelLayoutState; ensureLowerPanelLayoutState() skips max-preserve only on phase change, preserves within-phase expansion, and keeps the 120 px floor. New behavioral test testLowerPanelHeightShrinksBackAfterPhaseChange passes (fails-before/passes-after demonstrated).
- AC6 (seeker removal in all paths): SMRIV-05 routes beginSeekerActivationPhase() (no-inactive-hex early path) and completeSeekerActivationPhase() through applyMovementSeekerDamage(), which collects detonated IDs, resolves damage (UI) or clears outcomes (no-UI), then erases detonated seekers exactly once. SMF-06 visibility preserved (redraw before dialogs, erase after). New behavioral test testActivationPhaseImpactingSeekerRemovedAndDoesNotAttackOnNextPhase passes.
- AC7 (cleanup): SMRIV-06 removes the unreachable else-if(getState()==BS_PlaceSeekers) branch in placeOrdnanceAtHex() and adds a defensive m_minedHexList.erase(hex) for the no-matching-record case; behavior-preserving, covered by testStaleMinedHexErasedWhenNoPlacedOrdnanceRecord.
- AC8 (Carry-C): include guard in tests/tactical/FTacticalPreGameOrdnanceTest.h corrected PREAGAME -> PREGAME and is self-consistent (ifndef/define/endif all FTACTICALPREGAMEORDNANCETEST_H_).
- AC9: new fail-before/pass-after behavioral coverage exists for both SMRIV-04 and SMRIV-05.
- AC10: doc/rules/tactical_operations_manual.md has zero commits in the feature range.
- Cross-subtask integration verified: SMRIV-01..04 share FBattleDisplay layout/height helpers and merge coherently; SMRIV-05 (completeSeekerActivationPhase) and SMRIV-06 (placeOrdnanceAtHex) edit disjoint FTacticalGame functions with no conflict. Full tactical suite: 225/225 pass.
- doc/test-contracts.md contains entries for all six SMRIV subtasks.

Findings

BLOCKING
- None

WARNING
- tests/gui/TacticalGuiLiveTest.cpp:1233 - Deterministic failing GUI test testBattleDisplayLowerPanelLayoutStatePersistsAcrossPhaseAndGeometryChanges (expects panel min-height 120, gets 137) in the merged tree; CONFIRMED PRE-EXISTING (identical 120-vs-137 failure at pre-round-4 baseline commit 529b615), not introduced or worsened by round-4.
  The failure exercises an entirely within-PH_MOVE geometry resize-back path that round-4 did not modify; the within-phase max-preserve semantics are identical before and after SMRIV-04, so round-4 is not the cause. However the test's expectation (a within-phase geometry resize-back restores the panel to the 120 floor) now directly contradicts the SMRIV-04 / Overall AC5 design that deliberately keeps expanded height within a phase and shrinks only at phase change. The repo therefore ships a red behavioral test on the panel-height surface that should be reconciled with the now-formalized lifecycle.
- artifacts/seeker-missile-fixes-round4/SMRIV-01..SMRIV-05/tester_report.md - Verification-procedure miss: every subtask Tester/Verifier stage ran tests/gui/GuiTests from the repository-root CWD rather than from tests/gui, so all source-contract tests using ../../src/... relative paths false-failed on path resolution and were reported as an undifferentiated 'GuiTests: 9 pre-existing failures' count.
  Run from the canonical CWD (AGENTS.md: cd tests/gui && ... ./GuiTests) only one deterministic real failure remains (the pre-existing persistence test above) plus one flaky BattleSim modal test. The wrong-CWD procedure masked which failures were real vs path artifacts. The stages' end conclusion ('pre-existing, unrelated to the subtask') was correct, but the procedure did not actually establish that and could have hidden a genuine regression.

NOTE
- tests/gui/BattleSimGuiLiveTest.cpp:242 - Flaky live test testLocalGameDialogLaunchesPredefinedAndCustomModalChains: failed on one of three correct-CWD runs and also fails intermittently at the pre-round-4 baseline. Unrelated to round-4.
  Live modal-chain timing on a real X display (:0) is non-deterministic; this is a pre-existing test-harness robustness issue, not a feature defect.

Missed functionality or edge cases:
- No round-4-attributable functional gaps found. All six subtasks deliver the specified behavior and all new behavioral tests pass.
- The only red GUI test in the merged tree is pre-existing and lies on a code path (within-PH_MOVE geometry resize) that round-4 did not touch; it is out of scope for this feature's acceptance but worth a reconciliation follow-up because SMRIV-04 formalized the very within-phase semantic it contradicts.
- applyMovementSeekerDamage() reuse for the activation path was reviewed for the no-UI branch: it clears pending outcomes and still erases detonated seekers, so the activation early-path that previously did nothing when m_ui==NULL now correctly removes impacting seekers without dereferencing a null UI.

Follow-up feature requests for planning:
- Reconcile the pre-existing failing GUI test tests/gui/TacticalGuiLiveTest.cpp::testBattleDisplayLowerPanelLayoutStatePersistsAcrossPhaseAndGeometryChanges with the SMRIV-04 panel-height lifecycle: either update the test's within-phase geometry resize-back assertion to match the accepted 'keep expanded height within a phase, recompute only at phase change' design (AC5), or, if within-phase geometry-driven shrink-back is actually desired, extend the height lifecycle to distinguish content-overflow expansion from geometry-driven stacking expansion. Scope: tests/gui/TacticalGuiLiveTest.cpp and possibly src/tactical/FBattleDisplay.cpp ensureLowerPanelLayoutState().
- Harden the GUI test verification procedure so it is run from the canonical tests/gui CWD (per AGENTS.md), and make the source-contract helper resolve source paths from a fixed repo root instead of a CWD-relative ../../src/... path, so source-contract tests do not false-fail when the runner is launched from elsewhere. This prevents real failures from being masked inside a path-artifact failure count.
- Stabilize the flaky live modal-chain test tests/gui/BattleSimGuiLiveTest.cpp::testLocalGameDialogLaunchesPredefinedAndCustomModalChains (deterministic harness scheduling / bounded waits) so the GUI suite is reliably green.

Artifacts written:
- artifacts/seeker-missile-fixes-round4/reviewer/reviewer_report.md
- artifacts/seeker-missile-fixes-round4/reviewer/reviewer_result.json

Final outcome:
- PASS
