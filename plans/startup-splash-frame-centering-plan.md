# Requested agent: `planner`

## Definition Resolution
- Repository-local definition found: no repository-local `planner` definition was identified in this repository.
- Shared definition found: `/home/tstephen/repos/agents/agents/planner.md`
- Definition path used: `/home/tstephen/repos/agents/agents/planner.md`
- Precedence decision: shared `planner` definition applies because this repository does not define a repository-local `planner` override.

## Feature Restatement
Fix startup window placement regressions so that, in both SSW and BattleSim, the splash screen opens centered on the user’s screen, the initial application window also opens centered on screen, both may exist at the same time with the splash remaining on top, and the startup path no longer depends on inconsistent window-manager placement. Also correct the current splash style usage so the splash keeps the intended wx splash-window behavior on wxWidgets 3.3. Add regression coverage for the real startup-placement logic rather than relying only on direct frame-constructor tests.

## Confirmed Repository Facts
- `src/FApp.cpp` creates a parentless `wxSplashScreen`, then immediately constructs and shows `FMainFrame`.
- `src/FBattleSimApp.cpp` creates a parentless `wxSplashScreen`, then immediately constructs and shows `BattleSimFrame`.
- `src/FApp.cpp` still constructs `FMainFrame` with explicit fixed position `wxPoint(50,50)`.
- `src/FMainFrame.cpp` does not currently call `Centre()` or `CentreOnScreen()` in its constructor.
- `include/battleSim/BattleSimFrame.h` now defaults to `wxDefaultPosition` and `wxDefaultSize`, and `src/battleSim/BattleSimFrame.cpp` calls `CentreOnScreen(wxBOTH)`.
- Both splash paths pass an explicit style `wxSIMPLE_BORDER|wxSTAY_ON_TOP`, overriding the default `wxSplashScreen` style set that normally includes `wxFRAME_NO_TASKBAR`.
- Existing GUI tests cover direct `BattleSimFrame` construction and dialog-launch paths, but do not exercise the real `FApp::OnInit()` / `FBattleSimApp::OnInit()` startup sequence or assert splash placement.
- `tests/gui/Makefile` currently links `FMainFrame.cpp` and `FGamePanel.cpp` directly for GUI tests, but does not compile `FApp.cpp` or `FBattleSimApp.cpp` into `GuiTests`.
- The shared GUI harness bootstraps its own `wxApp`, so direct testing of `IMPLEMENT_APP(...)` startup paths is not already covered by the current GUI runner.

## Assumptions
- Extracting startup window creation/positioning into small helper seams is acceptable so the existing GUI harness can test real startup behavior without introducing a second conflicting wx application bootstrap.
- The desired startup policy is identical in both apps: splash centered on screen, startup frame centered on screen, splash shown above the frame while both are visible.
- It is acceptable for the startup helper seam to return or expose the created splash/frame objects so GUI tests can assert their geometry and style behavior before cleanup.

## Files To Modify
- Likely production files:
  - `src/FApp.cpp`
  - `src/FBattleSimApp.cpp`
  - `include/FMainFrame.h`
  - `src/FMainFrame.cpp`
  - `include/battleSim/BattleSimFrame.h`
  - `src/battleSim/BattleSimFrame.cpp`
  - Possibly one new shared startup helper header/source pair under `include/gui/` and `src/gui/` if that is the cleanest extraction point
- Likely test files:
  - `tests/gui/StrategicGuiLiveTest.h`
  - `tests/gui/StrategicGuiLiveTest.cpp`
  - `tests/gui/BattleSimGuiLiveTest.h`
  - `tests/gui/BattleSimGuiLiveTest.cpp`
  - `tests/gui/Makefile`

## Overall Documentation Impact
- No user-facing rules or gameplay documentation update is expected.
- Developer-facing documentation impact is likely limited to brief test intent comments if the new startup helper seam or startup-positioning assertions need explanation.
- If the implementation introduces a reusable startup-placement helper, comments should make the startup placement contract explicit so future GUI changes do not regress back to mixed explicit-position and window-manager-driven behavior.

## Subtasks

### Subtask 1: Extract deterministic startup splash/frame creation seams for both apps
Scope:
- Refactor the startup logic now embedded directly in `FApp::OnInit()` and `FBattleSimApp::OnInit()` into small helper seams that create the splash and initial frame in a testable, deterministic way.
- Preserve the existing application bootstrap behavior while making splash/frame creation callable from the GUI test harness without requiring a second wx app bootstrap path.
- Keep this subtask focused on extraction and startup ownership flow, not on the final centering-policy changes themselves beyond what is required to preserve current behavior through the seam.

Likely files:
- `src/FApp.cpp`
- `src/FBattleSimApp.cpp`
- Possibly one new helper under `include/gui/` and `src/gui/`

Acceptance Criteria:
- The SSW startup path exposes a testable seam for constructing its splash screen and initial frame without requiring direct execution of the `IMPLEMENT_APP(FApp)` bootstrap.
- The BattleSim startup path exposes a corresponding testable seam for constructing its splash screen and initial frame.
- `FApp::OnInit()` and `FBattleSimApp::OnInit()` continue to use the extracted seam rather than duplicating independent startup logic inline.
- The seam preserves concurrent splash-plus-frame startup behavior so both windows may exist at the same time.

Documentation Impact:
- No standalone documentation update expected beyond concise code comments if the helper’s purpose would otherwise be unclear.

### Subtask 2: Fix startup placement and splash styling policy for SSW and BattleSim
Scope:
- Apply the desired startup policy consistently across both apps.
- Ensure the splash is centered on screen in both apps.
- Ensure the initial frame for both apps is centered on screen after its final startup size is established.
- Remove the startup dependence on mixed explicit coordinates and implicit window-manager placement.
- Correct the splash style usage so the splash keeps the intended wx splash-window behavior, including staying on top, without discarding the framework’s default splash style semantics.

Likely files:
- `src/FApp.cpp`
- `src/FBattleSimApp.cpp`
- `include/FMainFrame.h`
- `src/FMainFrame.cpp`
- `include/battleSim/BattleSimFrame.h`
- `src/battleSim/BattleSimFrame.cpp`
- Possibly the extracted startup helper file from Subtask 1

Acceptance Criteria:
- In SSW startup logic, the splash screen is explicitly centered on screen.
- In BattleSim startup logic, the splash screen is explicitly centered on screen.
- `FMainFrame` no longer starts from the legacy fixed startup position and instead opens centered on screen during app startup.
- `BattleSimFrame` startup placement remains centered on screen through the real startup path.
- The splash and startup frame may be shown concurrently, with the splash remaining above the frame while visible.
- The splash style no longer overrides away the default wx splash-window semantics needed for expected taskbar/top-level behavior.
- The resulting startup behavior is deterministic and no longer depends on unspecified window-manager placement for either app.

Documentation Impact:
- No standalone documentation update expected.

### Subtask 3: Add GUI regression coverage for extracted startup seams and startup geometry
Scope:
- Extend the existing GUI live test suites to cover the extracted startup helper seams directly.
- Assert that, for both apps, the startup frame and splash are both created, both are centered on the display, and the splash is shown above the startup frame while visible.
- Update GUI test build wiring only as needed to compile any extracted startup helper seam or app-side source required by those tests.
- Keep the test additions scoped to startup behavior; do not broaden them into unrelated launch-path checks already covered elsewhere.

Likely files:
- `tests/gui/StrategicGuiLiveTest.h`
- `tests/gui/StrategicGuiLiveTest.cpp`
- `tests/gui/BattleSimGuiLiveTest.h`
- `tests/gui/BattleSimGuiLiveTest.cpp`
- `tests/gui/Makefile`
- Possibly one extracted startup helper header if tests need to include it directly

Acceptance Criteria:
- GUI live coverage includes a startup-path test for SSW that exercises the extracted startup seam and verifies centered splash plus centered initial frame.
- GUI live coverage includes a startup-path test for BattleSim that exercises the extracted startup seam and verifies centered splash plus centered initial frame.
- The startup tests assert that the splash is shown and remains top-most relative to the initial frame while both are visible, using the narrowest reliable observable assertion supported by the harness and wxGTK.
- The GUI test target builds and runs with the startup tests included.
- Existing constructor-level `BattleSimFrame` centering coverage remains valid and is not treated as the only startup regression protection.

Documentation Impact:
- No standalone documentation update expected beyond brief test comments if needed.

## Dependency Ordering
1. Subtask 1 first.
   Reason: startup extraction is required to make the real startup path testable under the existing GUI harness without fighting wx app bootstrap constraints.
2. Subtask 2 second.
   Reason: the actual placement/style fix should be implemented against the extracted startup seam so production behavior and testable behavior stay identical.
3. Subtask 3 last.
   Reason: the startup tests should lock the final extracted-and-fixed behavior, not a temporary intermediate path.

Parallelization guidance:
- Keep all three subtasks sequential.
- Subtasks 1 and 2 overlap on the same startup code paths and ownership decisions.
- Subtasks 2 and 3 overlap on the exact startup seam and geometry contract being introduced.

## Implementer Agent Prompts

### Implementer Prompt: Subtask 1
You are the implementer agent

Allowed files:
- `src/FApp.cpp`
- `src/FBattleSimApp.cpp`
- One new helper pair if needed under:
  - `include/gui/`
  - `src/gui/`

Task:
- Extract the startup splash-and-initial-frame creation logic from `FApp::OnInit()` and `FBattleSimApp::OnInit()` into small helper seams that can be invoked from the GUI test harness without direct dependence on the `IMPLEMENT_APP(...)` startup path.
- Preserve the current startup lifecycle shape: create splash, create initial frame, show the frame, and keep the splash visible concurrently.
- Keep the helper design minimal and oriented around testability of startup behavior rather than broad architectural cleanup.

Acceptance criteria:
- The SSW app startup path uses an extracted, testable seam for splash and initial-frame creation.
- The BattleSim app startup path uses an extracted, testable seam for splash and initial-frame creation.
- `FApp::OnInit()` and `FBattleSimApp::OnInit()` delegate to the extracted seam instead of duplicating startup creation logic inline.
- The extracted seam preserves concurrent splash/frame startup behavior.

Validation guidance:
- Smallest likely validation command: `cd tests/gui && make && xvfb-run -a ./GuiTests`
- If `xvfb-run` is unavailable, run `cd tests/gui && make && ./GuiTests` as an explicit environment assumption.

Expected Tester file locations:
- `tests/gui/StrategicGuiLiveTest.cpp`
- `tests/gui/BattleSimGuiLiveTest.cpp`
- `tests/gui/Makefile`

Artifact-directory guidance:
- Write artifacts under `artifacts/startup-splash-frame-centering/subtask-1/` if orchestration does not provide a different repository-root-relative path.

Begin implementation immediately when the required blocking inputs are present. Do not stop after a preflight restatement alone.

Do not report success unless all required artifacts exist and all changes are committed.

### Implementer Prompt: Subtask 2
You are the implementer agent

Allowed files:
- `src/FApp.cpp`
- `src/FBattleSimApp.cpp`
- `include/FMainFrame.h`
- `src/FMainFrame.cpp`
- `include/battleSim/BattleSimFrame.h`
- `src/battleSim/BattleSimFrame.cpp`
- The extracted helper files from Subtask 1 if they exist

Task:
- Fix startup placement behavior in both apps so the splash is centered on screen and the initial application frame is also centered on screen.
- Preserve the requirement that both windows may be drawn at the same time while the splash remains on top.
- Remove the legacy explicit SSW startup position and any remaining startup dependence on implicit window-manager placement.
- Correct the splash style usage so the wx splash window retains the framework’s intended default splash semantics instead of replacing them with a reduced custom style set.

Acceptance criteria:
- SSW startup centers the splash on screen and opens the initial `FMainFrame` centered on screen.
- BattleSim startup centers the splash on screen and opens the initial `BattleSimFrame` centered on screen.
- The splash and startup frame may coexist, with the splash remaining above the frame while visible.
- `FMainFrame` no longer relies on the old fixed startup position.
- The splash style no longer strips away wx splash-window default behavior needed for expected top-level/taskbar handling.
- Startup placement is deterministic for both apps.

Validation guidance:
- Smallest likely validation command: `cd tests/gui && make && xvfb-run -a ./GuiTests`
- If a narrower iteration scope is useful, use the smallest available GUI test subset as an explicit assumption, but finish with the full `GuiTests` command.

Expected Tester file locations:
- `tests/gui/StrategicGuiLiveTest.cpp`
- `tests/gui/BattleSimGuiLiveTest.cpp`

Artifact-directory guidance:
- Write artifacts under `artifacts/startup-splash-frame-centering/subtask-2/` if orchestration does not provide a different repository-root-relative path.

Begin implementation immediately when the required blocking inputs are present. Do not stop after a preflight restatement alone.

Do not report success unless all required artifacts exist and all changes are committed.

### Implementer Prompt: Subtask 3
You are the implementer agent

Allowed files:
- `tests/gui/StrategicGuiLiveTest.h`
- `tests/gui/StrategicGuiLiveTest.cpp`
- `tests/gui/BattleSimGuiLiveTest.h`
- `tests/gui/BattleSimGuiLiveTest.cpp`
- `tests/gui/Makefile`
- Any extracted startup helper header needed for test inclusion from Subtask 1

Task:
- Add live GUI regression coverage for the extracted SSW and BattleSim startup seams.
- Verify each startup path creates both the splash and the initial frame, that both are centered on the display, and that the splash remains above the startup frame while visible.
- Update the GUI test build only as needed so these startup tests compile and run under the existing harness.
- Keep the assertions focused on observable startup placement/visibility behavior and avoid broadening into unrelated launch-path coverage.

Acceptance criteria:
- `GuiTests` includes startup-path coverage for SSW using the extracted startup seam.
- `GuiTests` includes startup-path coverage for BattleSim using the extracted startup seam.
- The startup tests verify display centering for both splash and startup frame in each app.
- The startup tests verify, using the narrowest reliable observable assertion available, that the splash remains above the frame while both are visible.
- The GUI test target still builds and runs successfully with the new startup coverage.

Validation guidance:
- Required validation command: `cd tests/gui && make && xvfb-run -a ./GuiTests`
- If `xvfb-run` is unavailable in the environment, use `cd tests/gui && make && ./GuiTests` as an explicit fallback assumption and record that limitation.

Expected Tester file locations:
- `tests/gui/StrategicGuiLiveTest.cpp`
- `tests/gui/BattleSimGuiLiveTest.cpp`

Artifact-directory guidance:
- Write artifacts under `artifacts/startup-splash-frame-centering/subtask-3/` if orchestration does not provide a different repository-root-relative path.

Begin implementation immediately when the required blocking inputs are present. Do not stop after a preflight restatement alone.

Do not report success unless all required artifacts exist and all changes are committed.

## Output Markdown File
- `plans/startup-splash-frame-centering-plan.md`
