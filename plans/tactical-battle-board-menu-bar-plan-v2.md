# Tactical Battle Board Menu Bar Plan v2

## Feature Restatement
Add a native wxWidgets menu bar across the top of the tactical battle board window. The initial menu bar has three menus: `File`, `Settings`, and `Help`. `File` contains `Load Game`, `Save Game`, and `Quit`, with only `Quit` enabled and wired to close the tactical battle board. `Settings` contains disabled `Damage Details`. `Help` contains disabled `User's Guide` and `About`.

The prior plan at `plans/tactical-battle-board-menu-bar-plan.md` is intentionally ignored. This plan is based on fresh repository inspection.

## Confirmed Repository Facts
- The strategic game window, `FMainFrame`, builds its menu bar with `wxMenu`, `wxMenuBar`, and `SetMenuBar(...)` in `src/FMainFrame.cpp`.
- The tactical battle board is `FBattleScreen` in `include/tactical/FBattleScreen.h` and `src/tactical/FBattleScreen.cpp`.
- `FBattleScreen` is currently a `wxDialog`, while wxWidgets 3.3 exposes `SetMenuBar(...)` on `wxFrame`, not `wxDialog`.
- Existing tactical battle launch paths include non-modal strategic launch from `src/FMainFrame.cpp` and modal stack-owned launch from `src/gui/SelectCombatGUI.cpp`, `src/battleSim/ScenarioDialog.cpp`, and `src/battleSim/ScenarioEditorGUI.cpp`.
- `FBattleScreen::closeBattleScreen(...)` already centralizes tactical close behavior with a modal-first path and a non-modal `Destroy()` path.

## Assumptions
- "Use wxWidgets' menu system just like the strategic game window" means a native `wxMenuBar` attached with `SetMenuBar(...)`, not a row of buttons or custom-painted controls.
- `Quit` should close only the current tactical battle board window, not terminate the whole application process.
- Disabled placeholder entries should be visible in the menus but not execute any behavior.

## Material Decisions
No further user-facing decisions remain. The native menu requirement determines that `FBattleScreen` must become frame-backed or otherwise expose the same native frame menu-bar surface while preserving current tactical launch behavior.

## Subtasks

### TBBM-001 - Make FBattleScreen Native-Menu-Capable Without Breaking Launch Lifecycle
Change `FBattleScreen` from a dialog-only top-level into a native menu-capable tactical top-level, preserving its current constructor surface and modal/non-modal launch contracts.

Acceptance Criteria:
- `FBattleScreen` can use `SetMenuBar(...)` and `GetMenuBar()` through a native wxWidgets frame menu-bar surface.
- Existing call sites that construct `FBattleScreen` on the stack and call `ShowModal()` remain source-compatible and block until the battle screen closes.
- Existing non-modal call sites that allocate `FBattleScreen` and call `Show(true)` remain source-compatible and destroy the battle screen through the guarded close path.
- `closeBattleScreen(...)` keeps one modal-first close path and one non-modal destroy path; it must not call `exit(...)` or close the parent application.
- Lifecycle counters still return to zero live instances after modal and non-modal battle screen close flows.
- `FBattleBoard` and `FBattleDisplay` still receive a valid `FBattleScreen*` parent and existing map/display layout behavior remains intact below the future menu bar.

Documentation Impact:
- Update `include/tactical/FBattleScreen.h` Doxygen comments to reflect frame/menu-capable top-level behavior and any modal-compatibility methods added or changed.
- Do not edit `doc/rules/tactical_operations_manual.md`.

### TBBM-002 - Add Tactical Menu Bar Items And Quit Wiring
Build and install the tactical menu bar on `FBattleScreen`, following the strategic window's wx menu construction pattern while keeping all placeholder entries disabled.

Acceptance Criteria:
- `FBattleScreen` installs a `wxMenuBar` during construction using `wxMenu`, `wxMenuBar`, and `SetMenuBar(...)`.
- The top-level menus appear in this order: `File`, `Settings`, `Help`.
- `File` has visible entries `Load Game`, `Save Game`, and `Quit`; `Load Game` and `Save Game` are disabled, and `Quit` is enabled.
- `Settings` has visible entry `Damage Details`, disabled.
- `Help` has visible entries `User's Guide` and `About`, both disabled.
- Selecting `Quit` invokes the existing tactical close path, so modal callers return and non-modal windows are destroyed without terminating the process.
- Disabled entries do not have active handlers or side effects.

Documentation Impact:
- Update `FBattleScreen` header comments for any new menu-building or menu-command handler declarations.
- User-facing docs may later mention the new tactical menu placeholders, but no rules-document change is allowed or expected.

## Dependency Ordering
1. `TBBM-001` must be completed first because native menu attachment depends on a frame menu-bar surface.
2. `TBBM-002` depends on `TBBM-001` and should be implemented after the battle screen lifecycle remains stable.
3. No subtasks are safe to run in parallel because both touch `FBattleScreen` construction and close behavior.

## Overall Acceptance Criteria
- The tactical battle board displays a native wxWidgets menu bar across the top of the battle board window.
- Only `File -> Quit` is enabled and operational in the initial menu set.
- `Quit` closes the tactical board consistently for strategic non-modal launch, strategic combat modal launch, and BattleSim scenario/editor modal launch.
- Existing tactical board layout, action buttons, map rendering, and lower display panel remain usable after the menu bar is added.
- No implementation changes require editing `doc/rules/tactical_operations_manual.md`.

## Documentation Impact
Overall documentation impact is limited. Header Doxygen for `FBattleScreen` should be updated with any changed inheritance/lifecycle/menu methods. The Documenter stage may update `doc/UsersGuide.md` if the project wants user-facing tactical menu documentation, but `doc/rules/tactical_operations_manual.md` must remain unchanged.

## Implementer Prompts

### TBBM-001 Prompt
Your role is 'implementer'. Your task is as follows:

Allowed files:
- `include/tactical/FBattleScreen.h`
- `src/tactical/FBattleScreen.cpp`
- `src/FMainFrame.cpp`
- `src/gui/SelectCombatGUI.cpp`
- `src/battleSim/ScenarioDialog.cpp`
- `src/battleSim/ScenarioEditorGUI.cpp`

Task to implement:
Make `FBattleScreen` native-menu-capable by moving it from the current `wxDialog` inheritance model to a `wxFrame`-backed top-level that can use `SetMenuBar(...)`. Preserve the existing public constructor defaults and keep the existing tactical launch API source-compatible, including stack-owned `FBattleScreen bb; bb.ShowModal();` modal paths and heap-owned `bb->Show(true);` non-modal paths. Add class-owned modal compatibility methods and return-code storage where needed because `wxFrame` does not provide `wxDialog` modal APIs. Do not add the requested menu items in this subtask beyond whatever scaffolding is necessary to compile.

Acceptance criteria:
- `FBattleScreen` can compile as a native menu-capable top-level with access to `SetMenuBar(...)` and `GetMenuBar()`.
- Existing modal launch call sites remain source-compatible and still block until the battle screen closes.
- Existing non-modal launch call sites remain source-compatible and still destroy the battle screen through `closeBattleScreen(...)`.
- `closeBattleScreen(...)` preserves the modal-first behavior before the non-modal `Destroy()` behavior and does not terminate the app process.
- Lifecycle counters still represent constructed, destroyed, and live battle screens accurately.
- Existing `FBattleBoard` and `FBattleDisplay` parent relationships and layout policy remain intact.

Validation guidance:
- Build the affected modules with `cd tests && make tactical-tests`.
- Run `cd tests && ./tactical/TacticalTests`.
- Build the GUI suite with `cd tests/gui && make`.
- If a display or xvfb is available, run `cd tests/gui && xvfb-run -a ./GuiTests`.

Tester test-file location guidance:
- Lifecycle and close-path source-contract coverage belongs in `tests/tactical/FTacticalBattleScreenElectricalFireTest.*`.
- Live launch and top-level lifecycle coverage belongs in `tests/gui/TacticalGuiLiveTest.*`, `tests/gui/StrategicGuiLiveTest.*`, and `tests/gui/BattleSimGuiLiveTest.*`.

Artifact guidance:
Write implementation artifacts under `artifacts/tactical-battle-board-menu-bar-plan-v2/TBBM-001/` using the active implementer role conventions.

Startup behavior:
Continue past preflight when blockers are absent.

Completion gate:
Do not report success unless all required artifacts exist and all changes are committed.

### TBBM-002 Prompt
Your role is 'implementer'. Your task is as follows:

Allowed files:
- `include/wxWidgets.h`
- `include/tactical/FBattleScreen.h`
- `src/tactical/FBattleScreen.cpp`

Task to implement:
Add the tactical battle board menu bar to `FBattleScreen` using wxWidgets menus in the same native pattern as `FMainFrame`: construct `wxMenu` instances, append them to a `wxMenuBar`, and install the menu bar with `SetMenuBar(...)`. Add any tactical-specific command IDs and an `FBattleScreen` menu handler needed for `Quit`. Keep `Load Game`, `Save Game`, `Damage Details`, `User's Guide`, and `About` visible but disabled and unwired. Wire only `Quit` to the existing tactical close path so it closes the current battle board in both modal and non-modal launch contexts.

Acceptance criteria:
- `FBattleScreen::GetMenuBar()` returns a menu bar with top-level menus `File`, `Settings`, and `Help` in that order.
- `File` contains visible `Load Game`, `Save Game`, and `Quit` entries; only `Quit` is enabled.
- `Settings` contains disabled `Damage Details`.
- `Help` contains disabled `User's Guide` and disabled `About`.
- Triggering the `Quit` menu command calls the tactical close path and does not call `exit(...)`.
- Disabled menu entries have no active behavior.

Validation guidance:
- Build and run the tactical tests with `cd tests && make tactical-tests && ./tactical/TacticalTests`.
- Build the GUI tests with `cd tests/gui && make`.
- If a display or xvfb is available, run `cd tests/gui && xvfb-run -a ./GuiTests`.

Tester test-file location guidance:
- Menu label, order, enabled-state, and quit-close live coverage belongs in `tests/gui/TacticalGuiLiveTest.*`.
- Source-contract checks for using native `wxMenu` / `wxMenuBar` / `SetMenuBar(...)` can go in `tests/tactical/FTacticalBattleScreenElectricalFireTest.*` or a focused tactical source-contract fixture if the Tester prefers.

Artifact guidance:
Write implementation artifacts under `artifacts/tactical-battle-board-menu-bar-plan-v2/TBBM-002/` using the active implementer role conventions.

Startup behavior:
Continue past preflight when blockers are absent.

Completion gate:
Do not report success unless all required artifacts exist and all changes are committed.

## Output Artifact Path
`plans/tactical-battle-board-menu-bar-plan-v2.md`
