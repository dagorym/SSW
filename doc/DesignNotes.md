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
- **Fire-phase resolution:** defensive and offensive fire completion now call
  `resolveCurrentFirePhase()`, show the shared tactical damage summary dialog,
  clear destroyed ships, and then finish the phase through the dedicated model
  completion APIs instead of running local fire-resolution logic. The shipped
  remediation path preserves cached destroyed-ship IDs long enough for
  `FBattleScreen::clearDestroyedShips()` to remove those ships from the wx map,
  then derives the destroyed side as the active player's opponent so fallback
  winner detection still works when model cleanup was already performed.
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

This leaves the runtime tactical wx path only partially rewired in Milestone 8:
`FBattleDisplay` now behaves as a wx renderer/input translator for delegated
fire/setup flows and its live move-completion callback routes through
`FTacticalGame::completeMovePhase()`, while `FBattleBoard` has been narrowed to
renderer/hit-test responsibilities. A remaining legacy bypass through
`FBattleScreen::setPhase(PH_FINALIZE_MOVE)` is still documented in verifier
artifacts rather than treated as completed migration work.

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
(the active player's opponent) when model cleanup has already run.

Validation commands:

```bash
cd tests/tactical && make && ./TacticalTests
```

Result: `OK (54 tests)`

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

Milestone 8 Subtask 3 remediation validation then confirmed that the live
`FBattleDisplay::onMoveDone()` callback and `FTacticalGame::setPhase(PH_FINALIZE_MOVE)`
route through model-owned `completeMovePhase()` behavior, with targeted tests
covering mine checks, relocation/removal, off-board or planet destruction,
mine damage, selected-ship clearing, and transition to defense fire.

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

Milestone 8 Subtask 5 validation then confirmed that no additional tactical/UI
code changes were required after the Milestone 8 tactical board refactor work:
the repository root build still succeeds, the tactical module runner still
passes, and `BattleSim` still launches for GUI smoke validation. The automated
validation basis accepted for subtask completion is:

- `make -j2` passes at the repository root.
- `cd tests/tactical && make clean && make -j2 && ./TacticalTests` passes with
  `OK (62 tests)`.
- `cd src && timeout 5s ./BattleSim` is sufficient as a launch smoke in this
  CLI session.

The aggregate `tests/SSWTests` runner still reproduces the known baseline linker
failure for unresolved `FTacticalCombatReportTest` symbols, so that result does
not change the subtask outcome. Full interactive tactical combat coverage for
movement, defensive fire, offensive fire, offensive resolution, and battle
completion was not executed here because the non-interactive CLI cannot drive
the wxWidgets GUI. Per the accepted milestone orchestration decision, Subtask 5
is recorded as complete on the basis of the passing build and tactical
automated validation above, while a manual GUI tactical combat playthrough
remains a follow-up outside this CLI session rather than something completed by
this run.
