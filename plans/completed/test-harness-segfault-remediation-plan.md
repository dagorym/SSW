# Requested agent: `planner`

## Definition Resolution
- Repository-local definition found: no repository-local `planner` definition was identified in this repository.
- Shared definition found: `/home/tstephen/repos/agents/agents/planner.md`
- Definition path used: `/home/tstephen/repos/agents/agents/planner.md`
- Precedence decision: shared `planner` definition applies because this repository does not define a repository-local `planner` override.

## Feature Restatement
Analyze the current `SSWTests` and `TacticalTests` segfaults that appeared after the makefile/test-inclusion cleanup, identify where each crash is actually occurring, and produce a remediation plan that fixes the real failing test path while also preventing stale test-runner binaries from being mistaken for current source failures.

## Confirmed Repository Facts
- A freshly rebuilt `tests/SSWTests` still crashes on April 17, 2026 after emitting GDK/wx display assertions.
- A `gdb` backtrace from the freshly rebuilt `tests/SSWTests` shows the crash path is:
  - `FrontierTests::WXStrategicUITest::testGuardedDialogMethodsReturnNonZeroWhenNoUIContext()`
  - `Frontier::WXStrategicUI::selectRetreatCondition()`
  - `SatharRetreatGUI::SatharRetreatGUI(...)`
  - wx GTK dialog creation with `wxAppConsoleBase::GetAppDisplayName(this=0x0)`
- `tests/core/WXStrategicUITest.cpp` explicitly expects strategic dialog-entry methods to return non-zero when UI context is unavailable.
- `src/gui/WXStrategicUI.cpp` currently constructs `SatharRetreatGUI dialog(m_parent);` unconditionally in `selectRetreatCondition()`, even when there is no active wx application or display context.
- `tests/gui/StrategicGuiLiveTest.cpp` also explicitly covers the parentless runtime case and expects `WXStrategicUI noParentUI(NULL); noParentUI.selectRetreatCondition();` to work when a live wx harness exists.
- Therefore the fix cannot simply be `if (m_parent == NULL) return 1;`; it must distinguish “no parent window” from “no usable wx UI runtime”.
- The initially observed `tests/tactical/TacticalTests` segfault was reproducible with the existing binary, but after `cd tests/tactical && make clean && make`, `./TacticalTests` completes successfully with `OK (92 tests)`.
- A `gdb` run of the freshly rebuilt `tests/tactical/TacticalTests` exits normally with no stack, confirming the current source tree does not reproduce the tactical segfault once the runner is rebuilt cleanly.
- `tests/Makefile` currently builds `SSWTests` and provides `gui-tests`, but it does not provide a top-level convenience target that rebuilds the standalone tactical runner binary at `tests/tactical/TacticalTests`.
- `tests/tactical/Makefile` does rebuild the tactical runner correctly when invoked directly.

## Assumptions
- It is acceptable to add a small runtime guard/helper inside `src/gui/WXStrategicUI.cpp` to detect whether dialog creation is safe before constructing modal strategic dialogs.
- It is acceptable for the strategic adapter to return a non-zero cancel-style status when no usable wx UI runtime exists, while still allowing parentless dialogs when a live wx harness or GUI runtime is present.
- It is acceptable to add additive top-level test-build ergonomics for the tactical runner without changing the documented meaning of `cd tests && make`.

## Files To Modify
- Likely production files:
  - `src/gui/WXStrategicUI.cpp`
  - Possibly `include/gui/WXStrategicUI.h` if a helper must be declared rather than kept local to the implementation file
- Likely test files:
  - `tests/core/WXStrategicUITest.h`
  - `tests/core/WXStrategicUITest.cpp`
  - `tests/gui/StrategicGuiLiveTest.h`
  - `tests/gui/StrategicGuiLiveTest.cpp`
- Likely build files:
  - `tests/Makefile`
  - Possibly `tests/tactical/Makefile` if a narrow alignment tweak is needed for the new top-level tactical-runner target

## Overall Documentation Impact
- No user-facing gameplay or GUI documentation update is expected.
- Developer-facing documentation impact should be minimal and limited to concise comments only if the no-UI-context guard logic would otherwise be ambiguous.
- If the top-level test makefile gains an additive tactical-runner target, brief inline makefile comments may be useful, but a standalone documentation update is likely unnecessary.

## Subtasks

### Subtask 1: Guard strategic modal dialog entry points when no wx UI runtime exists
Scope:
- Fix the `WXStrategicUI` modal dialog path so headless/unit-test execution without a live wx app or valid display does not attempt to construct GTK dialogs.
- Preserve current parentless runtime behavior for environments where a live wx harness does exist.
- Keep the change narrowly focused on the strategic adapter entry points implicated by the crash rather than broad GUI refactoring.

Likely files:
- `src/gui/WXStrategicUI.cpp`
- Possibly `include/gui/WXStrategicUI.h`

Acceptance Criteria:
- `WXStrategicUI::selectRetreatCondition()` no longer constructs a dialog when no usable wx UI runtime exists and instead returns a non-zero cancel-style result.
- Any other `WXStrategicUI` dialog-entry methods that can be reached without a valid parent or runtime are guarded consistently where needed.
- Parentless strategic dialog flows still work in a live wx runtime and are not regressed into unconditional early returns.
- A freshly rebuilt `cd tests && make && ./SSWTests` no longer segfaults in `WXStrategicUITest`.

Documentation Impact:
- No standalone documentation update expected.
- A brief local comment is acceptable if needed to explain why parentless dialogs remain valid in live GUI runs while headless/unit runs must bail out.

### Subtask 2: Lock the strategic adapter behavior with headless and live regression coverage
Scope:
- Strengthen the strategic UI tests so the intended distinction is explicit:
  - headless/no-wx-runtime paths must fail safely without dialog construction
  - parentless-but-live-wx paths must still show modals and remain centered correctly
- Keep this subtask limited to regression coverage, not broader GUI harness cleanup.

Likely files:
- `tests/core/WXStrategicUITest.h`
- `tests/core/WXStrategicUITest.cpp`
- `tests/gui/StrategicGuiLiveTest.h`
- `tests/gui/StrategicGuiLiveTest.cpp`

Acceptance Criteria:
- Core/unit coverage explicitly asserts that `WXStrategicUI` dialog methods fail safely when no usable wx runtime exists.
- GUI live coverage continues to prove that a parentless `WXStrategicUI` can still launch the retreat dialog when the harness owns a valid wx runtime.
- The tests clearly distinguish “no parent window” from “no UI context” so future changes do not collapse those cases together.
- Freshly rebuilt `SSWTests` passes the strategic adapter coverage without crashing.

Documentation Impact:
- No standalone documentation update expected.

### Subtask 3: Add top-level tactical-runner build hygiene so stale binaries are not mistaken for current failures
Scope:
- Add a narrow top-level build entry point that rebuilds the standalone tactical runner binary from `tests/` so developers can refresh `tests/tactical/TacticalTests` without manually descending into the module directory.
- Keep this additive; do not broaden `cd tests && make` into building every module runner unless that is clearly necessary.
- Use the observed stale tactical binary problem as the target: make current tactical-runner rebuilds explicit and easy to invoke.

Likely files:
- `tests/Makefile`
- Possibly `tests/tactical/Makefile`

Acceptance Criteria:
- The top-level tests makefile exposes a clear tactical-runner build target that rebuilds `tests/tactical/TacticalTests`.
- Invoking that target produces a fresh tactical runner binary using the current test objects and current linked model/gui libraries.
- After a clean tactical rebuild, `cd tests/tactical && ./TacticalTests` completes successfully in the current tree.
- The change does not break the existing top-level `SSWTests` build flow.

Documentation Impact:
- No standalone documentation update expected.
- A brief inline makefile comment is acceptable if it clarifies why the target exists.

## Dependency Ordering
1. Subtask 1 first.
   Reason: the `SSWTests` crash is a real current-source defect in `WXStrategicUI`, so the production guard behavior must be fixed before locking it with stronger tests.
2. Subtask 2 second.
   Reason: regression coverage should codify the intended distinction only after the runtime behavior has been corrected.
3. Subtask 3 third.
   Reason: the tactical issue is currently a build-hygiene/freshness problem rather than an active source crash, so it can follow the actual functional fix.

Parallelization guidance:
- Keep Subtasks 1 and 2 sequential because they touch the same strategic adapter contract.
- Subtask 3 may be run after Subtask 1 independently of Subtask 2 if coordination pressure requires it, but the conservative plan is sequential because all three subtasks affect the test-debugging workflow and validation story.

## Implementer Agent Prompts

### Implementer Prompt: Subtask 1
You are the implementer agent

Allowed files:
- `src/gui/WXStrategicUI.cpp`
- `include/gui/WXStrategicUI.h` only if a helper declaration is truly necessary

Task:
- Fix the strategic wx adapter so `WXStrategicUI` does not construct modal dialogs when no usable wx UI runtime exists.
- Preserve the existing parentless runtime behavior used by live GUI tests: a null parent alone must not be treated as a headless failure when a valid wx runtime is active.
- Keep the change narrowly scoped to the strategic adapter logic implicated by the `SSWTests` crash.

Acceptance criteria:
- `WXStrategicUI::selectRetreatCondition()` returns a non-zero cancel-style result instead of constructing a dialog when no usable wx runtime exists.
- Any other reachable `WXStrategicUI` modal entry points that share the same unsafe assumption are guarded consistently where needed.
- Parentless live-wx dialog behavior is preserved.
- `cd tests && make && ./SSWTests` no longer segfaults in the strategic adapter path.

Validation guidance:
- Smallest relevant validation command: `cd tests && make && ./SSWTests`
- If a live display session is available, also run `cd tests/gui && make && ./GuiTests` to confirm parentless strategic GUI behavior remains intact.
- If `xvfb-run` is available in the target environment, `cd tests/gui && make && xvfb-run -a ./GuiTests` is the preferred headless GUI validation command; in the current local environment this command is an assumption because `xvfb-run` is not installed.

Expected Tester file locations:
- `tests/core/WXStrategicUITest.cpp`
- `tests/gui/StrategicGuiLiveTest.cpp`

Artifact-directory guidance:
- Write artifacts under `artifacts/test-harness-segfault-remediation/subtask-1/` if orchestration does not provide a different repository-root-relative path.

Begin implementation immediately when the required blocking inputs are present. Do not stop after a preflight restatement alone.

Do not report success unless all required artifacts exist and all changes are committed.

### Implementer Prompt: Subtask 2
You are the implementer agent

Allowed files:
- `tests/core/WXStrategicUITest.h`
- `tests/core/WXStrategicUITest.cpp`
- `tests/gui/StrategicGuiLiveTest.h`
- `tests/gui/StrategicGuiLiveTest.cpp`

Task:
- Add or tighten regression coverage so the strategic adapter contract is explicit:
  - headless/no-wx-runtime execution must fail safely without dialog construction
  - parentless execution in a live wx harness must still work
- Keep the test updates focused on the crash contract and its live-runtime counterpart.

Acceptance criteria:
- Core strategic adapter tests explicitly cover the safe no-runtime failure path.
- Live GUI tests still cover the parentless retreat-dialog path in a valid wx runtime.
- The coverage makes the distinction between “null parent” and “no usable UI context” obvious to future maintainers.
- Freshly rebuilt `SSWTests` passes without crashing.

Validation guidance:
- Required validation command: `cd tests && make && ./SSWTests`
- If a display session is available, also run `cd tests/gui && make && ./GuiTests`
- If `xvfb-run` is available in the target environment, prefer `cd tests/gui && make && xvfb-run -a ./GuiTests`; treat that as an environment assumption when unavailable.

Expected Tester file locations:
- `tests/core/WXStrategicUITest.cpp`
- `tests/gui/StrategicGuiLiveTest.cpp`

Artifact-directory guidance:
- Write artifacts under `artifacts/test-harness-segfault-remediation/subtask-2/` if orchestration does not provide a different repository-root-relative path.

Begin implementation immediately when the required blocking inputs are present. Do not stop after a preflight restatement alone.

Do not report success unless all required artifacts exist and all changes are committed.

### Implementer Prompt: Subtask 3
You are the implementer agent

Allowed files:
- `tests/Makefile`
- `tests/tactical/Makefile` only if required for a narrow compatibility adjustment

Task:
- Add top-level build hygiene for the standalone tactical runner so developers can rebuild `tests/tactical/TacticalTests` from the repository’s top-level test entry point instead of accidentally reusing a stale module runner binary.
- Keep the change additive and avoid broadening the default top-level test build unnecessarily.

Acceptance criteria:
- The top-level tests makefile exposes a tactical-runner build target that rebuilds `tests/tactical/TacticalTests`.
- The target produces a fresh tactical runner binary from current sources and linked libraries.
- Running the freshly rebuilt tactical runner completes successfully in the current tree.
- The existing top-level `SSWTests` build path remains intact.

Validation guidance:
- Smallest relevant validation command after implementation: `cd tests && make tactical-tests && ./tactical/TacticalTests`
- Also confirm `cd tests && make && ./SSWTests` still builds and runs through the current non-GUI suite path.

Expected Tester file locations:
- No new tester-owned source files are expected; validation should focus on `tests/Makefile`, `tests/tactical/Makefile`, and the rebuilt `tests/tactical/TacticalTests` runner behavior.

Artifact-directory guidance:
- Write artifacts under `artifacts/test-harness-segfault-remediation/subtask-3/` if orchestration does not provide a different repository-root-relative path.

Begin implementation immediately when the required blocking inputs are present. Do not stop after a preflight restatement alone.

Do not report success unless all required artifacts exist and all changes are committed.

## Output Markdown File
- `plans/test-harness-segfault-remediation-plan.md`
