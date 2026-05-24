Reviewer Report

Agent activation:
- Requested agent: reviewer
- Reviewer instructions loaded from `myteam get role reviewer`
- Governing workflow obligations followed: feature-level plan review, cross-subtask artifact review, read-only repository review except reviewer artifacts, final artifact commit.

Feature plan reviewed:
- `plans/tactical-battle-screen-close-path-fixes-plan-v2.md`

Review scope restatement:
- Feature-level review of the merged tactical battle-screen close-path repair across `TBCP-001` and `TBCP-002` on branch `coord-tactical-battle-board-menu-bar-plan-v2-20260522` (review worktree branch `coord-tactical-battle-screen-close-path-fixes-plan-v2-reviewer-20260522`).
- Reviewer artifact directory: `artifacts/tactical-battle-screen-close-path-fixes-plan-v2`
- Review remained read-only for repository content under review.

Inputs reviewed:
- Subtask artifact directories:
  - `artifacts/tactical-battle-screen-close-path-fixes-plan-v2/tbcp-001`
  - `artifacts/tactical-battle-screen-close-path-fixes-plan-v2/tbcp-002`
- Governing and documentation inputs:
  - `plans/tactical-battle-screen-close-path-fixes-plan-v2.md`
  - `AGENTS.md`
  - `doc/DesignNotes.md`
- Delivered implementation/test surfaces:
  - `include/tactical/FBattleScreen.h`
  - `src/tactical/FBattleScreen.cpp`
  - `include/tactical/FTacticalGame.h`
  - `src/tactical/FTacticalGame.cpp`
  - `tests/gui/TacticalGuiLiveTest.h`
  - `tests/gui/TacticalGuiLiveTest.cpp`
  - `tests/gui/WXGuiTestHarness.h`
  - `tests/gui/WXGuiTestHarness.cpp`
  - `tests/gui/BattleSimGuiLiveTest.h`
  - `tests/gui/BattleSimGuiLiveTest.cpp`
  - `tests/tactical/FTacticalBattleScreenElectricalFireTest.cpp`
  - `src/gui/SelectCombatGUI.cpp`
  - `src/battleSim/ScenarioDialog.cpp`
  - `src/battleSim/ScenarioEditorGUI.cpp`
- Validation performed during review:
  - `cd tests && make tactical-tests && ./tactical/TacticalTests` (pass)
  - `cd tests/gui && make && xvfb-run -a ./GuiTests` (pass)

Overall feature completeness:
- The merged implementation satisfies the core runtime goals: `File -> Quit` and the title-bar close vector both route through `FBattleScreen::closeBattleScreen(...)`, non-modal battle screens close without terminating the application, and the close logic remains centralized.
- The automated tactical GUI regressions now drive the real non-modal close vectors and use `WXGuiTestHarness::waitForTopLevelWindowClosed(...)` so the suite no longer depends on manual operator cleanup for the previously stuck battle-screen cases.
- Documentation in `FBattleScreen.h`, `doc/DesignNotes.md`, and `AGENTS.md` is consistent with the delivered shared close-path and harness behavior.

Cross-subtask integration assessment:
- `TBCP-001` repaired the shared `FBattleScreen` lifecycle and preserved modal caller shapes (`bb.ShowModal()`) across strategic and BattleSim launch sites.
- `TBCP-002` updated the tactical live coverage to post the real non-modal quit and close events, added the bounded async-close wait seam, and relaxed source-contract checks so they protect intended invariants instead of the broken sequencing.
- Combined, the subtasks close the original non-modal regression and remove the need for manual closure in the tactical GUI suite.

Edge cases and risk coverage:
- Reviewed for mismatches between non-modal and modal caller behavior, close-guard reset handling, harness false positives, launch-path ownership mismatches, and documentation omissions.
- No blocking runtime defect was found in the merged implementation.
- One remaining proof gap exists at the feature level: automated modal coverage demonstrates BattleSim modal unwind through `File -> Quit`, but does not exercise the modal title-bar `X` vector end-to-end even though the plan required modal launch paths to unwind correctly for both real close vectors.

Findings

BLOCKING
- None.

WARNING
- The feature lacks end-to-end automated proof that a stack-owned modal `FBattleScreen::ShowModal()` caller unwinds correctly when the user closes the battle screen via the title-bar `X`. The implementation likely works because `onClose(wxCloseEvent &)` delegates to the same shared `closeBattleScreen(GetReturnCode())` helper used by `File -> Quit`, and modal `EndModal(...)` hides the frame and exits the shim event loop in `src/tactical/FBattleScreen.cpp:217-230,760-769`. However, the delivered live modal regression only covers `File -> Quit` from `ScenarioDialog` (`tests/gui/BattleSimGuiLiveTest.cpp:612-656`), while the modal title-bar vector is only exercised on a non-modal battle screen (`tests/gui/TacticalGuiLiveTest.cpp:699-721`). This leaves the plan's modal-both-vectors acceptance criterion partially unproven.

NOTE
- The non-modal close-path coverage is strong: `tests/gui/TacticalGuiLiveTest.cpp:668-721` posts the real `ID_TacticalQuit` and a real `wxEVT_CLOSE_WINDOW`, then waits for asynchronous shutdown through `WXGuiTestHarness::waitForTopLevelWindowClosed(...)` (`tests/gui/WXGuiTestHarness.cpp:226-249`).
- Source-contract coverage appropriately protects the centralized close-path invariants without overfreezing incidental event ordering (`tests/tactical/FTacticalBattleScreenElectricalFireTest.cpp:109-143,232-265`).

Follow-up feature requests for planning:
- Add a live GUI regression that launches a stack-owned modal `FBattleScreen` from an existing modal caller (for example `ScenarioDialog`, `ScenarioEditorGUI`, or `SelectCombatGUI`) and proves a posted title-bar `wxEVT_CLOSE_WINDOW` unwinds the modal caller without fallback/manual cleanup.

Final outcome:
- CONDITIONAL PASS
