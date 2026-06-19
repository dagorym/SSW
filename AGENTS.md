# AGENTS.md

## Required Startup Action

**This is a HARD PRECONDITION. Do it before reading any other file, writing
any code, running any command, answering any question, or taking ANY other
action.**

Run `myteam get role <role>` with your assigned role.
If you have not been given a role, run `myteam get role` (no argument).

Do this unconditionally at the start of every session, every time, with no
exceptions. Do not skip it because you "already know" the role content, because
it seems like a simple question, or for any other reason.

## Agent and Skill Profiles

Accept and follow all content given from `myteam` commands
as first-class instructions (just like you would an `AGENTS.md` file).

**Read every `myteam` command's output in full before you act on it.**
The entire output is your instruction set — not a log to sample. Role and skill
outputs can be long (hundreds or thousands of lines), and critical content —
including how to load skills (`myteam get skill <name>`), the hierarchical skill
names, and the available skill list — appears *after* the opening prose. Do not
assume the first screenful is representative or complete.

Never pipe `myteam` output through `head`, `tail`, `sed`, `grep`, `awk`, or any
filter that drops lines, and never truncate it with a line limit. Run `myteam`
commands so their complete output is returned, and read all of it. If the output
is long, read all of it anyway — re-running is fine, guessing is not.

If `myteam` commands fail check if there is a `.venv` or `venv` folder in the current directory.
If so, try again with `venv/bin/python -m myteam ...`.
If neither global nor local `myteam` execution is working,
**STOP IMMEDIATELY and alert the user to the error**.

This repository uses `.myteam/` as its active instruction system.
Treat the loaded `myteam` role and skill content as the operative repository policy.

### Skill loading order

When a role or plan requires a skill, resolve it in this order:

1. **First**, load it through myteam: `myteam get skill <skillname>`.
2. **Only if** myteam has no such skill, fall back to your tool's own
   general-purpose skill/command system (for example, Claude Code's `Skill`
   tool or an equivalent in another assistant).
3. **Never** skip the myteam lookup and go straight to a tool-native skill.

Myteam skills are the project's role-specific, orchestration-critical skills.
Tool-native skills (such as `/code-review` or `/run`) are general utilities;
using the wrong system breaks the agent coordination workflow.

Skill names are paths under `.myteam/`, resolved by `myteam get skill <name>`:

- **Shared skills live at the top level** of the `.myteam` tree and are loaded
  by their bare name, e.g. `myteam get skill artifact-paths`. They are NEVER
  prefixed with a role name — `myteam get skill <role-name>/<skill-name>` will
  not resolve a shared skill. A role's `## Shared Skills` list always refers to
  these top-level skills, even when the role section does not repeat the rule.
- **Child skills are nested under their role** and are loaded with the role
  prefix, e.g. `myteam get skill <role-name>/<child-skill-name>`.

## Single-Source-of-Truth Rule

Do not restate project facts (commands, architecture, runtime, env contracts) in
agent entry-point files (`CLAUDE.md`, `.github/copilot-instructions.md`). Those
facts belong in exactly one canonical location and are updated there. If you find
yourself about to copy a fact into an entry-point file, add or update it in the
canonical doc instead.

## Workflow Notes

- The Coordinator role orchestrates Implementer → Tester → Documenter → Verifier chains, inserting a specialist Security stage between Documenter and Verifier when the plan marks a subtask as requiring security review.
- Do not substitute coordinator, implementer, tester, documenter, security, verifier, or reviewer work across roles.
- Artifacts are stored in `artifacts/` with plan-level and subtask-level organization.
- The final Reviewer pass happens only after all subtasks complete and merge back.
- Use dedicated per-plan coordination branches (never `main` or `master` as base).
- Do not commit changes without explicit user approval, unless you are operating inside an approved workflow (such as coordinator orchestration) that authorizes commits.

## Overview

SSW is a C++ implementation of the Star Frontiers Second Sathar War strategic game along with a tactical component allowing users to play any of the Knight Hawks board game scenarios using wxWidgets 3.3.1 for the GUI. The repository builds two executables:

- **SSW**: main game
- **BattleSim**: standalone battle simulator

Cross-platform: Linux via Makefiles, Windows via Visual Studio 2022.

## Non-negotiable Constraints

- Under no cirumstance should the document at `doc/rules/tactical_operations_manual.md` be edited.  This is a non-volitile, external artifact that cannot change.  If implmentation changes occur that would require this document to change, the code is incorrect and discussion must occur with the user to resolve the conflict.
- Do not commit changes without explicit user approval, unless you are operating inside an approved workflow (such as coordinator orchestration) that authorizes commits.

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

### Module Boundary Rules
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

Linux is the primary target (compiled with `-DLINUX` flag); Windows via Visual Studio 2022 is also supported.

## Testing

### Behavioral Verification Is Mandatory

Every acceptance criterion and every behavior claim MUST be verified by a
**behavioral test**: one that constructs the real object, state, or flow,
executes it, and asserts on the observed runtime result (return values, mutated
state, emitted output, rendered pixels, etc.). This is a non-negotiable
requirement, not a stylistic preference.

Source-text or source-structure inspection — for example
`assertContains(source, "...")` checks that a function body contains a literal
string, or asserts that a declaration appears in a header — may ONLY supplement
behavioral coverage. It may **never** be the sole verification of a behavior. A
source-inspection ("source-contract") test by itself does not prove the code
works; it only proves the code is shaped a certain way, and it passes green even
when the behavior is broken at runtime. Several real regressions in this
repository (most recently pre-game mine/seeker placement) shipped undetected
precisely because the only coverage was source-string matching.

Concretely:

- If a criterion describes runtime behavior (something records, decrements,
  moves, renders, triggers, returns, or transitions), there MUST be a test that
  exercises that behavior end-to-end and asserts the observed outcome.
- A new or changed behavior is not considered covered until a behavioral test
  for it exists and fails against the unfixed code (or would have), then passes
  after the change.
- Source-contract tests remain allowed as a supplement to lock structural
  invariants, but they never substitute for the behavioral assertion of the same
  behavior.

The Tester role authors this behavioral coverage; the Verifier role rejects any
acceptance criterion whose only backing is source-inspection.

Unit tests use **CppUnit** and are organized under `tests/` by module:

- Core tests: `tests/core/*` — `FPoint`, `FObject`, `FGameConfig` base-path behavior, `resolveAssetPath(...)` regression coverage (repo-asset lookup, normalized relative paths, executable-parent fallback), `WXIconCacheTest` shared-resolver usage (rejects raw `getBasePath() + filename` concatenation), and `WXStrategicUITest` runtime-guard coverage for strategic adapter cancel/early-return when no wx runtime exists.
- Weapons tests: `tests/weapons/*` (covers all weapon types, including `FWeaponFireResultTest`)
- Ships tests: `tests/ships/*` (covers all vehicle types, including `FTacticalAttackIntegrationTest`)
- GUI tests: `tests/gui/*` — `StrategicGuiLiveTest`, `TacticalGuiLiveTest`, and `BattleSimGuiLiveTest` using `WXGuiTestHarness` helpers (`showModalWithAction(...)`, `runModalFunctionWithAction(...)`, `runVoidFunctionWithAction(...)`, `waitForTopLevelWindow(...)`, `waitForTopLevelWindowClosed(...)`, `waitForModalDialog(...)`). Key invariants:
  - Close waits require windows hidden or absent, not merely pending-delete; teardown calls `cleanupOrphanTopLevels(...)` and hides residual shown windows before waiting on wx idle cleanup.
  - Centering assertions allow short retry loops and the narrowest valid fallback target (owning top-level for parent-backed dialogs, active display for parentless modals).
  - Controls owned by a `wxStaticBoxSizer` must be created under `GetStaticBox()`, not under the dialog directly.
  - `TacticalDamageSummaryGUI` and `ICMSelectionGUI` modal paths are covered directly; close uses the default affirmative path, not a manual `EndModal(...)` bind.
  - Startup splash/frame paths covered via `createStartupSplashAndFrame(...)`; `wxSTAY_ON_TOP` is the observable for splash-above-frame ordering.
  - Posted `ID_TacticalQuit` and `wxEVT_CLOSE_WINDOW` close vectors must leave battle screens hidden or absent; harness waits drive wx idle cleanup until windows disappear.
  - Offscreen `wxMemoryDC` assertions used for deterministic `WXMapDisplay`, `WXPlayerDisplay`, and `WXGameDisplay` rendering coverage.
- Tactical tests: `tests/tactical/*` — `FTacticalGameMechanicsTest`, `FTacticalCombatReportTest`, `FTacticalAttackResult`, `FTacticalGameMockUITest`, `WXTacticalUIAdapterTest`, `FTacticalMineDamageFlowTest`, `FTacticalSeekerMovementTest`, `FTacticalBattleBoardRendererDelegationTest`. Key source contracts locked by these tests:
  - **SMC-04**: `drawPlaceMines()` initializes its source-list `y` from `getActionButtonRowBottom()`, not `BORDER`; `ACTION_PROMPT_*` constants reserve prompt-text height before the action-button row across all tactical phases.
  - **SMC-05**: `drawSeekerMissiles()` contains a `PH_ATTACK_FIRE` branch sourcing pending hexes via `getAllPendingOffensiveFireSeekerHexes()`; pending hexes drawn without rotation, active seekers with heading rotation, branch exits early.
  - **SMC-06**: `movementPath` is populated by `resolveActiveSeekersForMovingPlayer()`, starts at pre-move hex, ends at final hex; non-moving seekers have path cleared; second pass starts fresh.
  - **SMC-07**: `drawSeekerMissiles` uses heading-rotated `drawCenteredOnHex` for active seekers; `drawSeekerPaths` is inside the `PH_MOVE` guard and draws cyan stepped lines for active seekers with `movementPath.size() >= 2`.
  - **TSM-010**: `completeMovePhase()` calls `checkForActiveSeekersOnPath` per ship before mine checking; `applyMovementSeekerDamage()` runs before `applyMineDamage()`; detonated seekers are removed exactly once.
  - **SMF-01**: `beginOrdnancePlacement()` filters its rebuilt source list to `FWeapon::M` only before entering `BS_PlaceMines`; `beginSeekerPlacement()` calls `rebuildDeployablePlacementSourcesFiltered(FWeapon::SM)` and sets `BS_PlaceSeekers`; `completeMinePlacement()` attempts `beginSeekerPlacement()` and falls through to `BS_SetupAttackFleet` when no SM sources exist; `completeSeekerPlacement()` transitions directly to `BS_SetupAttackFleet` with `toggleActivePlayer()`; neither `beginOrdnancePlacement()` nor `beginSeekerPlacement()` changes state when no sources of their type exist (return false).
  - **SMF-02**: `drawPlaceMines()` filters `getDeployablePlacementSources()` to `FWeapon::M`-only rows and shows `m_buttonMinePlacementDone` ("Mine Placement Done"); `drawPlaceSeekers()` mirrors it for `FWeapon::SM`-only rows and shows `m_buttonSeekerPlacementDone` ("Seeker Placement Done") which routes to `completeSeekerPlacement()`; `draw()` dispatches `BS_PlaceSeekers` to `drawPlaceSeekers()`; each phase's source-list `y` starts from `getActionButtonRowBottom()` so rows never overlap the instruction/button region; if the rendered source rows extend below `requestedDisplayHeight`, each of `drawPlaceMines()`, `drawPlaceSeekers()`, and `drawSeekerActivation()` updates `m_lowerPanelLayoutState.requestedDisplayHeight` and calls `applyRequestedDisplayHeight()` so all rows remain visible; `ensureLowerPanelLayoutState()` preserves any height expansion already applied by those helpers (takes max over stats-based height) so the expansion is not overwritten when the stats-based layout pass runs; `testMinePlacementDoneButtonLabelReflectsOrdnanceTypes` asserts that a Minelayer in `BS_PlaceMines` shows only the mine button and after `completeMinePlacement()` transitions to `BS_PlaceSeekers` shows only the seeker button; `testDrawPlaceMinesExpandsPanelHeightWhenRowsExceedMinimum`, `testDrawPlaceSeekersExpandsPanelHeightWhenRowsExceedMinimum`, and `testDrawSeekerActivationExpandsPanelHeightWhenRowsExceedMinimum` verify per-phase height expansion behaviorally; `testOrdnancePlacementAndActivationPanelHeightAutoExpands` (GUI live test) verifies the guard end-to-end including the `ensureLowerPanelLayoutState()` max-preservation path.
  - **SMF-03**: `drawOffensiveSeekerPendingRows()` is called from `draw()` (not `drawCurrentShipStats()`) inside a `PH_ATTACK_FIRE` guard, at `(leftOffset, getActionButtonRowBottom()+BORDER)`; `drawCurrentShipStats()` no longer calls it; the pending-list region is positioned left of the ship-status widget; if drawn rows extend below `requestedDisplayHeight`, the height is expanded and `applyRequestedDisplayHeight()` is called; `testOffensiveSeekerPendingListRegionVisibilityAndRecall` verifies `m_pendingSeekerRecallRegions` is empty after a draw with no pending seekers, non-`PH_ATTACK_FIRE` phases leave the recall list empty, and `recallSelectedOffensivePendingSeekerAtHex` returns false when no seekers are pending.
  - **SMF-04**: `FTacticalSeekerMissileState` carries an `activationPhaseIndex` int field (not persisted) stamped at activation time; `FTacticalGame` owns `m_seekerActivationPhaseIndex` (initialized to 0 in `reset()`, incremented by `++` in `beginSeekerActivationPhase()`); both `activateSelectedInactiveSeeker()` and `activateInactiveSeekerAtHex()` stamp `activationPhaseIndex = m_seekerActivationPhaseIndex` on the newly activated entry; `getActiveSeekersByMovingPlayerThisPhase()` on `FTacticalGame` filters the full active-seeker list to entries whose `activationPhaseIndex == m_seekerActivationPhaseIndex`; `FBattleScreen::getActiveSeekersByMovingPlayerThisPhase()` delegates to that model method and is declared `const` returning by value; the existing `getActiveSeekersByMovingPlayer()` accessor is unchanged and continues to serve as the full-list accessor for driving movement; `save()` does not reference `activationPhaseIndex`; no wx headers appear in `FTacticalGame.h`.
  - **Menu bar**: `File`/`Settings`/`Help` order; only `Quit` binds `wxEVT_MENU` and calls `Close(true)`; close path uses `onClose(...)`/`closeBattleScreen(GetReturnCode())`, never `exit(...)`.
  - **Speed spin control**: constructed with `wxSP_ARROW_KEYS | wxSP_VERTICAL`, `SetMinSize(GetBestSize())`, under a sizer-managed layout.
  - **Tactical zoom image**: loaded via `resolveAssetPath("data/zoom.png")`, not raw base-path concatenation.
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

The top-level `tests/Makefile` bootstraps the required `../src/*` model/gui libraries before linking `SSWTests` and also builds `tests/gui/GuiTests`; `cd tests && make` works from a clean state without a separate root build.

When you need a fresh top-level rebuild of the tactical standalone runner before executing it, use:
```bash
cd tests && make tactical-tests && ./tactical/TacticalTests
```
That target runs the tactical module clean step before relinking `tests/tactical/TacticalTests`, which is the preferred hygiene path when validating tactical-only changes. The standalone runner registers and executes `FTacticalCombatReportTest`, `FTacticalDamageSummaryGUITest`, and `WXTacticalUIAdapterTest`.

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
That target builds `libguiTests.a` and the `GuiTests` runner after bootstrapping the needed `../../src/*` libraries, including `src/battleSim`.

GUI test execution requires an X display. Run `./GuiTests` directly when a display session is already available; in headless automation, ensure `xvfb-run` (or an equivalent virtual-display wrapper) is installed and run the suite under it, for example `xvfb-run -a ./GuiTests`.

Canonical headless GUI validation command:
```bash
cd tests/gui && make && xvfb-run -a ./GuiTests
```

For deterministic live-dialog coverage, prefer the shared `tests/gui/WXGuiTestHarness` helpers over ad hoc timers: `showModalWithAction(...)` / `runModalFunctionWithAction(...)` / `runVoidFunctionWithAction(...)` schedule in-dialog interactions before the modal fallback closes; `waitForTopLevelWindow(...)` / `waitForModalDialog(...)` prove dialogs appeared; `waitForTopLevelWindowClosed(...)` provides a bounded wait for posted close requests. Use `showModalWithAction(...)` for direct modal ownership when app-level scheduling can race the dialog; raise fallback timeouts only where the close path can settle slowly. (Close-wait, centering, and teardown invariants are listed in the GUI tests bullet above.)

For deterministic strategic rendering coverage in headless runs, prefer offscreen `wxMemoryDC` assertions that check observable pixels or icon-placement regions for `WXMapDisplay`, `WXPlayerDisplay`, and `WXGameDisplay`; keep live parent frames for smoke-level paint-path checks such as `FGamePanel`.

Test makefiles include `-fprofile-arcs -ftest-coverage` for gcov coverage analysis. Enable coverage reporting with `make COVERAGE=1`.

To generate a browsable HTML coverage report with `gcovr`, rebuild and run the covered test binaries first, then emit the report from the repo root:
```bash
make all_clean
make COVERAGE=1 tests
cd tests && ./SSWTests
gcovr --root . --html-nested doc/coverage/index.html
```
If you need GUI coverage included in the report, also run the GUI suite before invoking `gcovr`, for example `cd tests/gui && xvfb-run -a ./GuiTests`, then rerun the `gcovr` command from the repository root.

There is no fully automated end-to-end wx GUI playback system; milestone acceptance relies on model/interface regression coverage via mock `IStrategicUI` and `ITacticalUI` seams plus build/test validation.

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
Use `#ifndef _<FILENAME>_H_` pattern (e.g., `#ifndef _FGAME_H_`). 

### Function Comments

Doxygen-style file headers are mandatory in all header files. You **MUST** add or update these when adding or editing functions.

For simple one-line functions or getters and setters you can use a simple one-line doxygen comment (preceded by a triple forward slash) that gives a very brief description of the function's role like this example:


```cpp
/// get the time left in the transit
const int getTransitTime() const { return m_transitTime; }
```

For classes and major functions use a full header block like below which should proceed the function declaration (classes do not need the `@param` fields):
```cpp
/**
 * @brief Brief description
 * 
 * Full description of the function's purpose, input values, return values, 
 * and any other notes needed to understand its operation or the reason a
 * specific algorithm was chosen.
 * 
 * @param Include a paramater description for each function parameter
 * ...
 * 
 * @author Name1, Name2, ...
 * @date Created: MMM DD, YYYY
 * @date Last Modified: MMM DD, YYYY
 */
```
When writing new functions or classes, this header should be added to the function or class with the author being the agent model (with reasoning level) that wrote the code.  The format for this should be `<model_name> (<reasoning level>)`. The `Created` and `Last Modified` dates should be the current date.  

When updating a function or class, the `Last Modified` date should be updated to the current date and the current agent model should be added to the list in the `@author` field if it is not already there. When updating authors, **never** remove an old author, only add the new authors to the author list which should be formatted as a comma separated list (i.e. `@author <author 1>, <author 2>, ...`).

Updates should update the description and parameter fields as needed to reflect changes made.

### Memory Management
- Raw pointers common (`FVehicle*`, `FWeapon*`) - no smart pointers in legacy code
- `FGame` destructor responsible for cleaning up `FPlayer` and `FMap` objects
- Component ownership follows composition hierarchy (ship owns weapons/defenses)

## wxWidgets Integration

### Common Patterns
- `wxInitAllImageHandlers()` is not needed (automatic since 3.1); use `wxOVERRIDE` instead of raw `override`. See `artifacts/WXWIDGETS_UPGRADE_CHANGES.md` for full migration notes.
- Main app classes inherit from `wxApp` (`FApp`, `FBattleSimApp`)
- UI panels inherit from generated GUI base classes in `include/gui/`; shared tactical wx type aggregation lives in `include/gui/GuiTypes.h`
- Use `wx-config --cxxflags` and `wx-config --libs` for GUI-facing executable or gui-module builds; keep active `wx-config` usage out of the six non-GUI module Makefiles and preserve their explicit non-GUI build flags separately.
- On wxGTK, prefer `wxDefaultSize` plus `SetMinSize(GetBestSize())` for sizer-managed `wxSpinCtrl` controls instead of narrow fixed widths so spin buttons keep valid geometry after layout; when the control is used as a value-selection spinner, explicitly request vertical arrows with `wxSP_ARROW_KEYS | wxSP_VERTICAL` in the style.
- For wxGTK dialog constructors that rely on sizers, ensure the first-show size is locked before the dialog is displayed: either use `Layout()`/`Fit(this)` plus `SetMinSize(GetBestSize())`, or when the constructor immediately computes the final fixed-content size use `SetSizerAndFit(...)` plus `SetMinSize(GetSize())`; in both cases center on the parent (or on screen when unparented) so first-show geometry does not clip controls or action buttons.
- For top-level frames without an owning parent, prefer `CentreOnScreen(wxBOTH)` after the final first-show size is established; for adapter-launched or parent-backed dialogs, prefer `CentreOnParent(wxBOTH)` with `Centre(wxBOTH)` only as the explicit no-parent fallback so live placement tests stay deterministic across BattleSim, strategic, and tactical flows.
- When a dialog rebuilds dynamic row content at runtime, re-fit after the rebuild and update both the minimum size and current size from the new best size so follow-on layout passes preserve the expanded geometry.
- Shared wx startup splash/frame bootstrap for both apps lives in `include/gui/WXStartupLaunch.h` via `createStartupSplashAndFrame(wxApp&, FStartupFrameFactory, int)`; keep `FApp::OnInit()` and `FBattleSimApp::OnInit()` delegating to that seam. Splash creation precedes frame creation/show; use `wxSPLASH_CENTRE_ON_SCREEN` and `wxFRAME_NO_TASKBAR | wxSTAY_ON_TOP` for the splash, center the startup frame on screen before showing it, and pass `wxDefaultPosition` into `FMainFrame`.
- Any control visually owned by a `wxStaticBoxSizer` must be created under that sizer's `GetStaticBox()` (or a child container owned by that static box), not under the dialog directly.
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
- `include/tactical/FTacticalGame.h` is the non-wx tactical model surface; it owns the canonical tactical mechanics state (`FTacticalHexData`/`FTacticalTurnData`) plus the delegation-facing API categories used by `FBattleScreen` forwarding (state/control, setup/scenario, ship/weapon/defense selection, hex-click dispatch, movement/fire progression, renderer-facing tactical state accessors, and tactical report access).
- `include/tactical/ITacticalUI.h` is the non-wx tactical UI boundary; keep it free of wx includes and limited to the tactical callback surface, including the `installUI(ITacticalUI*)` / `getUI()` seam on `FTacticalGame`. Tactical model regression tests install mock `ITacticalUI` implementations to validate deterministic winner and callback behavior without wx runtime objects.
- `FTacticalHexData`/`FTacticalTurnData` are the model-owned tactical state; `FBattleBoard` does not keep duplicate mechanics containers. Keep renderer-only geometry/cache data on the wx side and tactical occupancy, path, range, mine, and turn bookkeeping in `FTacticalGame`.
- `src/tactical/FTacticalGame.cpp` owns battle setup/state transitions, movement reset/finalization helpers, tactical report lifecycle (`FTacticalCombatReport`), `fireAllWeapons()` report aggregation, and winner/end-of-combat helpers.
- `include/gui/WXTacticalUI.h` and `src/gui/WXTacticalUI.cpp` are the wx-backed tactical adapter; `FBattleScreen` owns, installs, and detaches it against its `FTacticalGame` during setup and teardown. The GUI smoke harness exercises the parent-backed redraw, damage-summary, modal message, winner, and ICM-selection paths; `WXTacticalUI` is not the live runtime owner.
- Keep direct wx includes out of `include/tactical/FBattleScreen.h`, `include/tactical/FBattleDisplay.h`, `include/tactical/FBattleBoard.h`, and their `src/tactical/` counterparts; when those tactical GUI-adjacent files need wx declarations, route them through `gui/GuiTypes.h` so the include aggregation stays gui-owned.
- The active runtime tactical GUI flow is centered on `FBattleScreen`, `FBattleBoard`, and `FBattleDisplay`; `FTacticalGame`, `ITacticalUI`, and `WXTacticalUI` are the model/adapter layer. Key ownership rules:
  - `FTacticalGame::fireAllWeapons()` captures destroyed ship IDs; `FBattleDisplay` shows the summary dialog; `FBattleScreen::clearDestroyedShips()` consumes `getLastDestroyedShipIDs()`; `FTacticalGame::clearLastDestroyedShipIDs()` clears bookkeeping exactly once.
  - `runDestroyedShipCleanupLifecycle(...)` in `src/tactical/FBattleScreen.cpp` locks the order: selection-clear → redraw → bookkeeping-clear → winner handling.
  - `FBattleDisplay` acts as a HUD/event translator for weapon selection, defense selection, fire-phase completion, mine-placement setup/completion, destroyed-ship cleanup, and destroyed-side winner fallback.
  - `FBattleBoard` is narrowed to geometry/rendering/scrolling/hit-testing; clicks route through `FBattleScreen::handleHexClick()` to `FTacticalGame`; occupancy/path/range/mine overlays are read back through model-backed accessors.
  - `FTacticalGame::completeMovePhase()` is the canonical post-move resolution seam for mine interactions, destruction handling, and occupancy finalization.
  - `FBattleDisplay` reserves action-button space via `ACTION_PROMPT_TOP_MARGIN`, `ACTION_PROMPT_LINE_HEIGHT`, `ACTION_PROMPT_MAX_LINES`, `ACTION_PROMPT_BUTTON_GAP`, and helper accessors so completion buttons stay below the multi-line prompt block.
- Route strategic-layer dialogs, prompts, and notifications from `FGame` through `IStrategicUI`; keep wx dialog implementations in the gui module (for example `WXStrategicUI`) instead of reintroducing direct gui dependencies in strategic code. `WXStrategicUI` may still run parentless when a live wx runtime exists, but shared modal entry points must first confirm `wxTheApp` is available before constructing dialogs; when no usable wx runtime exists, modal-return helpers fall back to `wxID_CANCEL` and void dialog helpers return early while message-only paths use the non-modal output fallback.
- Strategic regression tests may install mock `IStrategicUI` implementations to validate `FGame` initialization, setup callbacks, and message-routing behavior without creating wx dialogs or windows; keep that seam usable for non-wx test coverage.
- `SatharRetreatGUI`, `CombatLocationGUI`, `TwoPlanetsGUI`, and `SelectResolutionGUI` use `SetSizerAndFit(...)` plus `SetMinSize(GetSize())` and `Centre(wxBOTH)` for first-show sizing.
- Keep icon filenames in model objects and resolve ship or fleet `wxImage` assets in GUI render paths through `WXIconCache`, including tactical displays that draw ships. `WXIconCache` follows the shared `FGameConfig::resolveAssetPath(...)` policy; `WXPlayerDisplay`/`WXGameDisplay` draw paths must be tolerant of invalid images before scaling or drawing.

## Documentation

- Doxygen configuration: `doc/Doxyfile`
- Generated documentation: `doc/` (`make docs` writes HTML output to `doc/html/`)
- AI agent information: `AGENTS.md` (this file)
- Design documentation: `doc/DesignNotes.md`
- User Guide: `doc/UsersGuide.md`
- Game rules for tactical combat: `doc/rules/tactical_operations_manual.md`
