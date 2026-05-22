# Tactical Battle Board Menu Bar Plan

## Feature Restatement

Add a wxWidgets menu bar to the tactical battle board window so it appears across the top of the battle board and mirrors the strategic window's menu-system style. The initial menu set should be:

- `File` with `Load Game`, `Save Game`, and `Quit`
- `Settings` with `Damage Details`
- `Help` with `User's Guide` and `About`

Only `Quit` should be enabled initially, and it should invoke the battle board's existing window-exit behavior.

## Repository Context

### Confirmed Facts

1. `FBattleScreen` is the tactical battle board window and currently owns tactical close behavior through `closeBattleScreen(int)` and `onClose(wxCloseEvent&)`.
2. `FBattleScreen` currently derives from `wxDialog` and already binds close-window events in [src/tactical/FBattleScreen.cpp](/home/tstephen/repos/SSW/src/tactical/FBattleScreen.cpp:104).
3. The strategic window builds its menu bar directly with `wxMenu` and `wxMenuBar` in [src/FMainFrame.cpp](/home/tstephen/repos/SSW/src/FMainFrame.cpp:34).
4. Shared custom menu identifiers currently live in [include/wxWidgets.h](/home/tstephen/repos/SSW/include/wxWidgets.h:13).
5. Tactical model boundary rules require wx-owned behavior to stay in gui/tactical wx surfaces rather than non-gui model modules.

### Assumptions

1. `Quit` should close only the tactical battle board window through the existing tactical close path, not terminate the entire application process.
2. The requested inactive entries should be present but disabled from first show, with no placeholder dialogs wired yet.

## Subtasks

### TBBM-01 Implement the tactical battle board menu bar on `FBattleScreen`

Scope:

1. Add menu-bar construction to `FBattleScreen` using `wxMenu`/`wxMenuBar` in the same style family as the strategic window.
2. Install three top-level menus in this order: `File`, `Settings`, `Help`.
3. Populate `File` with `Load Game`, `Save Game`, and `Quit`, leaving only `Quit` enabled.
4. Populate `Settings` with disabled `Damage Details`.
5. Populate `Help` with disabled `User's Guide` and `About`.
6. Reuse existing shared menu IDs where appropriate and add new shared IDs only if the tactical entries need distinct identifiers.
7. Wire the enabled `Quit` action to the existing `FBattleScreen` close flow so modal and model-cleanup behavior remain centralized.
8. Preserve the current tactical layout behavior after the menu bar is attached.

Acceptance Criteria:

1. Opening the tactical battle board shows a visible menu bar across the top edge of the window.
2. The top-level menus are exactly `File`, `Settings`, and `Help` in that order.
3. `File -> Quit` is enabled and selecting it closes the battle board through the existing tactical close path.
4. `File -> Load Game`, `File -> Save Game`, `Settings -> Damage Details`, `Help -> User's Guide`, and `Help -> About` are visible and disabled on first show.
5. The change stays confined to wx-owned tactical/window code and any shared wx menu-ID header support; it does not introduce wx menu logic into non-gui model modules.

Documentation Impact:

No rules-document change is allowed. Downstream documentation review should consider whether [doc/UsersGuide.odt](/home/tstephen/repos/SSW/doc/UsersGuide.odt) or [doc/DesignNotes.odt](/home/tstephen/repos/SSW/doc/DesignNotes.odt) mention the tactical battle board chrome; [doc/rules/tactical_operations_manual.md](/home/tstephen/repos/SSW/doc/rules/tactical_operations_manual.md) must remain unchanged.

## Documentation Impact

Likely minor or none. If existing user or design docs enumerate tactical window controls, document the new top-level menu bar there; do not modify [doc/rules/tactical_operations_manual.md](/home/tstephen/repos/SSW/doc/rules/tactical_operations_manual.md).

## Dependency Ordering

1. `TBBM-01` should run first and alone. The menu structure, enabled-state setup, shared ID changes, and quit wiring all touch the same window surface and should not be split into parallel implementer work.

## Implementer Prompts

### TBBM-01

Your role is 'implementer'. Your task is as follows:

Allowed files:
- `include/tactical/FBattleScreen.h`
- `src/tactical/FBattleScreen.cpp`
- `include/wxWidgets.h`

Task to implement:
Add a wxWidgets menu bar to the tactical battle board window implemented by `FBattleScreen`. Build it with the same `wxMenu` / `wxMenuBar` approach used by the strategic main window. The battle board menu bar must appear across the top of the tactical window and contain exactly three top-level menus in this order: `File`, `Settings`, and `Help`. `File` must contain `Load Game`, `Save Game`, and `Quit`. `Settings` must contain `Damage Details`. `Help` must contain `User's Guide` and `About`. Only `Quit` should be enabled initially; every other requested item should be present but disabled. Wire `Quit` to the existing `FBattleScreen` exit path so the current modal/non-modal close handling and tactical cleanup remain centralized rather than duplicating shutdown logic elsewhere.

Acceptance criteria:
- The tactical battle board shows a menu bar across the top when opened.
- The top-level menus are `File`, `Settings`, and `Help` in that order.
- `Quit` is enabled and closes the battle board via existing `FBattleScreen` close behavior.
- `Load Game`, `Save Game`, `Damage Details`, `User's Guide`, and `About` are all present and disabled on first show.
- No wx menu implementation detail is pushed into non-gui tactical/model modules.

Validation guidance:
- Inspect the tactical battle board manually or through existing tactical window launch paths to confirm the menu bar appears at the top and the disabled/enabled states match the specification.
- Build the affected target set with the repository's normal make-based workflow after the menu changes.
- If a focused tactical or GUI regression path is available without widening scope, use it to confirm the battle board still closes cleanly after the `Quit` action.

Tester test-file location guidance:
- `tests/tactical/*`
- `tests/gui/TacticalGuiLiveTest.cpp`
- `tests/gui/GuiTests.cpp`

Artifact guidance:
Use `plans/tactical-battle-board-menu-bar-plan.md` as the controlling plan artifact. No new implementation artifact is required unless implementation uncovers a repository-established need. Do not report completion against this subtask until the code changes are committed.

Startup behavior:
Continue past preflight when blockers are absent.

Completion gate:
Do not report success unless all required artifacts exist and all changes are committed.

## Output Artifact Path

`plans/tactical-battle-board-menu-bar-plan.md`
