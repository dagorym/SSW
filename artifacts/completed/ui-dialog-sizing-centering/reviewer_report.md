Reviewer Report

Review setup:
- Requested agent: reviewer
- Repository-local reviewer definition: not found under the assigned worktree
- Shared reviewer definition used: /home/tstephen/repos/agents/agents/reviewer.md
- Precedence decision: shared definition won because no repository-local reviewer definition was present
- Assigned worktree / branch confirmed: /home/tstephen/repos/SSW-worktrees/coord-ui-dialog-sizing-centering-reviewer-20260415 on coord-ui-dialog-sizing-centering-reviewer-20260415
- Review mode: feature-level, read-only for repository files except reviewer artifacts

Feature plan reviewed:
- plans/ui-dialog-sizing-centering-plan.md

Inputs reviewed:
- Merged feature branch coord-ui-dialog-sizing-centering at 73188e6
- Subtask artifacts:
  - artifacts/ui-dialog-sizing-centering/subtask-1-spin-controls/
  - artifacts/ui-dialog-sizing-centering/subtask-2-battlesim-frame/
  - artifacts/ui-dialog-sizing-centering/subtask-3-known-dialogs/
  - artifacts/ui-dialog-sizing-centering/subtask-4-dialog-audit/
  - artifacts/ui-dialog-sizing-centering/subtask-5-centering/
- Implementation/test/doc files touched by the merged feature, including BattleSim dialogs/frame, tactical/strategic UI adapters, GUI dialog constructors, GUI/tactical regression tests, AGENTS.md, doc/DesignNotes.md, and doc/UsersGuide.md
- Verifier reports and result JSON files for all five subtasks

Overall feature completeness:
- The merged feature substantially delivers the planned wxGTK sizing and centering cleanup: spin controls now use best-size-backed minimum sizing, the BattleSim frame uses content-driven default sizing, known BattleSim dialogs and audited strategic dialogs now lock first-show size and centering, adapter-launched dialogs apply parent-relative or screen fallback centering, and GUI/documentation coverage was updated across the feature.
- However, one cross-subtask regression remains: the merged ScenarioDialog launch path no longer preserves the BattleSim modal hide/show lifecycle around tactical battle launches, which breaks an existing tactical regression and conflicts with the plan's requirement to preserve existing modal ownership/lifecycle behavior while fixing dialog sizing.

Findings

BLOCKING
- ScenarioDialog no longer preserves the existing modal hide/show lifecycle for the four tactical scenario launches. The merged code now calls bb.ShowModal() directly in each scenario handler without surrounding Hide(); / Show(); calls (src/battleSim/ScenarioDialog.cpp:129-131, src/battleSim/ScenarioDialog.cpp:202-204, src/battleSim/ScenarioDialog.cpp:246-248, src/battleSim/ScenarioDialog.cpp:329-330). The existing tactical regression still requires four Hide(); and four Show(); occurrences for this flow (tests/tactical/FTacticalBattleScreenElectricalFireTest.cpp:159-185), and an independent reviewer rerun of cd tests/tactical && make && ./TacticalTests failed 1/92 at FTacticalBattleScreenElectricalFireTest::testBattleSimLaunchPathUsesModalDialogFlowIntoBattleScreen. This matters at the feature level because Subtask 3 explicitly required preserving modal ownership/lifecycle behavior while remediating sizing issues.

WARNING
- None.

NOTE
- BattleSimFrame now centers itself unconditionally after layout, so any future caller that passes an explicit non-default constructor position will still receive centered placement. This does not block the current plan because the shipped launch path uses the default centered behavior, but it is a small interface-semantic change worth keeping in mind for future callers (artifacts/ui-dialog-sizing-centering/subtask-2-battlesim-frame/verifier_report.md:32-38).

Missed functionality / edge cases:
- No additional plan-level sizing or centering gaps were confirmed beyond the blocking BattleSim modal-lifecycle regression above.
- Representative GUI coverage for parent-backed and parentless centering paths is present, and the documentation updates broadly match the shipped implementation.

Follow-up feature requests for planning:
- Restore and revalidate the BattleSim scenario modal lifecycle while keeping the new sizing/centering behavior. Specifically, update the four ScenarioDialog tactical launch handlers so the intended parent hide/show modal-flow contract is preserved (or consistently replace that contract and its tactical assertions everywhere), then rerun cd tests/tactical && make && ./TacticalTests and cd tests/gui && make && ./GuiTests, and update any affected documentation/artifacts.

Validation performed:
- cd tests/gui && make && ./GuiTests -> OK (29 tests)
- cd tests/tactical && make && ./TacticalTests -> FAIL (Run: 92   Failures: 1   Errors: 0), failing at FrontierTests::FTacticalBattleScreenElectricalFireTest::testBattleSimLaunchPathUsesModalDialogFlowIntoBattleScreen

Final outcome:
- CONDITIONAL PASS
