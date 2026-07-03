# Tester Report

Status:
- success

Task summary:
- Validated the FBattleScreen modality rework (drop GTK gtk_window_set_modal() grab in favor of wxWindowDisabler, generalize the GTK activate bypass to a delete-event bypass for the title-bar X, and convert WXTacticalUI's single tracked dialog pointer into an innermost-first m_dialogStack) by retiring the now-obsolete TMF-02 AC2 source-contract test and adding behavioral coverage for the parts of the change that can be safely driven in the headless harness.

Branch name:
- fixes-tmfr-01-tester-20260703

Test commit hash:
- d49f4fcc0e238e58c74c2cc1727cc7858a84adfa

Test files added or modified:
- tests/gui/TacticalGuiLiveTest.h
- tests/gui/TacticalGuiLiveTest.cpp
- tests/tactical/WXTacticalUIAdapterTest.h
- tests/tactical/WXTacticalUIAdapterTest.cpp
- tests/tactical/FTacticalGameMockUITest.h
- tests/tactical/FTacticalGameMockUITest.cpp

Commands run:
- cd tests/gui && make && ./GuiTests
- cd tests && make tactical-tests && ./tactical/TacticalTests
- cd tests && make && ./SSWTests
- make (repo root)

Pass/fail totals:
- GuiTests_fail_pre_existing: 1
- GuiTests_pass: 66
- GuiTests_total: 67
- SSWTests_fail: 0
- SSWTests_pass: 195
- SSWTests_total: 195
- TacticalTests_fail: 0
- TacticalTests_pass: 251
- TacticalTests_total: 251

Unmet acceptance criteria:
- None

Final test outcomes:
- cd tests/gui && make && ./GuiTests -> 67 tests, 1 pre-existing failure (testLocalGameDialogLaunchesPredefinedAndCustomModalChains, confirmed flaky/pre-existing and unrelated to TMFR-01 files per the implementer report; reproduced as flaky across repeated runs -- sometimes passes, sometimes fails with the same 'centered' assertion, both before and independent of this subtask's test changes)
- cd tests && make tactical-tests && ./tactical/TacticalTests -> 251 tests, 0 failures, stable across 3 repeated runs
- cd tests && make && ./SSWTests -> 195 tests, 0 failures, stable across 2 repeated runs
- make (repo root) -> full SSW + BattleSim executable and test-library build succeeds with no errors

Cleanup status:
- No temporary non-handoff byproducts remain in the worktree: a debug argv-filtering change temporarily added to tests/gui/GuiTests.cpp to isolate a hang was fully reverted before committing (git diff on that file is empty).
- Standalone throwaway reproduction programs used to diagnose wxWidgets 3.3.2 IsBeingDeleted()/Destroy() timing were written only under the session scratchpad directory, never under the repository, and are not part of this commit.

Artifacts written:
- artifacts/tactical-misc-fixes-round2/TMFR-01/tester_report.md
- artifacts/tactical-misc-fixes-round2/TMFR-01/tester_result.json
- artifacts/tactical-misc-fixes-round2/TMFR-01/documenter_prompt.txt
