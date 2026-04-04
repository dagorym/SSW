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
make              # Build both SSW and BattleSim
make clean        # Remove executables
make all_clean    # Deep clean all modules
make docs         # Generate Doxygen documentation
```

Individual modules can also be built by navigating to their source directories and running `make`. Each module produces a static library (`.a` file) that is linked into the final executables.

Dependencies: wxWidgets 3.3.1 and CppUnit for testing. GUI-facing builds still resolve wx settings via `wx-config`; the six non-GUI module Makefiles (`src/core`, `src/strategic`, `src/ships`, `src/weapons`, `src/defenses`, and `src/tactical`) must not contain active `wx-config --cxxflags` or `wx-config --libs` usage. See `WXWIDGETS_UPGRADE_CHANGES.md` for current integration notes.

### Windows / Visual Studio 2022

Open `SSW.sln`. Requires wxWidgets 3.3.1 built locally at `C:\Users\steph26\repos\wxWidgets-3.3.1\`. Library names use `wxmsw33u*` (release) or `wxmsw33ud*` (debug). See `VS_PROJECT_CHANGES.md` for configuration details.

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

- Core tests: `tests/core/*` (covers `FPoint`, `FObject`, `FGameConfig`, etc.)
- Weapons tests: `tests/weapons/*` (covers all weapon types, including `FWeaponFireResultTest`)
- Ships tests: `tests/ships/*` (covers all vehicle types, including `FTacticalAttackIntegrationTest`)
- GUI tests: `tests/gui/*` (covers live wx dialog/widget behavior including `StrategicGuiLiveTest`, `TacticalGuiLiveTest`, and `BattleSimGuiLiveTest`; deterministic modal and top-level discovery through `WXGuiTestHarness` helpers such as `showModalWithAction(...)`, `waitForTopLevelWindow(...)`, and `waitForModalDialog(...)`; harness regression coverage for orphan top-level cleanup via `cleanupOrphanTopLevels(...)`; parent-backed `WXTacticalUI` redraw/message/damage-summary/winner smoke flows; `TacticalDamageSummaryGUI` live dialog coverage; offscreen rendering assertions for `WXMapDisplay`, `WXPlayerDisplay`, and `WXGameDisplay`; in-transit and in-system fleet drawing checks; `FGamePanel` live-parent paint-path smoke coverage; model-state assertions for strategic dialogs such as fleet setup, ship transfer, combat selection, and battle results flows; and live modal-launch coverage for `SelectCombatGUI`, `BattleSimFrame`, `LocalGameDialog`, `ScenarioDialog`, and `ScenarioEditorGUI` with `FBattleScreen` lifecycle assertions.)
- Tactical tests: `tests/tactical/*` (covers `FTacticalGame` header inclusion and additive mechanics ownership via `FTacticalGameMechanicsTest`, `FTacticalCombatReport`, `FTacticalAttackResult`, `ITacticalUI`/`WXTacticalUI` adapter coverage, deterministic mock-`ITacticalUI` winner coverage in `FTacticalGameMockUITest`, battery range clamping, station orbital movement, and the destroyed-ship cleanup lifecycle seam in `FBattleScreen.cpp`, etc.)
- Strategic tests: `tests/strategic/*` (covers `FGame`, `FPlayer`, `FFleet`, and mock-`IStrategicUI` initialization seam coverage in `FGameMockStrategicUITest`, etc.)
- Test classes named `<Class>Test` (e.g., `FPointTest` for `FPoint`)
- Use `CPPUNIT_TEST_SUITE` macros (see `tests/core/FPointTest.h`)
- Each test explicitly registered in the runner that links it, such as `tests/SSWTests.cpp` for the top-level suite or `tests/gui/GuiTests.cpp` for the GUI runner.

### Running Tests

From `tests/`:

```bash
make              # Builds SSWTests executable and all test libraries
./SSWTests        # Run all tests
```

Main test runner: `tests/SSWTests.cpp`

The top-level `tests/Makefile` now bootstraps the required `../src/*` model/gui libraries before linking `SSWTests`, so `cd tests && make` is expected to work from a clean repository state without a separate root build first.

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

GUI test execution requires an X display. Run `./GuiTests` directly when a display session is already available; in headless automation, ensure `xvfb-run` (or an equivalent virtual-display wrapper) is installed and run the suite under it, for example `xvfb-run -a ./GuiTests`.

Canonical headless GUI validation command:
```bash
cd tests/gui && make && xvfb-run -a ./GuiTests
```

For deterministic live-dialog coverage, prefer the shared `tests/gui/WXGuiTestHarness` helpers over ad hoc timers; `showModalWithAction(...)`, `runModalFunctionWithAction(...)`, and `runVoidFunctionWithAction(...)` are the seams that schedule in-dialog interactions before the modal fallback closes the window, while `waitForTopLevelWindow(...)` / `waitForModalDialog(...)` let tests prove that launched dialogs or frames actually appeared. Live GUI fixtures should explicitly `Destroy()` shown parent frames or dialogs, pump events afterward, and call `cleanupOrphanTopLevels(...)` during teardown so `GuiTests` finishes with no orphaned top-level windows.

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
- See `WXWIDGETS_UPGRADE_CHANGES.md` for complete migration details

### Common Patterns
- Main app classes inherit from `wxApp` (`FApp`, `FBattleSimApp`)
- UI panels inherit from generated GUI base classes in `include/gui/`; shared tactical wx type aggregation lives in `include/gui/GuiTypes.h`
- Use `wx-config --cxxflags` and `wx-config --libs` for GUI-facing executable or gui-module builds; keep active `wx-config` usage out of the six non-GUI module Makefiles and preserve their explicit non-GUI build flags separately.

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
- Keep the active runtime tactical GUI flow centered on `FBattleScreen`, `FBattleBoard`, and `FBattleDisplay` until later milestones complete the delegation rewrite; Milestones 5-8 documentation should still describe `FTacticalGame`, `ITacticalUI`, and `WXTacticalUI` as additive infrastructure rather than a fully model-driven live runtime path. Milestone 8 Subtask 1 expanded the `FBattleScreen`/`FTacticalGame` forwarding surface for selection, placement, hex-click handling, movement/fire progression, and renderer-facing state queries, and the remediation follow-up made the destroyed-ship cleanup contract explicit: `FTacticalGame::fireAllWeapons()` captures destroyed IDs, `FBattleDisplay` shows the summary dialog, `FBattleScreen::clearDestroyedShips()` performs wx-side cleanup by consuming `getLastDestroyedShipIDs()`, and `FTacticalGame::clearLastDestroyedShipIDs()` clears bookkeeping exactly once after that wx seam consumes it. The selected-ship cleanup ordering is now also locked by the extracted `runDestroyedShipCleanupLifecycle(...)` seam in `src/tactical/FBattleScreen.cpp`, which preserves the wx/model ownership split while allowing direct tactical regression coverage for selection clearing, redraw, bookkeeping clear, and winner handling order. Milestone 8 Subtask 2 then moved `FBattleDisplay` fire/setup selection handling onto that seam so the display now behaves as a HUD/event translator for weapon selection, defense selection, fire-phase completion, mine-placement setup/completion, destroyed-ship cleanup, and destroyed-side winner fallback. Milestone 8 Subtask 3 narrowed `FBattleBoard` to geometry/rendering/scrolling/hit-testing by routing clicks through `FBattleScreen::handleHexClick()` to `FTacticalGame` and reading occupancy/path/range/mine overlays back through model-backed accessors; the live `FBattleDisplay` move-completion callback and `FTacticalGame::setPhase(PH_FINALIZE_MOVE)` now both route through `FTacticalGame::completeMovePhase()`, preserving that method as the canonical post-move resolution seam for mine interactions, destruction handling, and occupancy finalization. Milestone 10 validation still does not depend on full end-to-end wx playback, but the GUI suite now includes deterministic tactical smoke coverage for parent-backed `WXTacticalUI` redraw/message/damage-summary/winner interactions and the `TacticalDamageSummaryGUI` live dialog path.
- Route strategic-layer dialogs, prompts, and notifications from `FGame` through `IStrategicUI`; keep wx dialog implementations in the gui module (for example `WXStrategicUI`) instead of reintroducing direct gui dependencies in strategic code.
- Strategic regression tests may install mock `IStrategicUI` implementations to validate `FGame` initialization, setup callbacks, and message-routing behavior without creating wx dialogs or windows; keep that seam usable for non-wx test coverage.
- Keep icon filenames in model objects and resolve ship or fleet `wxImage` assets in GUI render paths through `WXIconCache`, including tactical displays that draw ships; preserve the cache loader's `../` fallback for headless or executable-layout compatibility, and keep `WXPlayerDisplay`/`WXGameDisplay` draw paths tolerant of invalid images before scaling or drawing.
- Maintain cross-platform compatibility across Linux Make builds and Visual Studio builds.
- Update or add tests alongside functional changes.

## Documentation

- Doxygen configuration: `doc/Doxyfile`
- Generated documentation: `doc/`
- AI agent information: `AGENTS.md` (this file)
- Design documentation: `doc/DesignNotes.odt`
- User Guide: `doc/UsersGuide.odt`

## Agent and Skill Profiles

You *must* read ~/repos/agents/AGENTS_LOOKUP.md before proceeding.
