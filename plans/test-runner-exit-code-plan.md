# Plan: Test Runner Exit Code Is Inverted

## Feature Restatement

Correct the top-level CppUnit test runner so the `SSWTests` process returns a conventional shell exit status: `0` when the suite passes and non-zero when any test fails.

## Confirmed Repository Facts

- [tests/SSWTests.cpp](/home/tstephen/repos/SSW/tests/SSWTests.cpp#L54) is the main test entry point for the full suite.
- [tests/SSWTests.cpp](/home/tstephen/repos/SSW/tests/SSWTests.cpp#L101) stores the result of `runner.run("", false)` in `wasSuccessful`.
- [tests/SSWTests.cpp](/home/tstephen/repos/SSW/tests/SSWTests.cpp#L102) currently returns that boolean directly, which inverts process semantics for shells and CI.
- [tests/tactical/TacticalTests.cpp](/home/tstephen/repos/SSW/tests/tactical/TacticalTests.cpp#L29) already uses the expected pattern `return wasSuccessful ? 0 : 1;`.

## Assumptions

- No wrapper script or Makefile intentionally depends on the current inverted exit code.
- Scope remains limited to Prompt 2, so this plan does not include unrelated failing test repairs.

## Files To Modify

- [tests/SSWTests.cpp](/home/tstephen/repos/SSW/tests/SSWTests.cpp)

## Files To Inspect During Implementation

- [tests/tactical/TacticalTests.cpp](/home/tstephen/repos/SSW/tests/tactical/TacticalTests.cpp)

## Subtasks

### 1. Correct the main test runner exit-status mapping

Update the `SSWTests` main program so the boolean returned by `runner.run()` is translated into a process exit code rather than returned directly.

Acceptance criteria:
- [tests/SSWTests.cpp](/home/tstephen/repos/SSW/tests/SSWTests.cpp) returns `0` when `runner.run()` reports success.
- [tests/SSWTests.cpp](/home/tstephen/repos/SSW/tests/SSWTests.cpp) returns `1` when `runner.run()` reports failure.
- The change is limited to process exit semantics and does not alter which tests are registered or how the suite is executed.

### 2. Verify the test-runner contract against existing repository patterns

Check the repository for other CppUnit runner entry points and confirm the `SSWTests` change aligns with the already-correct tactical runner behavior.

Acceptance criteria:
- The implementer confirms whether any other test runner entry point uses the inverted pattern.
- [tests/tactical/TacticalTests.cpp](/home/tstephen/repos/SSW/tests/tactical/TacticalTests.cpp) is used as the reference pattern for conventional exit handling.
- Verification includes observing a non-zero shell exit code from `SSWTests` when the suite still contains known failing tests, or another equivalent failure-path check if the suite state changes during implementation.

## Dependency Ordering

1. Subtask 1 must happen first because it changes the actual exit-code behavior.
2. Subtask 2 follows Subtask 1 and verifies the behavior against repository conventions and runtime results.

Parallelism:
- No meaningful parallel split is needed; this is a narrowly scoped infrastructure fix.

## Implementer Agent Prompts

### Prompt For Subtask 1

```text
You are the implementer agent.

Allowed files:
- tests/SSWTests.cpp

Task:
- Fix the top-level CppUnit runner so the `SSWTests` executable returns a conventional process exit code instead of returning the `runner.run()` boolean directly.
- Keep the change scoped to Prompt 2 only. Do not work on unrelated failing tests or other cleanup items.

Acceptance criteria:
- `tests/SSWTests.cpp` returns `0` when `runner.run()` succeeds.
- `tests/SSWTests.cpp` returns `1` when `runner.run()` fails.
- Test registration order and suite composition remain unchanged.
```

### Prompt For Subtask 2

```text
You are the implementer agent.

Allowed files:
- tests/SSWTests.cpp

Files to inspect but not necessarily modify:
- tests/tactical/TacticalTests.cpp

Task:
- Verify that the `SSWTests` exit-code handling matches the established repository pattern for CppUnit runners.
- Confirm whether any other test entry points still return the boolean directly.
- Validate that a failing `SSWTests` run now produces a non-zero shell exit code.

Acceptance criteria:
- The implementation is checked against `tests/tactical/TacticalTests.cpp` as a reference.
- Any additional inverted runner pattern discovered during the audit is explicitly reported.
- Verification demonstrates that `SSWTests` no longer exits with code `0` when tests fail.
```
