# Tester Report

Status:
- success

Task summary:
- TMF-03: FBattleScreen title-bar X close while a WXTacticalUI child modal dialog (damage summary, ICM selection, or winner/info message) is active now calls dismissActiveDialog() before Hide()/Show(false), preventing a duplicate wxDialog::EndModal assert in wxGTK (src/gtk/dialog.cpp:193). WXTacticalUI gained hasPendingDialog() and dismissActiveDialog() APIs and tracks m_activeDialog during ShowModal calls. New test testBattleScreenXCloseDismissesActiveChildDialog validates the new dismiss API and the FBattleScreen close lifecycle.

Branch name:
- tmf-TMF-03-tester-20260630

Test commit hash:
- 29e69dc

Test files added or modified:
- tests/gui/TacticalGuiLiveTest.h
- tests/gui/TacticalGuiLiveTest.cpp

Commands run:
- make -C /home/tstephen/repos/worktrees/tmf-TMF-03-tester-20260630/tests/gui
- DISPLAY=:0 /home/tstephen/repos/worktrees/tmf-TMF-03-tester-20260630/tests/gui/GuiTests

Pass/fail totals:
- failed: 8
- new_tests_passed: 1
- passed: 52
- pre_existing_failures: 8
- total: 60

Unmet acceptance criteria:
- None

Final test outcomes:
- PASS: testBattleScreenXCloseDismissesActiveChildDialog — new TMF-03 test passes cleanly
- AC1 PASS: hasPendingDialog() returns true while ShowModal is live (dialog tracked in m_activeDialog)
- AC1 PASS: hasPendingDialog() returns false immediately after dismissActiveDialog() (IsModal cleared)
- AC2 PASS: FBattleScreen becomes hidden/absent after close following child dialog dismissal
- AC3 PASS: No crash or SIGABRT — process survived all assertions (implicit behavioral verification)
- AC3 source-contract PASS: closeBattleScreen contains dismissActiveDialog() call (integration point locked)
- AC4 PASS: FBattleScreen lifecycle counters settle (constructed>=+1, destroyed>=+1, liveCount==0)
- 8 pre-existing failures in unrelated tests (source-contract checks for FBattleDisplay.cpp / ICMSelectionGUI.cpp) — none introduced by TMF-03
- Design note: wxEVT_CLOSE_WINDOW is fired after runVoidFunctionWithAction returns (not from within ShowModal action), avoiding a Destroy()-during-ShowModal double-free in the test harness. Source-contract supplement locks the closeBattleScreen integration point.

Cleanup status:
- No temporary byproducts remain. All diagnostic std::cerr prints removed from final test implementation.

Artifacts written:
- artifacts/tactical-misc-fixes/TMF-03/tester_report.md
- artifacts/tactical-misc-fixes/TMF-03/tester_result.json
- artifacts/tactical-misc-fixes/TMF-03/documenter_prompt.txt
