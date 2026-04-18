# AGENTS.md

## Overview

SSW is a C++ implementation of the Star Frontiers Second Sathar War strategic game along with a tactical component allowing users to play any of the Knight Hawks board game scenarios using wxWidgets 3.3.1 for the GUI. The repository builds two executables:

- **SSW**: main game
- **BattleSim**: standalone battle simulator

Cross-platform: Linux via Makefiles, Windows via Visual Studio 2022.

## Project Structure

The codebase is organized into distinct modules, each with its own Makefile and static library that is linked into the final executables:

- **core**: Base types and persistence (`FObject`, `FPObject`, `FPoint`, `FHexMap`) - foundation for all game objects
- **strategic**: Game state and turn logic (`FGame`, `FPlayer`, `FFleet`, `FMap`, `FSystem`) - turn-based gameplay layer
- **ships**: Vehicle implementations (`FVehicle` base, concrete ship types like `FAssaultScout`, `FBattleship`)
- **weapons**: Weapon systems (all inherit from `FWeapon` - see `FLaserCannon`, `FTorpedo`, etc.)
- **defenses**: Defensive systems (shields, screens, hull types)
- **tactical**: Combat resolution and battle mechanics
- **gui**: wxWidgets UI components
- **battleSim**: Battle simulator specific UI/logic

Headers live under `include/` and source code under `src/`, generally mirroring module boundaries. Icons and game assets live in `icons/` and `data/`.

## Architecture

### Key Design Patterns
- **Singleton**: `FGame` manages the game instance via `FGame::create()` or `FGame::create(IStrategicUI*)`; `FGame::create(wxWindow*)` remains only as a compatibility shim - never use `new FGame()`
- **Single-rooted hierarchy**: All game classes inherit from `FObject` (base) or `FPObject` (persistent objects)
- **Serialization**: Persistent objects implement pure virtual `save(std::ostream&)` and `load(std::istream&)`
- **Component composition**: Ships contain `std::vector<FWeapon*>` and defense components

### Core Class Structure

- `FGame`: central game controller managing players, maps, and game state
- `FVehicle`: base class for ships and stations, including HP, weapons, and defenses
- `FPlayer`: game participants, including UPF and Sathar factions
- `FFleet`: non-copyable collection of vehicles belonging to a player; uses `NO_DESTINATION`/`NO_ROUTE` sentinel constants for unset navigation state
- `FMap`: strategic-level game board and system management
- `FTacticalGame`: additive pure-C++ tactical model surface that owns tactical mechanics state, delegation-facing setup/selection/report APIs, and an `ITacticalUI` seam without pulling in wx headers
- `FTacticalAttackResult`: structured result from a weapon attack (outcome, damage, and hit details)
- `FTacticalCombatReport`: per-ship and per-weapon combat reporting for a battle round

### Data Flow

1. Game initialization creates the singleton `FGame` instance.
2. `FGame` manages `FPlayer` objects and the strategic `FMap`.
3. Combat transitions to the tactical system with `FBattleBoard`.
4. Vehicle damage and other state changes persist through the serialization system.

### Namespace Convention
All game code lives in `namespace Frontier`. GUI classes (inheriting from wxWidgets) use `using namespace Frontier;` at file scope. Core/strategic/ships/weapons modules declare `namespace Frontier` and fully qualify external namespaces.

### Milestone 10 Final Module Boundary Rules
- Non-GUI model modules (`core`, `strategic`, `ships`, `weapons`, `defenses`, and non-gui tactical model code) must remain non-wx in both includes and build/link settings.
- Keep active wx headers/types isolated to gui-owned surfaces (`include/gui/*`, `src/gui/*`, app entrypoints, and tactical wx runtime files that explicitly bridge to GUI rendering).
- `IStrategicUI` and `ITacticalUI` are model-facing interface seams; strategic and tactical model code consume these abstractions without owning wx-backed UI classes.
- `WXStrategicUI` and `WXTacticalUI` are gui-module implementations of those seams and retain ownership of wx dialog/window behavior.

## Build System

### Linux / Make

From repo root:
```bash
make              # Build the main executables and the top-level test targets
make tests        # Build the test suites from the repo root
make clean        # Clean both src/ and tests/ outputs
make all_clean    # Deep clean all modules
make docs         # Generate Doxygen documentation
```

Individual modules can also be built by navigating to their source directories and running `make`. Each module produces a static library (`.a` file) that is linked into the final executables.

Dependencies: wxWidgets 3.3.1 and CppUnit for testing. GUI-facing builds still resolve wx settings via `wx-config`; the six non-GUI module Makefiles (`src/core`, `src/strategic`, `src/ships`, `src/weapons`, `src/defenses`, and `src/tactical`) must not contain active `wx-config --cxxflags` or `wx-config --libs` usage. See `artifacts/WXWIDGETS_UPGRADE_CHANGES.md` for current integration notes.

### Windows / Visual Studio 2022

Open `SSW.sln`. Requires wxWidgets 3.3.1 built locally at `C:\Users\steph26\repos\wxWidgets-3.3.1\`. Library names use `wxmsw33u*` (release) or `wxmsw33ud*` (debug). See `artifacts/VS_PROJECT_CHANGES.md` for configuration details.

### Module Dependencies
Executables link in this order: `-lgui -ltactical -lweapons -ldefenses -lships -lstrategic -lcore`. Changes to core require rebuilding all dependent modules. Each `src/*/Makefile` produces its module's static library independently.

## Development Environment

### Dependencies

- **wxWidgets**: GUI framework (configured via `wx-config`)
- **CppUnit**: unit testing framework
- **gcov**: coverage analysis support (opt-in; enable with `make COVERAGE=1`)

### Platform Support

- Primary target: Linux (compiled with `-DLINUX` flag)
- Cross-platform GUI through wxWidgets
- Cross-platform support also includes Visual Studio on Windows
- Uses standard C++ with STL containers

## Testing

Unit tests use **CppUnit** and are organized under `tests/` by module:

- Core tests: `tests/core/*` (covers `FPoint`, `FObject`, `FGameConfig` singleton/base-path behavior plus focused `resolveAssetPath(...)` regression coverage for repo-asset lookup, normalized relative paths, and the executable-parent fallback contract, `WXIconCacheTest` shared-resolver usage that rejects raw `getBasePath() + filename` concatenation, and `WXStrategicUITest` runtime-guard regression coverage for strategic adapter cancel/early-return behavior when no usable wx runtime exists, including the guarded non-null-parent/no-runtime case that stays distinct from parentless live-wx behavior, etc.)
- Weapons tests: `tests/weapons/*` (covers all weapon types, including `FWeaponFireResultTest`)
- Ships tests: `tests/ships/*` (covers all vehicle types, including `FTacticalAttackIntegrationTest`)
- GUI tests: `tests/gui/*` (covers live wx dialog/widget behavior including `StrategicGuiLiveTest`, `TacticalGuiLiveTest`, and `BattleSimGuiLiveTest`; deterministic modal and top-level discovery through `WXGuiTestHarness` helpers such as `showModalWithAction(...)`, `waitForTopLevelWindow(...)`, and `waitForModalDialog(...)`; harness regression coverage for orphan top-level cleanup via `cleanupOrphanTopLevels(...)`; parent-backed `WXTacticalUI` redraw/message/damage-summary/winner smoke flows, including no-detail and empty `showDamageSummary(...)` modal-close coverage plus centering assertions that still target the tactical parent while allowing only owning-top-level-parent fallback during a brief wxGTK geometry-settle window, alongside the parentless damage-summary fallback path when no tactical parent window is installed; direct `TacticalDamageSummaryGUI` modal assertions for title/context/multi-line ship-summary text (including `ADF (-N)` and `MR (-N)` bullets without the legacy `effects:` wording)/empty-state, Close-button default-focus initialization, simulated Enter-key dismissal through the affirmative path, and real `Close` handling without a manual `EndModal(...)` callback; direct `ICMSelectionGUI` modal selection/finalization coverage that proves assignment and ammo updates through the production completion path, asserts the Done button is visible inside the dialog client area on first show, and verifies dynamically rebuilt ICM allocation spin controls remain inside the client area with positive width/height after row selection while retaining `wxSP_ARROW_KEYS | wxSP_VERTICAL` through both runtime style-bit checks and a source-token regression lock; offscreen rendering assertions for `WXMapDisplay`, `WXPlayerDisplay`, and `WXGameDisplay`; in-transit and in-system fleet drawing checks; `FGamePanel` live-parent paint-path smoke coverage; model-state assertions for strategic dialogs such as fleet setup, ship transfer, combat selection, and battle results flows, including live `SelectCombatGUI` checks that the attacker and defender list boxes are parented to static boxes labeled `Attacking Fleets` and `Defending Fleets and Stations` before the attack-launch path continues, plus source-inspection assertions that the seven remediated SSW dialogs keep `GetStaticBox()`-parented constructors and reject the legacy `new ... ( this, ...)` static-box anti-pattern, the four dialog-audit strategic dialogs (`SatharRetreatGUI`, `CombatLocationGUI`, `TwoPlanetsGUI`, and `SelectResolutionGUI`) retain their first-show sizing contract (`SetSizerAndFit(...)`, `SetMinSize(GetSize())`, and `Centre(wxBOTH)`), strategic adapter coverage now explicitly keeps the parentless live-wx retreat modal path separate from the guarded no-usable-runtime path for the shared `WXStrategicUI` entry points so null-parent behavior is not mistaken for missing-runtime behavior, parent-backed strategic adapter dialogs remain centered relative to their triggering surface while allowing a transient top-level-parent fallback during geometry settling, and `BattleResultsGUI` coverage explicitly checks the edit-stats `m_textCtrl2`, `m_staticText8`, and `m_staticText12` controls under `Edit Ship Statistics`; startup-seam coverage in `StrategicGuiLiveTest` and `BattleSimGuiLiveTest` now invokes `createStartupSplashAndFrame(...)` directly, asserts that each app creates both a visible splash and startup frame, verifies display centering for both windows, and uses the splash window's `wxSTAY_ON_TOP` style bit as the narrowest reliable observable for splash-above-frame behavior while both remain visible; a tactical smoke check that `FBattleScreen` exposes a live speed `wxSpinCtrl` with nonzero size under a child container rather than as a direct top-level child; live tactical geometry checks that the movement, defensive-fire, offensive-fire, and mine-placement completion buttons stay sizer-positioned to the right of the zoom control and below the full instruction-text block after they are shown, with the prompt-clearance assertion derived from `FBattleDisplay`'s shared spacing contract constants/helpers and explicit non-intersection checks against the per-phase prompt-text region; explicit documentation in that fixture that `FBattleScreen` paint timing is not deterministic enough for the harness to prove runtime `Show()/Layout()` ordering without the source-contract tactical test; and live modal-launch coverage for `SelectCombatGUI`, `BattleSimFrame`, `LocalGameDialog`, `ScenarioDialog`, and `ScenarioEditorGUI` that now proves concrete dialog identities and parent linkage, asserts first-show in-client visibility for the key BattleSim dialog buttons and editor action controls, source-audits that those three BattleSim launch dialogs keep their existing `Fit(...)` plus `SetMinSize(GetBestSize())` sizing contract for this audit-only pass, asserts that the default `BattleSimFrame` launch geometry keeps all three launcher buttons visible on first show (including a reachable `Quit` button) while centering the frame on the active display, verifies representative parent-backed BattleSim dialogs open centered on their launcher after brief geometry-settle retries with display fallback for transient modal placement, covers the deterministic screen-centered fallback for parentless modal BattleSim dialog cases, asserts the scenario editor's defender and attacker controls use the expected static-box parents before battle launch, preserves `FBattleScreen` lifecycle assertions, and restores shown top-level windows to the pre-launch baseline.)
- Tactical tests: `tests/tactical/*` (covers `FTacticalGame` header inclusion and additive mechanics ownership via `FTacticalGameMechanicsTest`, `FTacticalCombatReport`, `FTacticalAttackResult`, `ITacticalUI`/`WXTacticalUI` adapter coverage, deterministic mock-`ITacticalUI` winner coverage in `FTacticalGameMockUITest`, battery range clamping, station orbital movement, the destroyed-ship cleanup lifecycle seam in `FBattleScreen.cpp`, multi-line `FTacticalCombatReportSummary` ship-rollup formatting including exact `ADF (-N)` / `MR (-N)` bullets plus grouped weapon/defense-hit abbreviations, and source-inspection checks that `FBattleDisplay` constructs its speed spin control with explicit `wxSP_ARROW_KEYS | wxSP_VERTICAL` style plus default positioning and `SetMinSize(GetBestSize())` under a sizer-managed layout rather than the legacy absolute-positioned, hard-coded-width path, loads the tactical zoom image through `resolveAssetPath("data/zoom.png")` instead of raw base-path concatenation, exposes the shared `ACTION_PROMPT_*` spacing constants plus helper seams that reserve prompt-text height before the tactical action-button row, applies that contract across move/defensive-fire/offensive-fire/mine-placement prompt drawing, and relayouts those completion buttons immediately after their show/hide visibility changes, etc.)
- Strategic tests: `tests/strategic/*` (covers `FGame`, `FPlayer`, `FFleet`, and mock-`IStrategicUI` initialization seam coverage in `FGameMockStrategicUITest`, etc.)
- Test classes named `<Class>Test` (e.g., `FPointTest` for `FPoint`)
- Use `CPPUNIT_TEST_SUITE` macros (see `tests/core/FPointTest.h`)
- Each test explicitly registered in the runner that links it, such as `tests/SSWTests.cpp` for the top-level suite or `tests/gui/GuiTests.cpp` for the GUI runner.

### Running Tests

From `tests/`:

```bash
make              # Builds SSWTests, the GUI test runner, and the linked test libraries
./SSWTests        # Run all tests
```

Main test runner: `tests/SSWTests.cpp`

The top-level `tests/Makefile` now bootstraps the required `../src/*` model/gui libraries before linking `SSWTests`, and its default `all` target also builds `tests/gui/GuiTests`, so `cd tests && make` is expected to work from a clean repository state without a separate root build first.

When you need a fresh top-level rebuild of the tactical standalone runner before executing it, use:
```bash
cd tests && make tactical-tests && ./tactical/TacticalTests
```
That target runs the tactical module clean step before relinking `tests/tactical/TacticalTests`, which is the preferred hygiene path from the top-level `tests/` makefile when validating tactical-only changes.

To run a **single module's tests**, each module under `tests/` also has its own standalone runner:
```bash
cd tests/tactical && make && ./TacticalTests   # tactical module only
cd tests/weapons  && make && ./WeaponsTests    # weapons module only
cd tests/gui      && make && ./GuiTests        # GUI module only (needs display)
# pattern: tests/<module> && make && ./<Module>Tests
```

To build only the GUI test module from the top-level tests makefile without running GUI tests:
```bash
cd tests && make gui-tests
```
That target delegates to `tests/gui`, which now builds both `libguiTests.a` and the `GuiTests` runner after bootstrapping the needed `../../src/*` libraries, including `src/battleSim`.

GUI test execution requires an X display. Run `./GuiTests` directly when a display session is already available; in headless automation, ensure `xvfb-run` (or an equivalent virtual-display wrapper) is installed and run the suite under it, for example `xvfb-run -a ./GuiTests`.

Canonical headless GUI validation command:
```bash
cd tests/gui && make && xvfb-run -a ./GuiTests
```

For deterministic live-dialog coverage, prefer the shared `tests/gui/WXGuiTestHarness` helpers over ad hoc timers; `showModalWithAction(...)`, `runModalFunctionWithAction(...)`, and `runVoidFunctionWithAction(...)` are the seams that schedule in-dialog interactions before the modal fallback closes the window, while `waitForTopLevelWindow(...)` / `waitForModalDialog(...)` let tests prove that launched dialogs or frames actually appeared. When centering assertions race transient wxGTK geometry updates, allow short retry loops and the narrowest valid fallback target documented by the fixture (for example the owning top-level window for parent-backed tactical dialogs, the active display for transient parentless modal dialogs, or the owning top-level window for adapter-launched strategic surfaces) rather than dropping the placement assertion entirely; likewise, use `showModalWithAction(...)` for direct modal ownership when repository evidence shows app-level scheduling can race the dialog, and raise modal fallback timeouts only where the close path can settle slowly. Live GUI fixtures should explicitly hide and `Destroy()` shown parent frames or dialogs, pump events afterward, and call `cleanupOrphanTopLevels(...)` during teardown so `GuiTests` finishes with no orphaned top-level windows; BattleSim launch-path fixtures also stabilize and force-close any remaining shown top levels before asserting that zero residual shown windows remain beyond the pre-launch baseline.

For deterministic strategic rendering coverage in headless runs, prefer offscreen `wxMemoryDC` assertions that check observable pixels or icon-placement regions for `WXMapDisplay`, `WXPlayerDisplay`, and `WXGameDisplay`; keep live parent frames for smoke-level paint-path checks such as `FGamePanel`.

Test makefiles include `-fprofile-arcs -ftest-coverage` for gcov coverage analysis. Enable coverage reporting with `make COVERAGE=1`.

Milestone 10 note: there is no fully automated end-to-end wx GUI playback system in this repository. Milestone acceptance relies on model/interface regression coverage (including mock `IStrategicUI` and mock `ITacticalUI` seams) plus existing build/test validation; the absence of full GUI automation is expected and does not block Milestone 10 completion.

### Adding New Tests
1. Copy `tests/test_template.h` and `tests/test_template.cpp` as starting points
2. Create `tests/<module>/<Class>Test.h` and `.cpp`, replacing `XXX` with the class name
3. Update `tests/<module>/Makefile` to include the new test object
4. Add `#include` and `runner.addTest()` call in the runner that actually links the fixture (`tests/SSWTests.cpp` for top-level coverage, or a module-specific runner such as `tests/tactical/TacticalTests.cpp` when the fixture remains module-local)
5. Test include guards use `#ifndef CLASSNAME_H_` (no leading underscore, unlike production headers)

## Critical Conventions

### File Organization
- Headers: `include/<module>/<Class>.h`
- Source: `src/<module>/<Class>.cpp`
- Always use `#include "<module>/<Class>.h"` (module-qualified paths)

### Class Naming
- Prefix all classes with `F` (e.g., `FGame`, `FWeapon`, `FVehicle`)
- Test classes: `<ClassName>Test` in `namespace FrontierTests`

### Include Guards
Use `#ifndef _<FILENAME>_H_` pattern (e.g., `#ifndef _FGAME_H_`). Doxygen-style file headers mandatory:
```cpp
/**
 * @file ClassName.h
 * @brief Brief description
 * @author Name
 * @date Created: MMM DD, YYYY
 */
```

### Memory Management
- Raw pointers common (`FVehicle*`, `FWeapon*`) - no smart pointers in legacy code
- `FGame` destructor responsible for cleaning up `FPlayer` and `FMap` objects
- Component ownership follows composition hierarchy (ship owns weapons/defenses)

## wxWidgets Integration

### Recent Upgrade (3.0.3 → 3.3.1)
- `wxInitAllImageHandlers()` removed (automatic in 3.1+)
- Virtual methods now use `wxOVERRIDE` macro instead of raw `override`
- See `artifacts/WXWIDGETS_UPGRADE_CHANGES.md` for complete migration details

### Common Patterns
- Main app classes inherit from `wxApp` (`FApp`, `FBattleSimApp`)
- UI panels inherit from generated GUI base classes in `include/gui/`; shared tactical wx type aggregation lives in `include/gui/GuiTypes.h`
- Use `wx-config --cxxflags` and `wx-config --libs` for GUI-facing executable or gui-module builds; keep active `wx-config` usage out of the six non-GUI module Makefiles and preserve their explicit non-GUI build flags separately.
- On wxGTK, prefer `wxDefaultSize` plus `SetMinSize(GetBestSize())` for sizer-managed `wxSpinCtrl` controls instead of narrow fixed widths so spin buttons keep valid geometry after layout; when the control is used as a value-selection spinner, explicitly request vertical arrows with `wxSP_ARROW_KEYS | wxSP_VERTICAL` in the style.
- For wxGTK dialog constructors that rely on sizers, ensure the first-show size is locked before the dialog is displayed: either use `Layout()`/`Fit(this)` plus `SetMinSize(GetBestSize())`, or when the constructor immediately computes the final fixed-content size use `SetSizerAndFit(...)` plus `SetMinSize(GetSize())`; in both cases center on the parent (or on screen when unparented) so first-show geometry does not clip controls or action buttons.
- For top-level frames without an owning parent, prefer `CentreOnScreen(wxBOTH)` after the final first-show size is established; for adapter-launched or parent-backed dialogs, prefer `CentreOnParent(wxBOTH)` with `Centre(wxBOTH)` only as the explicit no-parent fallback so live placement tests stay deterministic across BattleSim, strategic, and tactical flows.
- When a dialog rebuilds dynamic row content at runtime, re-fit after the rebuild and update both the minimum size and current size from the new best size so follow-on layout passes preserve the expanded geometry.
- Shared wx startup splash/frame bootstrap for both apps now lives in `include/gui/WXStartupLaunch.h` via `createStartupSplashAndFrame(wxApp&, FStartupFrameFactory, int)`; keep `FApp::OnInit()` and `FBattleSimApp::OnInit()` delegating to that seam so GUI tests exercise the same startup path, with splash creation preceding frame creation/show and `SetTopWindow(...)`, `wxSPLASH_CENTRE_ON_SCREEN` plus `wxFRAME_NO_TASKBAR | wxSTAY_ON_TOP` preserving the expected splash-window/taskbar behavior, and the created startup frame centered on screen before it is shown. The SSW startup path should continue to pass `wxDefaultPosition` into `FMainFrame` rather than reintroducing legacy fixed startup coordinates.
- Route direct GUI, startup, and tactical asset loads through `FGameConfig::resolveAssetPath(...)` instead of ad hoc `getBasePath()` concatenation or local `../` fallbacks so helpers such as `WXStartupLaunch`, `FBattleDisplay`, `FBattleBoard`, `FBattleScreen`, and `WXIconCache` stay on the same asset-location policy.

## Debugging & Development

### Compilation Flags
All modules compile with: `-Wall -Woverloaded-virtual -DLINUX -fprofile-arcs -ftest-coverage -g`

### Common Issues
- Missing `FGame::create()` call causes singleton access failures
- Forgetting to update `tests/SSWTests.cpp` when adding tests = test won't run
- Cross-module changes require `make all_clean` from root to rebuild libraries properly
- Windows builds need exact wxWidgets path in `.vcxproj` (not portable across machines)

## Key Entry Points

- Main game app: `src/FApp.cpp`
- Battle simulator app: `src/FBattleSimApp.cpp`
- Battle simulator main flow: `src/BattleSim.cpp`

## Contributor Notes

- Keep module boundaries intact; new features should go in the appropriate module directory.
- Prefer existing core and strategic abstractions before introducing new ones, especially `FObject`, `FGame`, `FMap`, `FVehicle`, and `FWeapon`.
- Treat `include/tactical/FTacticalGame.h` as the non-wx tactical model surface for Milestones 5-8; it now owns the canonical tactical mechanics state, including `FTacticalHexData`/`FTacticalTurnData`, plus the delegation-facing API categories used by `FBattleScreen` forwarding (state/control, setup/scenario, ship/weapon/defense selection, hex-click dispatch, movement/fire progression, renderer-facing tactical state accessors, and tactical report access).
- Treat `include/tactical/ITacticalUI.h` as the additive non-wx tactical UI boundary introduced in Milestone 6; keep it free of wx includes and limited to the tactical callback surface used by later delegation work, including the `installUI(ITacticalUI*)` / `getUI()` seam now exposed on `FTacticalGame`. Tactical model regression tests can install mock `ITacticalUI` implementations to validate deterministic winner and callback behavior without any wx runtime objects.
- Since Milestone 8 Subtask 3, `FTacticalHexData`/`FTacticalTurnData` are the model-owned tactical state and `FBattleBoard` no longer keeps duplicate mechanics containers; keep renderer-only geometry/cache data on the wx side and tactical occupancy, path, range, mine, and turn bookkeeping in `FTacticalGame`.
- Treat `src/tactical/FTacticalGame.cpp` as the additive mechanics owner for battle setup/state transitions, movement reset/finalization helpers, tactical report lifecycle built on `FTacticalCombatReport`, `fireAllWeapons()` report aggregation, and winner/end-of-combat helpers.
- Keep `include/gui/WXTacticalUI.h` and `src/gui/WXTacticalUI.cpp` as the additive wx-backed tactical adapter introduced in Milestone 6; it wraps existing dialogs and winner messaging semantics, Milestone 7 Subtask 2 has `FBattleScreen` own/install/detach that adapter against its owned `FTacticalGame` during setup and teardown, and the GUI smoke harness now exercises the parent-backed redraw, damage-summary, modal message, winner, and ICM-selection paths without turning `WXTacticalUI` into the live runtime owner.
- Keep direct wx includes out of `include/tactical/FBattleScreen.h`, `include/tactical/FBattleDisplay.h`, `include/tactical/FBattleBoard.h`, and their `src/tactical/` counterparts; when those tactical GUI-adjacent files need wx declarations, route them through `gui/GuiTypes.h` so the include aggregation stays gui-owned.
- Keep the active runtime tactical GUI flow centered on `FBattleScreen`, `FBattleBoard`, and `FBattleDisplay` until later milestones complete the delegation rewrite; Milestones 5-8 documentation should still describe `FTacticalGame`, `ITacticalUI`, and `WXTacticalUI` as additive infrastructure rather than a fully model-driven live runtime path. Milestone 8 Subtask 1 expanded the `FBattleScreen`/`FTacticalGame` forwarding surface for selection, placement, hex-click handling, movement/fire progression, and renderer-facing state queries, and the remediation follow-up made the destroyed-ship cleanup contract explicit: `FTacticalGame::fireAllWeapons()` captures destroyed IDs, `FBattleDisplay` shows the summary dialog, `FBattleScreen::clearDestroyedShips()` performs wx-side cleanup by consuming `getLastDestroyedShipIDs()`, and `FTacticalGame::clearLastDestroyedShipIDs()` clears bookkeeping exactly once after that wx seam consumes it. The selected-ship cleanup ordering is now also locked by the extracted `runDestroyedShipCleanupLifecycle(...)` seam in `src/tactical/FBattleScreen.cpp`, which preserves the wx/model ownership split while allowing direct tactical regression coverage for selection clearing, redraw, bookkeeping clear, and winner handling order. Milestone 8 Subtask 2 then moved `FBattleDisplay` fire/setup selection handling onto that seam so the display now behaves as a HUD/event translator for weapon selection, defense selection, fire-phase completion, mine-placement setup/completion, destroyed-ship cleanup, and destroyed-side winner fallback. Milestone 8 Subtask 3 narrowed `FBattleBoard` to geometry/rendering/scrolling/hit-testing by routing clicks through `FBattleScreen::handleHexClick()` to `FTacticalGame` and reading occupancy/path/range/mine overlays back through model-backed accessors; the live `FBattleDisplay` move-completion callback and `FTacticalGame::setPhase(PH_FINALIZE_MOVE)` now both route through `FTacticalGame::completeMovePhase()`, preserving that method as the canonical post-move resolution seam for mine interactions, destruction handling, and occupancy finalization. `FBattleDisplay` now also reserves tactical action-button space from an explicit prompt-spacing contract (`ACTION_PROMPT_TOP_MARGIN`, `ACTION_PROMPT_LINE_HEIGHT`, `ACTION_PROMPT_MAX_LINES`, `ACTION_PROMPT_BUTTON_GAP`, plus helper accessors) so the movement, defensive-fire, offensive-fire, and mine-placement completion buttons stay below the multi-line prompt block instead of depending on incidental layout. Milestone 10 validation still does not depend on full end-to-end wx playback, but the GUI suite now includes deterministic tactical smoke coverage for parent-backed `WXTacticalUI` redraw/message/damage-summary/winner interactions together with direct `TacticalDamageSummaryGUI` and `ICMSelectionGUI` modal dialog paths that prove real close/finalization behavior, empty/no-detail damage-summary dismissal through the runtime adapter, source-guarded reliance on the default affirmative close path instead of a manual bind plus `EndModal(...)`, orphan-free teardown, and prompt-reservation checks that keep tactical action buttons below the instruction text across the covered phases.
- Route strategic-layer dialogs, prompts, and notifications from `FGame` through `IStrategicUI`; keep wx dialog implementations in the gui module (for example `WXStrategicUI`) instead of reintroducing direct gui dependencies in strategic code. `WXStrategicUI` may still run parentless when a live wx runtime exists, but shared modal entry points must first confirm `wxTheApp` is available before constructing dialogs; when no usable wx runtime exists, modal-return helpers fall back to `wxID_CANCEL` and void dialog helpers return early while message-only paths use the non-modal output fallback.
- Strategic regression tests may install mock `IStrategicUI` implementations to validate `FGame` initialization, setup callbacks, and message-routing behavior without creating wx dialogs or windows; keep that seam usable for non-wx test coverage.
- For wxGTK dialog work, any control visually owned by a `wxStaticBoxSizer` must be created under that sizer's `GetStaticBox()` (or a child container owned by that static box) rather than with the dialog as parent; current regression coverage locks this down for `SelectCombatGUI`, the seven Subtask 3 SSW dialogs, and the BattleSim `ScenarioEditorGUI` defender/attacker assignment surfaces.
- The Subtask 4 dialog-audit follow-up was intentionally strategic-only on the implementation side: it remediated `SatharRetreatGUI`, `CombatLocationGUI`, `TwoPlanetsGUI`, and `SelectResolutionGUI`, while BattleSim coverage in that pass remained an audit-only source contract check that its launch dialogs kept their existing sizing pattern.
- Keep icon filenames in model objects and resolve ship or fleet `wxImage` assets in GUI render paths through `WXIconCache`, including tactical displays that draw ships; `WXIconCache` now follows the shared `FGameConfig::resolveAssetPath(...)` policy used by startup and tactical direct image loads, and `WXPlayerDisplay`/`WXGameDisplay` draw paths should remain tolerant of invalid images before scaling or drawing.
- Maintain cross-platform compatibility across Linux Make builds and Visual Studio builds.
- Update or add tests alongside functional changes.

## Documentation

- Doxygen configuration: `doc/Doxyfile`
- Generated documentation: `doc/` (`make docs` writes HTML output to `doc/html/`)
- AI agent information: `AGENTS.md` (this file)
- Design documentation: `doc/DesignNotes.odt`
- User Guide: `doc/UsersGuide.odt`
- Game rules for tactical combat: `doc/rules/tactical_operations_manual.md`

## Agent and Skill Profiles

You *must* read ~/repos/agents/AGENTS_LOOKUP.md before proceeding.
