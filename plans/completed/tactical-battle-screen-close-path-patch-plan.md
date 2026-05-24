# Tactical Battle Screen Close Path Patch Plan

## Feature Restatement

Patch the current tactical battle-screen close implementation in place so `File -> Quit`, the title-bar `X`, automated GUI cleanup, and modal tactical launch paths actually close or unwind without manual intervention. This plan supersedes `plans/tactical-battle-screen-close-path-fixes-plan-v2.md` without rolling back the current branch history.

## Confirmed Repository Facts

1. The implementation series after `7e6fe41f6eb46acbf7864eecc87d4e5ce7a705dd` converted `FBattleScreen` to a `wxFrame`, added a class-owned `ShowModal()` shim, installed a tactical menu bar, and added `File -> Quit` handling.
2. The current production close logic is centralized in `FBattleScreen::closeBattleScreen(int)` and `FBattleScreen::onClose(wxCloseEvent&)` in `src/tactical/FBattleScreen.cpp`.
3. `closeBattleScreen(int)` currently stores close-in-progress state in `FTacticalGame`, uses `EndModal(...)` for modal shim callers, and calls `Destroy()` directly for non-modal callers.
4. `onClose(wxCloseEvent&)` currently delegates to `closeBattleScreen(GetReturnCode())` and may call `event.Skip()` after the shared helper returns.
5. wxWidgets 3.3.2 `wxTopLevelWindowBase::Destroy()` schedules top-level destruction through `wxPendingDelete` and may leave a frame shown while it is pending deletion, especially when it is the only shown top-level window.
6. wxWidgets 3.3.2 deletes pending top-level windows during idle cleanup, not merely by processing pending wx events.
7. `WXGuiTestHarness::waitForTopLevelWindowClosed(...)` currently treats `IsBeingDeleted()` as success, so tests can pass while a native frame remains visible.
8. `WXGuiTestHarness::cleanupOrphanTopLevels(...)` currently ignores top-level windows already marked `IsBeingDeleted()`, so it can miss exactly the windows left behind by the current close path.
9. The user has observed that the GUI tests require manual window closure, and direct executable testing shows `File -> Quit` and the title-bar `X` do not close the tactical battle screen as intended.
10. Running `xvfb-run -a tests/gui/GuiTests` works when `xvfb-run` is allowed outside the sandbox; the current suite still reports unrelated source-token failures and does not prove close correctness because the close oracle is too weak.

## Assumptions

1. The desired implementation path is patch-in-place, not rollback.
2. `FBattleScreen` should remain frame-backed so the tactical menu bar work remains intact.
3. `File -> Quit` and the title-bar `X` should share one accepted-close handler and should close only the active tactical battle screen, not the full application.
4. Stack-owned modal tactical callers should remain stack-owned and should never be destroyed by wx default frame close handling.
5. It is acceptable to update test infrastructure in this feature because the existing harness behavior is part of the escaped defect.

## Likely Files To Modify

1. `include/tactical/FBattleScreen.h`
2. `src/tactical/FBattleScreen.cpp`
3. `include/tactical/FTacticalGame.h`
4. `src/tactical/FTacticalGame.cpp`
5. `tests/gui/WXGuiTestHarness.h`
6. `tests/gui/WXGuiTestHarness.cpp`
7. `tests/gui/TacticalGuiLiveTest.h`
8. `tests/gui/TacticalGuiLiveTest.cpp`
9. `tests/gui/BattleSimGuiLiveTest.h`
10. `tests/gui/BattleSimGuiLiveTest.cpp`
11. `tests/tactical/FTacticalBattleScreenElectricalFireTest.cpp`
12. `doc/DesignNotes.md`
13. `AGENTS.md`

Files 12-13 are documentation targets only if the implementation changes the documented close lifecycle contract. Do not edit `doc/rules/tactical_operations_manual.md`.

## Unresolved Design Decisions

No material user-facing design decisions remain. The user chose patch-in-place. The behavioral target is defect repair: close actions must visibly close or unwind tactical battle screens and tests must fail if that does not happen.

## Overall Documentation Impact

Update design or agent documentation only where it currently documents the incorrect close behavior or the GUI harness contract. Do not edit `doc/rules/tactical_operations_manual.md`.

## Subtasks

### TBSCPP-001 - Repair FBattleScreen close semantics in place

**Scope**

Repair `FBattleScreen` so the tactical menu quit path and the native title-bar close path enter one accepted-close handler, hide non-modal frames immediately, destroy heap-owned non-modal frames safely, and unwind stack-owned modal callers without allowing wx default frame destruction to run on stack objects.

**Implementation Direction**

1. Move close-in-progress state into `FBattleScreen` instead of `FTacticalGame`. If no other valid model use remains, remove the `FTacticalGame` close guard API and member.
2. Make `File -> Quit` request a normal close, for example by calling `Close(true)` or a small `requestClose(returnCode)` wrapper, so menu and title-bar close share `onClose(...)`.
3. Make `onClose(wxCloseEvent&)` handle accepted close events completely. Once the tactical screen accepts close, do not call `event.Skip()`.
4. In modal shim mode, call `EndModal(returnCode)` and return. Do not call `Destroy()` or allow default close handling for stack-owned modal `FBattleScreen` instances.
5. In non-modal mode, set the return code, call `Hide()` before `Destroy()`, and leave deletion to wx pending-delete cleanup.
6. Ensure wxGTK title-bar close reaches the frame-backed modal shim when another modal dialog is open, such as by setting the appropriate top-level extra style for the modal compatibility surface or by an equivalent narrow wx-compatible approach.
7. Keep close behavior scoped to `FBattleScreen`; do not call `exit(...)`, `ExitMainLoop(...)`, or add caller-specific destruction logic.

**Acceptance Criteria**

1. Non-modal tactical screens launched from the strategic frame visibly disappear after `File -> Quit`.
2. Non-modal tactical screens launched from the strategic frame visibly disappear after the title-bar `X`.
3. Stack-owned modal tactical screens launched from BattleSim or strategic modal callers return from `FBattleScreen::ShowModal()` after `File -> Quit`.
4. Stack-owned modal tactical screens launched from BattleSim or strategic modal callers return from `FBattleScreen::ShowModal()` after the title-bar `X`.
5. The modal title-bar `X` path does not destroy a stack-owned `FBattleScreen`.
6. The close-in-progress guard cannot leave the first valid close request stuck open.
7. Close state is no longer owned by tactical model state unless a clear non-UI model reason remains and is documented in code comments.
8. The implementation does not terminate the application process or close unrelated parent windows.

**Documentation Impact**

Update `FBattleScreen` Doxygen comments to describe the actual frame close lifecycle. Update broader docs only if they currently describe the old model-owned guard or the previous pending-delete behavior.

### TBSCPP-002 - Fix GUI harness close oracle and tactical close coverage

**Scope**

Repair the GUI harness and close-path tests so automated validation fails when a frame is merely pending deletion but still shown, and so the test suite no longer requires manual window closure.

**Implementation Direction**

1. Update `WXGuiTestHarness::pumpEvents(...)` or add a narrow helper so GUI tests process idle cleanup after pending events when waiting for top-level deletion.
2. Change `waitForTopLevelWindowClosed(...)` so `IsBeingDeleted()` alone is not considered a successful close when the window is still shown.
3. Update `cleanupOrphanTopLevels(...)` so it can handle shown top-level windows even when they are already pending deletion, and so it hides or closes residual shown windows before waiting for cleanup.
4. Update tactical live close tests to assert the stronger observable: the battle screen is not shown or is absent, and lifecycle counters settle when the object is expected to be deleted.
5. Add or repair live coverage for both non-modal close vectors and both modal close vectors. The modal title-bar `X` coverage must exercise a real close event path against a modal-shim `FBattleScreen`, not merely direct `Destroy()`.
6. Update source-contract tactical tests so they protect the repaired lifecycle invariants and no longer require `event.Skip()`, direct non-modal `Destroy()` without `Hide()`, or model-owned close guard state.

**Acceptance Criteria**

1. GUI close tests fail if a tactical battle screen remains visible after a close request.
2. GUI cleanup no longer requires manual operator window closure after tactical close tests.
3. Non-modal tactical `File -> Quit` coverage dispatches the actual `ID_TacticalQuit` command or invokes the actual menu close path and proves the window is no longer shown.
4. Non-modal tactical title-bar close coverage proves the window is no longer shown.
5. Modal tactical `File -> Quit` coverage proves the modal caller returns and no live `FBattleScreen` instance remains afterward.
6. Modal tactical title-bar close coverage proves the modal caller returns and no live `FBattleScreen` instance remains afterward.
7. Harness close waits process idle cleanup sufficiently for wx pending-delete top-level windows.
8. Tactical source-contract coverage rejects the old broken pattern of accepting `IsBeingDeleted()` as enough or skipping wx default handling after the shared close helper has accepted close.

**Documentation Impact**

Update `WXGuiTestHarness.h` comments for any changed close-wait or cleanup semantics. Update tactical live-test comments only where they currently describe the weaker pending-delete behavior.

## Dependency Ordering

1. `TBSCPP-001` must run first.
   Reason: the harness and live tests should encode the repaired runtime behavior, not the current broken implementation.
2. `TBSCPP-002` must run second.
   Reason: test oracle hardening depends on knowing the intended final close semantics and modal behavior.

Parallelization: Do not parallelize these subtasks. They overlap in close lifecycle semantics and regression coverage.

## Overall Acceptance Criteria

1. The tactical battle screen closes visibly from `File -> Quit` in the compiled runtime.
2. The tactical battle screen closes visibly from the title-bar `X` in the compiled runtime.
3. Stack-owned modal tactical launch paths unwind cleanly from both close vectors.
4. Automated GUI tests no longer require manual window closure for tactical battle-screen close paths.
5. The test harness no longer treats a shown pending-delete top-level as closed.
6. The fix is patch-in-place and preserves the current frame-backed menu-bar direction.
7. No implementation change requires editing `doc/rules/tactical_operations_manual.md`.

## Implementer Prompts

### Implementer Prompt: TBSCPP-001

Your role is 'implementer'. Your task is as follows:

Allowed files:
- `include/tactical/FBattleScreen.h`
- `src/tactical/FBattleScreen.cpp`
- `include/tactical/FTacticalGame.h`
- `src/tactical/FTacticalGame.cpp`
- `src/FMainFrame.cpp`
- `src/gui/SelectCombatGUI.cpp`
- `src/battleSim/ScenarioDialog.cpp`
- `src/battleSim/ScenarioEditorGUI.cpp`

Task:
- Patch the current `FBattleScreen` close lifecycle in place so `File -> Quit` and the title-bar `X` use one accepted-close path and actually close or unwind the tactical screen.
- Keep `FBattleScreen` frame-backed and preserve the existing menu-bar direction.
- Move close-in-progress state out of `FTacticalGame` and into `FBattleScreen` unless you find a clear non-UI model reason to keep it in the model. If the `FTacticalGame` close API becomes unused, remove it.
- Make `File -> Quit` request the same close path as the title-bar close, such as through `Close(true)` or a narrow wrapper.
- Ensure `onClose(wxCloseEvent&)` handles accepted close requests completely. Do not skip to wx default close handling after the tactical close has been accepted.
- For non-modal tactical screens, hide the frame before scheduling destruction so the user-visible window disappears immediately.
- For modal shim paths, call `EndModal(returnCode)` and return without destroying stack-owned `FBattleScreen` instances.
- Ensure wxGTK title-bar close can reach the modal-shim battle screen while another modal dialog is open, using a narrow wx-compatible approach.
- Do not add caller-specific tactical destruction logic unless repository evidence proves a specific launch site is still broken after the shared lifecycle is repaired.

Acceptance criteria:
- Non-modal tactical screens launched from the strategic frame visibly disappear after `File -> Quit`.
- Non-modal tactical screens launched from the strategic frame visibly disappear after the title-bar `X`.
- Stack-owned modal tactical screens launched from BattleSim or strategic modal callers return from `FBattleScreen::ShowModal()` after `File -> Quit`.
- Stack-owned modal tactical screens launched from BattleSim or strategic modal callers return from `FBattleScreen::ShowModal()` after the title-bar `X`.
- The modal title-bar `X` path does not destroy a stack-owned `FBattleScreen`.
- The close-in-progress guard cannot leave the first valid close request stuck open.
- Close state is no longer owned by tactical model state unless a clear non-UI model reason remains and is documented in code comments.
- The implementation does not call `exit(...)`, does not call `ExitMainLoop(...)`, and does not close unrelated parent windows.

Validation guidance:
- Build the tactical runner with `cd tests && make tactical-tests`.
- Run `cd tests && ./tactical/TacticalTests`.
- Build the GUI runner with `cd tests/gui && make`.
- Run GUI validation from the repository root with `xvfb-run -a tests/gui/GuiTests` when an X display or approved `xvfb-run` execution is available.
- Also manually validate the compiled executable path if practical: open a tactical battle screen and confirm both `File -> Quit` and the title-bar `X` close the tactical window without closing the parent application.

Tester test-file location guidance:
- Expected source-contract updates are likely in `tests/tactical/FTacticalBattleScreenElectricalFireTest.cpp`.
- Expected live GUI follow-up coverage is likely in `tests/gui/TacticalGuiLiveTest.cpp` and `tests/gui/BattleSimGuiLiveTest.cpp`.

Artifact directory guidance:
- If no shared artifact directory is provided by orchestration context, default to `artifacts/tactical-battle-screen-close-path-patch-plan/TBSCPP-001/`.

Execution instruction:
- Begin implementation immediately using the files above if no blocking repository issue is discovered. Do not stop after preflight restatement alone.

Completion gate:
- Do not report success unless all required artifacts exist and all changes are committed.

### Implementer Prompt: TBSCPP-002

Your role is 'implementer'. Your task is as follows:

Allowed files:
- `tests/gui/WXGuiTestHarness.h`
- `tests/gui/WXGuiTestHarness.cpp`
- `tests/gui/TacticalGuiLiveTest.h`
- `tests/gui/TacticalGuiLiveTest.cpp`
- `tests/gui/BattleSimGuiLiveTest.h`
- `tests/gui/BattleSimGuiLiveTest.cpp`
- `tests/tactical/FTacticalBattleScreenElectricalFireTest.cpp`

Task:
- Patch the GUI harness and tactical close regressions so the automated suite proves windows actually become hidden or absent after close requests.
- Update `WXGuiTestHarness::pumpEvents(...)` or add a narrow helper so close waits can process wx idle cleanup after pending events.
- Change `waitForTopLevelWindowClosed(...)` so `IsBeingDeleted()` alone is not treated as success while the top-level remains shown.
- Update `cleanupOrphanTopLevels(...)` so shown top-level windows already pending deletion are not ignored and no tactical battle-screen windows require manual operator closure.
- Strengthen tactical close tests so non-modal `File -> Quit`, non-modal title-bar close, modal `File -> Quit`, and modal title-bar close all prove the correct observable behavior.
- Update source-contract tests so they protect the repaired lifecycle and stop locking in the broken `event.Skip()` or model-owned close-guard pattern.

Acceptance criteria:
- GUI close tests fail if a tactical battle screen remains visible after a close request.
- GUI cleanup no longer requires manual operator window closure after tactical close tests.
- Non-modal tactical `File -> Quit` coverage dispatches the actual `ID_TacticalQuit` command or invokes the actual menu close path and proves the window is no longer shown.
- Non-modal tactical title-bar close coverage proves the window is no longer shown.
- Modal tactical `File -> Quit` coverage proves the modal caller returns and no live `FBattleScreen` instance remains afterward.
- Modal tactical title-bar close coverage proves the modal caller returns and no live `FBattleScreen` instance remains afterward.
- Harness close waits process idle cleanup sufficiently for wx pending-delete top-level windows.
- Tactical source-contract coverage rejects the old broken pattern of accepting `IsBeingDeleted()` as enough or skipping wx default handling after tactical close has already been accepted.

Validation guidance:
- Build and run the tactical source-level runner with `cd tests && make tactical-tests && ./tactical/TacticalTests`.
- Build the GUI runner with `cd tests/gui && make`.
- Run GUI validation from the repository root with `xvfb-run -a tests/gui/GuiTests` when an X display or approved `xvfb-run` execution is available.
- During GUI validation, confirm the suite does not require manual window closure for tactical battle-screen tests.

Tester test-file location guidance:
- Expected live GUI coverage changes are likely in `tests/gui/TacticalGuiLiveTest.cpp` and `tests/gui/BattleSimGuiLiveTest.cpp`.
- Expected harness behavior changes are likely in `tests/gui/WXGuiTestHarness.cpp`.
- Expected source-contract changes are likely in `tests/tactical/FTacticalBattleScreenElectricalFireTest.cpp`.

Artifact directory guidance:
- If no shared artifact directory is provided by orchestration context, default to `artifacts/tactical-battle-screen-close-path-patch-plan/TBSCPP-002/`.

Execution instruction:
- Begin implementation immediately using the files above if no blocking repository issue is discovered. Do not stop after preflight restatement alone.

Completion gate:
- Do not report success unless all required artifacts exist and all changes are committed.

## Output Artifact Path

`plans/tactical-battle-screen-close-path-patch-plan.md`
