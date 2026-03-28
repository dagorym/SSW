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
   should be toggled but the moving player shouldn't be.

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
   moving player so that they may fire their weapons.

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

D. At the end of any firing phase, the target ships with ICMs should
   have the opportunity to declare how many ICMs they want to use in
   defense against incoming rocket weapons or mines. This will be
   implemented later when the defenses are added in version 0.0.10.

E. At the end of the moving player's fire phase, the movement data
   shall be cleared, the moving player marker will switch to the other
   player and the active player will switch to the new moving player.
   At which point the process will repeat but with the roles switched.

## Keeping State

1. We need a Point class/structure that holds x, y data. This can be used
   to store the physical positions (i.e. centers) of the hexes for
   drawing on the screen or pairs of coordinates that correspond to the
   hex numbers on the map. This should be contained by the hexData
   structure and all the places where the hex data structure is being
   used in where the vector of ship positions is not needed can be
   replaced by this structure. — **Done**

2. Instead of keeping track of the side as a boolean, maybe we should
   have two variables, `m_activePlayer` and `m_movingPlayer`, that store
   the ID of the player in question. All data structures will be
   referenced by the ID values stored here. This allows for expansion
   of the game from just two sides to multiple sides if
   necessary/desired. — *partial, still using pair of booleans*

3. All of the calls to `getSide()` should be replaced by `getActivePlayer()`.
   — **Done**

4. Lookups should use the ID to refer to and find the relevant data
   structures.

5. Likewise, when we need to access information about the moving
   player, the ID can be obtained by querying `getMovingPlayerID()`. The
   returned ID can then be used to access the relevant data
   structures. — **Done**

6. Data on ships needs to be indexable by the ship's ID value (i.e.
   the value returned by `getID()`).

7. For each moving ship, a list of the hexes moved through should be kept to
   facilitate determining where that ship can fire and whether or not
   it can be fired upon. This should probably not be part of the ship
   class but rather a separate structure or class.

8. We need a mechanism to keep track of whether or not a ship has fired
   its weapons defensively in a round or not. (This really isn't
   needed until we have more than two sides.)

9. However, we do need a mechanism to keep track of which weapons are
   firing at which ships. Should this be part of the weapon class?
   Probably, each weapon should keep track of the ship it is firing at
   as well. This may result in a bit of a problem. `FVessel` objects
   contain weapon objects; if the weapons in turn use the `FVessel`
   class, this may cause a circular dependency in the header files
   preventing compilation. Maybe not — if we use a forward declaration
   in the `FWeapon` header file and only have the `FVessel` header in the
   implementation file we should be okay. — **Done, no circular dependencies.**

10. We need a structure to keep track of which ships are where. Should
    ships know their position? Probably not, since this is related
    to the battle field implementation more than the ship itself. This
    should probably be a class as there are two ways to query this data:
    one is "tell me what ships are in a given hex" and the other is
    "tell me what hex a given ship is in". I can see both access methods being
    useful. I'm not sure what the best way to implement this is. On one
    hand the structure we currently have, namely an array of hexData
    structures with a vector of pointers to the ships in the hex, is
    useful for finding what ships are in the hex.

11. Thinking on this more I believe we may want to use a `std::map`
    structure to hold the actual battle map. The key will be a Point
    object that holds the array coordinate of the hex. The data member
    held would be a hexData structure containing the list of ships and
    the coordinates of the hex centers. This would arbitrarily allow us
    to expand the map as needed during the course of a battle if ships
    needed to move off the edge of what is currently displayed. It might
    even be able to only store hexes that ships actually are in and
    add more as the game goes and they travel through new hexes.
    Definitely something to investigate.

## Adding Defenses

1. Ships will need a `m_defenses` vector of `FDefense` pointers.
2. The first defense in the vector should be the default (i.e. None or
   Reflective Hull).
3. Add display of list of defenses.
4. Defense names should be green for the active defense, white for
   inactive defenses and red for damaged defenses.
5. The defense list should be active on your moving turn only for
   changing the active defense.
6. To change the defense, simply click on the one you want (clicking on
   an active defense will turn it off).
7. ICMs shouldn't be active but when fired upon give an option of
   how many to use.
8. When clicking a Masking Screen defense, confirm. This defense may
   be held off for now as it may be a bit tricky to do right.
9. Ships all get a `m_currentDefense` pointer and a `getCurrentDefense()`
   method that points to and returns the currently selected defense.
   This should be added to the `FVehicle` base class.
10. When firing a weapon, the weapon should query the target for the
    to-hit modifier of its current defense for the attacking weapon type
    and apply it to the to-hit roll.
11. When firing at a ship with ICMs and a functioning launcher, the
    defending player should be prompted to select the number of ICMs
    to use in defense for that attack.
12. It'd be nice to be able to show the player all the attacks against
    that ship so they can ration as needed. How do we implement this?
13. Masking screens are deployed at the start/end/middle? of round.
14. A Masking Screen needs to be cleared (i.e. turned off) if a turn is
    made during that round's movement phase. How do we do this? And how
    do we deal with orbits?
15. What visual cues do we use for the screens? The masking screen has a
    marker that we can use but the proton, electron and stasis screens
    don't. Maybe a blue circle for the proton, green for the electron
    and yellow for the stasis screen would work — put the letter (P,
    E, or S) in the middle. Add this as an overlay on the image of the
    ship.

## Advanced Damage Table

1. Initially just add this to the `FVehicle` class `takeDamage()` method.
   We can adapt it later.
2. Maybe what we should do is make a `takeBasicDamage()` method that
   keeps the basic behavior.
3. The basis for the table can be lifted directly from the KHVector
   Obiter code as a lot of this was implemented there, at least the
   form of it.
