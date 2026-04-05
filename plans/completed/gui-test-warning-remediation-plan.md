# GUI Test Warning Remediation Plan

## Restated Feature
Fix the gaps called out in the GUI test review so the live GUI suite actually enforces the intended tactical and BattleSim acceptance coverage, and ensure no dialogs or top-level windows are left open when GUI tests finish.

## Confirmed Repository Facts
- `tests/gui/TacticalGuiLiveTest.h` only registers `testWXTacticalUIParentBackedModalAndRedrawPaths`, while `tests/gui/TacticalGuiLiveTest.cpp` also defines `testTacticalDamageSummaryDialogDisplaysContextAndCloseBehavior()` and `testICMSelectionDialogInteractionFinalizesAssignedCountsAndAmmo()` that are not in the suite.
- The current tactical ICM test uses `ICMSelectionGUITestPeer::finalizeAssignmentsWithoutModal()` instead of proving the real modal completion path.
- `tests/gui/BattleSimGuiLiveTest.cpp` currently asserts only `frame.IsShown()` for the `BattleSimFrame` launch test and does not assert that `ScenarioDialog` or `ScenarioEditorGUI` actually appeared in the `LocalGameDialog` launch-chain test.
- `tests/gui/WXGuiTestHarness.cpp` destroys top-level windows during `shutdown()`, but there is no fixture-level assertion or helper proving that each live GUI test cleaned up its own dialogs/windows before teardown completes.
- `tests/gui/BattleSimGuiLiveTest.cpp` uses shown stack-allocated `wxFrame` parents in multiple tests, which is weaker than explicit `Destroy()` plus event pumping for deterministic top-level cleanup.

## Assumptions
- The warnings can be resolved without broad architecture changes; the likely work is contained to GUI tests and the shared GUI test harness.
- Minimal GUI-side test seams are acceptable if needed to observe modal appearance or deterministic close behavior, but product logic should not be refactored unless the harness cannot otherwise prove launch/cleanup behavior.

## Files To Modify
- `/home/tstephen/repos/SSW/tests/gui/WXGuiTestHarness.h`
- `/home/tstephen/repos/SSW/tests/gui/WXGuiTestHarness.cpp`
- `/home/tstephen/repos/SSW/tests/gui/GuiHarnessTest.h`
- `/home/tstephen/repos/SSW/tests/gui/GuiHarnessTest.cpp`
- `/home/tstephen/repos/SSW/tests/gui/TacticalGuiLiveTest.h`
- `/home/tstephen/repos/SSW/tests/gui/TacticalGuiLiveTest.cpp`
- `/home/tstephen/repos/SSW/tests/gui/BattleSimGuiLiveTest.cpp`
- `/home/tstephen/repos/SSW/tests/gui/BattleSimGuiLiveTest.h`
- Likely only if a minimal GUI-owned seam is required for observable launch/close behavior:
  - `/home/tstephen/repos/SSW/src/gui/WXTacticalUI.cpp`
  - `/home/tstephen/repos/SSW/include/gui/WXTacticalUI.h`
  - `/home/tstephen/repos/SSW/src/battleSim/BattleSimFrame.cpp`
  - `/home/tstephen/repos/SSW/src/battleSim/LocalGameDialog.cpp`
  - `/home/tstephen/repos/SSW/src/battleSim/ScenarioDialog.cpp`
  - `/home/tstephen/repos/SSW/src/battleSim/ScenarioEditorGUI.cpp`

## Overall Documentation Impact
No major documentation rewrite is expected. If the harness gains a new required cleanup/assertion pattern for live GUI tests, the contributor testing guidance in `AGENTS.md` should be reviewed and updated only if that pattern becomes a repo convention future GUI tests must follow.

## Subtasks
### 1. Add Harness-Level Top-Level Window Tracking and Cleanup Enforcement
Description:
Extend the shared GUI harness so tests can observe modal/top-level appearance by title or type, close launched dialogs deterministically, and assert that no orphaned dialogs or windows remain after each live GUI test. Normalize GUI tests to explicit `Destroy()` and event pumping rather than relying on stack lifetime for shown top-level windows.

Acceptance Criteria:
1. The harness exposes helper(s) to enumerate or find top-level windows/dialogs during a live GUI test and to wait for a launched modal/top-level window to appear before acting on it.
2. The harness exposes a cleanup/assertion path that can prove no orphaned top-level dialogs/windows remain at the end of a test or fixture teardown, while still force-closing stragglers so later tests do not inherit leaked UI state.
3. Harness regression tests cover the new observation/cleanup behavior directly.
4. Any shown parent frames/dialogs in the GUI live tests covered by this story are managed with deterministic destruction and event pumping, not left to implicit stack unwinding.

Documentation Impact:
No documentation update is required unless the new cleanup/assertion helper becomes the required pattern for future GUI fixtures.

### 2. Restore Tactical Live-Dialog Acceptance Coverage
Description:
Finish Subtask 4’s intended tactical GUI coverage by registering the missing live tests, converting the ICM path to prove real modal completion behavior, and asserting the dialog-specific behaviors that the original plan called out.

Acceptance Criteria:
1. `tests/gui/TacticalGuiLiveTest.h` registers the direct `TacticalDamageSummaryGUI` and ICM interaction tests so they run in `GuiTests`.
2. The tactical damage summary test proves the live dialog title, context text, summary text, empty-state text, and deterministic close behavior through real modal execution.
3. The tactical ICM test proves real dialog interaction, modal completion, assigned-count updates, and defender ammo consumption without bypassing the dialog’s production completion path.
4. The tactical fixture leaves no live top-level windows or dialogs behind when each test completes.
5. The resulting tactical GUI coverage now satisfies the original plan intent for live `WXTacticalUI`, `TacticalDamageSummaryGUI`, and real `ICMSelectionGUI` interaction coverage.

Documentation Impact:
No documentation update is expected unless the tactical GUI test section in `AGENTS.md` needs a brief note that live dialog coverage now includes the direct damage-summary and ICM paths.

### 3. Strengthen BattleSim Modal Launch-Chain Proof and Zero-Orphan Teardown
Description:
Upgrade the BattleSim live tests so they assert that child dialogs actually appeared in the launch chain, not just that the parent frame survived. Use harness observation helpers to prove `BattleSimFrame -> LocalGameDialog` and `LocalGameDialog -> ScenarioDialog` / `ScenarioEditorGUI` modal launches, then verify those paths close cleanly with no lingering windows.

Acceptance Criteria:
1. The `BattleSimFrame` launch test proves that clicking `Play a Local Game` actually opens `LocalGameDialog`, not just that the frame remains shown afterward.
2. The `LocalGameDialog` launch-chain test proves that both the predefined-scenario path and the custom-scenario path actually present `ScenarioDialog` and `ScenarioEditorGUI`, respectively.
3. The existing scenario and scenario-editor battle-launch tests continue to prove `FBattleScreen` lifecycle behavior while also leaving zero live top-level windows/dialogs after completion.
4. All BattleSim live tests explicitly destroy shown parent frames/dialogs and pump events before returning.
5. The BattleSim fixture teardown can detect and clean any remaining orphaned windows so the full `GuiTests` run ends with no leftover dialogs on screen.

Documentation Impact:
No documentation update is inherently required unless the repo’s GUI testing guidance needs to mention the explicit “no orphaned windows at test completion” expectation.

## Dependency Ordering
1. Subtask 1 must complete first because the later fixes need reliable modal/window observation and cleanup enforcement.
2. Subtask 2 should run after Subtask 1 because the tactical tests need the new harness helpers and cleanup assertions.
3. Subtask 3 should run after Subtask 2 to avoid duplicate churn in the harness and test idioms.

## Implementer Agent Prompts
### Subtask 1 Prompt
You are the implementer agent

Allowed files:
- `/home/tstephen/repos/SSW/tests/gui/WXGuiTestHarness.h`
- `/home/tstephen/repos/SSW/tests/gui/WXGuiTestHarness.cpp`
- `/home/tstephen/repos/SSW/tests/gui/GuiHarnessTest.h`
- `/home/tstephen/repos/SSW/tests/gui/GuiHarnessTest.cpp`
- `/home/tstephen/repos/SSW/tests/gui/TacticalGuiLiveTest.cpp`
- `/home/tstephen/repos/SSW/tests/gui/BattleSimGuiLiveTest.cpp`

Task:
Extend the shared GUI harness so live GUI tests can detect launched top-level windows/dialogs, wait for modal appearance, and enforce zero-orphan cleanup by fixture end. Update the affected GUI live tests to use explicit destruction/event pumping where shown top-level windows currently depend on implicit lifetime.

Acceptance criteria:
1. The harness can observe or locate launched top-level dialogs/windows during a test.
2. The harness can enforce and clean zero-orphan top-level state at test completion.
3. Harness regression coverage exists for the new observation/cleanup behavior.
4. The tactical and BattleSim GUI live tests touched by this story no longer rely on implicit stack lifetime for shown top-level window cleanup.

Do not report success unless all required artifacts exist and all changes are committed.

### Subtask 2 Prompt
You are the implementer agent

Allowed files:
- `/home/tstephen/repos/SSW/tests/gui/TacticalGuiLiveTest.h`
- `/home/tstephen/repos/SSW/tests/gui/TacticalGuiLiveTest.cpp`
- `/home/tstephen/repos/SSW/tests/gui/WXGuiTestHarness.h`
- `/home/tstephen/repos/SSW/tests/gui/WXGuiTestHarness.cpp`
- Likely only if a minimal GUI-owned seam is required:
  - `/home/tstephen/repos/SSW/src/gui/WXTacticalUI.cpp`
  - `/home/tstephen/repos/SSW/include/gui/WXTacticalUI.h`

Task:
Repair the tactical live GUI regression coverage so the missing direct dialog tests execute and the ICM path proves real dialog completion behavior instead of bypassing it. Keep the work on the GUI side and ensure each tactical GUI test finishes with no leftover top-level windows.

Acceptance criteria:
1. The direct tactical damage-summary and ICM tests are registered in the suite and run under `GuiTests`.
2. The damage-summary test proves title, context, summary, empty-state, and close behavior through real dialog execution.
3. The ICM test proves real selection/finalization behavior and resulting assignment/ammo updates without bypassing the production completion path.
4. The tactical live fixture leaves no orphaned dialogs/windows when each test completes.

Do not report success unless all required artifacts exist and all changes are committed.

### Subtask 3 Prompt
You are the implementer agent

Allowed files:
- `/home/tstephen/repos/SSW/tests/gui/BattleSimGuiLiveTest.h`
- `/home/tstephen/repos/SSW/tests/gui/BattleSimGuiLiveTest.cpp`
- `/home/tstephen/repos/SSW/tests/gui/WXGuiTestHarness.h`
- `/home/tstephen/repos/SSW/tests/gui/WXGuiTestHarness.cpp`
- Likely only if a minimal GUI-owned seam is required:
  - `/home/tstephen/repos/SSW/src/battleSim/BattleSimFrame.cpp`
  - `/home/tstephen/repos/SSW/src/battleSim/LocalGameDialog.cpp`
  - `/home/tstephen/repos/SSW/src/battleSim/ScenarioDialog.cpp`
  - `/home/tstephen/repos/SSW/src/battleSim/ScenarioEditorGUI.cpp`

Task:
Strengthen the BattleSim live GUI tests so they prove the intended modal launch chain actually occurred and that all launched windows/dialogs are closed before the tests finish. Preserve the existing `FBattleScreen` lifecycle assertions while adding launch-proof and cleanup-proof assertions.

Acceptance criteria:
1. The `BattleSimFrame` test proves `LocalGameDialog` was presented.
2. The `LocalGameDialog` test proves `ScenarioDialog` and `ScenarioEditorGUI` were each presented from their respective button paths.
3. The BattleSim scenario-launch tests still verify `FBattleScreen` lifecycle counts and also finish with zero live top-level windows/dialogs.
4. The BattleSim live tests explicitly destroy shown parents and pump events before returning.

Do not report success unless all required artifacts exist and all changes are committed.

## Output Artifact
- `/home/tstephen/repos/SSW/plans/gui-test-warning-remediation-plan.md`
