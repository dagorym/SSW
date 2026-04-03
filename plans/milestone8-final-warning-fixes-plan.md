# Milestone 8 Final Warning Fixes Plan

## Feature Restatement
Address the two remaining warning-level follow-ons from `artifacts/milestone8-remediation/reviewer_report.md`: remove the residual `FBattleScreen::setPhase(PH_FINALIZE_MOVE)` screen-side finalize branch so `FTacticalGame::completeMovePhase()` is the only post-move completion seam, and add direct tactical regression coverage for the selected-ship cleanup path inside `FBattleScreen::clearDestroyedShips()`.

## Confirmed Repository Facts
- The reviewer identified the two remaining warning items in `/home/tstephen/repos/SSW/artifacts/milestone8-remediation/reviewer_report.md`.
- `FTacticalGame::setPhase(int)` already treats `PH_FINALIZE_MOVE` as a canonical route into `completeMovePhase()` in `/home/tstephen/repos/SSW/src/tactical/FTacticalGame.cpp`.
- `FBattleDisplay::onMoveDone()` already routes the live wx callback through `FBattleScreen::completeMovePhase()` in `/home/tstephen/repos/SSW/src/tactical/FBattleDisplay.cpp`.
- `FBattleScreen::setPhase(int)` still contains a legacy `PH_FINALIZE_MOVE` branch that calls `m_tacticalGame->finalizeMovementState();` and `setShip(NULL);` in `/home/tstephen/repos/SSW/src/tactical/FBattleScreen.cpp`.
- `FTacticalBattleScreenDelegationTest` currently asserts that legacy `finalizeMovementState()` branch as expected behavior in `/home/tstephen/repos/SSW/tests/tactical/FTacticalBattleScreenDelegationTest.cpp`.
- `FBattleScreen::clearDestroyedShips()` already clears the selected ship when its ID appears in `getLastDestroyedShipIDs()`, redraws, clears bookkeeping once via `clearLastDestroyedShipIDs()`, and then checks winner state in `/home/tstephen/repos/SSW/src/tactical/FBattleScreen.cpp`.
- Existing tactical tests around `FBattleScreen` are currently source-inspection heavy, while `FTacticalBattleDisplayFireFlowTest` and `FTacticalMineDamageFlowTest` provide behavior-level tactical runtime coverage.

## Assumptions
- The desired end state is not a new tactical API; it is a cleanup of the remaining screen-side legacy branch so all `PH_FINALIZE_MOVE` semantics stay model-owned.
- Direct coverage for the selected-ship cleanup branch may require either a narrow non-wx-testable seam around `FBattleScreen::clearDestroyedShips()` or a minimal headless screen harness. The plan assumes implementers should prefer the smaller option that avoids introducing GUI-runtime fragility into tactical tests.
- `doc/DesignNotes.md` already describes the canonical move-completion seam correctly; documentation work here is expected to be limited to keeping milestone wording and in-source seam comments aligned if implementation details change.

## Files To Modify
- `/home/tstephen/repos/SSW/src/tactical/FBattleScreen.cpp`
- `/home/tstephen/repos/SSW/tests/tactical/FTacticalBattleScreenDelegationTest.cpp`
- `/home/tstephen/repos/SSW/tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`
- Likely one or more new focused tactical test files under `/home/tstephen/repos/SSW/tests/tactical/` if direct cleanup-branch coverage is cleaner than extending existing suites
- `/home/tstephen/repos/SSW/tests/tactical/Makefile` if a new test file is added
- `/home/tstephen/repos/SSW/tests/tactical/TacticalTests.cpp` if a new test suite is added
- Likely `/home/tstephen/repos/SSW/tests/SSWTests.cpp` only if the new tactical suite must also be registered in the aggregate runner
- Review `/home/tstephen/repos/SSW/doc/DesignNotes.md` for wording drift; update only if the final seam description changes

## Overall Documentation Impact
- Review Milestone 8 remediation wording in `/home/tstephen/repos/SSW/doc/DesignNotes.md` after the code and tests land to confirm it still describes `FTacticalGame::completeMovePhase()` as the only canonical post-move seam.
- Review the in-source remediation comments in `/home/tstephen/repos/SSW/src/tactical/FBattleScreen.cpp` and `/home/tstephen/repos/SSW/include/tactical/FTacticalGame.h` so they match the final seam contract and the new cleanup coverage.
- No user-facing documentation update is expected.

## Subtasks

### 1. Remove the residual `PH_FINALIZE_MOVE` screen-side finalize branch
Description:
Simplify `FBattleScreen::setPhase(int)` so it no longer duplicates post-move finalization logic on the wx side. The screen should continue to delegate phase ownership to `FTacticalGame`, retain the `PH_MOVE` redraw/reset behavior only where still needed, and stop encoding `PH_FINALIZE_MOVE` as a special screen-local mechanics path.

Acceptance Criteria:
- `FBattleScreen::setPhase(int)` no longer contains a `PH_FINALIZE_MOVE` branch that calls `m_tacticalGame->finalizeMovementState();`.
- `FBattleScreen::setPhase(int)` does not perform any screen-local post-move mechanics that duplicate `FTacticalGame::completeMovePhase()`.
- Existing live move completion still runs through `FBattleDisplay::onMoveDone()` -> `FBattleScreen::completeMovePhase()` -> `FTacticalGame::completeMovePhase()`.
- `FTacticalBattleScreenDelegationTest` is updated so it no longer locks the removed `finalizeMovementState()` branch in as correct behavior.
- Tactical assertions continue to protect the canonical model-owned `PH_FINALIZE_MOVE` contract without depending on the removed screen-side branch.

Documentation Impact:
- Review milestone and seam comments for references that imply `FBattleScreen::setPhase(PH_FINALIZE_MOVE)` still performs post-move finalization work.
- No permanent documentation update expected if current wording already matches the unified contract.

### 2. Add direct regression coverage for selected-ship destroyed cleanup
Description:
Strengthen tactical coverage around `FBattleScreen::clearDestroyedShips()` so the selected-ship cleanup branch is directly protected. The added coverage should specifically pin the behavior when the currently selected ship appears in `getLastDestroyedShipIDs()`, including selection clearing, redraw behavior, single bookkeeping clear, and winner handling across the delegated fire-cleanup seam.

Acceptance Criteria:
- Tactical coverage directly fails if `FBattleScreen::clearDestroyedShips()` stops clearing the selected ship when its ID is present in the destroyed-ship list.
- The added test(s) directly cover redraw behavior for the cleanup path rather than only asserting the presence of the surrounding method body.
- The added test(s) protect the single canonical bookkeeping clear point by failing if `clearLastDestroyedShipIDs()` is skipped or called more than once in the cleanup lifecycle.
- The added test(s) cover winner handling after cleanup so a destroyed selected ship does not bypass the winner notification path.
- If a new seam or helper is introduced solely to make this branch testable, it preserves the existing ownership split: model captures destroyed IDs, `FBattleScreen` performs wx-side cleanup orchestration, and the model bookkeeping clear still happens exactly once after consumption.

Documentation Impact:
- Update internal test comments and any in-source cleanup-contract comments so they mention the newly covered selected-ship branch explicitly.
- No external documentation update expected.

### 3. Wire any new tactical test suite, validate the warning follow-ons, and confirm doc alignment
Description:
Finalize the follow-on work by wiring any new tactical test file into the tactical module build, rerunning the focused tactical test target, and confirming the permanent milestone docs still match the unified seam contract after the code/test changes.

Acceptance Criteria:
- `tests/tactical/Makefile` and `tests/tactical/TacticalTests.cpp` are updated if a new tactical suite is introduced.
- The focused tactical validation needed for these follow-ons passes, including the updated delegation assertions and the new cleanup-branch regression coverage.
- Validation notes clearly identify which tactical test target(s) were rerun.
- `doc/DesignNotes.md` is reviewed and updated only if its Milestone 8 remediation narrative drifted from the final code/test contract.

Documentation Impact:
- Record validation scope/results in implementation artifacts or handoff notes.
- Permanent docs should change only if the code/test work changes the wording needed to describe the seam accurately.

## Dependency Ordering
1. Subtask 1 should complete first because the legacy `PH_FINALIZE_MOVE` branch needs to be removed before the delegation tests can be rewritten around the final intended contract.
2. Subtask 2 depends on the intended cleanup contract staying stable, but it can proceed in parallel with the tail end of Subtask 1 once the screen-side phase contract is settled.
3. Subtask 3 depends on Subtasks 1 and 2.

## Parallelization Notes
- The code change in `FBattleScreen::setPhase(int)` and the new selected-ship cleanup coverage can be split across two implementers if they coordinate on any shared edits to `FTacticalBattleScreenDelegationTest.cpp`.
- If the cleanup-branch coverage needs a new dedicated test file, one implementer can own test harness creation while another updates the existing delegation assertions.

## Implementer Agent Prompts

### Subtask 1 Prompt
You are the implementer agent.

Allowed files:
- `/home/tstephen/repos/SSW/src/tactical/FBattleScreen.cpp`
- `/home/tstephen/repos/SSW/tests/tactical/FTacticalBattleScreenDelegationTest.cpp`
- `/home/tstephen/repos/SSW/tests/tactical/FTacticalMineDamageFlowTest.cpp`
- `/home/tstephen/repos/SSW/doc/DesignNotes.md`

Task:
Remove the residual `PH_FINALIZE_MOVE` screen-side finalize branch from `FBattleScreen::setPhase(int)` so `FTacticalGame::completeMovePhase()` remains the only post-move completion seam. Update the tactical delegation assertions to reflect the unified contract and refresh milestone wording only if it no longer matches the final seam description.

Acceptance criteria:
- `FBattleScreen::setPhase(int)` no longer calls `m_tacticalGame->finalizeMovementState();` for `PH_FINALIZE_MOVE`.
- The screen no longer performs any duplicate post-move mechanics when `PH_FINALIZE_MOVE` is requested.
- `FTacticalBattleScreenDelegationTest` no longer asserts the removed legacy branch as expected behavior.
- Canonical `PH_FINALIZE_MOVE` routing remains protected by tactical tests.
- `doc/DesignNotes.md` is updated only if needed to stay accurate.

### Subtask 2 Prompt
You are the implementer agent.

Allowed files:
- `/home/tstephen/repos/SSW/src/tactical/FBattleScreen.cpp`
- `/home/tstephen/repos/SSW/tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`
- `/home/tstephen/repos/SSW/tests/tactical/FTacticalBattleScreenDelegationTest.cpp`
- New or updated focused test files under `/home/tstephen/repos/SSW/tests/tactical/`
- `/home/tstephen/repos/SSW/tests/tactical/Makefile`
- `/home/tstephen/repos/SSW/tests/tactical/TacticalTests.cpp`
- `/home/tstephen/repos/SSW/tests/SSWTests.cpp`

Task:
Add direct tactical regression coverage for the selected-ship cleanup branch in `FBattleScreen::clearDestroyedShips()`. The coverage must fail if a destroyed selected ship is not cleared from UI state, if redraw behavior regresses, if destroyed-ID bookkeeping is not cleared exactly once after consumption, or if winner handling is skipped after cleanup. Introduce the smallest test seam needed if the current wx-heavy class shape prevents direct behavior coverage.

Acceptance criteria:
- Tactical tests directly protect the selected-ship cleanup branch rather than only source-inspecting the surrounding method.
- The tests verify selection clearing, redraw behavior, single bookkeeping clear, and winner handling in the delegated fire-cleanup lifecycle.
- Any new helper or seam preserves the existing model/wx ownership contract.
- Test wiring is updated for any new tactical suite that gets introduced.

### Subtask 3 Prompt
You are the implementer agent.

Allowed files:
- `/home/tstephen/repos/SSW/tests/tactical/Makefile`
- `/home/tstephen/repos/SSW/tests/tactical/TacticalTests.cpp`
- `/home/tstephen/repos/SSW/tests/SSWTests.cpp`
- `/home/tstephen/repos/SSW/doc/DesignNotes.md`
- Implementation notes or validation artifacts under `/home/tstephen/repos/SSW/artifacts/`

Task:
Run and document the focused validation for the final Milestone 8 warning follow-ons, wire any new tactical suite into the tactical runners, and confirm the permanent milestone docs still match the final seam contract.

Acceptance criteria:
- All new or updated tactical tests for these warning follow-ons are wired into the appropriate runners.
- Focused tactical validation passes and the executed targets are documented.
- `doc/DesignNotes.md` remains aligned with the final code/test contract, with updates made only if necessary.

## Output Artifact Path
- `/home/tstephen/repos/SSW/plans/milestone8-final-warning-fixes-plan.md`
