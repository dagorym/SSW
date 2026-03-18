# AGENTS.md

## Overview

SSW is a C++ implementation of the Star Frontiers Second Sathar War stragegic game along with tactical component allowing users to play any of the Knight Hawks board game scenarios using wxWidgets for the GUI. The repository builds two executables:

- **SSW**: main game
- **BattleSim**: standalone battle simulator

## Project Structure

The codebase is organized into distinct modules, each with its own Makefile and static library that is linked into the final executables.

- **core**: base types and persistence (`FObject`, `FPObject`, `FPoint`, `FHexMap`)
- **strategic**: game state and turn logic (`FGame`, `FPlayer`, `FFleet`, `FMap`, `FSystem`)
- **ships**: `FVehicle` and concrete ship types
- **weapons**: weapon systems and combat mechanics
- **defenses**: defensive systems
- **tactical**: battle mechanics and combat resolution
- **gui**: wxWidgets UI
- **battleSim**: battle simulator UI and flow

Headers live under `include/` and source code under `src/`, generally mirroring module boundaries. Icons and game assets live in `icons/` and `data/`.

## Architecture

### Key design patterns

- **Singleton**: `FGame` is managed as a singleton game controller.
- **Inheritance hierarchy**: game objects derive from `FObject` or `FPObject`.
- **Composition / component system**: ships contain collections of weapons and defenses.
- **Serialization**: persistent objects implement `save()` and `load()`-style APIs.

### Core class structure

- `FGame`: central game controller managing players, maps, and game state
- `FVehicle`: base class for ships and stations, including HP, weapons, and defenses
- `FPlayer`: game participants, including UPF and Sathar factions
- `FFleet`: collections of vehicles belonging to players
- `FMap`: strategic-level game board and system management

### Data flow

1. Game initialization creates the singleton `FGame` instance.
2. `FGame` manages `FPlayer` objects and the strategic `FMap`.
3. Combat transitions to the tactical system with `FBattleBoard`.
4. Vehicle damage and other state changes persist through the serialization system.

## Build System

The project uses a hierarchical Makefile system. Run commands from the repository root unless noted otherwise.

### Linux / Make

- Build all: `make` or `make all`
- Clean build artifacts: `make clean`
- Deep clean all modules: `make all_clean`
- Build documentation: `make docs`

Individual modules can also be built by navigating to their source directories and running `make`.

wxWidgets must be available via `wx-config`. See `WXWIDGETS_UPGRADE_CHANGES.md` for current integration notes.

### Windows / Visual Studio

Open `SSW.sln` after building wxWidgets 3.3.1. Additional project details are in `VS_PROJECT_CHANGES.md`.

## Development Environment

### Dependencies

- **wxWidgets**: GUI framework
- **CppUnit**: unit testing framework
- **gcov**: coverage analysis support

WARP notes reference `wx-config-3.0`, while repository upgrade notes reference newer wxWidgets work. Follow the repo’s current build configuration in the Makefiles and upgrade notes when making environment changes.

### Platform support

- Primary target: Linux
- Cross-platform GUI through wxWidgets
- Cross-platform support also includes Visual Studio on Windows
- Uses standard C++ with STL containers

## Testing

Unit tests use **CppUnit** and are organized under `tests/` by module:

- Core tests: `tests/core/*`
- Weapons tests: `tests/weapons/*`
- Ships tests: `tests/ships/*`
- Strategic tests: `tests/strategic/*`

From `tests/`:

- Build tests: `make`
- Run tests: `./SSWTests`

Main test runner: `tests/SSWTests.cpp`

The build system also includes gcov support for coverage analysis.

## Key Entry Points

- Main game app: `src/FApp.cpp`
- Battle simulator app: `src/FBattleSimApp.cpp`
- Battle simulator main flow: `src/BattleSim.cpp`

## Documentation

- Doxygen configuration: `doc/Doxyfile`
- Generated documentation: `doc/`

## Contributor Notes

- Keep module boundaries intact; new features should go in the appropriate module directory.
- Prefer existing core and strategic abstractions before introducing new ones, especially `FObject`, `FGame`, `FMap`, `FVehicle`, and `FWeapon`.
- Maintain cross-platform compatibility across Linux Make builds and Visual Studio builds.
- Update or add tests alongside functional changes.

## Agent and Skill profiles

You *must* read ~/repos/agents/AGENT_LOOKUP.md before proceeding.
