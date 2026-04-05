# GUI Automated Tests Plan

## Restated Feature
Build a real automated GUI test suite for the wxWidgets-owned surfaces of SSW and BattleSim, using a headless-display execution path so the tests exercise live dialogs, windows, and rendering instead of stopping at source-inspection or null-guard coverage.

## Confirmed Repository Facts
- The repository already has wx-linked CppUnit runners under `tests/`, but there is no dedicated `tests/gui` module today.
- Existing GUI-related coverage is limited and mixed:
  - `tests/core/WXStrategicUITest.cpp` and `tests/core/WXIconCacheTest.cpp`
  - `tests/tactical/WXTacticalUIAdapterTest.cpp` and `tests/tactical/FTacticalDamageSummaryGUITest.cpp`
  - Most of that coverage is source inspection, compile checks, icon loading, or null-parent guard behavior rather than live widget interaction.
- `tests/SSWTests.cpp` initializes image handlers, but it does not bootstrap a real `wxApp` test harness.
- The main wx-heavy implementation surface is spread across `src/gui`, `src/battleSim`, `src/FMainFrame.cpp`, and `src/FGamePanel.cpp`.
- The repository currently has no checked-in CI workflow under `.github/workflows`.
- The current local environment has `wx-config` available (`3.3.2`) and does not have `xvfb-run` installed.
- `src/battleSim/Makefile` still uses `wx-config-3.0 --libs`, which is likely to interfere with stable GUI-test build automation.

## Assumptions
- GUI tests should run in a dedicated runner separate from `SSWTests` and `TacticalTests` so non-GUI suites remain display-free.
- Automated execution will provide a virtual display on Linux (for example Xvfb or an equivalent headless display service).
- Small testability seams are acceptable where modal dialog ownership, event pumping, or top-level app bootstrap would otherwise make automated execution hang or rely on manual dismissal.

## Files To Modify
- [tests/Makefile](/home/tstephen/repos/SSW/tests/Makefile)
- [tests/gui](/home/tstephen/repos/SSW/tests/gui)
- [src/battleSim/Makefile](/home/tstephen/repos/SSW/src/battleSim/Makefile)
- [src/gui](/home/tstephen/repos/SSW/src/gui)
- [include/gui](/home/tstephen/repos/SSW/include/gui)
- [src/battleSim](/home/tstephen/repos/SSW/src/battleSim)
- [include/battleSim](/home/tstephen/repos/SSW/include/battleSim)
- [src/FMainFrame.cpp](/home/tstephen/repos/SSW/src/FMainFrame.cpp)
- [include/FMainFrame.h](/home/tstephen/repos/SSW/include/FMainFrame.h)
- [src/FGamePanel.cpp](/home/tstephen/repos/SSW/src/FGamePanel.cpp)
- [include/FGamePanel.h](/home/tstephen/repos/SSW/include/FGamePanel.h)
- Likely documentation touch points:
  - [AGENTS.md](/home/tstephen/repos/SSW/AGENTS.md)
  - [.github/copilot-instructions.md](/home/tstephen/repos/SSW/.github/copilot-instructions.md)
  - [doc/DesignNotes.md](/home/tstephen/repos/SSW/doc/DesignNotes.md)

## Overall Documentation Impact
The repository documentation should be updated to describe the new GUI runner, the requirement for a headless display in automation, and the boundary between existing model/seam tests and the new live wx GUI tests. The Milestone 10 guidance should remain accurate: model-level seam tests still matter, but the repo will now also have an explicit automated GUI-side runner.

## Subtasks
### 1. Add a Dedicated wx GUI Test Harness and Build Path
Description:
Create a new `tests/gui` module with a dedicated `GuiTests` runner, shared wx bootstrap utilities, modal auto-dismiss helpers, and build wiring for gui and battleSim dependencies. Keep GUI execution separate from the existing non-GUI runners.

Acceptance Criteria:
1. A new `tests/gui` module exists with a dedicated CppUnit runner and shared wx test bootstrap that creates a real `wxApp`/event-loop context for GUI fixtures.
2. The shared GUI test utilities support event pumping, top-level frame creation, and deterministic modal-dialog dismissal without manual interaction.
3. `tests/gui/Makefile` builds `GuiTests` against the required `src/gui`, `src/battleSim`, and model libraries using the current `wx-config` toolchain.
4. `tests/Makefile` can build the GUI test module without forcing `SSWTests` or `TacticalTests` to run under a display.
5. Any `battleSim` build plumbing needed by the GUI runner no longer depends on `wx-config-3.0`.
6. The repository has one documented command path for GUI automation, with the expectation that CI wraps it in a virtual display.

Documentation Impact:
Document the existence of `GuiTests`, the headless-display requirement, and the intended separation between GUI and non-GUI runners.

### 2. Cover Main Frame, Strategic UI Adapters, and Read-Mostly Strategic Dialogs
Description:
Add live wx tests for top-level strategic window construction and the strategic adapter/dialog surfaces that are mostly construction, presentation, guarded invocation, and safe redraw/message behavior rather than deep model mutation.

Acceptance Criteria:
1. New GUI fixtures instantiate and tear down `FMainFrame`, `FGamePanel`, and `WXStrategicUI` under the shared wx test harness without leaking modal windows or hanging the runner.
2. Tests verify top-level strategic frame basics such as drawing-panel creation, initial menu/button state, and redraw-safe behavior.
3. Live dialog smoke tests cover representative read-mostly strategic dialogs and guarded wrappers, including `SatharRetreatGUI`, `SystemDialogGUI`, `ViewFleetGUI`, and `SelectJumpGUI`.
4. The strategic adapter tests exercise both guarded no-parent behavior and live parent-backed behavior for message and modal entry points.
5. Any product-code changes introduced for testability preserve the strategic/gui module boundary documented in the repository instructions.

Documentation Impact:
No architecture change is expected, but any newly introduced GUI harness conventions or top-level runner expectations should be reflected in testing guidance.

### 3. Cover Strategic Fleet-Management and Combat-Selection Dialog Workflows
Description:
Add interactive wx tests for the strategic dialogs that mutate fleets, ship assignment, combat selections, and related dialog-owned UI state.

Acceptance Criteria:
1. Live GUI tests cover `UPFUnattachedGUI` and `SatharFleetsGUI` flows that add ships, remove ships, select systems, update fleets, and commit resulting model changes.
2. Live GUI tests cover `TransferShipsGUI` state transitions and resulting fleet membership/name updates with representative strategic data.
3. Live GUI tests cover combat-selection dialogs such as `CombatFleetsGUI`, `CombatLocationGUI`, `TwoPlanetsGUI`, `SelectResolutionGUI`, and `BattleResultsGUI` for expected enablement, selection, and return-code behavior.
4. Tests verify model-side outcomes that are owned by the dialog workflow instead of stopping at widget existence checks.
5. The fixture data setup remains deterministic and self-contained; no test depends on prior saved-game files or manual interaction.

Documentation Impact:
No documentation update is inherently expected beyond runner coverage notes unless the test harness requires new conventions for constructing strategic GUI fixture data.

### 4. Replace Tactical GUI Source-Inspection Tests with Live Dialog and Adapter Interaction Coverage
Description:
Add live wx tests for the tactical GUI-owned dialogs and adapters so the tactical GUI path is validated through real widget behavior instead of only inspected source text.

Acceptance Criteria:
1. `WXTacticalUI` is exercised in a live parent-backed context for redraw, message, damage-summary, ICM-selection, and winner-notification entry points.
2. `TacticalDamageSummaryGUI` is tested as a real dialog, including context text, summary text, empty-state rendering, title behavior, and close behavior.
3. `ICMSelectionGUI` is tested through real grid/selection/spin-control interaction, and finalization updates weapon assignment counts plus defender ICM ammo state correctly.
4. Existing source-inspection tactical GUI fixtures are either replaced or narrowed so the primary acceptance coverage comes from live widget execution.
5. Any new test seams keep direct wx ownership on the GUI side and do not pull wx dependencies back into non-GUI tactical model headers.

Documentation Impact:
Testing guidance should note that tactical GUI validation now includes a live wx runner in addition to the existing tactical model seam tests.

### 5. Add Offscreen Rendering Coverage for Map, Player, Game, Icon, and Panel Display Helpers
Description:
Add deterministic offscreen rendering tests using `wxBitmap`/`wxMemoryDC` or an equivalent wx drawing surface to validate GUI rendering helpers without relying on human inspection.

Acceptance Criteria:
1. New GUI fixtures cover `WXMapDisplay`, `WXPlayerDisplay`, and `WXGameDisplay` using deterministic map/player/game setup and offscreen device contexts.
2. Tests verify observable rendering outcomes, such as non-background pixel changes, icon placement behavior, turn-counter drawing, or other stable bitmap-level assertions.
3. Rendering coverage includes at least one in-transit fleet case and one stationary/system fleet case for `WXPlayerDisplay`.
4. `WXIconCache` remains covered for resolved-path loading and cache reuse, and GUI rendering fixtures confirm icons can be consumed by live draw code.
5. `FGamePanel` paint-path coverage exists at least at a smoke level with a live parent frame and installed game/drawing context.

Documentation Impact:
No documentation change is required unless the rendering tests introduce shared bitmap-assertion conventions worth recording for future fixtures.

### 6. Add Modal Launch-Flow Coverage for SelectCombat and BattleSim
Description:
Add live wx tests for the modal launch chains that move from strategic or BattleSim dialogs into `FBattleScreen`, with deterministic close behavior so automated runs do not block.

Acceptance Criteria:
1. `SelectCombatGUI` battle-resolution paths are exercised through live modal interaction, including the path that opens `FBattleScreen`.
2. `BattleSimFrame`, `LocalGameDialog`, `ScenarioDialog`, and `ScenarioEditorGUI` each have live wx coverage for their primary modal launch paths.
3. GUI tests can open `FBattleScreen` from both strategic and BattleSim launch flows and close it deterministically without user input.
4. The modal launch-flow tests verify ownership/lifetime expectations that the current source-inspection tests only infer.
5. The final GUI runner command is suitable for automation on a headless Linux display service and is documented where contributors already look for test commands.

Documentation Impact:
Update contributor-facing testing guidance with the canonical headless GUI command and note that BattleSim GUI flow coverage is part of the automated GUI runner.

## Dependency Ordering
1. Subtask 1 must complete first.
2. Subtask 2 should run after Subtask 1 because it depends on the shared wx harness and runner.
3. Subtask 3 should run after Subtask 2 because it will likely reuse the strategic fixture patterns and shared GUI utilities.
4. Subtask 4 should run after Subtask 1 and is safest after Subtasks 2-3 because it will likely refine shared modal/event helpers.
5. Subtask 5 should run after Subtask 1 and can run in parallel with Subtask 4 if ownership stays limited to rendering fixtures and render-helper files.
6. Subtask 6 should run last because it depends on the modal harness patterns from earlier GUI dialog work and touches the broadest cross-module launch flow.

## Implementer Agent Prompts
### Subtask 1 Prompt
You are the implementer agent

Allowed files:
- `/home/tstephen/repos/SSW/tests/Makefile`
- `/home/tstephen/repos/SSW/tests/gui/*`
- `/home/tstephen/repos/SSW/src/battleSim/Makefile`
- `/home/tstephen/repos/SSW/AGENTS.md`
- `/home/tstephen/repos/SSW/.github/copilot-instructions.md`
- `/home/tstephen/repos/SSW/doc/DesignNotes.md`

Task:
Create a dedicated `tests/gui` wx test module and shared GUI test harness. Wire the new `GuiTests` runner into repository test builds without forcing the existing non-GUI runners to require a display. Normalize any build plumbing needed for GUI-test linkage, including the `battleSim` library path if required.

Acceptance criteria:
1. A shared wx bootstrap/event-pump/modal-dismiss test utility exists under `tests/gui`.
2. `tests/gui/Makefile` builds a `GuiTests` runner against the required libraries with the current `wx-config` toolchain.
3. `tests/Makefile` can build the GUI test module while keeping GUI execution separate from `SSWTests` and `TacticalTests`.
4. Any `battleSim` build dependency used by the GUI runner no longer requires `wx-config-3.0`.
5. Contributor-facing docs mention the new GUI runner and the need for a virtual display in headless automation.

Do not report success unless all required artifacts exist and all changes are committed.

### Subtask 2 Prompt
You are the implementer agent

Allowed files:
- `/home/tstephen/repos/SSW/tests/gui/*`
- `/home/tstephen/repos/SSW/src/FMainFrame.cpp`
- `/home/tstephen/repos/SSW/include/FMainFrame.h`
- `/home/tstephen/repos/SSW/src/FGamePanel.cpp`
- `/home/tstephen/repos/SSW/include/FGamePanel.h`
- `/home/tstephen/repos/SSW/src/gui/WXStrategicUI.cpp`
- `/home/tstephen/repos/SSW/include/gui/WXStrategicUI.h`
- `/home/tstephen/repos/SSW/src/gui/SystemDialogGUI.cpp`
- `/home/tstephen/repos/SSW/include/gui/SystemDialogGUI.h`
- `/home/tstephen/repos/SSW/src/gui/ViewFleetGUI.cpp`
- `/home/tstephen/repos/SSW/include/gui/ViewFleetGUI.h`
- `/home/tstephen/repos/SSW/src/gui/SatharRetreatGUI.cpp`
- `/home/tstephen/repos/SSW/include/gui/SatharRetreatGUI.h`
- `/home/tstephen/repos/SSW/src/gui/SelectJumpGUI.cpp`
- `/home/tstephen/repos/SSW/include/gui/SelectJumpGUI.h`

Task:
Implement live GUI tests for `FMainFrame`, `FGamePanel`, `WXStrategicUI`, and representative read-mostly strategic dialogs. Use the shared wx harness to exercise construction, guarded invocation, redraw behavior, and modal close behavior without manual input.

Acceptance criteria:
1. The new tests instantiate and tear down `FMainFrame`, `FGamePanel`, and `WXStrategicUI` under the GUI runner without hangs.
2. The tests verify top-level frame basics such as drawing-panel creation and initial strategic menu state.
3. Live dialog smoke coverage exists for `SatharRetreatGUI`, `SystemDialogGUI`, `ViewFleetGUI`, and `SelectJumpGUI`.
4. Parent-backed `WXStrategicUI` message/modal entry points are covered in addition to existing guarded no-parent behavior.
5. Any testability changes preserve the strategic/gui boundary rules in the repo instructions.

Do not report success unless all required artifacts exist and all changes are committed.

### Subtask 3 Prompt
You are the implementer agent

Allowed files:
- `/home/tstephen/repos/SSW/tests/gui/*`
- `/home/tstephen/repos/SSW/src/gui/UPFUnattachedGUI.cpp`
- `/home/tstephen/repos/SSW/include/gui/UPFUnattachedGUI.h`
- `/home/tstephen/repos/SSW/src/gui/SatharFleetsGUI.cpp`
- `/home/tstephen/repos/SSW/include/gui/SatharFleetsGUI.h`
- `/home/tstephen/repos/SSW/src/gui/TransferShipsGUI.cpp`
- `/home/tstephen/repos/SSW/include/gui/TransferShipsGUI.h`
- `/home/tstephen/repos/SSW/src/gui/CombatFleetsGUI.cpp`
- `/home/tstephen/repos/SSW/include/gui/CombatFleetsGUI.h`
- `/home/tstephen/repos/SSW/src/gui/CombatLocationGUI.cpp`
- `/home/tstephen/repos/SSW/include/gui/CombatLocationGUI.h`
- `/home/tstephen/repos/SSW/src/gui/TwoPlanetsGUI.cpp`
- `/home/tstephen/repos/SSW/include/gui/TwoPlanetsGUI.h`
- `/home/tstephen/repos/SSW/src/gui/SelectResolutionGUI.cpp`
- `/home/tstephen/repos/SSW/include/gui/SelectResolutionGUI.h`
- `/home/tstephen/repos/SSW/src/gui/BattleResultsGUI.cpp`
- `/home/tstephen/repos/SSW/include/gui/BattleResultsGUI.h`

Task:
Add live GUI tests for the strategic dialogs that mutate fleet state, ship assignment, combat selection, and related dialog-owned UI state. Focus on deterministic model outcomes, not just widget construction.

Acceptance criteria:
1. `UPFUnattachedGUI` and `SatharFleetsGUI` flows are covered for selecting fleets, adding/removing ships, choosing systems, and committing updates.
2. `TransferShipsGUI` has live interaction coverage for ship movement and fleet-update behavior.
3. `CombatFleetsGUI`, `CombatLocationGUI`, `TwoPlanetsGUI`, `SelectResolutionGUI`, and `BattleResultsGUI` have live return-code/state coverage.
4. The tests assert model-side outcomes owned by the dialogs rather than only widget existence.
5. Fixture setup remains deterministic and self-contained.

Do not report success unless all required artifacts exist and all changes are committed.

### Subtask 4 Prompt
You are the implementer agent

Allowed files:
- `/home/tstephen/repos/SSW/tests/gui/*`
- `/home/tstephen/repos/SSW/src/gui/WXTacticalUI.cpp`
- `/home/tstephen/repos/SSW/include/gui/WXTacticalUI.h`
- `/home/tstephen/repos/SSW/src/gui/TacticalDamageSummaryGUI.cpp`
- `/home/tstephen/repos/SSW/include/gui/TacticalDamageSummaryGUI.h`
- `/home/tstephen/repos/SSW/src/gui/ICMSelectionGUI.cpp`
- `/home/tstephen/repos/SSW/include/gui/ICMSelectionGUI.h`
- `/home/tstephen/repos/SSW/src/tactical/FBattleScreen.cpp`
- `/home/tstephen/repos/SSW/include/tactical/FBattleScreen.h`

Task:
Replace tactical GUI source-inspection acceptance coverage with live wx interaction coverage for `WXTacticalUI`, `TacticalDamageSummaryGUI`, and `ICMSelectionGUI`. Add only the minimum tactical GUI seams needed for deterministic automated execution.

Acceptance criteria:
1. `WXTacticalUI` is exercised in a live parent-backed context for redraw, message, damage-summary, ICM-selection, and winner notification.
2. `TacticalDamageSummaryGUI` is validated as a real dialog for title/context/summary/empty-state/close behavior.
3. `ICMSelectionGUI` is validated through real grid and spin-control interaction, and finalization updates assigned ICM counts plus ammo state correctly.
4. Existing tactical GUI source-inspection fixtures are no longer the primary acceptance mechanism for these behaviors.
5. No new wx dependency leaks into non-GUI tactical model headers.

Do not report success unless all required artifacts exist and all changes are committed.

### Subtask 5 Prompt
You are the implementer agent

Allowed files:
- `/home/tstephen/repos/SSW/tests/gui/*`
- `/home/tstephen/repos/SSW/src/gui/WXMapDisplay.cpp`
- `/home/tstephen/repos/SSW/include/gui/WXMapDisplay.h`
- `/home/tstephen/repos/SSW/src/gui/WXPlayerDisplay.cpp`
- `/home/tstephen/repos/SSW/include/gui/WXPlayerDisplay.h`
- `/home/tstephen/repos/SSW/src/gui/WXGameDisplay.cpp`
- `/home/tstephen/repos/SSW/include/gui/WXGameDisplay.h`
- `/home/tstephen/repos/SSW/src/gui/WXIconCache.cpp`
- `/home/tstephen/repos/SSW/include/gui/WXIconCache.h`
- `/home/tstephen/repos/SSW/src/FGamePanel.cpp`
- `/home/tstephen/repos/SSW/include/FGamePanel.h`

Task:
Add offscreen rendering tests for the strategic display helpers and panel paint path. Use deterministic bitmap-level assertions so rendering is validated automatically without human inspection.

Acceptance criteria:
1. Offscreen GUI fixtures cover `WXMapDisplay`, `WXPlayerDisplay`, and `WXGameDisplay`.
2. The tests assert stable observable rendering outcomes such as non-background pixel changes, icon placement behavior, or turn-counter rendering.
3. `WXPlayerDisplay` coverage includes both in-transit and in-system fleet drawing cases.
4. `WXIconCache` remains covered for resolved-path loading and cache reuse, and the rendering tests consume cached icons through live draw code.
5. `FGamePanel` has at least smoke-level paint-path coverage with a live parent frame.

Do not report success unless all required artifacts exist and all changes are committed.

### Subtask 6 Prompt
You are the implementer agent

Allowed files:
- `/home/tstephen/repos/SSW/tests/gui/*`
- `/home/tstephen/repos/SSW/src/gui/SelectCombatGUI.cpp`
- `/home/tstephen/repos/SSW/include/gui/SelectCombatGUI.h`
- `/home/tstephen/repos/SSW/src/battleSim/BattleSimFrame.cpp`
- `/home/tstephen/repos/SSW/include/battleSim/BattleSimFrame.h`
- `/home/tstephen/repos/SSW/src/battleSim/LocalGameDialog.cpp`
- `/home/tstephen/repos/SSW/include/battleSim/LocalGameDialog.h`
- `/home/tstephen/repos/SSW/src/battleSim/ScenarioDialog.cpp`
- `/home/tstephen/repos/SSW/include/battleSim/ScenarioDialog.h`
- `/home/tstephen/repos/SSW/src/battleSim/ScenarioEditorGUI.cpp`
- `/home/tstephen/repos/SSW/include/battleSim/ScenarioEditorGUI.h`
- `/home/tstephen/repos/SSW/src/tactical/FBattleScreen.cpp`
- `/home/tstephen/repos/SSW/include/tactical/FBattleScreen.h`
- `/home/tstephen/repos/SSW/AGENTS.md`
- `/home/tstephen/repos/SSW/.github/copilot-instructions.md`
- `/home/tstephen/repos/SSW/doc/DesignNotes.md`

Task:
Add live wx coverage for the modal launch chains that open `FBattleScreen` from both `SelectCombatGUI` and the BattleSim dialogs. Ensure the GUI runner can open and close these flows deterministically in headless automation, and document the final command path.

Acceptance criteria:
1. `SelectCombatGUI` has live coverage for the battle-resolution path that opens `FBattleScreen`.
2. `BattleSimFrame`, `LocalGameDialog`, `ScenarioDialog`, and `ScenarioEditorGUI` have live coverage for their primary modal launch paths.
3. The tests can open `FBattleScreen` from both strategic and BattleSim launch paths and close it deterministically without manual input.
4. The modal-flow tests verify real ownership/lifetime behavior rather than only source structure.
5. The contributor-facing docs identify the canonical headless GUI test command.

Do not report success unless all required artifacts exist and all changes are committed.

## Output Artifact
- `/home/tstephen/repos/SSW/plans/gui-automated-tests-plan.md`
