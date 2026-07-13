# Implementer Report

Status:
- success

Task summary:
- Resolve finding H9: bind all FBattleDisplay button click handlers exactly once at construction and drive Show/Enable/turn-panel position per phase from a new non-paint FBattleDisplay::updateForPhase(), removing the m_first paint-time connect/show/hide machinery. Wire updateForPhase() into FBattleScreen::setState, setPhase, and onSize (via the shared reDraw() seam).

Changed files:
- include/tactical/FBattleDisplay.h
- src/tactical/FBattleDisplay.cpp
- include/tactical/FBattleScreen.h
- src/tactical/FBattleScreen.cpp

Validation commands run:
- make (repo root) - full build of both executables and top-level test targets: PASS (exit 0, no warnings)
- cd tests/gui && make && xvfb-run -a ./GuiTests - PASS: OK (75 tests), including TMF-05 (TMF05-turn-hidden/enable/click) and TMF-06 (TMFAC2 defensive-skip/offensive-skip)
- cd tests/tactical && make && ./TacticalTests - 247/253 pass; 6 source-inspection failures in FTacticalBattleDisplayFireFlowTest (see expected failures)
- Recommended full gate to re-run after Tester updates the source-contract tests: make check (repo root)

Validation outcome:
- Build PASS (make, exit 0). Behavioral acceptance criteria PASS: GuiTests OK (75 tests) with TMF-05 and TMF-06 green. TacticalTests: 247/253 pass; the 6 failures are all pure source-inspection (assertContains on function-body literals) tests in FTacticalBattleDisplayFireFlowTest that assert the intentionally-removed old paint-time button Connect/Show/Hide/Disconnect and m_first literals. Classified per validation-triage as EXPECTED CONSEQUENCES of the approved H9 structural change (plan states only TMF-05/TMF-06 must stay green; P4-1 allowed files exclude test files). These require Tester-owned test updates, not an implementation fix.

Implementation/code commit hash:
- bb3ebfa514cb9bc9b749e99024bf30b0af3469de

Artifacts written:
- artifacts/phase4-tactical-gui-hybrid-cleanup/P4-1/implementer_report.md
- artifacts/phase4-tactical-gui-hybrid-cleanup/P4-1/tester_prompt.txt
- artifacts/phase4-tactical-gui-hybrid-cleanup/P4-1/implementer_result.json

Implementation context:
- H9 core change is in FBattleDisplay: the bool m_first member was removed from include/tactical/FBattleDisplay.h and every use site in src/tactical/FBattleDisplay.cpp.
- All 7 phase-button click handlers (m_button1 onSetSpeed, m_buttonMoveDone onMoveDone, m_buttonDefensiveFireDone onDefensiveFireDone, m_buttonOffensiveFireDone onOffensiveFireDone, m_buttonMinePlacementDone onMinePlacementDone, m_buttonSeekerPlacementDone onSeekerPlacementDone, m_buttonSeekerActivationDone onSeekerActivationDone) plus both turn-button handlers (m_buttonTurnLeft onTurnLeft, m_buttonTurnRight onTurnRight) are now Connect()ed exactly once in the FBattleDisplay constructor (src/tactical/FBattleDisplay.cpp ~lines 148-166). Zero Disconnect() calls remain anywhere.
- New public non-paint method FBattleDisplay::updateForPhase() (src/tactical/FBattleDisplay.cpp, defined just before draw()) computes Show()/Hide()+Enable() for each of the 7 phase buttons and the turn panel from FBattleScreen::getState()/getPhase()/getControlState()/getShip()/isMoveComplete()/canApplyEndOfMoveTurnLeft()/canApplyEndOfMoveTurnRight(). It ends with Layout(); it never draws and never constructs a DC.
- Turn-panel POSITION (SetPosition/SetSize) still happens in drawMoveShip() because it needs DC-based wrapped-text-width measurement; but the panel's Show/Hide/Enable now live only in updateForPhase(). draw() no longer contains the two TMF-05 Disconnect+Hide guard blocks.
- The on*FireDone handlers retain an immediate m_button...->Enable(false)/Hide() double-click guard before the (possibly modal) damage-summary dialog; the final settled state comes from updateForPhase() via completeDefensive/OffensiveFirePhase()'s reDraw(). This preserves TMF-06 (dialog shown only when summary.weaponsFired > 0).
- FBattleScreen wiring: reDraw() (include/tactical/FBattleScreen.h) now calls m_display->updateForPhase() before refreshing map+display; setState() and setPhase() (src/tactical/FBattleScreen.cpp) now delegate their refresh to reDraw() instead of calling m_map->Refresh()/m_display->Refresh() directly; onSize() calls m_display->updateForPhase() directly after reflowLowerPanelLayout(). onSetSpeed() adds a final m_parent->reDraw() so the ship-cleared/mine-placement-entered state is reflected after its setState/setPhase/setShip sequence.
- Because reDraw() is the shared seam used throughout FBattleScreen (ship/weapon/defense selection, all phase-completion helpers), updateForPhase() now fires on every model-visible transition, so per-phase button state is correct independent of the next paint.
- Doxygen: full header block added to updateForPhase() (FBattleDisplay.h); Last Modified + @author updated on FBattleDisplay ctor, draw, drawMoveShip, drawDefensiveFire, drawAttackFire, drawPlaceMines, drawPlaceSeekers, drawSeekerActivation, on* handlers, and FBattleScreen setState/setPhase/onSize/reDraw plus both file-level headers.

Expected validation failures carried forward:
- tests/tactical/FTacticalBattleDisplayFireFlowTest::testMoveDoneDisconnectsAndHidesMoveButtonAroundDelegation - asserts onMoveDone() body contains 'm_buttonMoveDone->Disconnect(', 'm_buttonMoveDone->Hide();', 'm_first=true;', and Hide after completeMovePhase(). H9 removed all of these; the button is now hidden by updateForPhase() via completeMovePhase()->reDraw(). Tester must rewrite this as a behavioral test (drive PH_MOVE -> Movement Done click -> assert button IsShown()==false and phase advanced).
- tests/tactical/FTacticalBattleDisplayFireFlowTest::testActionButtonShowPathsRelayoutAfterVisibilityChange - asserts drawMoveShip/drawDefensiveFire/drawAttackFire/drawPlaceMines bodies each contain 'm_button...->Show();' before 'Layout();'. H9 moved all Show() calls to updateForPhase(). Tester must assert observed IsShown() after setPhase/setState + paint, not the source literal.
- tests/tactical/FTacticalBattleDisplayFireFlowTest::testActionButtonHidePathsRelayoutAfterVisibilityChange - asserts on*Done() bodies contain 'm_button...->Hide();' before 'Layout();'. Move/Mine Done handlers no longer Hide inline (updateForPhase() hides via reDraw()); the two fire-Done handlers keep an intentional double-click-guard Hide(). Tester must assert observed IsShown()==false after the completion handler runs.
- tests/tactical/FTacticalBattleDisplayFireFlowTest::testSeekerActivationButtonUsesShowHideDisconnectAndRelayoutPattern - asserts drawSeekerActivation()/onSeekerActivationDone() contain the old Connect/Show/Disconnect/Hide pattern. H9 moved bind to ctor and Show/Hide to updateForPhase(). Tester must assert observed shown/hidden state across PH_SEEKER_ACTIVATION entry/exit.
- tests/tactical/FTacticalBattleDisplayFireFlowTest::testDrawPlaceSeekersUsesSeekerSpecificPromptsAndSMFilter - fails only on the trailing 'm_buttonSeekerPlacementDone->Show();' literal assertion; the seeker-specific prompt/SM-filter assertions still hold. Tester should keep the prompt/filter behavioral checks and replace the Show() literal check with an observed IsShown() assertion in BS_PlaceSeekers.
- tests/tactical/FTacticalBattleDisplayFireFlowTest::testMinePhaseUsesExactPromptTextAndMFilter - fails only on the trailing 'm_buttonMinePlacementDone->Show();' literal assertion; the mine prompt/M-filter assertions still hold. Same remediation as the seeker case for BS_PlaceMines.
- NOTE: All 6 are source-string-inspection tests only. Behavioral acceptance (TMF-05/TMF-06 in tests/gui/TacticalGuiLiveTest) is green. Per AGENTS.md these source-contract tests must be re-authored as behavioral tests (real setState/setPhase transitions + offscreen wxMemoryDC paint asserting IsShown()/IsEnabled()/model state). Do NOT restore the removed paint-time button lifecycle to satisfy them.
