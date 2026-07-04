Verifier Report

Scope reviewed:
- Implementer commit 57cf70b0: src/battleSim/ScenarioEditorGUI.cpp (onStartBattle) and src/battleSim/ScenarioDialog.cpp (onScenario1..onScenario4) - removed the premature Hide() before bb.ShowModal() (and the trailing Show() in ScenarioDialog) that caused a wxGTK double-EndModal assert (dialog.cpp:193).
- Tester commit f3bb588: added two behavioral tests in tests/gui/BattleSimGuiLiveTest.cpp/.h (testScenarioEditorStartBattleDoesNotHideDialogAndFinalizesOnce, testScenarioDialogScenario1DoesNotHideDialogAndRemainsReplayable) driving the real onStartBattle/onScenario1 handlers, plus a reconciled source-contract assertion in tests/tactical/FTacticalBattleScreenElectricalFireTest.cpp (0x Hide(), 1x bb.ShowModal(), 0x Show() per onScenarioN body).
- Documenter commit afecbbd: added a SEB-01 entry to doc/test-contracts.md describing the fix, the new tests, the reconciled source-contract test, and the deferred required user manual-verification pass.
- Verified no other production files changed: FBattleScreen, LocalGameDialog, BattleSimFrame, and include/battleSim/ScenarioEditorGUI.h (finalizeStartBattle) are byte-for-byte unchanged across the full SEB-01 change range (079034f..afecbbd).

Acceptance criteria / plan reference:
- plans/scenario-editor-startbattle-endmodal-fix-plan.md, subtask SEB-01
- Root cause: wxGTK Show(false) on a live modal dialog implicitly calls EndModal(wxID_CANCEL); the later explicit EndModal(0) in finalizeStartBattle() then double-ended the modal loop, tripping src/gtk/dialog.cpp:193.

Convention files considered:
- AGENTS.md (Behavioral Verification Is Mandatory policy; module boundary rules; Doxygen comment requirements)
- CLAUDE.md (pointer to AGENTS.md)
- doc/rules/tactical_operations_manual.md (must remain unchanged - confirmed 0 diff across the full change range)
- doc/test-contracts.md (tactical/battleSim test source-contract catalog, updated by the Documenter)

Findings

BLOCKING
- None

WARNING
- None

NOTE
- src/battleSim/ScenarioDialog.cpp:209,255,339 - Only onScenario1 has a dedicated new behavioral test; onScenario2/3/4 rely on the reconciled source-contract test (0x Hide/Show, 1x ShowModal) for coverage of the identical Hide()/Show() removal pattern.
  The three sibling handlers are mechanically identical edits to onScenario1 (same diff shape, confirmed by reading the diff), so the risk of a behavioral regression specific to one of them and not the others is very low; the behavioral test on onScenario1 plus the structural supplement on all four bodies is a reasonable, proportionate combination per the plan's own scoping of the required behavioral coverage. Not blocking.

Test sufficiency assessment:
- The two new BattleSimGuiLiveTest tests are genuinely behavioral: they construct real ScenarioEditorGUI/ScenarioDialog test-peer objects, click the real button handlers (clickStartBattle()/clickScenario1()), and assert on live runtime state (dialog->IsShown() while the nested FBattleScreen top-level window is present, dialog->getFinalizeCallCount()==1, and FBattleScreen construct/destroy/live-instance counters) rather than on source text.
- Read the pre-fix code path mentally against the discriminating assertion: with the removed Hide() restored, dialog->IsShown() would be false at the moment the nested FBattleScreen's top-level window appears (Hide() sets shown=false synchronously before ShowModal() is entered), so testScenarioEditorStartBattleDoesNotHideDialogAndFinalizesOnce would fail against unfixed code, and testScenarioDialogScenario1DoesNotHideDialogAndRemainsReplayable would likewise observe dialogShownDuringFirstBattleScreen==false. Both tests satisfy the repository's Behavioral Verification policy (AGENTS.md).
- FTacticalBattleScreenElectricalFireTest::testBattleSimLaunchPathUsesModalDialogFlowIntoBattleScreen was correctly reconciled to assert 0x 'Hide();', 1x 'bb.ShowModal();', 0x 'Show();' per onScenarioN body, matching the shipped diff exactly (verified by reading both the diff and the current test source); this is used only as a structural supplement alongside the behavioral coverage, consistent with policy.
- Independently rebuilt from a clean worktree state and reran all required suites: GuiTests 67/67 passing (one run showed a transient 'centered' assertion flake on testLocalGameDialogLaunchesPredefinedAndCustomModalChains, which is the documented pre-existing flake unrelated to SEB-01; an immediate rerun passed clean at 67/67), TacticalTests 253/253 passing, SSWTests 198/198 passing (one run showed a transient, unrelated flake in FFleetTest::testDecTransitTime outside the SEB-01 change surface; an immediate rerun passed clean at 198/198). Counts match the prior baselines (GUI 67, Tactical 253, SSW 198).

Documentation accuracy assessment:
- doc/test-contracts.md's new SEB-01 entry accurately describes the removed Hide()/Show() calls, the exact discriminating behavioral assertions in the two new BattleSimGuiLiveTest tests, the reconciled source-contract counts (0x Hide, 1x ShowModal, 0x Show), and explicitly and correctly defers the end-to-end no-assert runtime verification to a REQUIRED user manual-verification pass, matching the task's explicit scope constraint that this end-to-end path is unsafe to drive in the headless harness.
- doc/rules/tactical_operations_manual.md is confirmed unchanged (0 diff) across the full SEB-01 change range, satisfying the non-negotiable constraint in AGENTS.md.
- No other documentation files were touched or needed updates; AGENTS.md and doc/UsersGuide.md do not document BattleSim scenario-launch modal topology, so there was nothing to reconcile there, as the Documenter's commit message correctly notes.

Artifacts written:
- artifacts/scenario-editor-startbattle-endmodal-fix/SEB-01/verifier_report.md
- artifacts/scenario-editor-startbattle-endmodal-fix/SEB-01/verifier_result.json

Verdict:
- PASS
