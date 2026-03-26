# UI / Model Separation Design

## Problem Statement

The SSW codebase currently has significant entanglement between game mechanics and
the wxWidgets display engine.  This entanglement makes it impossible to swap out the
UI layer, complicates automated testing, and violates the single-responsibility
principle across multiple modules.

The goal of this refactor is to draw a clean boundary between the **model** (game
logic, rules, state) and the **view** (rendering, event handling, dialogs).  After
the refactor, swapping wxWidgets for another UI toolkit—or driving the game headlessly
from a test harness—should require touching only the GUI-side of that boundary.

---

## Current Coupling Inventory

The following violations were found during the survey.

### `FVehicle` (ships module)
- `#include "wx/wx.h"` at the top of the header.
- `wxImage * m_icon` — stores a rendered image object inside a game-logic class.
- `void setIcon(std::string)` — loads a `wxImage` inside a model class.
- `const wxImage * getIcon()` — exposes wxImage from a model class.

Note: `m_iconName std::string` already exists and is the clean model field; `m_icon`
is a redundant, display-specific parallel.

### `FPlayer` (strategic module)
- `wxImage m_fleetIcon` — same pattern as FVehicle.
- `void setFleetIcon(std::string)` — loads a wxImage inside a model class.

Note: `m_iconName std::string` already exists as the clean field.

### `FGame` (strategic module — the most deeply coupled class)
- Constructor parameter `wxWindow* win` — game singleton is tightly bound to a window.
- Private members: `wxWindow* m_parent`, `wxImage* m_tenday`, `wxImage* m_day`.
- Rendering methods: `draw()`, `draw(wxDC&)`, `drawTurnCounter()`.
- wx event handlers: `onLeftDClick(wxMouseEvent&)`, `onLeftUp(wxMouseEvent&)`.
- Dialog-launching methods: `showPlayers()`, `showRetreatConditions()`,
  `onAddSatharShips()`.
- Internal dialog calls throughout `init()`, `addSatharShips()`, `addUPFUnattached()`,
  `moveFleets()`, `checkForVictory()`, `resolveCombat()` — all directly create and show
  wxWidgets dialogs or message boxes.
- Includes GUI headers: `gui/SystemDialogGUI.h`, `gui/UPFUnattachedGUI.h`,
  `gui/SatharFleetsGUI.h`, `gui/ViewFleetGUI.h`, `gui/SatharRetreatGUI.h`,
  `gui/SelectCombatGUI.h`, `gui/WXGameDisplay.h`, `gui/WXMapDisplay.h`,
  `gui/WXPlayerDisplay.h`, `<wx/wx.h>`, `<wx/numdlg.h>`.

### `Frontier.h` (top-level shared header)
- `#include "wxWidgets.h"` — the catch-all game header pulls in wxWidgets globally.
- `typedef std::vector<wxImage> ImageList` — a display type embedded in the shared header.

### Tactical layer (`FBattleBoard`, `FBattleDisplay`, `FBattleScreen`)
- All three classes directly extend wxWidgets base classes *and* own game-mechanics
  state (phase, active player, current ship/weapon, movement tracking, combat resolution).
- Game state and rendering are completely intermingled with no separating boundary.

---

## Existing Infrastructure to Leverage

Three stub classes already exist that represent a prior (incomplete) attempt at
separation:

- `WXMapDisplay` (`include/gui/WXMapDisplay.h`) — has `draw(wxDC&)` and `getScale(wxDC&)`.
- `WXPlayerDisplay` (`include/gui/WXPlayerDisplay.h`) — has `drawFleets(wxDC&, FPlayer*)`.
- `WXGameDisplay` (`include/gui/WXGameDisplay.h`) — empty; exists as a placeholder.

These will be used and extended during the refactor.

Both `FVehicle` and `FPlayer` already store icon file names in `m_iconName`; the
`wxImage` members are redundant and can be removed without losing information.

---

## Design Approach

The refactor uses an **interface adapter** pattern rather than a strict MVC pattern,
since the existing architecture does not cleanly separate controllers.  The key artifact
is a pure-virtual interface header (`IStrategicUI`) that the strategic layer calls
back through, with a concrete `WXStrategicUI` implementation in the GUI module.

The same pattern is applied at the tactical level with `ITacticalUI` /
`WXTacticalUI`.

No new external dependencies are introduced.  All interfaces are pure C++ virtual
classes.  The GUI module remains the only module that may include wxWidgets headers.

---

## Milestones

All milestones are designed to leave the game in a compilable, runnable state unless
explicitly noted otherwise.

---

### Milestone 1 — Remove wxImage from Model Classes

**Objective:** Eliminate wxWidgets image objects from `FVehicle`, `FPlayer`, and
`Frontier.h`.  After this milestone, the ships and strategic modules no longer include
any wxWidgets headers.

**Summary of changes:**

1. **`FVehicle`**
   - Remove `wxImage * m_icon` member.
   - Remove `const wxImage * getIcon()` method.
   - Remove `#include "wx/wx.h"` from `FVehicle.h`.
   - Remove the body of `setIcon(std::string)` that calls `new wxImage(...)`.
     Keep `setIcon` as a setter that stores the name in `m_iconName` only.
   - Remove `#include <wx/wx.h>` from `FVehicle.cpp` if present.

2. **`FPlayer`**
   - Remove `wxImage m_fleetIcon` member.
   - Remove the body of `setFleetIcon(std::string)` that loads the image; keep the
     method so callers do not break, but have it only update `m_iconName`.
   - Remove any wx include pulled in solely for the image.

3. **`Frontier.h`**
   - Remove `#include "wxWidgets.h"`.
   - Remove `typedef std::vector<wxImage> ImageList`.
   - Move `ImageList` to `include/gui/GuiTypes.h` (new file) as
     `typedef std::vector<wxImage> ImageList`.

4. **GUI layer image cache** — Any display code that previously called `vehicle->getIcon()`
   must now obtain images through a new `WXIconCache` singleton in the gui module.
   - `WXIconCache` (`include/gui/WXIconCache.h` / `src/gui/WXIconCache.cpp`) maps icon
     file-name strings to loaded `wxImage` objects.  Icons never change at runtime, so
     images are loaded on first request and returned from cache on all subsequent calls.
   - The primary callers are `WXPlayerDisplay::drawFleets()` and `FBattleDisplay::draw()`.
     Both are updated to call `WXIconCache::instance().get(vehicle->getIconName())`.
   - `WXIconCache` is the only place in the codebase where `wxImage(filePath)` is called
     for vehicle or fleet icons.

**Game status after this milestone:** Fully functional.

**Risk:** Low.

---

### Milestone 2 — Define `IStrategicUI` Abstract Interface

**Objective:** Create a pure-virtual interface that represents all UI operations the
strategic layer needs.  No behavior changes; this is header-only preparation.

**New file:** `include/strategic/IStrategicUI.h`

```cpp
namespace Frontier {

class IStrategicUI {
public:
    virtual ~IStrategicUI() {}

    // Informational dialogs
    virtual void showMessage(const std::string& title,
                             const std::string& body) = 0;
    virtual void notifyFailedJump(const std::string& fleetName) = 0;
    virtual void notifyVictory(int result) = 0;  // 1=UPF, 2=Sathar, 3=Draw

    // Game setup dialogs — return 0 on success, non-zero to cancel
    virtual int selectRetreatCondition() = 0;
    virtual int runUPFUnattachedSetup(FPlayer* player, FMap* map) = 0;
    virtual int runSatharFleetSetup(FPlayer* player, FMap* map,
                                    bool isInitialSetup) = 0;

    // In-game interaction
    virtual void showSystemDialog(FSystem* sys, FMap* map,
                                  FPlayer* player) = 0;
    virtual void showFleetDialog(FFleet* fleet, FSystem* location,
                                 FSystem* destination) = 0;
    virtual void showRetreatConditions(const std::string& text) = 0;
    virtual int  selectCombat(FSystem* sys,
                              FleetList defenders,
                              FleetList attackers,
                              PlayerList* players) = 0;

    // Rendering coordination
    virtual void requestRedraw() = 0;
};

}
```

**Forward declarations** needed in this header: `FPlayer`, `FMap`, `FSystem`,
`FFleet`, `FleetList`, `PlayerList`.  Include only the minimal forward-declaration
headers required.

**Game status after this milestone:** Unchanged — no implementation exists yet.

**Risk:** None.

---

### Milestone 3 — Implement `WXStrategicUI` and Wire to `FGame`

**Objective:** Create the concrete wxWidgets implementation of `IStrategicUI`, change
`FGame` to use it instead of calling wx directly, and remove all gui headers and wx
includes from `FGame`.

**Summary of changes:**

1. **New file:** `include/gui/WXStrategicUI.h` / `src/gui/WXStrategicUI.cpp`
   - Class `WXStrategicUI : public Frontier::IStrategicUI`.
   - Constructor takes `wxWindow* parent`.
   - Each virtual method wraps the dialog or message box that `FGame` currently
     calls directly.
   - `showMessage` wraps `wxMessageBox`.
   - `notifyFailedJump` wraps `wxMessageDialog`.
   - `notifyVictory` wraps `wxMessageDialog`.
   - `selectRetreatCondition` wraps `SatharRetreatGUI::ShowModal()`.
   - `runUPFUnattachedSetup` wraps `UPFUnattachedGUI::ShowModal()`.
   - `runSatharFleetSetup` wraps `SatharFleetsGUI::ShowModal()`.
   - `showSystemDialog` wraps `SystemDialogGUI::ShowModal()`.
   - `showFleetDialog` wraps `ViewFleetGUI::ShowModal()`.
   - `showRetreatConditions` wraps `wxMessageDialog`.
   - `selectCombat` wraps `SelectCombatGUI::ShowModal()`.
   - `requestRedraw` calls `m_parent->Refresh()`.

2. **`FGame`**
   - Add a new protected constructor `FGame(IStrategicUI* ui)` (or modify the
     existing one).
   - Change `FGame::create(wxWindow*)` to `FGame::create(IStrategicUI* ui)`.
   - Replace every inline wx call with the corresponding `m_ui->method()` call.
   - Remove all `#include "gui/..."` headers.
   - Remove `#include <wx/wx.h>` and `#include <wx/numdlg.h>`.
   - Remove `wxWindow* m_parent`.
   - Retain `m_gui` flag as a convenience: `m_gui = (ui != nullptr)`.

3. **`FMainFrame`**
   - Construct a `WXStrategicUI` before calling `FGame::create()`.
   - Pass the `WXStrategicUI*` to `FGame::create()`.

4. **`src/strategic/Makefile`** — Remove `-lgui` dependency if it was previously
   required; the strategic module should now link cleanly without it.

**Game status after this milestone:** Fully functional.  `WXStrategicUI` faithfully
re-implements all existing dialog behavior.

**Risk:** Medium.  Every wx call in `FGame` must be identified and migrated.  The
coupling inventory above lists all known sites.

---

### Milestone 4 — Extract Rendering and Event Handling from `FGame`

**Objective:** Remove the rendering methods and wx event handlers from `FGame`.  After
this milestone, `FGame` has no concept of coordinates, device contexts, or input events.

**Summary of changes:**

1. **Rendering methods removed from `FGame`:**
   - `draw()` (no-arg), `draw(wxDC&)`, `drawTurnCounter()`.
   - Equivalent rendering logic is moved into the GUI layer.  The existing
     `WXMapDisplay::draw(wxDC&)` and `WXPlayerDisplay::drawFleets(wxDC&, FPlayer*)`
     are already in the gui module.  A new `WXGameDisplay::draw(wxDC&, FGame&)` method
     is added (extending the existing empty `WXGameDisplay` class) that calls
     `WXMapDisplay::draw`, `WXPlayerDisplay::drawFleets`, and a new
     `WXTurnCounterDisplay::draw` for the turn counter.
   - `FGamePanel::onPaint` is updated to call `WXGameDisplay::draw()` directly.

2. **New `FGame` getters** (if not already present):
   - `unsigned int getRound() const`
   - `unsigned int getCurrentPlayerID() const`
   - `const PlayerList& getPlayers() const`

3. **wx event handlers removed from `FGame`:**
   - `onLeftDClick(wxMouseEvent&)` and `onLeftUp(wxMouseEvent&)`.
   - `FMainFrame` already handles these events; the logic inside FGame's handlers is
     moved to `FMainFrame` (or `FGamePanel`).  Two new game-command methods replace the
     wx-specific versions:
     - `FGame::handleMapClick(double mapX, double mapY)` — called by FMainFrame after
       converting screen coordinates to map coordinates using `WXMapDisplay::getScale()`.
     - `FGame::handleEndTurnClick(double screenX, double screenY, double scale)` — OR
       the turn-end logic is triggered directly from the menu/button handlers instead.

4. **`FGame` private image members removed:**
   - `wxImage* m_tenday` and `wxImage* m_day` are moved to `WXGameDisplay` (or a new
     `WXTurnCounterDisplay` helper inside `WXGameDisplay`).

5. **`IStrategicUI`** gains `requestRedraw()` (already specified in Milestone 2) which
   replaces the `m_parent->Refresh()` calls that remain after the above removals.

**Coordinate ownership principle** — This milestone enforces a clear distinction
between two coordinate systems that must never cross the model/view boundary in the
wrong direction:

- *Logical/game-space coordinates* — positions in the abstract game world (the
  strategic map's float coordinate system; tactical hex grid positions).  These are
  model data and must reside entirely in the mechanics layer.
- *Display/screen coordinates* — pixel positions on the render surface.  These belong
  exclusively to the GUI layer.

`FGame::handleMapClick(double mapX, double mapY)` correctly accepts logical map
coordinates; the screen-to-map conversion is performed by the GUI layer in `FMainFrame`
using `WXMapDisplay::getScale()`.

Note: Tactical hex positions currently stored in `FBattleBoard` (`hexData`, `turnData`)
represent game-mechanics state that temporarily resides in a wx class.  These will be
properly relocated to `FTacticalGame` in Milestone 5.  By the end of all milestones,
all game-space position data resides on the mechanics side of the boundary.

**Game status after this milestone:** Fully functional.

**Risk:** Medium.  The coordinate-conversion math in `onLeftDClick` must be preserved
correctly when moved to `FMainFrame`.

---

### Milestone 5 — Create `FTacticalGame` (Additive)

**Objective:** Define the new pure-C++ `FTacticalGame` class that will eventually own
all tactical game-mechanics state.  At this stage the class is created and populated,
but the existing wx-based classes are left completely intact and continue to run.  The
game is fully functional throughout this milestone.

**Summary of changes:**

1. **New class `FTacticalGame`** — `include/tactical/FTacticalGame.h` /
   `src/tactical/FTacticalGame.cpp`.  This class owns all game-mechanics state
   currently scattered across the three wx-based tactical classes:
   - All state variables from `FBattleScreen`: phase (`m_phase`), active-player
     flags (`m_activePlayer`, `m_movingPlayer`), current ship (`m_curShip`), current
     weapon (`m_curWeapon`), ship lists, fleet lists, move-complete flag, combat
     reporting, ICM data, and all phase-transition logic (`setState`, `setPhase`).
   - All hex-map game state from `FBattleBoard`: the `hexData` map (hex-center
     positions and per-hex ship lists), the `turnData` map (per-ship movement records),
     movement validation logic, and gravity-turn logic.  These are game-space
     (hex-coordinate) data, not display data; this is their correct home.
   - The `fireAllWeapons()` logic and combat-phase methods from `FBattleDisplay`.
   - End-of-combat detection and winner declaration logic.

   `FTacticalGame` does **not** inherit from any wxWidgets class.  It holds a pointer
   `ITacticalUI* m_ui` (the interface is defined in Milestone 6) which is initially
   set to `nullptr`.

2. **Update `src/tactical/Makefile`** to compile `FTacticalGame.cpp`.

**Game status after this milestone:** Fully functional.  `FTacticalGame` exists but
is not yet wired into the running game.

**Risk:** Medium.  Large amount of state to catalogue and move into the new class, but
no existing code is modified.

---

### Milestone 6 — Define `ITacticalUI` and Implement `WXTacticalUI`

**Objective:** Create the tactical UI interface and its wxWidgets implementation.
No existing code is modified; this is purely additive.

**Summary of changes:**

1. **New interface `ITacticalUI`** — `include/tactical/ITacticalUI.h`:

   ```cpp
   namespace Frontier {

   class ITacticalUI {
   public:
       virtual ~ITacticalUI() {}

       virtual void requestRedraw() = 0;
       virtual void showMessage(const std::string& title,
                                const std::string& body) = 0;
       virtual int  showDamageSummary(
                        const FTacticalCombatReportSummary& summary) = 0;
       virtual int  runICMSelection(std::vector<ICMData*>& icmData,
                                    VehicleList* defenders) = 0;
       virtual void notifyWinner(bool attackerWins) = 0;
   };

   }
   ```

   `FTacticalGame` holds a pointer `ITacticalUI* m_ui` and calls through it.

2. **New class `WXTacticalUI`** — `include/gui/WXTacticalUI.h` /
   `src/gui/WXTacticalUI.cpp`.  Implements `ITacticalUI`:
   - Constructor takes `wxWindow* parent`.
   - `requestRedraw` calls `m_parent->Refresh()`.
   - `showMessage` wraps `wxMessageBox`.
   - `showDamageSummary` wraps the existing `TacticalDamageSummaryGUI`.
   - `runICMSelection` wraps the existing `ICMSelectionGUI`.
   - `notifyWinner` shows the winner dialog currently in `FBattleScreen::declareWinner()`.

3. **Update `src/gui/Makefile`** to compile `WXTacticalUI.cpp`.

**Game status after this milestone:** Fully functional.  New classes exist but are
not yet wired in.

**Risk:** Low.

---

### Milestone 7 — Refactor `FBattleScreen` to Delegate to `FTacticalGame`

**Objective:** Replace `FBattleScreen`'s internal game-state members with a
`FTacticalGame*` it delegates to.

**⚠ Game status during this milestone:** The tactical combat board is expected to be
non-functional while the state handover is in progress.  Use a feature branch.
Once the milestone is complete the board must be fully operational.

**Summary of changes:**

1. `FBattleScreen` gains members `FTacticalGame* m_tacticalGame` and
   `WXTacticalUI* m_tacticalUI`, constructed in `FBattleScreen`'s constructor.
2. Set `m_tacticalGame->m_ui = m_tacticalUI`.
3. All state accessors (`getState`, `getPhase`, `getShip`, `getWeapon`, etc.) are
   forwarded to `m_tacticalGame`.
4. All mutators (`setState`, `setPhase`, `setShip`, `setWeapon`, etc.) are forwarded
   to `m_tacticalGame`.
5. Phase-transition logic in `FBattleScreen` is moved to `FTacticalGame`; `FBattleScreen`
   calls `m_tacticalGame->advancePhase()` (or equivalent) instead.
6. Remove the corresponding state member variables from `FBattleScreen`.

**Game status after this milestone:** Fully functional.

**Risk:** High.  Every accessor and mutator in `FBattleScreen` must be identified and
redirected.

---

### Milestone 8 — Refactor `FBattleDisplay` and `FBattleBoard`

**Objective:** Convert `FBattleDisplay` and `FBattleBoard` to pure rendering and
input-translation classes that read all game state from `FTacticalGame`.

**⚠ Game status during this milestone:** The tactical combat board may be temporarily
non-functional while the display/board classes are refactored.  Use a feature branch.
Once the milestone is complete the board must be fully operational.

**Summary of changes:**

1. **`FBattleDisplay`** — becomes a pure rendering panel:
   - `draw()` and `onPaint()` read all state from a `FTacticalGame*` reference passed
     at construction (obtained from `FBattleScreen`).
   - Event handlers (`onLeftUp`, button callbacks) translate wx input events into
     `FTacticalGame` method calls; they no longer contain game logic themselves.
   - `fireAllWeapons()` (already moved to `FTacticalGame` in Milestone 5) is removed
     from this class.
   - Weapon/defense selection event handlers call `FTacticalGame::selectWeapon()` /
     `FTacticalGame::selectDefense()`.

2. **`FBattleBoard`** — becomes a pure hex-map rendering and hit-test class:
   - `draw()` reads ship positions and hex state from `FTacticalGame`.
   - `onLeftUp()` performs only hit-testing (which hex was clicked) and passes the
     hex coordinate to `FTacticalGame::handleHexClick(FPoint hex)`.
   - The `hexData` and `turnData` data structures are removed from `FBattleBoard`;
     they now live in `FTacticalGame` (moved in Milestone 5).

**Game status after this milestone:** Fully functional.

**Risk:** High.  All rendering code must be updated to read from `FTacticalGame`.

---

### Milestone 9 — Update BattleSim

**Objective:** Update the `BattleSim` executable to verify it works correctly with
the new `FTacticalGame` / `WXTacticalUI` architecture introduced in Milestones 5–8,
and address any integration gaps.

**Background:** `BattleSim` currently launches a tactical battle by directly
constructing an `FBattleScreen` with hardcoded fleets (see
`FMainFrame::onShowBattleScreen` and `src/BattleSim.cpp`).  After Milestone 7,
`FBattleScreen` delegates all state to `FTacticalGame`; most changes will have been
absorbed transparently.  This milestone handles any remaining wiring or cleanup.

**Summary of changes:**

1. Verify that `BattleSim` constructs and runs a complete battle correctly after
   Milestones 5–8.
2. If `BattleSim` creates `FBattleScreen` through any path that bypasses the new
   `WXTacticalUI` construction, update that path so `FBattleScreen` always receives
   a valid `WXTacticalUI*`.
3. Optionally refactor `BattleSim` to construct `FTacticalGame` directly (rather than
   through `FBattleScreen`) if a cleaner integration is desired; this is not required
   for the milestone to be complete.
4. Build `BattleSim` and run a full battle to completion; verify behavior is identical
   to pre-refactor.

**Game status after this milestone:** Fully functional (both SSW and BattleSim).

**Risk:** Low.  `BattleSim` already constructs fleet lists externally; the main
change is ensuring the new construction path is exercised correctly.

---

### Milestone 10 — Final Validation and Cleanup

**Objective:** Verify that the separation is complete, the build system is correct,
and no non-GUI module includes wxWidgets headers.

**Summary of changes:**

1. Grep all headers and source files in `core/`, `strategic/`, `ships/`, `weapons/`,
   `defenses/`, `tactical/` for `#include.*wx` — any hit is a bug that must be
   resolved.

2. Verify that the `Makefile` for `strategic`, `ships`, `weapons`, `defenses`, and
   `tactical` modules do **not** pass `wx-config --cxxflags` or `wx-config --libs`.
   Only `gui` and `battleSim` modules should link against wxWidgets.

3. Add or update unit tests:
   - Construct `FGame` with a mock `IStrategicUI` that records calls; verify the
     game runs the setup sequence without a real GUI.
   - Construct `FTacticalGame` with a mock `ITacticalUI`; run a two-ship combat to
     completion; verify outcome.

4. Run `make all_clean && make` from the repo root and confirm a clean build.

5. Run `cd tests && make && ./SSWTests` and confirm no regressions.

6. Update the `WARP.md` architecture guide to document the new module dependency rules.

**Game status after this milestone:** Fully functional.

**Risk:** Low (cleanup only).

---

## Dependency Rules After Refactor

```
core         ← no UI dependencies
ships        ← core only
weapons      ← core only
defenses     ← core only
strategic    ← core, ships, weapons, defenses, IStrategicUI (pure virtual header)
tactical     ← core, ships, weapons, defenses, ITacticalUI (pure virtual header)
gui          ← all of the above + wxWidgets
battleSim    ← gui, tactical, strategic + wxWidgets
```

Only `gui` and `battleSim` may include wxWidgets headers.

---

## Planner Agent Prompts

The following prompts can be fed directly to the Planner agent to produce detailed
implementation plans for each milestone.

---

### Planner Prompt -- Milestone 1

```
We are refactoring the SSW codebase (C++/wxWidgets 3.3.1) to separate UI from game
logic.  For Milestone 1, remove all wxImage objects from the model classes FVehicle
and FPlayer, remove the wxImage typedef from Frontier.h, and add an image cache to
the GUI module.

Context:
- FVehicle (include/ships/FVehicle.h, src/ships/FVehicle.cpp) contains a wxImage*
  m_icon member, setIcon(std::string) that loads a wxImage, and getIcon() that
  returns the wxImage*.  A parallel m_iconName std::string member already stores the
  file name.
- FPlayer (include/strategic/FPlayer.h, src/strategic/FPlayer.cpp) contains a
  wxImage m_fleetIcon member and setFleetIcon(std::string) that loads it.  A
  m_iconName std::string member already exists.
- Frontier.h (include/Frontier.h) has #include "wxWidgets.h" and
  typedef std::vector<wxImage> ImageList.
- The GUI layer (WXPlayerDisplay, FBattleDisplay, FBattleBoard) currently calls
  vehicle->getIcon() to draw ships.

Deliverables:
1. Remove wxImage* m_icon from FVehicle; update setIcon() to only store the name;
   remove getIcon().
2. Remove wxImage m_fleetIcon from FPlayer; update setFleetIcon() to only store the
   name.
3. Remove the wxWidgets include and ImageList typedef from Frontier.h.  Create a new
   header include/gui/GuiTypes.h containing the ImageList typedef.
4. Create a new WXIconCache singleton in the gui module (include/gui/WXIconCache.h,
   src/gui/WXIconCache.cpp).  It maps icon file-name strings to wxImage objects,
   loading each image on first request and returning the cached copy thereafter.
   WXIconCache is the only place in the codebase where wxImage(filePath) is called
   for vehicle or fleet icons.
5. Update all GUI-layer callers that referenced getIcon() to obtain images through
   WXIconCache::instance().get(vehicle->getIconName()) instead.
6. Build and run the tests to confirm no regressions.
```

---

### Planner Prompt -- Milestone 2

```
We are refactoring the SSW codebase (C++/wxWidgets 3.3.1) to separate UI from game
logic.  For Milestone 2, define a new pure-virtual interface IStrategicUI.

Context:
- FGame (include/strategic/FGame.h, src/strategic/FGame.cpp) currently calls
  wxWidgets dialogs directly: SatharRetreatGUI, UPFUnattachedGUI, SatharFleetsGUI,
  SystemDialogGUI, ViewFleetGUI, SelectCombatGUI, wxMessageBox, wxMessageDialog.
- We want to place these behind an abstract interface so that FGame can be driven
  without a GUI (e.g., for testing).
- No behavior changes happen in this milestone -- we are only adding the interface.

Deliverables:
1. Create include/strategic/IStrategicUI.h containing the pure-virtual class
   IStrategicUI with the methods specified in the plans/ui-model-separation-design.md
   Milestone 2 section.
2. Do not change any existing file.
3. Build to confirm the new header compiles cleanly with no errors.
```

---

### Planner Prompt -- Milestone 3

```
We are refactoring the SSW codebase (C++/wxWidgets 3.3.1) to separate UI from game
logic.  For Milestone 3, create WXStrategicUI and wire FGame to use IStrategicUI
instead of direct wxWidgets calls.

Prerequisites: Milestones 1 and 2 must be complete.

Context:
- IStrategicUI is defined in include/strategic/IStrategicUI.h (created in Milestone 2).
- FGame currently includes gui/SystemDialogGUI.h, gui/UPFUnattachedGUI.h,
  gui/SatharFleetsGUI.h, gui/ViewFleetGUI.h, gui/SatharRetreatGUI.h,
  gui/SelectCombatGUI.h, gui/WXGameDisplay.h, gui/WXMapDisplay.h,
  gui/WXPlayerDisplay.h, <wx/wx.h>, <wx/numdlg.h>.
- FGame::create() currently takes wxWindow*.
- See the full coupling inventory in plans/ui-model-separation-design.md.

Deliverables:
1. Create include/gui/WXStrategicUI.h and src/gui/WXStrategicUI.cpp implementing
   every IStrategicUI method by wrapping the corresponding wxWidgets dialog or
   message box (see design doc for the mapping).
2. Change FGame::create() to accept IStrategicUI* instead of wxWindow*.
3. Replace every direct wx call inside FGame with the corresponding m_ui->method()
   call.  Remove all gui/ and wx includes from FGame.h and FGame.cpp.
4. Update FMainFrame::onNew() and FMainFrame::onOpen() to construct a WXStrategicUI
   and pass it to FGame::create().
5. Update src/gui/Makefile to include WXStrategicUI.cpp.
6. Build and run tests to confirm no regressions.
```

---

### Planner Prompt -- Milestone 4

```
We are refactoring the SSW codebase (C++/wxWidgets 3.3.1) to separate UI from game
logic.  For Milestone 4, remove the rendering methods and wx event handlers from FGame.

Prerequisites: Milestones 1-3 must be complete.

Context:
- FGame still contains: draw(), draw(wxDC&), drawTurnCounter(), onLeftDClick(), onLeftUp().
- FGame still holds: wxImage* m_tenday, wxImage* m_day.
- WXMapDisplay (include/gui/WXMapDisplay.h) has draw(wxDC&) and getScale(wxDC&).
- WXPlayerDisplay (include/gui/WXPlayerDisplay.h) has drawFleets(wxDC&, FPlayer*).
- WXGameDisplay (include/gui/WXGameDisplay.h) is currently empty.
- FGamePanel::onPaint() currently calls FGame::draw(dc).
- FMainFrame::onLeftDClick() delegates to FGame::onLeftDClick().
- FMainFrame::onLeftUp() delegates to FGame::onLeftUp() and uses the return value to
  update menu items.
- Key principle: logical/game-space coordinates (the abstract map coordinate system,
  hex grid positions) are model data belonging to the mechanics layer.  Only
  pixel/screen coordinates belong to the GUI layer.  FGame::handleMapClick() must
  accept logical map coordinates; screen-to-map conversion is the GUI's responsibility.
- See plans/ui-model-separation-design.md for full details.

Deliverables:
1. Extend WXGameDisplay to implement a draw(wxDC& dc, Frontier::FGame& game) method
   that calls WXMapDisplay::draw(), WXPlayerDisplay::drawFleets() for each player, and
   a new turn-counter drawing helper.  Move the turn-counter image loading (m_tenday,
   m_day) and drawTurnCounter() logic from FGame into this class.
2. Add FGame getters: getRound(), getCurrentPlayerID(), getPlayers() if not already
   present.
3. Update FGamePanel::onPaint() to call WXGameDisplay::draw() instead of FGame::draw().
4. Remove draw(), draw(wxDC&), drawTurnCounter(), m_tenday, m_day from FGame.
5. Replace FGame::onLeftDClick() with a new FGame::handleMapClick(double mapX,
   double mapY) that accepts logical map coordinates instead of a wxMouseEvent.
   Update FMainFrame::onLeftDClick() to do the coordinate conversion itself using
   WXMapDisplay::getScale() and then call handleMapClick().
6. Replace FGame::onLeftUp() with FGame::processEndTurn() which returns int
   (0=no action, 1=UPF turn ended, 2=Sathar turn ended).  The hit-test against the
   button bounds (using the scale value) moves to FMainFrame::onLeftUp().
7. Build and run tests to confirm no regressions.
```

---

### Planner Prompt -- Milestone 5

```
We are refactoring the SSW codebase (C++/wxWidgets 3.3.1) to separate UI from game
logic.  For Milestone 5, create the new FTacticalGame class that will own all
tactical game-mechanics state.  This milestone is purely additive -- no existing
code is modified.

Prerequisites: Milestones 1-4 must be complete.

Context:
- FBattleScreen (include/tactical/FBattleScreen.h, src/tactical/FBattleScreen.cpp)
  extends wxDialog and owns all tactical game state: phase, active player, current
  ship/weapon, ship/fleet lists, move-complete flag, combat reporting, ICM data, and
  phase-transition logic.
- FBattleDisplay (include/tactical/FBattleDisplay.h, src/tactical/FBattleDisplay.cpp)
  extends wxPanel and contains fireAllWeapons(), weapon/defense selection logic, and
  mine placement logic.
- FBattleBoard (include/tactical/FBattleBoard.h, src/tactical/FBattleBoard.cpp)
  extends wxScrolledWindow and owns hex-map game state (hexData map, turnData map),
  movement validation, and gravity-turn logic.  Note: hexData and turnData represent
  game-space (hex-coordinate) positions -- not display data -- and belong in the
  mechanics layer.
- ITacticalUI will be defined in Milestone 6; for now FTacticalGame holds an
  ITacticalUI* m_ui initialized to nullptr.

Deliverables:
1. Create include/tactical/FTacticalGame.h and src/tactical/FTacticalGame.cpp
   containing a non-wx class that owns all the game-mechanics state listed above.
   Do not modify FBattleScreen, FBattleDisplay, or FBattleBoard.
2. Update src/tactical/Makefile to compile FTacticalGame.cpp.
3. Build and run tests to confirm no regressions (tactical board still uses old code).
```

---

### Planner Prompt -- Milestone 6

```
We are refactoring the SSW codebase (C++/wxWidgets 3.3.1) to separate UI from game
logic.  For Milestone 6, define the ITacticalUI interface and create the WXTacticalUI
implementation.  This milestone is purely additive -- no existing code is modified.

Prerequisites: Milestones 1-5 must be complete.

Context:
- FTacticalGame (created in Milestone 5) holds ITacticalUI* m_ui = nullptr.
- The following wx dialogs are currently called from within the tactical wx classes
  and need to be wrapped: TacticalDamageSummaryGUI, ICMSelectionGUI, and the winner
  dialog in FBattleScreen::declareWinner().

Deliverables:
1. Create include/tactical/ITacticalUI.h with the pure-virtual interface specified
   in the plans/ui-model-separation-design.md Milestone 6 section.
2. Create include/gui/WXTacticalUI.h and src/gui/WXTacticalUI.cpp implementing
   ITacticalUI by wrapping the wx dialogs listed above.
3. Update src/gui/Makefile to compile WXTacticalUI.cpp.
4. Build and run tests to confirm no regressions (tactical board still uses old code).
```

---

### Planner Prompt -- Milestone 7

```
We are refactoring the SSW codebase (C++/wxWidgets 3.3.1) to separate UI from game
logic.  For Milestone 7, refactor FBattleScreen to delegate all game state to
FTacticalGame.

Prerequisites: Milestones 1-6 must be complete.

WARNING: The tactical combat board is expected to be non-functional during this
milestone.  Use a feature branch.  The board must be fully operational before the
milestone is considered complete.

Context:
- FBattleScreen currently owns all tactical game state directly as member variables.
- FTacticalGame (Milestone 5) contains all the same state and logic.
- WXTacticalUI (Milestone 6) provides the wx dialog callbacks FTacticalGame needs.

Deliverables:
1. Add FTacticalGame* m_tacticalGame and WXTacticalUI* m_tacticalUI members to
   FBattleScreen; construct both in FBattleScreen's constructor.
2. Set m_tacticalGame->m_ui = m_tacticalUI.
3. Forward all state accessors (getState, getPhase, getShip, getWeapon, etc.) to
   m_tacticalGame.
4. Forward all mutators (setState, setPhase, setShip, setWeapon, etc.) to
   m_tacticalGame.
5. Move phase-transition logic to FTacticalGame; FBattleScreen calls
   m_tacticalGame->advancePhase() (or equivalent).
6. Remove the corresponding state member variables from FBattleScreen.
7. Build, run a full tactical combat, and confirm the board is fully functional.
```

---

### Planner Prompt -- Milestone 8

```
We are refactoring the SSW codebase (C++/wxWidgets 3.3.1) to separate UI from game
logic.  For Milestone 8, convert FBattleDisplay and FBattleBoard to pure rendering
and input-translation classes.

Prerequisites: Milestones 1-7 must be complete.

WARNING: The tactical combat board may be temporarily non-functional during this
milestone.  Use a feature branch.  The board must be fully operational before the
milestone is considered complete.

Context:
- After Milestone 7, FBattleScreen delegates to FTacticalGame for all state.
- FBattleDisplay still contains fireAllWeapons() (now also in FTacticalGame),
  weapon/defense selection logic, and mine placement logic.
- FBattleBoard still owns hexData and turnData (now also in FTacticalGame).

Deliverables:
1. Update FBattleDisplay so that draw()/onPaint() read all state from FTacticalGame
   (obtained via m_parent->getTacticalGame() or a direct reference).
2. Update FBattleDisplay event handlers to translate wx input events into
   FTacticalGame method calls; remove all game logic from event handlers.
3. Remove fireAllWeapons() from FBattleDisplay (it already lives in FTacticalGame).
4. Update weapon/defense selection handlers to call
   FTacticalGame::selectWeapon() / FTacticalGame::selectDefense().
5. Update FBattleBoard so that draw() reads ship positions and hex state from
   FTacticalGame.
6. Update FBattleBoard::onLeftUp() to only hit-test (determine which hex was clicked)
   and pass the result to FTacticalGame::handleHexClick(FPoint hex).
7. Remove hexData and turnData from FBattleBoard (they live in FTacticalGame).
8. Build, run a full tactical combat, and confirm the board is fully functional.
```

---

### Planner Prompt -- Milestone 9

```
We are refactoring the SSW codebase (C++/wxWidgets 3.3.1) to separate UI from game
logic.  For Milestone 9, update the BattleSim executable to work correctly with the
new FTacticalGame/WXTacticalUI architecture.

Prerequisites: Milestones 1-8 must be complete.

Context:
- BattleSim currently launches a tactical battle by directly constructing an
  FBattleScreen with hardcoded fleets (see FMainFrame::onShowBattleScreen and
  src/BattleSim.cpp).
- After Milestone 7, FBattleScreen constructs WXTacticalUI and FTacticalGame
  internally, so BattleSim may already work without changes.
- BattleSim already constructs fleet lists externally, so integration is expected
  to be straightforward.

Deliverables:
1. Build BattleSim and run a full battle to completion; confirm behavior is identical
   to pre-refactor.
2. If BattleSim creates FBattleScreen through any path that bypasses WXTacticalUI
   construction, fix that path.
3. Optionally refactor BattleSim to construct FTacticalGame directly rather than
   through FBattleScreen if a cleaner integration is desired (not required).
4. Confirm both the SSW and BattleSim executables build and run correctly.
```

---

### Planner Prompt -- Milestone 10

```
We are refactoring the SSW codebase (C++/wxWidgets 3.3.1) to separate UI from game
logic.  For Milestone 10, perform final validation and cleanup.

Prerequisites: Milestones 1-9 must be complete.

Deliverables:
1. Audit: grep all headers and source files in src/core, src/strategic, src/ships,
   src/weapons, src/defenses, and src/tactical for any remaining "#include.*wx"
   occurrences.  Each hit is a bug; fix every one found.
2. Verify that the Makefiles for core, strategic, ships, weapons, defenses, and
   tactical do NOT pass wx-config --cxxflags or wx-config --libs.  Fix any that do.
3. Add at least two new unit tests:
   a. A strategic test that constructs FGame with a mock IStrategicUI, runs the
      initialization sequence, and verifies game state without spawning any real GUI.
   b. A tactical test that creates FTacticalGame with a mock ITacticalUI, runs a
      simple two-ship combat to conclusion, and verifies the correct winner.
   Place tests in tests/strategic/ and tests/tactical/ respectively, following the
   existing CppUnit test structure.
4. Run: make all_clean && make from the repo root.  Confirm clean build.
5. Run: cd tests && make && ./SSWTests.  Confirm all tests pass.
6. Update WARP.md to document the new module dependency rules and the IStrategicUI /
   ITacticalUI interfaces.
```

