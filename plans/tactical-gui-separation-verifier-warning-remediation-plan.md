# Tactical GUI Separation Verifier Warning Remediation Plan

## Feature Restatement

Address the Subtask 2 verifier warnings for the move-route selection regression by making the runtime fixture part of the normal tactical test build/run path and by correcting the design-note validation instructions so they describe a reproducible, shipped validation path.

## Confirmed Repository Facts

- The verifier warnings are recorded in `artifacts/tactical-gui-separation-regressions/subtask-2/verifier_report.md`.
- The warning scope is limited to:
  - tactical test-runner/build integration for `FTacticalMoveRouteSelectionTest`
  - documentation accuracy in `doc/DesignNotes.md`
- Current repository state already shows the later remediation shape:
  - `tests/tactical/TacticalTests.cpp` includes and registers `FTacticalMoveRouteSelectionTest` at lines 19 and 42.
  - `tests/tactical/Makefile` includes `FTacticalMoveRouteSelectionTest.o` in the build/archive inputs at lines 29 and 45-46.
  - `doc/DesignNotes.md` documents the routine tactical-suite validation path for the move-route regression at lines 737-763.

## Assumptions

- The requested plan is for any branch or worktree that still matches the original Subtask 2 verifier state, even though the current repository appears to have already incorporated the fix shape later.
- The verifier note about the test header guard convention is out of scope because the request asked specifically to address the warnings, not the non-blocking note.

## Files To Modify

- `tests/tactical/TacticalTests.cpp`
- `tests/tactical/Makefile`
- `doc/DesignNotes.md`

## Overall Documentation Impact

`doc/DesignNotes.md` should describe only the validation path that actually ships with the tactical module runner. The move-route regression note should explicitly state that the fixture is enforced by the normal `cd tests/tactical && make && ./TacticalTests` flow once runner/build integration is in place, and it should remove any placeholder or non-runnable command text.

## Subtasks

### Subtask 1: Register the move-route regression fixture in the standard tactical test runner

Description:
Integrate `FTacticalMoveRouteSelectionTest` into the tactical module's normal compile/archive/run path so routine tactical validation compiles and executes the regression fixture automatically.

Acceptance Criteria:

1. `tests/tactical/TacticalTests.cpp` includes `FTacticalMoveRouteSelectionTest.h`.
2. `tests/tactical/TacticalTests.cpp` registers `FTacticalMoveRouteSelectionTest::suite()` in the normal tactical runner.
3. `tests/tactical/Makefile` includes `FTacticalMoveRouteSelectionTest.o` in `OBJS`.
4. `tests/tactical/Makefile` includes `FTacticalMoveRouteSelectionTest.o` in the `$(TARGET)` archive dependency/input list.
5. After the implementation, the documented routine tactical command path can compile and execute the move-route regression fixture as part of `./TacticalTests`.

Documentation Impact:

This subtask changes the true validation path for the move-route regression, so the design note must be updated afterward to describe the fixture as part of the routine tactical suite rather than a standalone or implied-only validation path.

### Subtask 2: Correct the move-route regression validation note in the design documentation

Description:
Update the move-route regression section in `doc/DesignNotes.md` so it accurately describes how the regression fixture is exercised after Subtask 1 and so all listed validation commands are directly runnable and reproducible.

Acceptance Criteria:

1. The move-route regression section states that the runtime fixture is part of the normal tactical module test run only after the runner/build integration is in place.
2. The validation subsection contains runnable commands only.
3. Any placeholder comment block or non-command text presented as a command is removed.
4. The documented validation result matches the actual standard tactical execution path used for this regression coverage.
5. The documentation does not imply that the routine tactical suite covers the fixture unless that is actually true in the code/build configuration delivered by Subtask 1.

Documentation Impact:

This subtask is itself the documentation correction. Scope should stay limited to the move-route regression note and its validation section unless another nearby sentence is required to keep the section internally accurate.

## Dependency Ordering

1. Subtask 1 must complete first because the design note should describe the delivered runner/build behavior, not a planned future state.
2. Subtask 2 depends on Subtask 1.
3. These subtasks should not run in parallel because the documentation wording depends on the exact integration outcome and test-run path established by Subtask 1.

## Implementer Agent Prompts

### Prompt For Subtask 1

You are the implementer agent

Allowed files:
- `tests/tactical/TacticalTests.cpp`
- `tests/tactical/Makefile`

Task:
Integrate `FTacticalMoveRouteSelectionTest` into the standard tactical module test runner and build/archive path so the normal `cd tests/tactical && make && ./TacticalTests` workflow compiles and executes this regression fixture.

Acceptance criteria:
1. `tests/tactical/TacticalTests.cpp` includes `FTacticalMoveRouteSelectionTest.h`.
2. `tests/tactical/TacticalTests.cpp` registers `FTacticalMoveRouteSelectionTest::suite()` in the tactical runner.
3. `tests/tactical/Makefile` adds `FTacticalMoveRouteSelectionTest.o` to `OBJS`.
4. `tests/tactical/Makefile` adds `FTacticalMoveRouteSelectionTest.o` to the archive dependency/input list for `$(TARGET)`.
5. The delivered build/run path makes the move-route regression fixture part of the normal tactical suite execution.

Do not report success unless all required artifacts exist and all changes are committed.

### Prompt For Subtask 2

You are the implementer agent

Allowed files:
- `doc/DesignNotes.md`

Task:
Update the move-route regression documentation so it accurately describes the delivered validation path after the fixture is wired into the normal tactical runner/build flow.

Acceptance criteria:
1. The move-route regression note states that the fixture is enforced by the routine tactical suite only if that integration is present in the delivered code.
2. The validation section contains runnable commands only.
3. Any placeholder or comment-only command block is removed.
4. The documented validation result aligns with the actual routine tactical execution path.
5. The update remains scoped to the move-route regression section unless a nearby clarification is required for accuracy.

Do not report success unless all required artifacts exist and all changes are committed.

## Output Artifact Path

- `/home/tstephen/repos/SSW/plans/tactical-gui-separation-verifier-warning-remediation-plan.md`
