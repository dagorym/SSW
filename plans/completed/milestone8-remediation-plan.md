# Milestone 8 Remediation Plan: Runtime Tactical Delegation Fixes

## Feature Restatement
Address the post-review Milestone 8 regressions by fixing the two blocking runtime delegation bugs and the associated warning-level test and documentation gaps. The remediation should preserve the Milestone 8 ownership boundary while restoring correct destroyed-ship cleanup, routing live move completion through `FTacticalGame::completeMovePhase()`, and documenting the actual shipped behavior accurately.

## Confirmed Repository Facts
- The initial Milestone 8 plan is `/home/tstephen/repos/SSW/plans/milestone8-battle-display-board-rendering-plan.md`.
- The combined reviewer result is `/home/tstephen/repos/SSW/artifacts/gui_sep/milestone8/reviewer_result.json`.
- The fire-phase regression was documented in `/home/tstephen/repos/SSW/artifacts/gui_sep/milestone8/m8-subtask2/verifier_report.md`.
- The move-finalization regression was documented in `/home/tstephen/repos/SSW/artifacts/gui_sep/milestone8/m8-subtask3/verifier_result.json`.
- The reviewer identifies the blocking runtime seams in:
  - `src/tactical/FBattleDisplay.cpp`
  - `src/tactical/FBattleScreen.cpp`
  - `src/tactical/FTacticalGame.cpp`
- The reviewer identifies warning-level gaps in:
  - `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`
  - `tests/tactical/FTacticalBattleScreenDelegationTest.cpp`
  - `tests/tactical/FTacticalMineDamageFlowTest.cpp`
  - `doc/DesignNotes.md`

## Assumptions
- The remediation should be additive to the shipped Milestone 8 architecture rather than a rollback to pre-delegation behavior.
- `FBattleScreen` should remain the wx-side coordinator, but the model should remain authoritative for battle-resolution state transitions.
- The desired fire cleanup behavior is: resolve fire in `FTacticalGame`, preserve the destroyed-ship ID list long enough for wx-side map cleanup, then clear model bookkeeping exactly once.
- The desired move-completion behavior is: the live UI callback path must execute `FTacticalGame::completeMovePhase()` before any phase transition logic that depends on post-move resolution.
- Existing aggregate test-runner linker issues remain out of scope unless they directly block the new tactical tests.

## Files To Modify
- `/home/tstephen/repos/SSW/src/tactical/FBattleDisplay.cpp`
- `/home/tstephen/repos/SSW/src/tactical/FBattleScreen.cpp`
- `/home/tstephen/repos/SSW/include/tactical/FTacticalGame.h`
- `/home/tstephen/repos/SSW/src/tactical/FTacticalGame.cpp`
- `/home/tstephen/repos/SSW/tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`
- `/home/tstephen/repos/SSW/tests/tactical/FTacticalBattleScreenDelegationTest.cpp`
- `/home/tstephen/repos/SSW/tests/tactical/FTacticalMineDamageFlowTest.cpp`
- Likely one or more new tactical test files under `/home/tstephen/repos/SSW/tests/tactical/`
- `/home/tstephen/repos/SSW/tests/tactical/TacticalTests.cpp`
- `/home/tstephen/repos/SSW/tests/tactical/Makefile`
- `/home/tstephen/repos/SSW/doc/DesignNotes.md`

## Overall Documentation Impact
- Update `/home/tstephen/repos/SSW/doc/DesignNotes.md` so the Milestone 8 narrative reflects the pre-remediation runtime state accurately and then records the remediation separately once implemented.
- Remove or revise any wording that claims the fire-cleanup and move-finalization remediations were already complete before the fixes land.
- Document the new tactical test coverage added for the live wx callback seams.

## Subtasks

### 1. Pin down the single-owner destroyed-ship cleanup contract
Description:
Define the exact ownership and call ordering for destroyed-ship cleanup across `FTacticalGame`, `FBattleScreen`, and `FBattleDisplay` so the implementation fix does not rely on implicit side effects.

Acceptance Criteria:
- The remediation notes identify which method owns destroyed-ship ID capture, which method owns wx map removal, and where bookkeeping is cleared.
- The plan for the runtime sequence is explicit enough that implementers can change one seam at a time without ambiguity.
- The resulting contract preserves model-owned fire resolution and wx-owned board cleanup responsibilities.

Documentation Impact:
- No permanent documentation update expected from this subtask alone.
- Any implementation notes should capture the agreed cleanup sequence for later milestone documentation.

### 2. Fix the fire-phase callback ordering in `FBattleDisplay`
Description:
Adjust the fire-phase completion path in `FBattleDisplay` so it no longer triggers a second model-side destroyed-ship clear before wx cleanup runs.

Acceptance Criteria:
- `FBattleDisplay` no longer invokes a cleanup sequence that empties model destroyed-ship bookkeeping before the wx map consumes it.
- Fire-phase completion still requests the same UI redraw and phase progression behavior after cleanup.
- Defensive-fire and offensive-fire paths both use the corrected ordering.

Documentation Impact:
- No direct documentation change expected.

### 3. Narrow `FBattleScreen` cleanup to wx-side removal orchestration
Description:
Refactor the `FBattleScreen` cleanup wrapper so it consumes the model’s destroyed-ship list and updates the wx map without re-triggering model bookkeeping resets at the wrong time.

Acceptance Criteria:
- `FBattleScreen` no longer causes a second destructive clear of model destroyed-ship state during delegated fire cleanup.
- The screen-level cleanup helper still removes all destroyed ships from the battle board/runtime map.
- The `FBattleScreen` seam remains the wx-side coordinator rather than moving map manipulation into `FTacticalGame`.

Documentation Impact:
- No direct documentation change expected.

### 4. Make `FTacticalGame` preserve destroyed-ship bookkeeping until the UI consumes it
Description:
Adjust `FTacticalGame`’s fire-resolution and destroyed-ship bookkeeping so the last destroyed-ship IDs remain available through the screen cleanup step and are cleared exactly once at the correct lifecycle boundary.

Acceptance Criteria:
- `FTacticalGame::fireAllWeapons()` and related cleanup helpers no longer erase `m_lastDestroyedShipIDs` before wx-side removal has a chance to consume it.
- Model state remains internally consistent after cleanup completes.
- The final lifecycle makes it clear when destroyed-ship bookkeeping becomes stale and when it is safe to clear.

Documentation Impact:
- No direct documentation change expected.

### 5. Rewire the live move-done callback to the canonical model completion path
Description:
Fix the `FBattleDisplay::onMoveDone()` runtime path so it executes `FTacticalGame::completeMovePhase()` through the intended `FBattleScreen`/model delegation seam rather than bypassing the post-move resolution logic.

Acceptance Criteria:
- The live move-done callback no longer relies on a path that skips `FTacticalGame::completeMovePhase()`.
- The callback still preserves the expected phase progression and redraw behavior after move completion.
- The change applies to the actual runtime path used by the wx UI, not just to helper methods or test-only call sites.

Documentation Impact:
- No direct documentation change expected.

### 6. Verify post-move resolution coverage inside `FTacticalGame`
Description:
Tighten the model-side move-completion seam as needed so the runtime callback fix exercises the full intended post-move behavior, including mine checks, mine damage, off-board destruction, and final occupancy updates.

Acceptance Criteria:
- `FTacticalGame::completeMovePhase()` remains the canonical place for post-move resolution.
- The runtime path now reaches the existing or corrected model logic for mine interactions, destruction handling, and occupancy finalization.
- No parallel UI-side logic duplicates the same move-finalization mechanics.

Documentation Impact:
- No direct documentation change expected.

### 7. Replace the fire-flow source assertions with runtime seam coverage
Description:
Update the tactical fire-flow tests so they validate destroyed-ship synchronization behavior instead of asserting the current broken call sequence.

Acceptance Criteria:
- `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp` no longer encodes the duplicate cleanup sequence as expected behavior.
- New or updated tactical tests assert that delegated fire cleanup removes destroyed ships from the wx/runtime side exactly once.
- The tests cover both defensive-fire and offensive-fire cleanup paths if both paths can destroy ships.

Documentation Impact:
- Internal test comments may need refresh to describe the runtime seam being protected.
- No external documentation update expected.

### 8. Add runtime-path coverage for delegated move completion
Description:
Extend the tactical tests to cover the actual `FBattleDisplay` to `FBattleScreen` move-done path so regressions cannot hide behind helper-only assertions.

Acceptance Criteria:
- The tactical suite includes a test that exercises the live move completion caller path identified in the reviewer report.
- The test verifies that the path reaches `FTacticalGame::completeMovePhase()` or its final equivalent canonical model seam.
- Coverage includes observable post-move outcomes such as mine checks, mine damage, off-board destruction, or final occupancy updates as applicable to the chosen harness.

Documentation Impact:
- Internal test comments may need refresh to identify the runtime callback seam.
- No external documentation update expected.

### 9. Correct Milestone 8 design notes to distinguish shipped state from remediation
Description:
Revise the Milestone 8 documentation so it no longer claims the blocking runtime remediations and seam validation were already complete before the fixes land, then record the remediation scope accurately.

Acceptance Criteria:
- `doc/DesignNotes.md` no longer states that the fire cleanup or move-finalization runtime issues were already fixed before this remediation.
- The design notes accurately describe the remaining accepted manual GUI limitation without conflating it with the two blocking bugs.
- The design notes mention the new automated coverage added for live wx callback seams once the remediation is implemented.

Documentation Impact:
- This subtask is itself a documentation update.
- Review milestone wording only; no user-guide update is expected.

### 10. Run tactical-focused validation for the remediated seams
Description:
Build and run the tactical validation needed to confirm that both blocking regressions are fixed and the new tests fail if the runtime seams regress again.

Acceptance Criteria:
- The relevant tactical test target builds successfully.
- The updated tactical suite passes with the new fire-flow and move-completion coverage enabled.
- Validation notes clearly state whether only tactical module tests were run or whether any broader build/test targets were also re-executed.

Documentation Impact:
- Record the validation scope and outcomes in implementation artifacts or handoff notes.
- No permanent repository documentation update expected unless a new limitation is discovered.

## Dependency Ordering
1. Subtask 1 must complete first because the fire cleanup bug spans three classes and needs a single agreed lifecycle before code changes split across files.
2. Subtasks 2, 3, and 4 depend on Subtask 1 and should be completed in that order or in close coordination, because they together implement the fire cleanup fix across display, screen, and model seams.
3. Subtask 5 depends on understanding the current runtime callback path and can proceed independently of the fire cleanup fix.
4. Subtask 6 depends on Subtask 5 because model-side move resolution only matters once the live callback is routed through it.
5. Subtask 7 depends on Subtasks 2 through 4 so the tests can lock in the corrected fire cleanup behavior.
6. Subtask 8 depends on Subtasks 5 and 6 so the tests validate the actual repaired move-completion path.
7. Subtask 9 depends on the intended remediation behavior being settled, but it can land after the code changes and tests are clear.
8. Subtask 10 depends on Subtasks 2 through 9.

## Parallelization Notes
- After Subtask 1, the fire cleanup implementation path and the move-finalization implementation path can proceed in parallel if different implementers own disjoint files.
- Documentation correction can begin once the final intended behavior is confirmed, but it should not claim success until the related code and tests are in place.
- The two testing subtasks can be developed alongside implementation, but final assertions should wait until the runtime seams are stable.

## Implementer Agent Prompts

### Subtask 1 Prompt
You are the implementer agent.

Allowed files:
- `/home/tstephen/repos/SSW/src/tactical/FBattleDisplay.cpp`
- `/home/tstephen/repos/SSW/src/tactical/FBattleScreen.cpp`
- `/home/tstephen/repos/SSW/include/tactical/FTacticalGame.h`
- `/home/tstephen/repos/SSW/src/tactical/FTacticalGame.cpp`
- Optional implementation notes under `/home/tstephen/repos/SSW/artifacts/`

Task:
Establish and document the exact destroyed-ship cleanup lifecycle for delegated fire resolution in Milestone 8. Determine which layer owns destroyed-ship ID capture, which layer owns wx map removal, and when the bookkeeping should be cleared so the later implementation subtasks can change the seam safely.

Acceptance criteria:
- The cleanup contract across display, screen, and model is explicit and internally consistent.
- The agreed sequence leaves destroyed-ship IDs available until wx-side cleanup consumes them.
- The contract preserves Milestone 8’s model-owned fire resolution and wx-owned map cleanup split.

### Subtask 2 Prompt
You are the implementer agent.

Allowed files:
- `/home/tstephen/repos/SSW/src/tactical/FBattleDisplay.cpp`

Task:
Fix the delegated fire-phase callback flow in `FBattleDisplay` so it no longer triggers the duplicate destroyed-ship cleanup sequence identified in the Milestone 8 reviewer findings.

Acceptance criteria:
- `FBattleDisplay` no longer initiates a sequence that clears model destroyed-ship bookkeeping before wx cleanup runs.
- Defensive-fire and offensive-fire completion paths both use the corrected ordering.
- The display still performs the same redraw and phase-advancement orchestration after cleanup.

### Subtask 3 Prompt
You are the implementer agent.

Allowed files:
- `/home/tstephen/repos/SSW/src/tactical/FBattleScreen.cpp`
- `/home/tstephen/repos/SSW/include/tactical/FTacticalGame.h`

Task:
Refine the `FBattleScreen` destroyed-ship cleanup seam so it consumes the model’s destroyed-ship list and updates the wx map without causing a second destructive model clear during delegated fire resolution.

Acceptance criteria:
- `FBattleScreen` no longer re-clears model destroyed-ship state before wx removal finishes.
- Destroyed ships are still removed from the battle map/runtime view through the screen seam.
- The screen remains responsible for wx-side cleanup orchestration only.

### Subtask 4 Prompt
You are the implementer agent.

Allowed files:
- `/home/tstephen/repos/SSW/include/tactical/FTacticalGame.h`
- `/home/tstephen/repos/SSW/src/tactical/FTacticalGame.cpp`

Task:
Adjust `FTacticalGame` destroyed-ship bookkeeping so fire resolution preserves the last destroyed-ship IDs until the UI consumes them and clears them exactly once at the correct lifecycle boundary.

Acceptance criteria:
- `FTacticalGame` no longer erases destroyed-ship IDs too early in the delegated fire path.
- The destroyed-ship bookkeeping remains internally consistent after cleanup completes.
- The resulting lifecycle makes the canonical clear point explicit.

### Subtask 5 Prompt
You are the implementer agent.

Allowed files:
- `/home/tstephen/repos/SSW/src/tactical/FBattleDisplay.cpp`
- `/home/tstephen/repos/SSW/src/tactical/FBattleScreen.cpp`
- `/home/tstephen/repos/SSW/include/tactical/FTacticalGame.h`

Task:
Rewire the live `FBattleDisplay::onMoveDone()` callback path so it routes through the canonical model move-completion seam instead of bypassing `FTacticalGame::completeMovePhase()`.

Acceptance criteria:
- The runtime move-done callback reaches `FTacticalGame::completeMovePhase()` or the final equivalent canonical screen/model seam.
- The callback still drives the expected phase progression and redraw behavior.
- The fix applies to the live wx UI path identified in the Milestone 8 reviewer report.

### Subtask 6 Prompt
You are the implementer agent.

Allowed files:
- `/home/tstephen/repos/SSW/include/tactical/FTacticalGame.h`
- `/home/tstephen/repos/SSW/src/tactical/FTacticalGame.cpp`

Task:
Confirm and tighten the post-move resolution behavior inside `FTacticalGame` so the repaired runtime callback path executes the intended canonical logic for mine checks, mine damage, off-board destruction, and final occupancy updates.

Acceptance criteria:
- `FTacticalGame::completeMovePhase()` remains the authoritative post-move resolution entry point.
- The runtime path now reaches the intended model behavior for mine interactions, destruction handling, and occupancy finalization.
- No duplicated UI-side move-finalization logic remains on the runtime path.

### Subtask 7 Prompt
You are the implementer agent.

Allowed files:
- `/home/tstephen/repos/SSW/tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`
- `/home/tstephen/repos/SSW/tests/tactical/TacticalTests.cpp`
- `/home/tstephen/repos/SSW/tests/tactical/Makefile`
- New or updated files under `/home/tstephen/repos/SSW/tests/tactical/`

Task:
Replace the existing fire-flow source assertions with runtime-oriented tactical tests that validate destroyed-ship synchronization across delegated fire cleanup.

Acceptance criteria:
- The tests no longer encode the duplicate cleanup sequence as expected behavior.
- The updated tactical coverage verifies destroyed ships are removed exactly once from the wx/runtime side after delegated fire resolution.
- Coverage protects both defensive-fire and offensive-fire cleanup paths if both paths can destroy ships.

### Subtask 8 Prompt
You are the implementer agent.

Allowed files:
- `/home/tstephen/repos/SSW/tests/tactical/FTacticalBattleScreenDelegationTest.cpp`
- `/home/tstephen/repos/SSW/tests/tactical/FTacticalMineDamageFlowTest.cpp`
- `/home/tstephen/repos/SSW/tests/tactical/TacticalTests.cpp`
- `/home/tstephen/repos/SSW/tests/tactical/Makefile`
- New or updated files under `/home/tstephen/repos/SSW/tests/tactical/`
- `/home/tstephen/repos/SSW/include/tactical/FTacticalGame.h`
- `/home/tstephen/repos/SSW/src/tactical/FTacticalGame.cpp`

Task:
Add tactical test coverage for the actual runtime move-completion seam used by the wx UI so the `FBattleDisplay` to `FBattleScreen` to `FTacticalGame::completeMovePhase()` path is exercised directly.

Acceptance criteria:
- The tactical suite includes coverage for the live move-done caller path identified by the reviewer.
- The test verifies the runtime path reaches `FTacticalGame::completeMovePhase()` or the final equivalent canonical model seam.
- The test observes post-move outcomes such as mine checks, mine damage, off-board destruction, or final occupancy updates as applicable.

### Subtask 9 Prompt
You are the implementer agent.

Allowed files:
- `/home/tstephen/repos/SSW/doc/DesignNotes.md`

Task:
Correct the Milestone 8 design notes so they distinguish the original shipped state from the remediation work and accurately describe the remaining accepted manual GUI validation limitation.

Acceptance criteria:
- The design notes no longer claim the fire cleanup and move-finalization fixes were already present before remediation.
- The notes separate the two blocking runtime bugs from the accepted manual GUI playthrough limitation.
- The notes record the new automated seam coverage once it exists.

### Subtask 10 Prompt
You are the implementer agent.

Allowed files:
- `/home/tstephen/repos/SSW/tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`
- `/home/tstephen/repos/SSW/tests/tactical/FTacticalBattleScreenDelegationTest.cpp`
- `/home/tstephen/repos/SSW/tests/tactical/FTacticalMineDamageFlowTest.cpp`
- `/home/tstephen/repos/SSW/tests/tactical/TacticalTests.cpp`
- `/home/tstephen/repos/SSW/tests/tactical/Makefile`
- New or updated files under `/home/tstephen/repos/SSW/tests/tactical/`
- `/home/tstephen/repos/SSW/doc/DesignNotes.md`
- Implementation artifact files under `/home/tstephen/repos/SSW/artifacts/` if needed for validation notes

Task:
Run and record the tactical-focused validation needed to confirm the remediated fire cleanup and move-completion seams are fixed and protected by automated coverage.

Acceptance criteria:
- The relevant tactical test target builds successfully.
- The updated tactical suite passes with the new seam coverage enabled.
- Validation notes clearly identify what was run and any remaining known non-blocking gaps.

## Output Artifact Path
- `/home/tstephen/repos/SSW/plans/milestone8-remediation-plan.md`
