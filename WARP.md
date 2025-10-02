# WARP.md

This file provides guidance to WARP (warp.dev) when working with code in this repository.

## Project Overview

**SSW** is a C++ implementation of the Star Frontiers Frontier Wars space combat game using wxWidgets for the GUI. The project builds two main executables: the main game (**SSW**) and a standalone battle simulator (**BattleSim**).

## Build System

### Main Build Commands

The project uses a hierarchical Makefile system. All commands should be run from the repository root:

- **Build everything**: `make` or `make all`
- **Clean build artifacts**: `make clean`
- **Deep clean all modules**: `make all_clean`
- **Build documentation**: `make docs`

### Module-Specific Building

Individual modules can be built by navigating to their source directories and running `make`. The build system creates static libraries (.a files) for each module before linking the final executables.

### Testing

- **Build and run tests**: Navigate to `tests/` directory and run `make`
- **Run test executable**: Execute `./SSWTests` from the tests directory
- **Coverage reports**: The build system includes gcov support for code coverage analysis

## Code Architecture

### Modular Design

The codebase is organized into distinct modules, each with its own static library:

- **core**: Base classes and fundamental game objects (FObject, FPObject, FPoint, FHexMap)
- **strategic**: High-level game management (FGame, FPlayer, FFleet, FMap, FSystem)
- **ships**: Vehicle classes and ship types (FVehicle base class, specific ship implementations)
- **weapons**: Weapon systems and combat mechanics
- **defenses**: Defensive systems (shields, screens, hull types)
- **tactical**: Combat resolution and battle mechanics
- **gui**: wxWidgets-based user interface components
- **battleSim**: Standalone battle simulator functionality

### Key Design Patterns

- **Singleton Pattern**: `FGame` class uses singleton pattern with static instance management
- **Inheritance Hierarchy**: All game objects inherit from `FObject` (base) or `FPObject` (persistent objects)
- **Component System**: Ships contain vectors of weapons and defenses as components
- **Serialization**: All persistent objects implement `save()` and `load()` methods for game state persistence

### Core Class Structure

- `FGame`: Central game controller managing players, maps, and game state
- `FVehicle`: Base class for all ships and stations with HP, weapons, defenses
- `FPlayer`: Represents game participants (UPF and Sathar factions)
- `FFleet`: Collections of vehicles belonging to players
- `FMap`: Strategic-level game board and system management

### Data Flow

1. Game initialization creates singleton FGame instance
2. FGame manages FPlayer objects and strategic FMap
3. Combat transitions to tactical system with FBattleBoard
4. Vehicle damage and state changes persist through serialization system

## Development Environment

### Dependencies

- **wxWidgets 3.0**: GUI framework (configured via wx-config-3.0)
- **CppUnit**: Unit testing framework
- **gcov**: Code coverage analysis (enabled by default with -fprofile-arcs -ftest-coverage)

### Platform Support

- Primary target: Linux (DLINUX flag set)
- Cross-platform GUI through wxWidgets
- Uses standard C++ with STL containers

### Code Organization

- Headers in `include/` directory with module subdirectories
- Source code in `src/` directory mirroring include structure
- Each module has its own Makefile for independent building
- Icons and game assets in dedicated directories (`icons/`, `data/`)

### Testing Strategy

Comprehensive unit test suite covering:
- Core functionality (FPoint, FObject, FGameConfig)
- Weapon systems (all weapon types)
- Ship classes (all vehicle types) 
- Strategic game components (FGame, FPlayer, FFleet, etc.)

Tests are organized in module-specific subdirectories matching the main codebase structure.