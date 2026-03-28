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
— **Done**: `m_activePlayer`, `m_movingPlayer` (booleans mapped to IDs via
`getActivePlayerID()` / `getMovingPlayerID()`); path tracked in
`m_movementHexes` et al. in `FBattleBoard`.

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
   `m_rightHexes` vectors in `FBattleBoard`; `computePath()` and
   `computeMovedWeaponRange()` iterate over the path for range/fire checks).

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
    `hexData m_hexData[100][100]` inside `FBattleBoard`, where each cell
    holds a `FPoint pos` (hex centre) and a `VehicleList ships` (ships
    present). Both query directions (hex→ships and ship→hex) are supported
    via `FBattleBoard` methods.

11. Thinking on this more I believe we may want to use a `std::map`
    structure to hold the actual battle map. The key will be a Point
    object that holds the array coordinate of the hex. The data member
    held would be a hexData structure containing the list of ships and
    the coordinates of the hex centers. This would arbitrarily allow us
    to expand the map as needed during the course of a battle if ships
    needed to move off the edge of what is currently displayed. It might
    even be able to only store hexes that ships actually are in and
    add more as the game goes and they travel through new hexes. —
    **Not implemented**: the battle map is still a fixed `hexData m_hexData[100][100]`
    array. A `std::map<FPoint,int>` is used for gravity turns and `FHexMap`
    is used for mine targets, but the main board remains a fixed array.
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
  `m_movementHexes`, `m_leftHexes`, `m_rightHexes`.
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

