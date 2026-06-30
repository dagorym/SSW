# Implementer Report

Status:
- success

Task summary:
- Fix BattleBoard title-bar X close while a WXTacticalUI child modal dialog (damage summary, ICM selection, or winner/info message) is active. The close path now dismisses the active child dialog before hiding the frame, preventing the duplicate wxDialog::EndModal assert at src/gtk/dialog.cpp:193.

Changed files:
- include/gui/WXTacticalUI.h
- src/gui/WXTacticalUI.cpp
- include/tactical/FBattleScreen.h
- src/tactical/FBattleScreen.cpp

Validation commands run:
- make (from repo root)
- cd tests/gui && make && xvfb-run -a ./GuiTests (or DISPLAY=:99 ./GuiTests with a running Xvfb)
- cd tests/tactical && make && ./TacticalTests

Validation outcome:
- PASS — build clean, 226 tactical tests pass. 9 pre-existing GUI test failures (source-pattern checks in files not touched by this change: ViewFleetGUI.cpp, FBattleDisplay.cpp, ICMSelectionGUI.cpp, strategic UI) are unchanged from branch baseline and are not regressions introduced by this fix.

Implementation/code commit hash:
- 233c1ad

Artifacts written:
- artifacts/tactical-misc-fixes/TMF-03/implementer_report.md
- artifacts/tactical-misc-fixes/TMF-03/tester_prompt.txt
- artifacts/tactical-misc-fixes/TMF-03/implementer_result.json

Implementation context:
- Root cause: FBattleScreen::closeBattleScreen -> EndModal -> Show(false) on FBattleScreen while a child wxDialog (owned by WXTacticalUI) still has IsModal()==true. wxGTK's dialog.cpp:Show(false) calls EndModal(wxID_CANCEL) on any child whose IsModal() is true, producing a duplicate EndModal -> assert.
- Fix: WXTacticalUI now tracks the active dialog pointer (m_activeDialog) in showMessage, showDamageSummary, and runICMSelection. FBattleScreen::closeBattleScreen calls m_tacticalUI->dismissActiveDialog() before any EndModal or Show(false) runs. This makes IsModal() return false before the frame hides, so wxGTK's auto-dismiss never fires a second EndModal.
- WXTacticalUI::hasPendingDialog() and dismissActiveDialog() are new public methods. dismissActiveDialog() calls dialog->EndModal(wxID_CANCEL) on the tracked dialog if it is still modal.
- FBattleScreen::m_tacticalUI is the owned WXTacticalUI* used directly (not through ITacticalUI interface, which is unchanged).
- The m_modalActive guard in FBattleScreen::EndModal is unchanged and still prevents double-EndModal on FBattleScreen itself.
- Test harness: WXTacticalUI::setModalAutoDismissMs can be set to a short timeout so the child dialog auto-dismisses in tests; use a small value (e.g. 50ms) and fire wxEVT_CLOSE_WINDOW from a timer before the auto-dismiss fires to replicate the race.
- WXGuiTestHarness::waitForTopLevelWindowClosed is the correct helper for asserting FBattleScreen disappears after close.

Expected validation failures carried forward:
- testStrategicDialogsUseStaticBoxChildParents — checks ViewFleetGUI.cpp source pattern, pre-existing
- testWXStrategicUISourceGuardsRuntimeAndPreservesParentlessFlow — strategic UI source check, pre-existing
- testRemediatedStrategicDialogsUseFirstShowSizingContract — strategic dialog sizing source check, pre-existing
- testTacticalActionButtonsStayBelowPromptReservationAcrossPhases — FBattleDisplay.cpp source check, pre-existing
- testSeekerActivationPanelSourceContracts — FBattleDisplay.cpp source check, pre-existing
- testSetupPlacementSourceRowsAndOrdnanceColorContracts — FBattleDisplay.cpp source check, pre-existing
- testBattleDisplayNarrowWidthStacksShipStatsBelowButtons — FBattleDisplay.cpp source check, pre-existing
- testICMSelectionDialogInteractionFinalizesAssignedCountsAndAmmo — ICMSelectionGUI.cpp source check, pre-existing
- testLocalGameDialogLaunchesPredefinedAndCustomModalChains — BattleSim centering check, pre-existing
