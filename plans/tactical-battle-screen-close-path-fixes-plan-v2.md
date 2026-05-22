# Tactical Battle Screen Close Path Fixes Plan v2

## Feature Restatement

Fix the tactical battle screen so both real user close vectors work again after the recent frame/menu changes: `File -> Quit` must close the battle screen, and the title-bar `X` must also close it. In addition, the automated GUI coverage must stop leaving the tactical battle screen open and requiring manual operator intervention to finish.

The prior plan at `plans/tactical-battle-screen-close-path-fixes-plan.md` is intentionally ignored. This plan is based on fresh repository inspection.

## Confirmed Repository Facts

1. `FBattleScreen` is the tactical battle-screen top-level in `include/tactical/FBattleScreen.h` and `src/tactical/FBattleScreen.cpp`.
2. `FBattleScreen` is currently `wxFrame`-backed, installs the tactical menu bar in its constructor, and binds both `wxEVT_MENU` for `ID_TacticalQuit` and `wxEVT_CLOSE_WINDOW` to battle-screen handlers in `src/tactical/FBattleScreen.cpp`.
3. The shared tactical close helper is `FBattleScreen::closeBattleScreen(int)`. It currently guards with `m_tacticalGame->isCloseInProgress()`, sets that guard true, uses `EndModal(...)` for modal compatibility mode, and otherwise falls through to non-modal `Destroy()`.
4. `FBattleScreen::onClose(wxCloseEvent &)` currently delegates into `closeBattleScreen(GetReturnCode())` and only calls `event.Skip()` when the close guard is already active.
5. Strategic launch still opens the tactical screen non-modally with `FBattleScreen *bb = new FBattleScreen(); bb->Show(true);` in `src/FMainFrame.cpp`.
6. BattleSim and other tactical modal callers still use stack-owned `FBattleScreen bb; ... bb.ShowModal();` flows, including `src/battleSim/ScenarioDialog.cpp` and `src/gui/SelectCombatGUI.cpp`.
7. The current live tactical GUI test named `testBattleScreenMenuQuitClosesViaSharedClosePath` in `tests/gui/TacticalGuiLiveTest.cpp` does not dispatch the actual `ID_TacticalQuit` command. It currently calls `battleScreen->Close()` directly.
8. The tactical source-contract coverage in `tests/tactical/FTacticalBattleScreenElectricalFireTest.cpp` currently locks in the existing close-helper sequence, including the model-owned close guard and direct non-modal `Destroy()` path.
9. The shared GUI harness in `tests/gui/WXGuiTestHarness.*` can discover top-level windows, wait for them, and clean up shown orphan top levels, but it has no battle-screen-specific helper for waiting until a frame-backed window is fully hidden or deleted after an asynchronous close request.

## Assumptions

1. Both reported failures are regressions from the recent `FBattleScreen` frame/menu-bar lifecycle work, not new product requirements.
2. `File -> Quit` and the title-bar `X` should close only the active tactical battle screen, not terminate the full SSW or BattleSim process and not close the parent strategic or BattleSim window.
3. The correct fix is to keep one shared tactical close contract and repair its lifecycle semantics, not to add separate ad hoc implementations for menu close, frame close, and test cleanup.
4. Automated GUI validation should exercise real runtime close vectors and rely on fallback orphan cleanup only after assertions or timeout handling, not as the primary proof that the screen closed.

## Likely Files To Modify

1. `include/tactical/FBattleScreen.h`
2. `src/tactical/FBattleScreen.cpp`
3. `include/tactical/FTacticalGame.h`
4. `src/tactical/FTacticalGame.cpp`
5. `tests/gui/TacticalGuiLiveTest.h`
6. `tests/gui/TacticalGuiLiveTest.cpp`
7. `tests/gui/WXGuiTestHarness.h`
8. `tests/gui/WXGuiTestHarness.cpp`
9. `tests/tactical/FTacticalBattleScreenElectricalFireTest.h`
10. `tests/tactical/FTacticalBattleScreenElectricalFireTest.cpp`
11. `src/FMainFrame.cpp`
12. `src/gui/SelectCombatGUI.cpp`
13. `src/battleSim/ScenarioDialog.cpp`
14. `src/battleSim/ScenarioEditorGUI.cpp`

Files 11-14 are launch-path validation surfaces and may not need changes if the shared `FBattleScreen` lifecycle fix is sufficient.

## Unresolved Design Decisions

No material user-facing design decisions remain. The requested behavior is a defect repair: the tactical screen must close correctly from both intended user actions and the GUI tests must prove that behavior without manual intervention.

## Overall Documentation Impact

Documentation impact should stay narrow. Update Doxygen comments for any changed lifecycle helpers or new GUI-harness helpers. Do not edit `doc/rules/tactical_operations_manual.md`.

## Subtasks

### TBCP-001 - Repair the shared FBattleScreen runtime close lifecycle

**Scope**

Repair the frame-backed tactical close path so the actual runtime `File -> Quit` command and the title-bar `X` both close the tactical battle screen reliably. Keep one shared close contract inside `FBattleScreen`, and if the current `closeInProgress` guard, modal-return handling, or non-modal `Destroy()` sequencing prevents the first close request from completing, fix that lifecycle rather than creating divergent code paths.

**Acceptance Criteria**

1. In the strategic non-modal launch path, triggering `File -> Quit` closes the tactical battle-screen window without terminating the parent application.
2. In the strategic non-modal launch path, clicking the title-bar `X` closes the tactical battle-screen window without terminating the parent application.
3. In stack-owned modal launch paths using `FBattleScreen::ShowModal()`, both close vectors unwind the modal compatibility loop and return control to the caller without leaving the tactical window visible or hung.
4. `FBattleScreen` retains a single shared close-path implementation for tactical shutdown behavior instead of separate menu-only and frame-only implementations.
5. Any close reentrancy guard permits the first close request to finish cleanly and does not leave the battle screen stuck open after a legitimate close action.
6. The repaired close path does not call `exit(...)`, does not shut down the main wx application loop, and does not require caller-specific tactical-window destruction logic.
7. Battle-screen lifecycle bookkeeping remains coherent after close, including modal return-code handling and zero residual live battle-screen instances once the relevant close flow has completed.

**Documentation Impact**

1. Update `FBattleScreen` header Doxygen to reflect any changed close-lifecycle helpers or guard ownership.
2. Do not edit `doc/rules/tactical_operations_manual.md`.

### TBCP-002 - Harden unattended GUI close-path coverage and harness support

**Scope**

Replace the current surrogate tactical close test with live coverage that drives the real tactical close vectors, and add any narrow GUI-harness support needed so the tests can wait for frame-backed battle-screen shutdown deterministically. Update source-contract coverage only enough to describe the intended repaired close contract instead of freezing the current broken sequencing.

**Acceptance Criteria**

1. `tests/gui/TacticalGuiLiveTest.cpp` dispatches the real `ID_TacticalQuit` command against a shown `FBattleScreen` and proves the screen closes without manual interaction.
2. Live tactical GUI coverage includes a distinct title-bar close path, driven by an actual close event or equivalent top-level close action rather than by direct `Destroy()`.
3. If frame-backed battle-screen shutdown remains asynchronous, `WXGuiTestHarness` provides a bounded helper or equivalent deterministic pattern for waiting until the tactical top-level is hidden or deleted, and the tactical close tests use that seam.
4. Tactical GUI close-path validation no longer depends on a human clicking `X` to finish the suite.
5. Tactical source-contract coverage still protects the intended shared close-path structure and menu binding, but it no longer blocks necessary lifecycle repairs by encoding the exact broken sequence as immutable behavior.
6. GUI-suite teardown still leaves no orphaned shown top-level windows after the tactical close tests complete.

**Documentation Impact**

1. Update Doxygen comments in `tests/gui/WXGuiTestHarness.h` if new wait/cleanup helpers are added.
2. Update tactical live-test comments if the close-path assertions or cleanup strategy materially change.

## Dependency Ordering

1. `TBCP-001` must be completed first.
   Reason: the tests should encode the repaired runtime lifecycle, not the current broken or uncertain one.
2. `TBCP-002` must run second.
   Reason: the GUI tests and any harness helper should lock in the final close semantics after the shared close path is corrected.

Parallelization note: these subtasks should be treated as sequential. They overlap in `FBattleScreen` lifecycle semantics and the regression surfaces that must validate those semantics.

## Overall Acceptance Criteria

1. In the compiled tactical runtime, both `File -> Quit` and the title-bar `X` close the current battle-screen window correctly.
2. The repaired tactical close behavior works for both strategic non-modal launch and stack-owned modal tactical launch flows.
3. The tactical close path remains centralized and closes only the tactical battle screen, not the entire application process.
4. Automated GUI validation of the tactical battle screen completes without manual operator closure of a stuck battle-screen window.
5. No implementation changes require editing `doc/rules/tactical_operations_manual.md`.

## Implementer Prompts

### Implementer Prompt: TBCP-001

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
- Repair the shared `FBattleScreen` close lifecycle so the real tactical `File -> Quit` command and the title-bar close button both shut the tactical window correctly after the recent frame/menu transition.
- Preserve the existing strategic non-modal launch and stack-owned modal `ShowModal()` launch shapes.
- If the current close guard, modal unwind path, or non-modal destroy sequencing prevents closure, fix that shared lifecycle rather than adding divergent tactical menu and frame close implementations.
- Keep tactical shutdown scoped to the battle screen only. Do not terminate the application process and do not introduce caller-specific tactical destruction logic unless a narrow launch-path adjustment is truly required.

Acceptance criteria:
- `File -> Quit` closes the non-modal strategic battle screen.
- The title-bar `X` closes the non-modal strategic battle screen.
- Modal tactical launch paths unwind and return to their callers after either close vector.
- The shared tactical close path remains centralized in `FBattleScreen`.
- A first legitimate close request can complete cleanly without leaving the screen stuck open due to guard state.
- The implementation does not call `exit(...)` and does not stop the main application loop as part of closing only the tactical window.

Validation guidance:
- Build the tactical runner with `cd tests && make tactical-tests`.
- Run `cd tests && ./tactical/TacticalTests`.
- Build the GUI runner with `cd tests/gui && make`.
- If a display or xvfb is available, run `cd tests/gui && xvfb-run -a ./GuiTests`.

Expected Tester file locations:
- `tests/tactical/FTacticalBattleScreenElectricalFireTest.cpp`
- `tests/gui/TacticalGuiLiveTest.cpp`
- `tests/gui/BattleSimGuiLiveTest.cpp` if modal caller-owned launch behavior needs end-to-end proof

Artifact directory guidance:
- If no shared artifact directory is provided by orchestration context, default to `artifacts/tactical-battle-screen-close-path-fixes-plan-v2/TBCP-001/`.

Execution instruction:
- Begin implementation immediately using the files above if no blocking repository issue is discovered. Do not stop after preflight restatement alone.

Completion gate:
- Do not report success unless all required artifacts exist and all changes are committed.

### Implementer Prompt: TBCP-002

Your role is 'implementer'. Your task is as follows:

Allowed files:
- `tests/gui/TacticalGuiLiveTest.h`
- `tests/gui/TacticalGuiLiveTest.cpp`
- `tests/gui/WXGuiTestHarness.h`
- `tests/gui/WXGuiTestHarness.cpp`
- `tests/tactical/FTacticalBattleScreenElectricalFireTest.h`
- `tests/tactical/FTacticalBattleScreenElectricalFireTest.cpp`

Task:
- Replace the current surrogate tactical close test with live coverage that drives the real tactical close vectors.
- Dispatch the actual `ID_TacticalQuit` command in the tactical GUI regression, add distinct top-level close coverage, and add any narrow GUI-harness helper needed so the frame-backed tactical window can be awaited deterministically after an asynchronous close request.
- Update tactical source-contract coverage only as needed so it protects the intended shared close-path behavior and menu binding without freezing the current broken close sequencing.

Acceptance criteria:
- Tactical live GUI coverage dispatches the actual `ID_TacticalQuit` menu command and proves the battle screen closes without manual intervention.
- Tactical live GUI coverage separately proves the title-bar close vector.
- If asynchronous frame deletion needs explicit handling, the GUI harness exposes a bounded wait/cleanup seam and the tactical close tests use it.
- The tactical GUI suite no longer requires a human to click `X` to finish the battle-screen close test.
- Tactical source-contract checks still cover the shared close path and tactical quit binding after the regression update.

Validation guidance:
- Build and run the tactical source-level runner with `cd tests && make tactical-tests && ./tactical/TacticalTests`.
- Build the GUI runner with `cd tests/gui && make`.
- If a display or xvfb is available, run `cd tests/gui && xvfb-run -a ./GuiTests`.

Expected Tester file locations:
- `tests/gui/TacticalGuiLiveTest.cpp`
- `tests/gui/WXGuiTestHarness.cpp`
- `tests/tactical/FTacticalBattleScreenElectricalFireTest.cpp`

Artifact directory guidance:
- If no shared artifact directory is provided by orchestration context, default to `artifacts/tactical-battle-screen-close-path-fixes-plan-v2/TBCP-002/`.

Execution instruction:
- Begin implementation immediately using the files above if no blocking repository issue is discovered. Do not stop after preflight restatement alone.

Completion gate:
- Do not report success unless all required artifacts exist and all changes are committed.

## Output Artifact Path

`plans/tactical-battle-screen-close-path-fixes-plan-v2.md`
