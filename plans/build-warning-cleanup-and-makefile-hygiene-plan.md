# Build Warning Cleanup And Makefile Hygiene Plan

## Confirmed Repository Facts
- Several Makefiles still use `ar rcsu`, which current GNU `ar` warns about because `u` is ignored.
- Coverage flags are enabled unconditionally in normal build Makefiles such as `src/Makefile`, `src/core/Makefile`, `src/gui/Makefile`, `src/ships/Makefile`, `src/strategic/Makefile`, `src/weapons/Makefile`, `src/defenses/Makefile`, `src/battleSim/Makefile`, and the test Makefiles.
- Tactical GUI source has a small set of unused `wxColour` locals in `src/tactical/FBattleBoard.cpp`, `src/tactical/FBattleScreen.cpp`, and `src/tactical/FBattleDisplay.cpp`.
- `src/FApp.cpp` initializes wx image handlers in production startup, while `tests/SSWTests.cpp` does not.
- `src/FBattleSimApp.cpp` also initializes wx image handlers in production startup.

## Assumptions
- Build-hygiene work should be split from any correctness fixes so the warning cleanup is easier to verify and revert independently.
- Coverage instrumentation should be gated by an explicit build mode or variable rather than being part of the default compile flags.
- The image-handler warning noise in tests should be addressed in the test bootstrap if tactical/core tests rely on wx image loading.

## Files To Modify
- `src/Makefile`
- `src/core/Makefile`
- `src/gui/Makefile`
- `src/ships/Makefile`
- `src/strategic/Makefile`
- `src/weapons/Makefile`
- `src/defenses/Makefile`
- `src/battleSim/Makefile`
- `src/tactical/Makefile`
- `tests/Makefile`
- `tests/core/Makefile`
- `tests/ships/Makefile`
- `tests/strategic/Makefile`
- `tests/tactical/Makefile`
- `src/tactical/FBattleBoard.cpp`
- `src/tactical/FBattleScreen.cpp`
- `src/tactical/FBattleDisplay.cpp`
- `tests/SSWTests.cpp`
- possibly `src/FApp.cpp` only if production image-handler initialization needs to be modernized as part of the cleanup

## Subtasks

### 1. Remove obsolete archive-command warnings from Makefiles
- Replace `ar rcsu` with `ar rcs` across the Makefiles that currently emit the warning.
- Keep the archive behavior otherwise unchanged.
- Apply the same pattern consistently across production and test static libraries.

Acceptance criteria:
- No build step emits the `ar: 'u' modifier ignored since 'D' is the default` warning.
- Archive creation still succeeds for all affected modules.
- The Makefile change is mechanical and consistent across modules.

### 2. Separate coverage instrumentation from default builds
- Introduce a build switch or equivalent Makefile structure so `-fprofile-arcs -ftest-coverage` is not part of the normal compile flags.
- Preserve the ability to build coverage-enabled binaries intentionally for testing or profiling.
- Apply the change across the main module Makefiles and any test Makefiles that currently inherit the unconditional coverage flags.

Acceptance criteria:
- Default builds no longer include coverage instrumentation unless the coverage mode is explicitly enabled.
- Coverage builds remain available through a dedicated flag or target.
- `libgcov` checksum noise caused by stale coverage artifacts is reduced or eliminated in normal test runs.

### 3. Clean up the tactical unused-local-variable warnings
- Remove or inline the unused `wxColour` locals in the tactical source files identified by the cleanup review.
- Keep the drawing logic behavior unchanged while eliminating the warning source.
- Limit the change to the minimal set of tactical functions that actually contain unused locals.

Acceptance criteria:
- The tactical build no longer reports the identified unused-local-variable warnings.
- The tactical rendering behavior is unchanged.
- The source remains clear without introducing no-op assignments or placeholder use.

### 4. Reduce wx image-loading noise in the test bootstrap
- Decide whether the test runner should initialize wx image handlers in `tests/SSWTests.cpp`, or whether that should stay only in production startup and the tests should instead avoid image-loading paths.
- If the tests legitimately load PNG assets, add the same image-handler initialization used by the applications.
- Keep the bootstrap change in the test harness rather than spreading it through individual tests.

Acceptance criteria:
- Test execution no longer emits avoidable PNG load warnings when image assets are loaded during tests.
- The change is localized to the test bootstrap unless a stronger production-side fix is required.
- The test runner still starts and executes the existing suite normally.

## Dependency Ordering
1. Subtask 1 can be done independently and should land first because it is mechanical and low risk.
2. Subtask 2 depends only on build-system coordination and can run in parallel with Subtask 3 once the build flag shape is known.
3. Subtask 3 is independent of Subtask 1 and can be completed at any point.
4. Subtask 4 depends on understanding current test asset loading and should be validated after the build noise is under control.

## Implementer Agent Prompts

### Prompt 1
You are the implementer agent.

Files you are allowed to change: the Makefiles under `src/` and `tests/` that currently use `ar rcsu`.

Task: Replace obsolete `ar rcsu` invocations with `ar rcs` across the project so the archive-step warning disappears without changing build behavior.

Acceptance criteria:
- No affected Makefile uses `ar rcsu` anymore.
- Archive targets still build successfully.
- The change is mechanical and does not alter unrelated build logic.

### Prompt 2
You are the implementer agent.

Files you are allowed to change: the Makefiles under `src/` and `tests/` that currently inject `-fprofile-arcs -ftest-coverage` into normal `CXXFLAGS`, and any shared build-include file if one exists.

Task: Gate coverage instrumentation behind an explicit build mode so normal builds do not compile with coverage flags by default.

Acceptance criteria:
- Default builds do not include coverage instrumentation.
- A dedicated coverage-enabled build path still exists.
- The change applies consistently across the affected modules and test builds.

### Prompt 3
You are the implementer agent.

Files you are allowed to change: `src/tactical/FBattleBoard.cpp`, `src/tactical/FBattleScreen.cpp`, and `src/tactical/FBattleDisplay.cpp`.

Task: Remove the unused tactical `wxColour` locals called out by the cleanup review without changing rendering behavior.

Acceptance criteria:
- The identified unused-local warnings are eliminated.
- Tactical drawing behavior is unchanged.
- The code remains readable and does not replace the locals with dead code.

### Prompt 4
You are the implementer agent.

Files you are allowed to change: `tests/SSWTests.cpp` and, if needed, a small shared test bootstrap helper under `tests/`.

Task: Decide how to handle wx image handlers in the test harness so PNG loads used by tests no longer emit avoidable warnings.

Acceptance criteria:
- The test harness either initializes image handlers when needed or avoids image-loading paths that require them.
- Avoidable image-load warnings no longer appear during test runs.
- Existing test execution remains stable after the bootstrap adjustment.

## Output Artifact
- Written plan file: `plans/build-warning-cleanup-and-makefile-hygiene-plan.md`
