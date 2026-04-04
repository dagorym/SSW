# SSW AI Agent Instructions

## Project Overview
SSW is a C++ implementation of the Star Frontiers Frontier Wars space combat game using wxWidgets 3.3.1. Two executables: **SSW** (main game) and **BattleSim** (standalone battle simulator). Cross-platform: Linux via Makefiles, Windows via Visual Studio 2022.

## Architecture & Module Structure

### Static Library Modules
Each module builds a static library (`.a` file) before linking executables:
- **core**: Base types (`FObject`, `FPObject`, `FPoint`, `FHexMap`) - foundation for all game objects
- **strategic**: Game state (`FGame`, `FPlayer`, `FFleet`, `FMap`, `FSystem`) - turn-based gameplay layer
- **ships**: Vehicle implementations (`FVehicle` base, concrete ship types like `FAssaultScout`, `FBattleship`)
- **weapons**: Weapon systems (all inherit from `FWeapon` - see `FLaserCannon`, `FTorpedo`, etc.)
- **defenses**: Defensive systems (shields, screens, hull types)
- **tactical**: Combat resolution and battle mechanics
- **gui**: wxWidgets UI components
- **battleSim**: Battle simulator specific UI/logic

### Key Design Patterns
- **Singleton**: `FGame` manages the game instance via `FGame::create()` or `FGame::create(IStrategicUI*)`; `FGame::create(wxWindow*)` remains only as a compatibility shim - never use `new FGame()`
- **Single-rooted hierarchy**: All game classes inherit from `FObject` (base) or `FPObject` (persistent objects)
- **Serialization**: Persistent objects implement pure virtual `save(std::ostream&)` and `load(std::istream&)` 
- **Component composition**: Ships contain `std::vector<FWeapon*>` and defense components

### Namespace Convention
All game code lives in `namespace Frontier`. GUI classes (inheriting from wxWidgets) use `using namespace Frontier;` at file scope. Core/strategic/ships/weapons modules declare `namespace Frontier` and fully qualify external namespaces.

## Build System

### Linux (Primary Platform)
From repo root:
```bash
make              # Build both SSW and BattleSim
make clean        # Remove executables
make all_clean    # Deep clean all modules
make docs         # Generate Doxygen documentation
```

Dependencies: wxWidgets 3.3.1 via `wx-config`, CppUnit for testing.

### Windows (Visual Studio 2022)
Open `SSW.sln`. Requires wxWidgets 3.3.1 built locally at `C:\Users\steph26\repos\wxWidgets-3.3.1\`. Library names use `wxmsw33u*` (release) or `wxmsw33ud*` (debug). See [VS_PROJECT_CHANGES.md](../artifacts/VS_PROJECT_CHANGES.md) for configuration details.

### Module Dependencies
Executables link in this order: `-lgui -ltactical -lweapons -ldefenses -lships -lstrategic -lcore`. Changes to core require rebuilding all dependent modules. Each `src/*/Makefile` produces its module's static library independently.

## Testing

### CppUnit Framework
Tests mirror module structure under `tests/`:
- Core tests: `tests/core/*` (covers `FPoint`, `FObject`, `FGameConfig`, etc.)
- Weapons tests: `tests/weapons/*` (covers all weapon types, including `FWeaponFireResultTest`)
- Ships tests: `tests/ships/*` (covers all vehicle types, including `FTacticalAttackIntegrationTest`)
- GUI tests: `tests/gui/*` (covers live wx dialog/widget behavior including `StrategicGuiLiveTest`, deterministic modal driving through `WXGuiTestHarness::showModalWithAction(...)`, offscreen rendering assertions for `WXMapDisplay`, `WXPlayerDisplay`, and `WXGameDisplay`, in-transit and in-system fleet drawing checks, `FGamePanel` live-parent paint-path smoke coverage, and model-state assertions for strategic dialogs such as fleet setup, ship transfer, combat selection, and battle results flows)
- Tactical tests: `tests/tactical/*` (covers `FTacticalGame` header inclusion and additive mechanics ownership via `FTacticalGameMechanicsTest`, `FTacticalCombatReport`, `FTacticalAttackResult`, `ITacticalUI`/`WXTacticalUI` adapter coverage, battery range clamping, station orbital movement, etc.)
- Strategic tests: `tests/strategic/*` (covers `FGame`, `FPlayer`, `FFleet`, etc.)
- Test classes named `<Class>Test` (e.g., `FPointTest` for `FPoint`)
- Use `CPPUNIT_TEST_SUITE` macros (see [tests/core/FPointTest.h](../tests/core/FPointTest.h))
- Each test explicitly registered in the runner that links it, such as [tests/SSWTests.cpp](../tests/SSWTests.cpp) for the top-level suite or [tests/gui/GuiTests.cpp](../tests/gui/GuiTests.cpp) for the GUI runner.

### Running Tests
```bash
cd tests
make              # Builds SSWTests executable and all test libraries
./SSWTests        # Run all tests
```

Main test runner: `tests/SSWTests.cpp`

To run a **single module's tests**, each module under `tests/` also has its own standalone runner:
```bash
cd tests/tactical && make && ./TacticalTests   # tactical module only
cd tests/weapons  && make && ./WeaponsTests    # weapons module only
cd tests/gui      && make && ./GuiTests        # GUI module only (needs display)
# pattern: tests/<module>/ produces a standalone <Module>Tests executable
```

To build only the GUI test module from the top-level test makefile:
```bash
cd tests && make gui-tests
```

GUI test execution requires an X display. Run `./GuiTests` directly when a display session is already available; in headless automation, ensure `xvfb-run` (or an equivalent virtual-display wrapper) is installed and run GUI tests with it, such as `xvfb-run -a ./GuiTests`.

For deterministic strategic rendering coverage in headless runs, prefer offscreen `wxMemoryDC` assertions that check observable pixels or icon-placement regions for `WXMapDisplay`, `WXPlayerDisplay`, and `WXGameDisplay`; keep live parent frames for smoke-level paint-path checks such as `FGamePanel`.

Test makefiles include `-fprofile-arcs -ftest-coverage` for gcov coverage analysis. Enable coverage reporting with `make COVERAGE=1`.

### Adding New Tests
1. Copy `tests/test_template.h` and `tests/test_template.cpp` as starting points
2. Create `tests/<module>/<Class>Test.h` and `.cpp`, replacing `XXX` with the class name
3. Update `tests/<module>/Makefile` to include the new test object
4. Add `#include` and `runner.addTest()` call in [tests/SSWTests.cpp](../tests/SSWTests.cpp)
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
- See [WXWIDGETS_UPGRADE_CHANGES.md](../artifacts/WXWIDGETS_UPGRADE_CHANGES.md) for complete migration details

### Common Patterns
- Main app classes inherit from `wxApp` (`FApp`, `FBattleSimApp`)
- UI panels inherit from generated GUI base classes in `include/gui/`
- Use `wx-config --cxxflags` and `wx-config --libs` for compiler/linker flags

## Debugging & Development

### Compilation Flags
All modules compile with: `-Wall -Woverloaded-virtual -DLINUX -fprofile-arcs -ftest-coverage -g`

### Common Issues
- Missing `FGame::create()` call causes singleton access failures
- Forgetting to update [tests/SSWTests.cpp](../tests/SSWTests.cpp) when adding tests = test won't run
- Cross-module changes require `make all_clean` from root to rebuild libraries properly
- Windows builds need exact wxWidgets path in `.vcxproj` (not portable across machines)

## Key Entry Points

- Main game app: `src/FApp.cpp`
- Battle simulator app: `src/FBattleSimApp.cpp`
- Battle simulator main flow: `src/BattleSim.cpp`

## Contributor Notes

- Keep module boundaries intact; new features should go in the appropriate module directory.
- Prefer existing core and strategic abstractions before introducing new ones, especially `FObject`, `FGame`, `FMap`, `FVehicle`, and `FWeapon`.
- Treat `include/tactical/FTacticalGame.h` as the additive non-wx tactical model surface for Milestone 5; use it for model-owned tactical mechanics state and ownership bookkeeping types such as `FTacticalHexData` and `FTacticalTurnData`.
- Treat `include/tactical/ITacticalUI.h` as the additive non-wx tactical UI boundary introduced in Milestone 6; keep it free of wx includes and limited to the tactical callback surface used by later delegation work.
- During Milestone 5 isolation work, keep `FTacticalHexData`/`FTacticalTurnData` aligned with the legacy `hexData`/`turnData` still owned by `FBattleBoard`; the duplication is an intentional compatibility boundary so model-only includes stay wx-free until Milestones 7-8 remove the legacy copies.
- Treat `src/tactical/FTacticalGame.cpp` as the additive mechanics owner for battle setup/state transitions, movement reset/finalization helpers, tactical report lifecycle built on `FTacticalCombatReport`, `fireAllWeapons()` report aggregation, and winner/end-of-combat helpers.
- Keep `include/gui/WXTacticalUI.h` and `src/gui/WXTacticalUI.cpp` as the additive wx-backed tactical adapter introduced in Milestone 6; it wraps existing dialogs and winner messaging semantics but is not the live tactical runtime path yet.
- Keep the active runtime tactical GUI flow on `FBattleScreen`, `FBattleBoard`, and `FBattleDisplay` until later milestones explicitly rewire delegation; Milestones 5-6 documentation should describe `FTacticalGame`, `ITacticalUI`, and `WXTacticalUI` as additive only and not yet the live wx runtime path.
- Route strategic-layer dialogs, prompts, and notifications from `FGame` through `IStrategicUI`; keep wx dialog implementations in the gui module (for example `WXStrategicUI`) instead of reintroducing direct gui dependencies in strategic code.
- Keep icon filenames in model objects and resolve ship or fleet `wxImage` assets in GUI render paths through `WXIconCache`, including tactical displays that draw ships; preserve the cache loader's `../` fallback for headless or executable-layout compatibility, and keep `WXPlayerDisplay`/`WXGameDisplay` draw paths tolerant of invalid images before scaling or drawing.
- Maintain cross-platform compatibility across Linux Make builds and Visual Studio builds.
- Update or add tests alongside functional changes.

## Documentation
Doxygen config: [doc/Doxyfile](../doc/Doxyfile). All classes/methods should have `@brief`, `@author`, `@date` tags. Generated HTML lives in `doc/html/`.

- Design documentation: `doc/DesignNotes.odt`
- User Guide: `doc/UsersGuide.odt`

Read ~/repos/agents/AGENTS_LOOKUP.md for your agent definitions.
