# Design Notes

## Combat Game Entry

Upon entering the combat game you have the following information:

1. A list of the attacking ships
2. A list of the defending ships
3. A flag indicating whether or not there is a planet (this could
   eventually be replaced by a planet object pointer that would have
   information about the planet such as the name and icon to use)
4. A pointer to the station object if there is a station orbiting the
   planet. This parameter is null if there is no station.

At any given point in the control flow, you need to know who the active
player is. You also need to know which player was the last moving player
so you can draw the paths of their ships. You need some mechanism to
track the ships' paths, locations, number of turns, speed and heading.
— **Done**: the live wx flow still keeps high-level dialog/control flow in
`FBattleScreen`, but the canonical tactical battle state now lives in
`FTacticalGame`. That model owns movement bookkeeping (`FTacticalTurnData`),
hex occupancy (`FTacticalHexData`), tactical report state, and the
renderer-facing accessors `FBattleScreen` forwards to the wx layer.
Milestone 8 Subtask 3 narrows `FBattleBoard` to geometry caching,
scrolling, rendering, and hit-testing; it no longer owns duplicate tactical
mechanics containers and now forwards clicked hexes through
`FBattleScreen::handleHexClick()` into `FTacticalGame`.

## Combat

A. During the attacking player's movement phase, you have to be able
   to select a MP ship and see its projected path. You should be able
   to select points along the path and have options to turn at the
   points selected. You should be able to back out any of your moves
   before committing the final movements. In this phase you have to
   know who the moving player is and that it is the same as the active
   player. You need a structure to keep track of all the movements. At
   the end of this phase the ships' speed and heading should be updated
   to reflect the final move position. The active player designation
   should be toggled but the moving player shouldn't be. — **Done**.

B. During the defensive fire phase, you should still draw the paths of
   the moving player's ships. The structure used to hold the movement
   should still be associated with the moving player and not the active
   player. The active player should be able to select his ships, select
   a weapon from that ship and then select a target ship from the
   enemy's ships to fire at. The game should compute the closest
   approach of the enemy's ship to the firing ship to validate that
   the ship was in range of the weapon and if necessary if it was in
   the field of fire for FF weapons. You should be able to change your
   weapons assignment before declaring that all assignments are done.
   Once this phase is over the weapons fire should be resolved and all
   damage applied to the target ships. Any destroyed ships should be
   removed from the game. The active player should switch back to the
   moving player so that they may fire their weapons. — **Done**.

C. During the offensive fire phase, the moving player now has the
   opportunity to fire its weapons at the non-moving player. Again,
   the player selects the ships and then the weapon and then the
   target. The moving player can fire its weapons from any point
   along its movement and the system has to calculate the optimal
   location to fire the selected weapon at the selected target or tell
   the player that it may not fire at that vessel. When selecting a
   moving vessel's weapon, the system should calculate every hex that
   the vessel can fire into during its move and highlight these
   hexes, storing them in a list of some sort. When the player attempts
   to select a target, it must be within the area defined. Target
   selections should be allowed to be changed until all weapon
   assignments have been completed and the player signifies that the
   turn is over. At this point the system should calculate the damage
   effects and apply the damage to the non-moving player's ships.
   — **Done**.

D. At the end of any firing phase, the target ships with ICMs should
   have the opportunity to declare how many ICMs they want to use in
   defense against incoming rocket weapons or mines. — **Done**
   (`fireICM()` / `ICMSelectionGUI`).

E. At the end of the moving player's fire phase, the movement data
   shall be cleared, the moving player marker will switch to the other
   player and the active player will switch to the new moving player.
   At which point the process will repeat but with the roles switched.
   — **Done** (`toggleActivePlayer()`, `toggleMovingPlayer()`, phase
   transitions in `FBattleScreen::setPhase()`).

## Keeping State

1. We need a Point class/structure that holds x, y data. This can be used
   to store the physical positions (i.e. centers) of the hexes for
   drawing on the screen or pairs of coordinates that correspond to the
   hex numbers on the map. This should be contained by the hexData
   structure and all the places where the hex data structure is being
   used in where the vector of ship positions is not needed can be
   replaced by this structure. — **Done** (`FPoint` in `include/core/FPoint.h`;
   `hexData` struct in `FBattleBoard.h` contains `FPoint pos` for hex centres.)

2. Instead of keeping track of the side as a boolean, maybe we should
   have two variables, `m_activePlayer` and `m_movingPlayer`, that store
   the ID of the player in question. All data structures will be
   referenced by the ID values stored here. This allows for expansion
   of the game from just two sides to multiple sides if
   necessary/desired. — **Partial**: `m_activePlayer` and `m_movingPlayer`
   are still `bool` in `FBattleScreen.h`; however `getActivePlayerID()` and
   `getMovingPlayerID()` map those booleans to real player IDs, so external
   code always works with IDs. The internal storage remains boolean.

3. All of the calls to `getSide()` should be replaced by `getActivePlayer()`.
   — **Done** (no remaining `getSide()` calls found in the codebase).

4. Lookups should use the ID to refer to and find the relevant data
   structures. — **Done** (ID-based lookups used throughout `FBattleBoard`
   and `FBattleDisplay`).

5. Likewise, when we need to access information about the moving
   player, the ID can be obtained by querying `getMovingPlayerID()`. The
   returned ID can then be used to access the relevant data
   structures. — **Done** (`getMovingPlayerID()` exists in `FBattleScreen.h`
   and is used throughout `FBattleDisplay.cpp` and `FBattleBoard.cpp`).

6. Data on ships needs to be indexable by the ship's ID value (i.e.
   the value returned by `getID()`). — **Done**.

7. For each moving ship, a list of the hexes moved through should be kept to
   facilitate determining where that ship can fire and whether or not
   it can be fired upon. — **Done** (`m_movementHexes`, `m_leftHexes`, and
   `m_rightHexes` now live in `FTacticalGame`; `FBattleBoard` renders them via
   `FBattleScreen` forwarding accessors and `computePath()` /
   `computeMovedWeaponRange()` still iterate over that model-owned path for
   range/fire checks).

8. We need a mechanism to keep track of whether or not a ship has fired
   its weapons defensively in a round or not. (This really isn't
   needed until we have more than two sides.) — **Not implemented**; tracking
   is phase-based rather than per-ship. Acceptable for the current two-player
   system.

9. However, we do need a mechanism to keep track of which weapons are
   firing at which ships. Should this be part of the weapon class?
   Probably, each weapon should keep track of the ship it is firing at
   as well. — **Done**: `FWeapon` holds `m_target` (`FVehicle*`) and
   `m_targetRange`; `FVehicle` is forward-declared in `FWeapon.h` to avoid
   circular dependencies.

10. We need a structure to keep track of which ships are where. Should
    ships know their position? Probably not, since this is related
    to the battle field implementation more than the ship itself. — **Done**:
    `FVehicle` has no position members; positions are stored in
    `FTacticalHexData m_hexData[100][100]` inside `FTacticalGame`, where each
    cell holds a `FPoint pos` (hex centre) and a `VehicleList ships` (ships
    present). `FBattleBoard` now reads occupants through `FBattleScreen`
    forwarding methods instead of owning a duplicate occupancy map.

11. Thinking on this more I believe we may want to use a `std::map`
    structure to hold the actual battle map. The key will be a Point
    object that holds the array coordinate of the hex. The data member
    held would be a hexData structure containing the list of ships and
    the coordinates of the hex centers. This would arbitrarily allow us
    to expand the map as needed during the course of a battle if ships
    needed to move off the edge of what is currently displayed. It might
    even be able to only store hexes that ships actually are in and
    add more as the game goes and they travel through new hexes. —
    **Not implemented**: the battle map is still a fixed
    `FTacticalHexData m_hexData[100][100]` array in `FTacticalGame`. A
    `std::map<FPoint,int>` is used for gravity turns and `FHexMap` is used for
    mine targets, but the main tactical occupancy map remains a fixed array.
    This refactor is still outstanding.

## Adding Defenses

1. Ships will need a `m_defenses` vector of `FDefense` pointers. — **Done**
   (`DefenseList m_defenses` in `FVehicle.h`; seven concrete types: `FNone`,
   `FReflectiveHull`, `FMaskingScreen`, `FICM`, `FStasisScreen`,
   `FProtonScreen`, `FElectronScreen`).

2. The first defense in the vector should be the default (i.e. None or
   Reflective Hull). — **Done**.

3. Add display of list of defenses. — **Done** (`drawDefenseList()` in
   `FBattleDisplay.cpp`).

4. Defense names should be green for the active defense, white for
   inactive defenses and red for damaged defenses. — **Done** (green = active,
   white = available, red = damaged, orange = unavailable due to power damage).

5. The defense list should be active on your moving turn only for
   changing the active defense. — **Done** (`checkDefenseSelection()` in
   `FBattleDisplay.cpp`).

6. To change the defense, simply click on the one you want (clicking on
   an active defense will turn it off). — **Done**.

7. ICMs shouldn't be active but when fired upon give an option of
   how many to use. — **Done** (`ICMSelectionGUI` dialog; triggered in
   `FBattleBoard.cpp` when weapons target a ship carrying ICMs).

8. When clicking a Masking Screen defense, confirm. This defense may
   be held off for now as it may be a bit tricky to do right. — **Done**:
   Masking Screen is fully implemented with 2-charge ammo, turn counting
   (`m_maskingScreenTurnCount`), and auto-clear when the count reaches zero.

9. Ships all get a `m_currentDefense` pointer and a `getCurrentDefense()`
   method that points to and returns the currently selected defense.
   This should be added to the `FVehicle` base class. — **Done**
   (`m_currentDefense` and `getCurrentDefense()` / `setCurrentDefense()` in
   `FVehicle`).

10. When firing a weapon, the weapon should query the target for the
    to-hit modifier of its current defense for the attacking weapon type
    and apply it to the to-hit roll. — **Done** (`FWeapon::attack()` calls
    `getCurrentDefense()->getAttackModifier(m_type)` on the target; special
    case for laser weapons dealing half damage against a Masking Screen).

11. When firing at a ship with ICMs and a functioning launcher, the
    defending player should be prompted to select the number of ICMs
    to use in defense for that attack. — **Done** (see item 7 above).

12. It'd be nice to be able to show the player all the attacks against
    that ship so they can ration as needed. How do we implement this?
    — **Partially addressed** via `ICMSelectionGUI` which shows nearby ships
    and incoming weapons; full ration-across-all-attacks display not yet done.

13. Masking screens are deployed at the start/end/middle? of round. —
    **Done**: activated by clicking the defense during the moving player's
    turn; charges deducted via `setCurrentDefense()`.

14. A Masking Screen needs to be cleared (i.e. turned off) if a turn is
    made during that round's movement phase. — **Done**: `decrementMSTurnCount()`
    is called each turn from `FBattleBoard.cpp`; the screen reverts to `None`
    when the counter reaches zero. Orbit handling uses a fixed turn count of
    5 for station types.

15. What visual cues do we use for the screens? The masking screen has a
    marker that we can use but the proton, electron and stasis screens
    don't. Maybe a blue circle for the proton, green for the electron
    and yellow for the stasis screen would work — put the letter (P,
    E, or S) in the middle. Add this as an overlay on the image of the
    ship. — **Partially done**: Masking Screen replaces the ship icon with
    `MaskingScreen.png`. **No circle overlays exist yet for Proton, Electron,
    or Stasis screens** — the P/E/S letter-in-circle idea is still outstanding.

## Advanced Damage Table

1. Initially just add this to the `FVehicle` class `takeDamage()` method.
   We can adapt it later. — **Done**: `FVehicle::takeDamage(int damage,
   int damageMod, bool basic, FTacticalDamageResolution*)` in `FVehicle.cpp`;
   the `basic` flag routes to hull-only damage or the full advanced table.

2. Maybe what we should do is make a `takeBasicDamage()` method that
   keeps the basic behavior. — **Not implemented as a separate method**:
   basic damage is handled by passing `basic=true` to `takeDamage()` instead.

3. The basis for the table can be lifted directly from the KHVector
   Obiter code as a lot of this was implemented there, at least the
   form of it. — **Done**: `FVehicle::advancedDamage()` implements the full
   d100 Knight Hawks damage table with double hull, ADF/MR loss, weapon hits
   (by priority list), defense hits, power system damage, combat control
   damage, navigation errors, electrical fire, disastrous fire, and DCR loss.
   Outcomes are recorded in `FTacticalDamageResolution`.

## Combat Phases

All three combat phases and their supporting mechanics are fully implemented:

- **Movement phase** (`PH_MOVE`): path projection via `computePath()`; hex
  highlighting for forward movement, left turns, and right turns stored in
  model-owned `m_movementHexes`, `m_leftHexes`, `m_rightHexes` on
  `FTacticalGame` and rendered by `FBattleBoard` through `FBattleScreen`
  accessors.
- **Defensive fire phase** (`PH_DEFENSE_FIRE`): closest-approach range
  checking in `setIfValidTarget()`; FF weapon arc validation in
  `computeFFRange()`.
- **Offensive fire phase** (`PH_ATTACK_FIRE`): moving ship fires from the
  optimal point along its path via `computeMovedWeaponRange()`; iterates
  every hex in the ship's path and checks weapon range/arc at each position.
- **ICM interception**: `fireICM()` in `FBattleScreen.cpp` presents
  `ICMSelectionGUI` before weapon damage is resolved.
- **End-of-phase cleanup**: `toggleActivePlayer()` and `toggleMovingPlayer()`
  in `FBattleScreen`; masking screen turn count decremented via
  `decrementMSTurnCount()` each movement phase.

Milestone 5 also adds an additive `FTacticalGame` implementation that mirrors
these mechanics without rewiring the runtime wx flow yet. The new model owns
safe-default reset/setup helpers, movement reset/finalization helpers,
tactical report lifecycle built on `FTacticalCombatReport`, `fireAllWeapons()`
combat report aggregation/cleanup, and winner/combat-end detection so later
migration milestones can delegate to it incrementally.

The raw `FTacticalReportEvent` payload now also preserves structured damage
metadata from both attack-generated internal events and standalone immediate
damage-resolution events. Weapon-hit effects carry `damageEffectType`,
`damagedWeaponType`, `damagedWeaponID`, and `damagedWeaponName`, while
defense-hit effects also preserve `damagedDefenseType` and
`damagedDefenseName`. That defense metadata now travels with the raw report
events alongside the existing label/detail text, even though the current
player-facing ship-summary rollup still relies on those labels/details for
defense-effect wording. Repeated weapon hits, defense hits, and other
non-weapon effects remain separate event entries in the same report path.

The summary rollup now consumes that structured weapon metadata to emit one
player-facing weapon effect entry per ship in the form `Weapon Hit:
<abbr-list>`, preserving event order and duplicates such as `Weapon Hit: LB,
LB, AR`. Mixed-effect lines keep that aggregated weapon text alongside other
effect summaries on the same ship display line, and `TacticalDamageSummaryGUI`
continues to render the prebuilt `FTacticalShipReportSummary::displayLines`
without inspecting raw attack or event collections itself.

Milestone 6 adds the tactical UI boundary alongside that model-only work:
`ITacticalUI` now defines the non-wx tactical callback surface and
`WXTacticalUI` provides the wx-backed adapter for redraw requests,
informational messaging, the tactical damage summary dialog, ICM selection,
and winner notification semantics. This remains additive infrastructure only;
the live tactical wx runtime still flows through `FBattleScreen`,
`FBattleBoard`, and `FBattleDisplay` until Milestone 7 rewires delegation.

Milestone 7 Subtask 1 extends `FTacticalGame` further as the canonical
additive tactical state-owner surface for upcoming `FBattleScreen`
delegation. Newly exposed API categories now cover:

- **State/control:** installed UI access (`installUI(ITacticalUI*)`,
  `getUI()`), control-mode toggles, done-state tracking, and close-in-progress
  status.
- **Setup/scenario:** planet choice, planet/station placement, station pointer,
  and direct access to the attacker/defender fleet and ship lists established
  during setup.
- **Ship/weapon selection:** current ship and weapon getters/setters remain on
  the model surface so UI callers can forward selection state instead of
  owning duplicate members.
- **Movement-state helpers:** previously added movement reset/finalization
  helpers continue to live in the model beside the newer state accessors,
  keeping tactical bookkeeping wx-free.
- **Tactical report access:** callers can continue to build, append, clear, and
  inspect `FTacticalCombatReport` state from the model layer.

The UI seam remains additive: `FTacticalGame` only stores an `ITacticalUI*`
through `installUI()` / `getUI()` and still introduces no wx headers or wx
types. This unblocks Milestone 7 delegation work by giving `FBattleScreen` a
stable forwarding surface for existing runtime state while preserving current
behavior until later subtasks move the active tactical wx path over to that
model.

Milestone 7 Subtask 2 keeps that migration additive but moves ownership
responsibility into `FBattleScreen` itself. The dialog now constructs and owns
its `FTacticalGame*` and `WXTacticalUI*`, installs the wx adapter onto the
model during setup, detaches it with `installUI(NULL)` during teardown, and
then deletes both delegated runtime objects safely. Existing guarded close-path
behavior is preserved through the `m_closeInProgress` gate and the same modal
`EndModal()` handling, while the tactical `Makefile` change remains limited to
the new `FBattleScreen` header dependencies needed for that wiring.

Milestone 8 Subtask 1 extends that additive delegation surface again, but only
on the model and `FBattleScreen` seam. `FTacticalGame` now exposes the
non-wx interaction APIs that later `FBattleDisplay` and `FBattleBoard` work
need in order to stop owning tactical mechanics directly. The newly validated
surface now includes:

- **Selection APIs:** model-side weapon selection, defense selection, ship
  selection from a clicked hex, and target assignment from a clicked hex.
- **Setup and placement APIs:** planet placement, station placement, ship
  placement, pending-rotation heading confirmation, and setup-oriented
  state access needed during scenario initialization.
- **Movement and fire progression APIs:** hex-click dispatch, mine placement
  start/completion helpers, move completion, defensive/offensive fire phase
  completion, and model-owned weapon-range computation.
- **Renderer-facing state accessors:** read access to hex occupancy, current
  selected ship hex, route and turn highlight lists, target and head-on range
  sets, mined hexes, mine targets/ownership, mine-capable ship lists, turn
  bookkeeping, and simple in-bounds / occupied checks.

`FBattleScreen` now forwards those calls directly to `FTacticalGame` and
requests redraws when the forwarded operation mutates tactical state. That
model/screen forwarding layer was the first Milestone 8 compatibility step.

Milestone 8 Subtask 2 then converts `FBattleDisplay` into a tactical HUD/view
translator for the flows already exposed on `FBattleScreen` / `FTacticalGame`.
The panel now reads battle state through `m_parent->getState()`,
`m_parent->getControlState()`, and `m_parent->getPhase()` during draw/paint,
removes its local `fireAllWeapons()` helper entirely, and delegates the
remaining non-view interactions through the existing forwarding seam:

- **Weapon/defense selection:** click handling now calls
  `FBattleScreen::selectWeapon()` / `selectDefense()` instead of mutating the
  selected ship directly from `FBattleDisplay`.
- **Fire-phase resolution (shipped Subtask 2 state):** defensive and offensive
  fire completion delegated through `resolveCurrentFirePhase()`, shared tactical
  summary UI, and `FBattleScreen::clearDestroyedShips()` rather than local
  `FBattleDisplay` fire-resolution logic. However, the shipped Milestone 8 build
  still had a blocking runtime cleanup-order bug in this seam; the explicit
  single-clear lifecycle contract was added later by Milestone 8 remediation
  Subtasks 1/2/4.
- **Mine placement/setup:** setup-speed completion now calls
  `beginMinePlacement()`, mine placement completion calls
  `completeMinePlacement()`, and the mine-placement UI reads the selectable
  ship list from `getShipsWithMines()` rather than caching a local copy.

Milestone 8 Subtask 3 then applies that forwarding surface to
`FBattleBoard`. The board now keeps only wx-side geometry, scaling,
scrolling, drawing, and hex hit-testing helpers (`m_hexCenters`,
`getHex()`, `drawGrid()`, and related rendering code). Tactical mechanics data
such as occupancy, turn bookkeeping, movement highlights, target/head-on
ranges, and mined hex overlays are read from `FTacticalGame` through
`FBattleScreen` pass-through accessors during rendering. Hex clicks are now
handled by computing the clicked board hex in `FBattleBoard::onLeftUp()` and
forwarding that `FPoint` to `FBattleScreen::handleHexClick()`, which delegates
the behavior to `FTacticalGame::handleHexClick()`.

This leaves the originally shipped Milestone 8 runtime tactical wx path only
partially rewired: `FBattleDisplay` behaved as a wx renderer/input translator
for delegated fire/setup flows and `FBattleBoard` was narrowed to
renderer/hit-test responsibilities, but two blocking runtime seams remained.
First, fire cleanup ordering could clear destroyed-ship bookkeeping before wx
consumption. Second, the live move-done path still bypassed canonical
`FTacticalGame::completeMovePhase()` via `setPhase(PH_FINALIZE_MOVE)`.

These blocking runtime defects were remediated after Milestone 8 shipment.
They are distinct from the accepted non-blocking limitation that full tactical
GUI scenario playthrough still requires manual wx validation in addition to the
module's automated seam-level tests.

### Validation Completed

Milestone 5 production-fix validation confirmed that the additive
`FTacticalGame` model now removes destroyed ships from the active side list,
`m_hexData` occupancy, and `m_turnInfo` bookkeeping while keeping
`m_hasWinner`/`m_winnerID` consistent when a side is eliminated. The same
validation also confirmed that `fireICM()` clears any stale `m_ICMData`,
resolves target occupants by scanning `m_hexData`, and only queues actionable
interception entries with a non-null `vehicles` pointer for same-side defenders
that still have usable ICM capability. Runtime tactical wx rewiring remained
out of scope for this milestone.

Validation command:

```bash
cd tests/tactical && make -s && ./TacticalTests
```

Result: `OK (65 tests)`

Milestone 8 Subtask 1 validation then confirmed that the new model-side
interaction API surface and `FBattleScreen` forwarding layer compile and behave
as documented without introducing wx dependencies into `FTacticalGame`.

Validation commands:

```bash
make -C src/tactical clean && make -C src/tactical
cd tests/tactical && make clean && make && ./TacticalTests
```

Result: `OK (48 tests)`

Milestone 8 Subtask 2 validation then confirmed that the `FBattleDisplay`
runtime path now delegates fire/setup interactions through the existing
`FBattleScreen` → `FTacticalGame` APIs and no longer carries its own local
fire-resolution helper.

Validation command:

```bash
cd tests/tactical && make -s && ./TacticalTests
```

Result: `OK (51 tests)`

The tactical source-inspection coverage for this step was updated at the same
time. `FTacticalBattleDisplayFireFlowTest` no longer expects a local
`FBattleDisplay::fireAllWeapons()` implementation or other transitional
source-string patterns from the pre-delegation flow; instead it checks the
delegation-oriented behavior that is now authoritative for weapon selection,
defense selection, fire-phase completion ordering, and mine-placement setup.

Milestone 8 Subtask 2 remediation validation then confirmed that fire-phase
destroyed-ship cleanup preserves cached destroyed IDs long enough for wx map
removal and still declares the correct winner by checking the destroyed side
(the active player's opponent) when model cleanup has already run. The same
runtime callback hardening now disables and hides the defensive-fire and
offensive-fire completion buttons before delegated fire-phase resolution runs,
while preserving the existing summary-dialog, cleanup, redraw, and
phase-advancement lifecycle.

The tactical GUI relayout follow-up now also locks in the sizer-managed action
button contract that those runtime paths depend on. Source-contract coverage in
`FTacticalBattleDisplayFireFlowTest` checks that the movement, defensive-fire,
offensive-fire, and mine-placement completion-button show/hide handlers call
`Layout()` immediately after toggling visibility, so a future regression that
leaves those controls at the panel origin is caught before live GUI review.

Validation commands:

```bash
make -C src/tactical
cd tests/tactical && make && ./TacticalTests
```

Result: `OK (65 tests)`

Milestone 8 remediation Subtask 1 then made the destroyed-ship lifecycle
contract itself explicit in both the model seam and the wx coordinator. The
documented ordering is now:

1. `FBattleDisplay::{onDefensiveFireDone(),onOffensiveFireDone()}` disables and
   hides the relevant fire-done button before calling
   `resolveCurrentFirePhase()`.
2. `FTacticalGame::fireAllWeapons()` resolves fire, clears any stale cached
   destroyed IDs, and captures the current destroyed-ship IDs while removing
   destroyed ships from model-owned tactical state.
3. `FBattleDisplay` shows the tactical summary dialog before any wx cleanup.
4. `FBattleScreen::clearDestroyedShips()` consumes
   `getLastDestroyedShipIDs()`, performs wx/runtime cleanup orchestration
   (including clearing a now-destroyed selected ship, redrawing the board/view,
   and preserving winner handling at the screen seam), and only then clears the
   bookkeeping via `clearLastDestroyedShipIDs()`. The screen seam does not call
   back into model destructive cleanup during this step.
5. `FBattleDisplay` advances the phase through
   `completeDefensiveFirePhase()` / `completeOffensiveFirePhase()`.

This preserves the Milestone 8 ownership split: fire resolution and destroyed
ID capture stay model-owned, while runtime view cleanup ordering remains a
wx-side responsibility owned by `FBattleScreen`.

Validation commands:

```bash
make -C src/tactical
cd tests/tactical && make && ./TacticalTests
cd tests/tactical && make && ./TacticalTests
make -C src/tactical
```

Result: `OK (63 tests)`

Milestone 8 Subtask 3 validation then confirmed that `FBattleBoard` renders
from model-backed tactical state, forwards hex clicks through
`FBattleScreen::handleHexClick()` to `FTacticalGame`, and no longer owns
duplicate mechanics members such as `hexData`, `turnData`, `m_hexData`, or
`m_turnInfo`.

Validation commands:

```bash
make -C src/tactical && make -C tests/tactical && ./tests/tactical/TacticalTests
make -C tests/tactical clean && make -C src/tactical && make -C tests/tactical && ./tests/tactical/TacticalTests
```

Result: `OK (54 tests)`

Milestone 8 Subtask 3 remediation validation confirmed the model-side
post-move seam itself: `FTacticalGame::setPhase(PH_FINALIZE_MOVE)` routes into
model-owned `completeMovePhase()` behavior, with targeted tests covering mine
checks, relocation/removal, off-board or planet destruction, mine damage,
selected-ship clearing, and transition to defense fire. The live wx
`FBattleDisplay::onMoveDone()` callback still remained a separate runtime seam
for later remediation at that point.

Validation commands:

```bash
cd tests/tactical && make && ./TacticalTests
cd tests/tactical && make clean && make && ./TacticalTests
```

Result: `OK (57 tests)`

Milestone 8 Subtask 4 validation then confirmed that the tactical
source-inspection suite is now anchored on the model-owned
`FTacticalGame` selection and hex-click delegation surface introduced by the
earlier Milestone 8 rewiring steps. The updated tactical tests now verify that:

- `FTacticalBattleDisplayFireFlowTest` no longer depends on any
  transitional `FBattleDisplay::fireAllWeapons()` helper and instead checks
  the click-flow forwarding APIs that remain authoritative on the wx side.
- `FTacticalBattleScreenDelegationTest` explicitly covers the selection and
  hex-interaction mutators (`selectShipFromHex()`, `assignTargetFromHex()`,
  and `placeMineAtHex()`) as redraw-gated forwarders into `FTacticalGame`.
- `FTacticalModelSelectionHexClickSurfaceTest` validates the canonical
  `FTacticalGame` header/implementation API for weapon selection, defense
  selection, ship selection, target assignment, mine placement, occupancy
  queries, in-bounds checks, and `handleHexClick()` routing.
- Tactical test registration now includes that new suite in both
  `tests/tactical/TacticalTests.cpp` and `tests/tactical/Makefile`, matching
  repository test-runner conventions.

This validation also confirms that the tactical test coverage now reflects the
Milestone 8 architecture where `FBattleBoard` no longer owns duplicate
mechanics state and hex-click behavior is documented and tested through the
`FBattleScreen` → `FTacticalGame` delegation seam.

Validation command:

```bash
cd tests/tactical && make clean && make && ./TacticalTests
```

Result: `OK (62 tests)`

Milestone 8 remediation Subtask 5 validation then confirmed that the live
wx move-done callback now follows the same canonical seam as the model-side
post-move finalization path. `FBattleDisplay::onMoveDone()` disconnects the
move-done button callback, calls `FBattleScreen::completeMovePhase()`, hides the
button, and restores `m_first=true` without falling back to the legacy direct
`setPhase(PH_FINALIZE_MOVE)` call. This means the actual wx runtime path now
reaches `FTacticalGame::completeMovePhase()` before post-move phase progression
continues.

The targeted tactical source-inspection coverage added for this remediation now
locks in three expectations for the live callback seam:

- `FBattleDisplay::onMoveDone()` delegates through
  `FBattleScreen::completeMovePhase()` rather than directly calling
  `setPhase(PH_FINALIZE_MOVE)`.
- The legacy direct finalize-phase call is absent from the move-done handler.
- The callback still performs the expected wx-side teardown flow around that
  seam delegation by disconnecting the move button handler, hiding the button,
  and resetting `m_first`.

Validation commands:

```bash
make -C src/tactical
cd tests/tactical && make && ./TacticalTests
```

Result: tactical build passed and `OK (67 tests)`.

Milestone 8 remediation Subtask 7 then tightened the fire-flow coverage around
the runtime destroyed-ship lifecycle instead of the earlier duplicate-cleanup
source expectations. `FTacticalBattleDisplayFireFlowTest` now builds a minimal
`FTacticalGame`/fleet harness, resolves real defensive-fire and offensive-fire
kill scenarios, and asserts the same lifecycle the wx runtime depends on:
destroyed ship IDs are captured once by model fire resolution, the destroyed
ship is removed from the model-owned side list, and the cached IDs can then be
cleared exactly once without being repopulated during fire-phase completion.
The same suite still keeps the wx callback ordering checks that the fire-done
buttons disable and hide before delegated resolution starts.

The tactical test `Makefile` was updated alongside that coverage so the module
runner now builds the tactical model/library prerequisites first and links
against the tactical, weapons, defenses, ships, strategic, and core static
libraries. That linkage keeps the tactical test target capable of executing the
runtime model harness directly instead of limiting this seam to source-only
inspection.

Validation commands:

```bash
cd tests/tactical && make && ./TacticalTests
make -C src/tactical && cd tests/tactical && make && ./TacticalTests
```

Result: `OK (68 tests)` on both runs.

Milestone 8 remediation Subtask 8 then expanded automated move-completion seam
coverage instead of relying only on helper-level or source-inspection
assertions. `FTacticalBattleScreenDelegationTest` covers the live wx callback
delegation boundary (`FBattleDisplay` move-done flow delegating through
`FBattleScreen::completeMovePhase()`), while `FTacticalMineDamageFlowTest`
locks canonical post-move outcomes by calling
`FTacticalGame::completeMovePhase()` directly.

The validated runtime outcomes recorded by that coverage are:

- the live move-done callback reaches the canonical completion seam;
- post-move phase progression advances to `PH_DEFENSE_FIRE`;
- selected-ship state is cleared after move completion;
- the mover's starting hex is cleared from tactical occupancy;
- an off-board mover is reduced to zero HP during post-move resolution; and
- defender occupancy remains stable while the mover is finalized.

This keeps the Milestone 8 remediation framing consistent with the additive
wx/model split: `FBattleDisplay` still acts as the live runtime event source,
`FBattleScreen` remains the wx-side coordinator, and `FTacticalGame` remains
the authoritative post-move resolution seam whose observable outcomes are now
locked by tactical runtime coverage.

Validation commands:

```bash
make -C src/tactical
cd tests/tactical && make && ./TacticalTests
```

Result: tactical build passed and `OK (69 tests)`.

The final Milestone 8 warning-fix follow-up then replaced the remaining
selected-ship cleanup source inspection with direct lifecycle coverage at the
wx seam itself. `FBattleScreen.cpp` now exposes a minimal
`runDestroyedShipCleanupLifecycle(...)` helper plus
`FDestroyedShipCleanupLifecycle`, and `FBattleScreen::clearDestroyedShips()`
builds that context from the live screen/model objects without changing the
existing ownership contract. The focused
`FTacticalDestroyedShipCleanupLifecycleTest` suite now verifies that:

- a destroyed selected ship clears the active screen selection;
- redraw still happens on the destroyed-list cleanup path;
- `clearLastDestroyedShipIDs()` bookkeeping is triggered exactly once per
  cleanup lifecycle; and
- winner handling remains sequenced after bookkeeping clear.

`FTacticalBattleScreenDelegationTest` was tightened at the same time to assert
that `clearDestroyedShips()` wires the extracted seam instead of bypassing it,
while the tactical test runner continues linking the GUI objects needed to
exercise that boundary.

Validation command:

```bash
cd tests/tactical && make clean && make && ./TacticalTests
```

Result: `OK (72 tests)`.

Milestone 9 Subtask 1 validation then confirmed that the BattleSim tactical
launch paths still honor the supported `FBattleScreen` modal ownership and
constructor contract introduced by the tactical separation work. The expanded
`FTacticalBattleScreenElectricalFireTest` source-inspection coverage now
asserts that:

- `BattleSimFrame`, `LocalGameDialog`, `ScenarioDialog`, and
  `ScenarioEditorGUI` continue to reach tactical combat through caller-owned
  stack `ShowModal()` flows instead of heap-owned or bypassing launch patterns.
- BattleSim launcher coverage still identifies the tactical entry files that
  hand off into `FBattleScreen`.
- `FBattleScreen`'s standard constructor path still creates `FTacticalGame`,
  creates `WXTacticalUI`, and installs that UI on the tactical model rather
  than depending on caller-managed tactical wiring.

This keeps Milestone 9 Subtask 1 scoped to automated regression coverage rather
than architecture changes: the live BattleSim runtime path remains the existing
modal `FBattleScreen` path, while the tests now lock in the constructor-owned
model/UI pairing that later milestone validation depends on.

Validation command:

```bash
cd tests/tactical && make && ./TacticalTests
```

Result: `OK (74 tests)`.

The station orbital-movement regression follow-up then documented the restored
model-owned runtime behavior for defender stations orbiting a planet.
`FTacticalGame::resetTurnInfoForCurrentMover()` now carries the orbital turn
heading into move finalization, and `resetMovementState()` immediately follows
that rebuild by routing through `checkMoveStatus()`. This lets pre-seeded
station orbital movement auto-complete the move phase as soon as the side enters
`PH_MOVE` when the station orbit is the only required movement. The refreshed
`FTacticalStationOrbitalMovementTest` suite verifies the runtime seam through
`resetMovementState()` and `completeMovePhase()`, while
`FTacticalGameMechanicsTest` keeps the mechanics-source assertion aligned with
the current `resetMovementState() -> checkMoveStatus()` path. That focused
regression coverage now confirms that:

- a station adjacent to a planet stays in orbit after move completion rather
  than drifting straight away;
- a moving side whose only required movement is the pre-seeded station orbit is
  already move-complete at movement-phase entry;
- the station heading reflects the orbital turn resolved during the move;
- station speed preservation remains intact across repeated orbit completion;
  and
- non-station movement semantics remain unchanged by the station-specific fix.

Validation command:

```bash
cd tests/tactical && make && ./TacticalTests
```

Result: `OK (84 tests)`.

The move-route selection regression follow-up then documented the restored
exact-click contract for model-owned tactical movement selection.
`FTacticalGame::handleMoveHexSelection()` now lands the route on the exact
highlighted hex the player clicks instead of stopping one hex early, and
clicking an earlier path node trims the route back to that exact hex without an
extra backward click. The new `FTacticalMoveRouteSelectionTest` runtime fixture
locks that behavior through `handleHexClick()` coverage for:

- the first highlighted movement hex extending the route by exactly one hex;
- later highlighted movement hexes becoming the actual route endpoint; and
- earlier path hexes trimming the current route back to the clicked hex while
  preserving `nMoved`, `curHeading`, and `finalHeading` invariants.

Subtask 4 then registered that move-route fixture together with the refreshed
station-orbit and final-orientation fixtures in `tests/tactical/TacticalTests.cpp`,
so the routine tactical module run now enforces all three runtime regressions.

No `FBattleBoard` renderer change was required for this fix; the existing move
overlay already matched the corrected model selection contract.

Validation commands:

```bash
cd tests/tactical && make && ./TacticalTests
```

Result: `OK (84 tests)`.

The forward-fire final-orientation regression follow-up then documented the
restored moving-ship fire-arc contract for model-owned range highlighting and
target validation. `FTacticalGame` now derives a shared per-path heading
sequence from `FTacticalTurnData`, so the endpoint of a move uses
`finalHeading` when the last movement step is a turn and both
`computeWeaponRange()` and `setIfValidTarget()` evaluate the same heading
progression. The new `FTacticalForwardFireFinalOrientationTest` runtime fixture
locks that behavior through tactical-model coverage for:

- endpoint forward-fire highlights using the post-turn orientation instead of
  the stale pre-turn heading;
- target acceptance and rejection staying aligned with the displayed
  final-orientation highlights for moving ships;
- straight-line movement preserving the pre-existing forward-fire behavior; and
- non-forward-firing battery weapons remaining heading-independent.

Validation commands:

```bash
cd tests/tactical && make && ./TacticalTests
cd tests/tactical && ./TacticalTests
```

Result: `OK (84 tests)`.

The strategic live-GUI follow-up then expanded the wx-backed regression surface
for top-level strategic UI. `tests/gui/StrategicGuiLiveTest` runs under the
shared `WXGuiTestHarness`, which can bootstrap against an existing `wxApp`,
tear down leftover top-level windows during shutdown, and now drive modal
dialogs deterministically through `showModalWithAction(...)` before a fallback
auto-dismiss closes the window. That live GUI suite now confirms that:

- `FMainFrame` constructs its `MapPanel` as an `FGamePanel` and starts with the
  expected disabled strategic menu items before a game is loaded;
- `FGamePanel` repaint behavior tracks its parent frame's client size in a live
  wx window;
- `SatharRetreatGUI`, `SystemDialogGUI`, `ViewFleetGUI`, and `SelectJumpGUI`
  can all be opened modally and closed deterministically by the harness; and
- parent-backed `WXStrategicUI` coverage now exercises retreat selection,
  system/fleet dialog entry points, and redraw-triggered paint handling in
  addition to the existing guarded no-parent paths;
- `UPFUnattachedGUI` and `SatharFleetsGUI` mutate fleet composition and system
  placement through live add/remove/update interactions, with assertions on the
  owning `FPlayer`, `FFleet`, and `FSystem` state after commit;
- `TransferShipsGUI` moves ships between fleets in a live dialog and commits the
  resulting fleet membership changes back to the model when Done is selected;
- `CombatFleetsGUI`, `CombatLocationGUI`, `TwoPlanetsGUI`, and
  `SelectResolutionGUI` now have live modal return-code coverage tied to their
  dialog-owned selection state instead of widget-existence-only checks; and
- `BattleResultsGUI` coverage now verifies both modal completion and ship-stat
  mutation through the dialog's update path.

The Subtask 2 remediation follow-up preserved that coverage but changed the
parent-backed informational message path inside `src/gui/WXStrategicUI.cpp`:
`WXStrategicUI::showMessage(...)` now uses `wxGenericMessageDialog` so the live
GUI harness can auto-dismiss those modal dialogs without hanging. That keeps
`StrategicGuiLiveTest::testWXStrategicUIParentBackedModalAndRedrawPaths`
covering `showMessage`, `notifyFailedJump`, `notifyVictory`, and
`showRetreatConditions` on the wx-owned GUI side while leaving the
strategic/model boundary unchanged.

No strategic/model boundary changed for this work: the added regression
coverage stays in `tests/gui/*` and exercises the wx-owned surface from the GUI
side rather than introducing new model dependencies.

The modal-launch follow-up extends that live coverage to the battle-board entry
chains. `StrategicGuiLiveTest` now drives `SelectCombatGUI::onAttack(...)`
through the real dialog chain into `FBattleScreen`, while `BattleSimGuiLiveTest`
covers the `BattleSimFrame` local-game launch path plus the `LocalGameDialog`,
`ScenarioDialog`, and `ScenarioEditorGUI` modal launch flows. The BattleSim
tests now wait for newly launched top-level or modal windows so they can prove
that `BattleSimFrame` really presented a `LocalGameDialog` parented by the
frame and that each `LocalGameDialog` button path really presented the concrete
downstream modal type (`ScenarioDialog` or `ScenarioEditorGUI`) owned by the
launching dialog. `FBattleScreen` continues to expose
constructor/destructor/live-instance counters that those GUI tests use to
assert real launch ownership and deterministic teardown rather than relying on
source-structure checks alone.

The next harness remediation tightened those live-GUI expectations around
top-level ownership and cleanup. `WXGuiTestHarness` now exposes
`getTopLevelWindows(...)`, `findTopLevelWindow(...)`, `waitForTopLevelWindow(...)`,
`findModalDialog(...)`, `waitForModalDialog(...)`, and
`cleanupOrphanTopLevels(...)` so fixtures can prove that launched frames or
dialogs actually appeared, wait for modal ownership before acting, and enforce a
zero-orphan top-level state before teardown completes. `GuiHarnessTest` now
regresses those observation and cleanup paths directly, while the tactical and
BattleSim live fixtures switched from shown stack-lifetime assumptions to
explicit `Destroy()` plus event pumping on their parent frames and dialogs.
The BattleSim scenario-launch paths also preserve `FBattleScreen` lifecycle
accounting, explicitly hide and destroy shown parents before pumping events, and
compare the final shown top-level count to the pre-launch baseline after
stabilization and forced-close cleanup passes, so the live suite now proves
those launch chains return to a zero-residual shown-window state.

The tactical follow-up extends that same live-dialog discipline inside
`TacticalGuiLiveTest`, which now registers direct
`testTacticalDamageSummaryDialogDisplaysContextAndCloseBehavior` and
`testICMSelectionDialogInteractionFinalizesAssignedCountsAndAmmo` coverage under
`GuiTests`. The damage-summary test drives `TacticalDamageSummaryGUI` modally,
asserts the dialog title plus populated and empty-state report text, verifies
the rendered report context, and dismisses the real `Close` button through the
modal harness. It also proves that the dialog renders a labeled `Ship Damage
Summary` section for the existing per-ship rollups and, when
`showHitDetails == true`, a second labeled `Hit Details` section containing the
player-readable per-hit lines such as `Destroyer Alpha [Laser Battery] ->
Sathar Frigate: 4 hull damage`. The same live fixture now toggles
`showHitDetails` to `false` and verifies that the ship rollup still renders
while the hit-detail section is omitted entirely. The ICM test now drives
`ICMSelectionGUI` modally through row selection, spin-control assignment, and
the production `Done` completion path, then asserts the assigned interceptor
count and defender ammo decrements without bypassing the dialog's finalization
logic. Those direct tactical dialog tests finish by destroying any shown
parents or dialogs, pumping events, and calling `cleanupOrphanTopLevels(...)`
so the fixture proves zero orphaned top-level windows before teardown
completes.

That same fixture now also includes
`testTacticalActionButtonsRemainSizerPositionedWhenShown`, which checks that
the live `FBattleScreen` exposes movement, defensive-fire, offensive-fire, and
mine-placement completion buttons with nonzero geometry to the right of the
zoom/prompt area after the tactical states are shown. Because the harness does
not deterministically reproduce `FBattleScreen` paint timing strongly enough to
prove the runtime `Show()/Layout()` call order by itself, the test documents
that limitation inline and relies on the tactical source-contract coverage
above to lock down the post-show/post-hide relayout contract.

Canonical headless GUI validation command:

```bash
cd tests/gui && make && xvfb-run -a ./GuiTests
```

Result: `OK (26 tests)`.

The tactical combat report hit-detail follow-up then documented the richer
player-facing summary shape used by `FTacticalCombatReportSummary`.
`include/tactical/FTacticalCombatReport.h` now adds
`FTacticalHitDetailSummary`, a `showHitDetails` toggle that defaults to `true`,
and a `hitDetails` collection alongside the existing per-ship rollups. The
summary builder only appends hit-detail entries for attacks where `hit == true`,
so reports with misses or report-level immediate events do not invent empty
detail rows. Each emitted hit detail keeps the attacker ship, weapon, target
ship, hull damage, and any effect labels/detail text in a player-readable
`outcome`/`displayLine` form, while suppressing only the redundant placeholder
note `Attack hit target` and preserving other meaningful note text such as
`rear arc` or `armor bypassed`. The existing ship rollup summaries remain the
canonical aggregate view for per-ship damage and effects.

That aggregate rollup contract now has a tighter player-facing shape as well.
Defense-damage entries render concrete defense names in the form `Defense Hit:
<name-list>` instead of falling back to a generic defense-damaged label, ship
lines with only non-hull effects omit the old `0 hull damage` clause entirely,
and hull-damage summaries no longer repeat `Hull Damage xN` inside the effects
section when the hull-loss total is already shown in the leading damage clause.
Weapon-damage rollups intentionally keep the prior comma-separated abbreviation
format such as `Weapon Hit: LB, LB, AR`, preserving duplicates and order.

The updated tactical regression coverage locks that contract in by checking:

- `FTacticalCombatReportSummary` starts with `showHitDetails == true` and can be
  toggled off by callers that want to suppress the detail section;
- hit-detail rows are emitted only for successful attacks and stay empty for
  no-hit summaries and immediate electrical-fire or mine-damage report shapes;
- player-readable detail text includes attacker, weapon, target, hull damage,
  any internal-effect outcome text, and meaningful non-placeholder notes while
  omitting the redundant `Attack hit target` suffix; and
- `FTacticalReportEvent` now preserves both weapon and defense damage metadata
  across immediate damage-resolution events and attack-effect construction,
  with source-contract and runtime tactical tests checking those fields directly;
- the ship-summary rollups now show named defense hits, retain the existing
  comma-separated weapon-hit abbreviation list, omit zero-hull clauses for
  defense-only summaries, and suppress duplicate hull-damage effect text across
  mixed-effect and hull-plus-effects cases.

The dialog follow-up then made that summary contract visible to players without
changing the report model. `TacticalDamageSummaryGUI` now splits its text
construction into a `Ship Damage Summary` section and a conditional `Hit
Details` section. `buildSummaryText()` always renders the ship rollup text
first, preserves the existing `No ships sustained damage in this report.`
empty-state when no ship summaries exist, and appends the labeled detail section
only when `showHitDetails` is enabled and `hitDetails` contains rows.

The close-path hardening follow-up then tightened how that modal dialog is
dismissed on wxGTK. The `Close` button remains the dialog's affirmative/default
action, but the manual button callback that also forced `EndModal(wxID_OK)` was
removed so the dialog closes through one modal-safe path instead of a possible
double-close pattern. That change specifically covers the previously fragile
no-detail and fully empty report shapes that can travel through the runtime
`WXTacticalUI::showDamageSummary(...)` path.

The regression coverage now locks that behavior in at two levels. The tactical
source-contract test checks for the dedicated ship-rollup and hit-detail
builders, preserves the direct content assertions, and rejects reintroduction of
the manual bind-plus-`EndModal(...)` close path. The live GUI regression drives
the parent-backed `WXTacticalUI::showDamageSummary(...)` flow with populated,
no-detail, and empty summaries and verifies clean modal return through the real
`Close` button handling.

Validation command:

```bash
cd tests/tactical && make && ./TacticalTests && cd ../gui && make && xvfb-run -a ./GuiTests
```

Result: `OK (88 tests)` tactical, `OK (25 tests)` GUI.
