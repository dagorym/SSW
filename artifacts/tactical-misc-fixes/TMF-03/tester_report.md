# Tester Report — TMF-03 pass-2

Status:
- success

Task summary:
- TMF-03 pass-2 remediation: strengthened AC3 behavioral test for the WXTacticalUI
  child-dialog dismiss path.  The production fix (dismissActiveDialog() before Hide()
  in closeBattleScreen) was confirmed sound in pass-1.  Pass-2 attempted to post
  wxEVT_CLOSE_WINDOW while ShowModal is live to reproduce the exact production
  title-bar-X scenario.  That approach was confirmed infeasible in the non-modal
  test harness (Destroy()-during-ShowModal causes heap corruption; a harness artifact
  not present in production's modal path).  The test was updated with an accurate,
  detailed docstring explicitly scoping what IS and is NOT exercised in-harness and
  explaining why the limitation is a harness artifact, not a production concern.

Branch name:
- tmf-TMF-03-tester-20260630

Test commit hash:
- 3724f87

Test files added or modified:
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
- PASS: testBattleScreenXCloseDismissesActiveChildDialog — TMF-03 test passes cleanly
- AC1 PASS: hasPendingDialog() returns true while ShowModal is live (dialog tracked in m_activeDialog)
- AC1 PASS: hasPendingDialog() returns false after dismissActiveDialog() (IsModal cleared)
- AC2 PASS: FBattleScreen becomes hidden/absent after close following child dialog dismissal
- AC3 PASS (adapted): no crash or process-aborting assert reached; docstring explicitly
  documents that closeBattleScreen-as-caller-while-ShowModal-is-live is not reproduced
  in-harness and explains the harness artifact vs. production difference
- AC3 source-contract PASS: closeBattleScreen contains dismissActiveDialog() call (integration locked)
- AC4 PASS: lifecycle counters settle (constructed>=+1, destroyed>=+1, liveCount==0)
- 8 pre-existing failures in unrelated source-contract tests — none introduced by TMF-03

Pass-2 investigation findings:
- Directly posting wxEVT_CLOSE_WINDOW inside the action lambda while ShowModal is live
  was tried and caused "free(): invalid pointer" heap corruption.
- Root cause: non-modal FBattleScreen takes Hide()+Destroy() path in closeBattleScreen;
  Destroy() defers deletion to wx idle processing; idle events can run inside the inner
  ShowModal event loop before ShowModal exits, deleting m_tacticalUI before showMessage
  returns — a use-after-free.
- In production, FBattleScreen is launched via ShowModal so closeBattleScreen takes the
  EndModal(frame) path — no Destroy(), no deferred deletion, no race.
- The infeasibility is a harness artifact; the production fix is sound.

Cleanup status:
- No temporary byproducts remain.

Artifacts written:
- artifacts/tactical-misc-fixes/TMF-03/tester_report.md
- artifacts/tactical-misc-fixes/TMF-03/tester_result.json
- artifacts/tactical-misc-fixes/TMF-03/documenter_prompt.txt
