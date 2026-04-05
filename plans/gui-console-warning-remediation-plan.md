# GUI Console Warning Remediation Plan

## Request Restatement

Identify the source of the reported Gtk, dconf, and wxWidgets console messages, then define an implementation-ready plan to eliminate repository-caused warnings and separate environment-only fixes that the user must handle outside the codebase.

## Confirmed Repository Facts

1. The wx debug warning naming `wxDialog ... ("Combat")` matches `SelectCombatGUI`, whose default title is `"Combat"` in `include/gui/SelectCombatGUI.h`.
2. `SelectCombatGUI` creates list boxes as children of the dialog instead of the static boxes that own their `wxStaticBoxSizer`s in `src/gui/SelectCombatGUI.cpp:58-90`.
3. The same static-box child-parenting anti-pattern appears in multiple other GUI files:
   - `src/gui/ViewFleetGUI.cpp`
   - `src/gui/SystemDialogGUI.cpp`
   - `src/gui/TransferShipsGUI.cpp`
   - `src/gui/UPFUnattachedGUI.cpp`
   - `src/gui/SatharFleetsGUI.cpp`
   - `src/gui/SelectJumpGUI.cpp`
   - `src/gui/BattleResultsGUI.cpp`
   - `src/battleSim/ScenarioEditorGUI.cpp`
4. The tactical battle screen always constructs `FBattleDisplay`, and `FBattleDisplay` immediately creates a `wxSpinCtrl` in `src/tactical/FBattleDisplay.cpp:24-45`.
5. The only other `wxSpinCtrl` usage in the repo is the dynamically rebuilt ICM assignment UI in `src/gui/ICMSelectionGUI.cpp:239-289`, which is reached through `src/gui/WXTacticalUI.cpp:70-78`.
6. No repository source file emits the `dconf` warning strings; the reported `dconf` messages are runtime-environment issues rather than application log messages.

## Assumptions

1. The reported `GtkSpinButton` assertion most likely originates from the startup-created tactical speed control in `FBattleDisplay`, because that control is always constructed with the battle screen and uses fixed positioning instead of sizer-managed layout.
2. `ICMSelectionGUI` is a secondary spin-control candidate if the Gtk warning still reproduces after the `FBattleDisplay` fix.
3. The `dconf` messages come from launching the app in a session without a writable user runtime directory and/or user D-Bus session.

## Files To Modify

Confirmed likely implementation files:

- `include/tactical/FBattleDisplay.h`
- `src/tactical/FBattleDisplay.cpp`
- `src/gui/SelectCombatGUI.cpp`
- `src/gui/ViewFleetGUI.cpp`
- `src/gui/SystemDialogGUI.cpp`
- `src/gui/TransferShipsGUI.cpp`
- `src/gui/UPFUnattachedGUI.cpp`
- `src/gui/SatharFleetsGUI.cpp`
- `src/gui/SelectJumpGUI.cpp`
- `src/gui/BattleResultsGUI.cpp`
- `src/battleSim/ScenarioEditorGUI.cpp`

Likely regression coverage files:

- `tests/gui/TacticalGuiLiveTest.cpp`
- `tests/gui/StrategicGuiLiveTest.cpp`
- `tests/gui/BattleSimGuiLiveTest.cpp`
- `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`

Conditional-only file if the secondary Gtk spin-control path also needs work:

- `src/gui/ICMSelectionGUI.cpp`

## Overall Documentation Impact

No end-user documentation update is expected. A short developer-facing note may be warranted in `artifacts/WXWIDGETS_UPGRADE_CHANGES.md` if the project wants to preserve two wxGTK rules explicitly:

- controls inside `wxStaticBoxSizer` must be parented to the static box, not the dialog/frame
- tactical spin controls should remain sizer-managed rather than free-positioned

## Environment Fixes For The User

These are outside the implementer subtasks because they are runtime-session issues, not repository code issues.

### dconf Runtime Session Fix

Observed warning:

- `unable to create directory '/run/user/1000/dconf': Permission denied`
- `failed to commit changes to dconf`

User actions:

1. Run the application as the desktop login user, not through `sudo` or another user-switch that leaves `XDG_RUNTIME_DIR` pointing at `/run/user/1000`.
2. Verify that `XDG_RUNTIME_DIR` exists and is owned by UID 1000:
   - `echo $XDG_RUNTIME_DIR`
   - `ls -ld /run/user/1000`
   - `id -u`
3. Verify a user D-Bus session exists:
   - `echo $DBUS_SESSION_BUS_ADDRESS`
4. If launching headlessly or from a non-login shell, start the app inside a proper user session or `dbus-run-session`.
5. For GUI test automation, keep using the repository’s documented `xvfb-run -a` flow rather than ad hoc root/session workarounds.

Exit criterion for the user section:

- launching `SSW` no longer prints the `dconf` permission/connectivity warnings before any repo-side code is changed

## Subtasks

### Subtask 1: Stabilize Tactical Spin-Control Layout

Scope:

- Remove the fragile tactical speed-control layout in `FBattleDisplay` that currently constructs a `wxSpinCtrl` as a free-positioned child at battle-screen startup.
- Keep existing tactical behavior intact: hidden by default, shown during speed selection, and wired to the same move/setup flow.
- If the Gtk warning remains reproducible after the `FBattleDisplay` refactor, audit the secondary `ICMSelectionGUI` spin-control path and fix it in the same change set.

Acceptance Criteria:

1. `FBattleDisplay` no longer relies on a free-positioned startup `wxSpinCtrl` that can be allocated invalid GTK space during tactical-screen creation or resize.
2. The speed-selection UI still appears and functions correctly during tactical setup.
3. If `ICMSelectionGUI` is touched, its rebuilt row controls remain correctly connected and laid out after weapon-row changes.
4. Tactical GUI coverage exercises the relevant dialog/screen construction path and the reported `GtkSpinButton` assertion is absent from the covered flow.

Documentation Impact:

- No user-facing docs expected.
- Optional developer note in `artifacts/WXWIDGETS_UPGRADE_CHANGES.md` if the implementation introduces a project rule about sizer-owned tactical controls.

### Subtask 2: Eliminate The Confirmed `SelectCombatGUI` Static-Box Parenting Warning

Scope:

- Fix the concrete warning source in `SelectCombatGUI` by parenting the attacking and defending list boxes to the corresponding static boxes rather than the dialog.
- Preserve current selection, enable/disable, and modal flow behavior.

Acceptance Criteria:

1. The controls inside the `"Attacking Fleets"` and `"Defending Fleets and Stations"` static boxes are created with the correct parentage for wxGTK.
2. The `"Combat"` dialog still behaves the same for attack selection, fleet viewing, and tactical launch.
3. Strategic GUI regression coverage explicitly exercises `SelectCombatGUI`.
4. The specific wx debug message about `wxListBox ... should be created as child of its wxStaticBox and not of wxDialog ... ("Combat")` is absent from the covered `SelectCombatGUI` flow.

Documentation Impact:

- No documentation update expected.

### Subtask 3: Sweep Remaining SSW Dialogs For The Same Static-Box Ownership Defect

Scope:

- Apply the same wxGTK ownership rule to the remaining SSW dialogs that use `wxStaticBoxSizer` with child controls currently parented to `this`.
- Prioritize:
  - `ViewFleetGUI`
  - `SystemDialogGUI`
  - `TransferShipsGUI`
  - `UPFUnattachedGUI`
  - `SatharFleetsGUI`
  - `SelectJumpGUI`
  - `BattleResultsGUI`

Acceptance Criteria:

1. In each scoped dialog, controls visually contained by a `wxStaticBoxSizer` are parented to that sizer’s static box or to a child container owned by that static box.
2. Existing behavior, selection flows, and enable/disable logic remain unchanged.
3. `tests/gui/StrategicGuiLiveTest.cpp` covers the affected dialog construction paths closely enough to catch ownership regressions.
4. No remaining scoped SSW dialog in this sweep contains the same parentage anti-pattern for controls inside static-box sizers.

Documentation Impact:

- No documentation update expected.

### Subtask 4: Sweep BattleSim Scenario Editor For The Same Static-Box Ownership Defect

Scope:

- Fix `ScenarioEditorGUI` so the defender/attacker controls and list boxes under static-box sizers use wxGTK-correct parenting.
- Keep battle-launch behavior and existing Scenario Editor live test flow intact.

Acceptance Criteria:

1. `ScenarioEditorGUI` no longer creates list boxes or choice controls under static-box sizers with `this` as the parent where a static-box child is required.
2. The scenario editor still supports team selection, ship assignment, and launching `FBattleScreen`.
3. `tests/gui/BattleSimGuiLiveTest.cpp` continues to prove the scenario editor launch path and interaction flow.
4. No BattleSim scenario-editor construction path emits the same static-box parenting warning.

Documentation Impact:

- No documentation update expected.

## Dependency Ordering

1. Subtask 1 first.
   - The Gtk spin-control warning is the least certain of the code-side issues, and it should be isolated before broader dialog cleanup changes complicate attribution.
2. Subtask 2 second.
   - This is the confirmed direct source of the reported `"Combat"` wx debug warning.
3. Subtask 3 third.
   - Same defect family as Subtask 2, but across the remaining main-game dialogs.
4. Subtask 4 fourth.
   - Same defect family as Subtask 3, but isolated to BattleSim files and tests.

Parallelization guidance:

- Subtasks 3 and 4 can run in parallel after Subtask 2 because they touch disjoint product areas and disjoint primary live-test files.
- Do not parallelize Subtasks 1 and 2 with other work; they are the directly reported warnings and should establish the remediation pattern first.

## Implementer Agent Prompts

### Implementer Prompt 1

You are the implementer agent

Allowed files:

- `include/tactical/FBattleDisplay.h`
- `src/tactical/FBattleDisplay.cpp`
- `tests/gui/TacticalGuiLiveTest.cpp`
- `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`
- `src/gui/ICMSelectionGUI.cpp` only if the Gtk spin-button warning is still reproducible after the primary `FBattleDisplay` fix

Task:

Refactor the tactical speed-selection control path so the battle screen no longer creates a fragile startup `wxSpinCtrl` layout that can trigger the reported Gtk spin-button allocation warning. Preserve existing tactical setup behavior. If the warning still reproduces after the primary fix, extend the same stabilization to `ICMSelectionGUI`.

Acceptance criteria:

1. `FBattleDisplay` uses a stable layout/ownership model for the speed-selection control path.
2. Tactical speed selection still works and keeps existing battle-state transitions intact.
3. Tactical GUI coverage exercises the corrected path.
4. The covered tactical flow no longer emits the reported `GtkSpinButton` assertion.

Do not report success unless all required artifacts exist and all changes are committed.

### Implementer Prompt 2

You are the implementer agent

Allowed files:

- `src/gui/SelectCombatGUI.cpp`
- `tests/gui/StrategicGuiLiveTest.cpp`

Task:

Fix the confirmed wxGTK parenting defect in `SelectCombatGUI` by ensuring the list boxes owned by the two `wxStaticBoxSizer`s are created under the appropriate static boxes instead of the dialog. Preserve all existing selection and launch behavior.

Acceptance criteria:

1. The `"Combat"` dialog constructs its static-box child controls with wxGTK-correct parentage.
2. Existing attack-selection behavior is unchanged.
3. Strategic GUI regression coverage explicitly exercises the corrected dialog path.
4. The `wxListBox ... should be created as child of its wxStaticBox ... ("Combat")` warning no longer appears in the covered flow.

Do not report success unless all required artifacts exist and all changes are committed.

### Implementer Prompt 3

You are the implementer agent

Allowed files:

- `src/gui/ViewFleetGUI.cpp`
- `src/gui/SystemDialogGUI.cpp`
- `src/gui/TransferShipsGUI.cpp`
- `src/gui/UPFUnattachedGUI.cpp`
- `src/gui/SatharFleetsGUI.cpp`
- `src/gui/SelectJumpGUI.cpp`
- `src/gui/BattleResultsGUI.cpp`
- `tests/gui/StrategicGuiLiveTest.cpp`

Task:

Sweep the remaining SSW dialogs for the same `wxStaticBoxSizer` child-parenting defect and fix each affected control without changing dialog behavior or ownership semantics outside the wxGTK parenting correction.

Acceptance criteria:

1. Each scoped dialog uses wxGTK-correct parentage for controls placed inside static-box sizers.
2. Existing dialog behavior remains intact.
3. Strategic GUI live coverage still exercises the affected dialogs.
4. No scoped SSW dialog retains the same static-box parenting anti-pattern after the sweep.

Do not report success unless all required artifacts exist and all changes are committed.

### Implementer Prompt 4

You are the implementer agent

Allowed files:

- `src/battleSim/ScenarioEditorGUI.cpp`
- `tests/gui/BattleSimGuiLiveTest.cpp`

Task:

Fix `ScenarioEditorGUI` to use wxGTK-correct child parenting for controls inside its `wxStaticBoxSizer`s while preserving the scenario-editing and battle-launch workflow.

Acceptance criteria:

1. `ScenarioEditorGUI` no longer constructs static-box child controls with the dialog as the parent where wxGTK requires the static box.
2. Scenario editing and tactical launch behavior are preserved.
3. BattleSim live GUI coverage continues to exercise the corrected scenario editor path.
4. No scenario-editor launch path emits the same static-box parenting warning.

Do not report success unless all required artifacts exist and all changes are committed.

## Output Artifact Path

- `plans/gui-console-warning-remediation-plan.md`
