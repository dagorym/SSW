Reviewer Report

Feature plan reviewed:
- `plans/milestone8-battle-display-board-rendering-plan.md`

Inputs reviewed:
- Subtask artifacts under `artifacts/milestone8/m8-subtask1` through `artifacts/milestone8/m8-subtask5`
- Milestone design notes in `doc/DesignNotes.md`
- Merged implementation and test files on `gui_sep`
- Verifier outputs for Subtasks 2, 3, and 5

Overall feature completeness:
- Milestone 8 is not complete at the feature level.
- Subtasks 1 and 4 delivered the intended model API and test-suite reshaping, and Subtask 5 correctly documented the accepted non-interactive validation basis.
- However, the merged runtime tactical flow still fails two core plan requirements from Subtasks 2 and 3: delegated fire cleanup does not reliably synchronize destroyed ships back to the wx map, and live move completion still bypasses the model-owned `completeMovePhase()` path.
- Because those regressions affect actual tactical gameplay, the milestone should not be treated as complete despite passing tactical tests and accepted Subtask 5 automation.

Findings

BLOCKING
- Fire-phase destroyed-ship cleanup remains broken in the merged runtime path. `FTacticalGame::resolveCurrentFirePhase()` clears destroyed ships internally (`src/tactical/FTacticalGame.cpp:445-448`), but `FBattleDisplay::onDefensiveFireDone()` / `onOffensiveFireDone()` still call `FBattleScreen::clearDestroyedShips()` afterward (`src/tactical/FBattleDisplay.cpp:541-555`). `FBattleScreen::clearDestroyedShips()` invokes `FTacticalGame::clearDestroyedShips()` again (`src/tactical/FBattleScreen.cpp:463-468`), which starts by clearing `m_lastDestroyedShipIDs` (`src/tactical/FTacticalGame.cpp:452-478`). That leaves the screen-side wx cleanup without the destroyed-ship ID cache it needs, matching the Subtask 2 verifier FAIL and violating the plan requirement that delegated fire resolution remain operational.
- Live move completion still bypasses the model-owned move-resolution flow required by the plan. `FBattleDisplay::onMoveDone()` still calls `m_parent->setPhase(PH_FINALIZE_MOVE)` (`src/tactical/FBattleDisplay.cpp:491-498`), and `FTacticalGame::setPhase(PH_FINALIZE_MOVE)` still only runs `finalizeMovementState()` and advances phases (`src/tactical/FTacticalGame.cpp:270-282`). The authoritative move-resolution logic remains in `FTacticalGame::completeMovePhase()` (`src/tactical/FTacticalGame.cpp:1437-1485`), but the live UI path does not call it. This leaves mine checks, ship relocation, off-board destruction, and selected-ship cleanup off the shipped runtime path, matching the Subtask 3 verifier FAIL and missing a central Milestone 8 acceptance criterion.

WARNING
- `doc/DesignNotes.md` overstates Milestone 8 remediation status. It claims Subtask 2 remediation preserved destroyed-ship cleanup semantics (`doc/DesignNotes.md:449-452`) and also states the live move-completion callback now routes through `FTacticalGame::completeMovePhase()` (`doc/DesignNotes.md:388-394`), but the merged code still shows both runtime regressions above. This makes the milestone documentation internally inconsistent with the implementation and verifier artifacts.
- The tactical test suite remains insufficient for feature-level signoff. Subtask 4/5 validation proves the source-inspection-heavy suite passes, but the Subtask 2 and 3 verifier artifacts already show that those tests did not exercise the live wx callback paths where the regressions remain. That means Milestone 8’s architectural tests are aligned with the intended structure, yet still do not validate the full runtime behavior the plan required.

NOTE
- The lack of an interactive GUI playthrough in Subtask 5 is not a milestone-review finding by itself. The accepted scope exception is documented consistently in `artifacts/milestone8/m8-subtask5/verifier_report.md` and `doc/DesignNotes.md:524-545`, so this review treats the missing manual run as an accepted limitation rather than a new issue.

Missed functionality / edge cases:
- Destroyed ships are still at risk of remaining visible/on-board after delegated fire resolution because the ID handoff between model cleanup and wx cleanup is not preserved through the live callback sequence.
- Movement completion still does not guarantee mine detonation, off-board destruction, or final occupancy updates on the runtime UI path, so the board/model split is incomplete where it matters most: post-move resolution.
- Because both defects survive the passing tactical suite, the milestone still lacks runtime coverage for the most important wx-to-model transition seams introduced by Milestones 7-8.

Follow-up feature requests for planning:
- Update the delegated fire-resolution cleanup path so model-side destroyed-ship bookkeeping and wx map removal happen exactly once in a consistent order, with automated coverage that validates destroyed ships are removed from both model state and the live board after defensive and offensive fire.
- Rewire the live move-completion callback path to execute `FTacticalGame::completeMovePhase()` as the canonical resolution step, and add runtime-oriented tests that cover movement finalization, mine checks/damage, off-board destruction, and board occupancy updates.
- Reconcile Milestone 8 design notes and milestone artifacts with the actual shipped runtime behavior so documentation does not claim remediations or validation outcomes that the merged code has not yet achieved.

Final outcome:
- FAIL
