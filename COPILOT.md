# COPILOT.md

## Overview

SSW is a C++ implementation of the Star Frontiers Frontier Wars space combat game using wxWidgets. The repository builds two executables: **SSW** (main game) and **BattleSim** (standalone battle simulator). See [WARP.md](WARP.md).

## Architecture

### Module layout (static libraries)
- **core**: base types and persistence (e.g., `FObject`, `FPObject`, `FPoint`, `FHexMap`)
- **strategic**: game state and turn logic (`FGame`, `FPlayer`, `FFleet`, `FMap`, `FSystem`)
- **ships**: `FVehicle` and concrete ship types
- **weapons**: weapon systems (`FWeapon` and variants)
- **defenses**: defensive systems
- **tactical**: battle mechanics
- **gui**: wxWidgets UI
- **battleSim**: battle simulator UI and flow

Each module has its own Makefile and produces a static library used when linking the final executables. See [src/Makefile](src/Makefile) and [src/battleSim/Makefile](src/battleSim/Makefile).

### Key design patterns
- **Singleton**: the game controller (`FGame`) is managed as a singleton.
- **Inheritance hierarchy**: game objects derive from `FObject`/`FPObject`.
- **Composition**: ships contain collections of weapons/defenses.
- **Serialization**: persistent objects provide `save()`/`load()` style APIs.

(These patterns are described in [WARP.md](WARP.md).)

## Build system

### Linux/Make
Run from repo root:
- Build all: `make` or `make all`
- Clean: `make clean`
- Deep clean: `make all_clean`
- Docs: `make docs`

wxWidgets must be available via `wx-config` (see [WXWIDGETS_UPGRADE_CHANGES.md](WXWIDGETS_UPGRADE_CHANGES.md)).

### Windows/Visual Studio
Open [SSW.sln](SSW.sln) after building wxWidgets 3.3.1. Details are in [VS_PROJECT_CHANGES.md](VS_PROJECT_CHANGES.md).

## Testing

Unit tests use **CppUnit** and are organized under `tests/` by module:

- Core tests: `tests/core/*`
- Weapons tests: `tests/weapons/*`
- Ships tests: `tests/ships/*`
- Strategic tests: `tests/strategic/*`

Run from `tests/`:
- `make`
- `./SSWTests`

Main test runner: [tests/SSWTests.cpp](tests/SSWTests.cpp)

## Key entry points

- Main game app: `src/FApp.cpp` (wxWidgets app for SSW)
- Battle simulator app: `src/FBattleSimApp.cpp` + [src/BattleSim.cpp](src/BattleSim.cpp)

## Documentation

Doxygen configuration: [doc/Doxyfile](doc/Doxyfile)  
Generated docs live under [doc/](doc/).

## Assets

- UI assets and data: [icons/](icons/), [data/](data/)

## Notes for contributors

- Keep module boundaries: new features should belong to the appropriate module directory.
- Prefer reusing existing core/strategic types (`FObject`, `FGame`, `FMap`, `FVehicle`, `FWeapon`) before introducing new abstractions.
- Maintain cross-platform compatibility (Linux Make + Visual Studio).
- Update tests alongside new functionality.