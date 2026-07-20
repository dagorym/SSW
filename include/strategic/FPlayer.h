/**
 * @file FPlayer.h
 * @brief Header file for FPlayer class
 * @author Tom Stephens
 * @date Created:  Jan 17, 2005
 *
 */

#ifndef _FPLAYER_H_
#define _FPLAYER_H_

#include "core/FPObject.h"
#include "StrategicTypes.h"
#include "FFleet.h"
#include "FMap.h"
#include <string>
#include <vector>

namespace Frontier
{

/**
 * @brief Class to hold details of a player
 *
 * This class holds all the information pertaining to a single player
 * in the game.
 *
 * The FPlayer class is responsible for deleting all of the fleets that
 * the player posses when the class is destroyed.  Fleet icon image data
 * is no longer stored here; only the icon file name is retained and
 * resolved at render time. FPlayer is also the sole owner of ships in its
 * destroyed-ship list (see addDestroyedShip()) and frees them on
 * destruction.
 *
 * @author Tom Stephens, gpt-5.3-codex (medium), Claude Sonnet 5 (medium)
 * @date Created:  Jan 17, 2005
 * @date Last Modified:  Jul 17, 2026
 */
class FPlayer : public Frontier::FPObject
{
public:
  /// Default Constructor
	FPlayer();

  /**
   * @brief Destructor -- frees every fleet, unattached ship, and destroyed ship owned by this player
   *
   * Deletes every fleet in m_fleets and every ship in m_unattached, then
   * deletes every ship in m_destroyed and clears all three lists. FPlayer
   * is the sole owner of a ship once it has been handed to
   * addDestroyedShip() (see that method's ownership contract); such a ship
   * has already been removed -- not deleted -- from whatever fleet or
   * unattached list previously held it, so it is not reachable through
   * m_fleets or m_unattached by the time this destructor runs and freeing
   * it here cannot double-delete a ship a surviving fleet still owns.
   *
   * @author Tom Stephens, Claude Sonnet 5 (medium)
   * @date Created:  Jan 17, 2005
   * @date Last Modified:  Jul 11, 2026
   */
	virtual ~FPlayer();

  /**
   * @brief Get the player's name
   *
   * This method returns the player's name.
   *
   * @author Tom Stephens
   * @date Created:  Jan 17, 2005
   * @date Last Modified:  Jan 17, 2005
   */
  const std::string & getName() const { return m_name; }

  /**
   * @brief Set the player's name
   *
   * This method take as input a string containing the player's name.  The
   * m_name variable is updated to hold this new name.
   *
   * @param name The player's new name.
   *
   * @author Tom Stephens
   * @date Created:  Jan 17, 2005
   * @date Last Modified:  Jan 17, 2005
   */
  void setName(std::string name) { m_name = name; }

  /**
   * @brief Get the player's ID value
   *
   * This method returns the ID value for the player
   *
   * @author Tom Stephens
   * @date Created:  Jan 17, 2005
   * @date Last Modified:  Jan 17, 2005
   */
  const unsigned int & getID() const { return m_ID; }

  /**
   * @brief Add a fleet to the player's list
   *
   * This method takes a pointer to a FFleet object and adds it to the m_fleets
   * variable.
   *
   * @param fleet The pointer to the new fleet
   *
   * @author Tom Stephens
   * @date Created:  Feb 07, 2008
   * @date Last Modified:  Feb 07, 2008
   */
  void addFleet(FFleet * fleet) { m_fleets.push_back(fleet); }

  /**
   * @brief Get a pointer to a player's fleet at a map position
   *
   * This method takes as input the position of a fleet in the internal map
   * coordinate scale and returns a pointer to that fleet.  The search uses
   * a Euclidean distance threshold of 0.5 map units to locate the fleet.
   * If the player does not own a fleet at that position or if an error
   * occured, the method returns a NULL pointer.
   *
   * @param x  The x coordinate of the fleet
   * @param y The y coordinate of the fleet
   *
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created:  Mar 21, 2008
   * @date Last Modified:  Mar 28, 2026
   */
  FFleet * getFleet (double x, double y) const;

  /**
   * @brief Set the fleet icon file name
   *
   * This method stores the file name for the fleet icon in m_iconName.
   * No image loading is performed here; GUI render paths are responsible
   * for resolving and loading the image at draw time via WXIconCache or
   * equivalent helpers.
   *
   * @param file The file name of the icon to use
   *
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created:  Feb 10, 2008
   * @date Last Modified:  Mar 27, 2026
   */
  void setFleetIcon(std::string file);

  /**
   * @brief Get the fleet icon file name
   *
   * Returns the file name of the icon used to represent this player's
   * fleets on the map.  The returned string is relative to the game
   * installation root and must be resolved through
   * FGameConfig::resolveAssetPath() before use.
   *
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created:  Mar 27, 2026
   * @date Last Modified:  Mar 27, 2026
   */
  const std::string & getFleetIconName() const { return m_iconName; }

  /// gets a reference to the player's fleet list
  FleetList & getFleetList() { return m_fleets; }

  /**
    * @brief Add a ship to the unattached list
    *
    * This method takes a point to a FVehicle object and adds that object
    * to the m_unattached member variable which contains list of the ships
    * not currently assigned to a fleet.
    *
    * If there is a problem adding the ship to the fleet the method will
    * return a non zero error code.  Otherwise it will return a 0.
    *
    * @param ship Pointer a FVehicle object that holds the ship information
    *
    * @author Tom Stephens
    * @date Created:  Feb 21, 2008
    * @date Last Modified:  Feb 21, 2008
    */
   int addShip( FVehicle * ship );

   /**
     * @brief Add a ship to the destroyed ship list
     *
     * This method takes a point to a FVehicle object and adds that object
     * to the m_destroyed member variable which contains the list of the ships
     * that have been destroyed in battle.  This list will be used for
     * reinfocements if that optional rule is used
     *
     * Ownership contract: once a ship pointer is passed to this method,
     * FPlayer becomes its sole owner and will delete it in ~FPlayer(). The
     * caller must have already removed the ship from any fleet or
     * unattached list that previously held it (without deleting it -- see
     * FFleet::removeShip()) before calling this method, so the ship is
     * referenced only by m_destroyed afterward. Do not retain or free the
     * pointer elsewhere after calling this method. m_destroyed is serialized
     * by save()/load() using the same type-tag + createShip() + FVehicle::load()
     * pattern as m_unattached and fleet ships (F2-serialization); the list is
     * persisted and restored but not yet consumed by any rule.
     *
     * If there is a problem adding the ship to the fleet the method will
     * return a non zero error code.  Otherwise it will return a 0.
     *
     * @param ship Pointer a FVehicle object that holds the ship information.
     *   Ownership transfers to FPlayer; the caller must not delete it.
     *
     * @author Tom Stephens, Claude Sonnet 5 (medium)
     * @date Created:  May 30, 2008
     * @date Last Modified:  Jul 17, 2026
     */
    int addDestroyedShip( FVehicle * ship );

//  /**
//   * @brief Remove a ship from the unassigned list
//   *
//   * This method removes a ship from the unassigned list.  It takes as input the
//   * ID of the ship to remove.  If all goes well, the method
//   * returns a pointer to the ship that was removed.  If there is a problem
//   * it returns a NULL pointer.
//   *
//   * @param id The ID code of the ship to remove.
//   *
//   * @author Tom Stephens
//   * @date Created:  Feb 21, 2008
//   * @date Last Modified:  Feb 21, 2008
//   */
//  FVehicle * removeShip( unsigned int id );

//  /**
//   * @brief Get a pointer to a ship in the unattached list
//   *
//   * This method returns a pointer to the ship indicated by the id
//   * input parameter. all goes well, the method
//   * returns a pointer to the ship specified.  If there is a problem
//   * it returns a NULL pointer.
//   *
//   * @param id The ID code of the ship to return a pointer to.
//   *
//   * @author Tom Stephens
//   * @date Created:  Feb 21, 2008
//   * @date Last Modified:  Feb 21, 2008
//   */
//  FVehicle * getShip( unsigned int id );

  /**
   * @brief Get the list of ships
   *
   * @author Tom Stephens
   * @date Last Modified:
   */
  VehicleList & getShipList() { return m_unattached; }

  /**
   * @brief Get a pointer to a player's fleet
   *
   * This method takes as input the name of a fleet and returns a pointer
   * to that fleet.  If the fleet is not owned by the player or if an error
   * occured, the method returns a NULL pointer.
   *
   * @param name  The name of the fleet to return
   *
   * @author Tom Stephens
   * @date Created:  Jan 17, 2005
   * @date Last Modified:  Jan 17, 2005
   */
  FFleet * getFleet (std::string name) const;

  /**
   * @brief Method to save the player data
   *
   * This method implements the FPObject base class virtual write method to
   * save all the player's data. Counts (unattached/fleet/destroyed-ship list
   * sizes) and the player ID are written via the fixed-width little-endian
   * writeU32() helper; each ship/fleet is delegated to its own save().
   *
   * @param os The output stream to write to
   *
   * @author Tom Stephens, Claude Sonnet 5 (medium)
   * @date Created:  Mar 05, 2008
   * @date Last Modified:  Jul 17, 2026
   */
  const virtual int save(std::ostream &os) const;

	/**
	 * @brief Method to read data contents
	 *
	 * This method is the inverse of the save method.  It reads the data for
	 * the class from the designated input stream, restoring the player ID,
	 * name, fleet icon file name, unattached ship list, fleet list, and
	 * destroyed-ship list (F2-serialization). No image loading is performed
	 * during deserialization; icon images are resolved lazily at render time.
	 * Counts and the player ID are read via the fixed-width little-endian
	 * readU32() helper. After restoring the player ID, the static m_nextID
	 * counter is advanced past it (H3) so a freshly-constructed FPlayer never
	 * reuses an ID restored from a save file. Each ship-list entry's type tag
	 * is resolved via createShip(); an unknown/NULL type aborts the load by
	 * returning nonzero without dereferencing the NULL result. (FR-1 /
	 * SF-nested-load-returns) The nested FVehicle::load()/FFleet::load()
	 * return value for every entry in the unattached-ship, fleet, and
	 * destroyed-ship loops is also checked: a nonzero nested return means the
	 * stream was truncated or corrupt partway through that sub-object's own
	 * record, so the just-allocated sub-object (not yet reachable from
	 * m_unattached/m_fleets/m_destroyed) is deleted and this method returns
	 * nonzero immediately, so the aggregate-abort guarantee in the caller
	 * (FGame::load()) cannot be bypassed by deep-truncation input.
	 * (FF2-3 / FR-D) The container-level scalar reads owned directly by this
	 * method -- the player ID, name, icon name, and the unattached/fleet/
	 * destroyed-ship count reads -- are likewise return-checked: a nonzero
	 * return from any of them (stream truncated/failed strictly inside the
	 * player's own scalar region, before any sub-object is allocated) aborts
	 * the load immediately, completing the aggregate-abort guarantee at this
	 * container's own scalar depth (mirroring the FF-2 FVehicle::load()
	 * pattern).
	 * This method returns 0 if everything is okay and a positive integer
	 * error code if there is a failure.
	 *
	 * @param is The input stream to read from
	 *
	 * @author Tom Stephens, gpt-5.3-codex (medium), Claude Sonnet 5 (medium)
	 * @date Created:  Mar 07, 2008
	 * @date Last Modified:  Jul 19, 2026
	 */
	virtual int load(std::istream &is);

	/**
	 * @brief Returns the highest ID of any fleet the player owns
	 *
	 * This method scans through the players fleets and returns the value
	 * the the largest fleet ID
	 *
	 * @author Tom Stephens
	 * @date Created:  Mar 25, 2008
	 * @date Last Modified:  Mar 25, 2008
	 */
	unsigned int getLargestFleetID();

	/**
	 * @brief Remove a fleet from the player's list
	 *
	 * This method takes as input the ID value of a fleet.  If the fleet is
	 * owned by the player it is removed from the player's list of fleets and a pointer
	 * to the fleet is returned.  If the fleet is not in the player's list a NULL
	 * pointer is returned.
	 *
	 * @param id The ID number of the fleet to find
	 *
	 * @author Tom Stephens
	 * @date Created:  Apr 07, 2008
	 * @date Last Modified:  Apr 07, 2008
	 */
	FFleet * removeFleet(unsigned int id);

private:
  /// The player's name
  std::string m_name;
  /// Player ID number
  unsigned int m_ID;
//  /// The player's credits
//  long m_credits;
  /// Vector containing pointers to all of a players fleets
  FleetList m_fleets;
  /// Incremented player ID counter
  static unsigned int m_nextID;
  /// Name of icon image file
  std::string m_iconName;
  /// list of unattached ships
  VehicleList m_unattached;
  /// List of destroyed ships. FPlayer owns every ship in this list once it
  /// has been passed to addDestroyedShip() and frees them all in
  /// ~FPlayer(); see addDestroyedShip() for the full ownership contract.
  /// Serialized by save()/load() (F2-serialization) using the same
  /// type-tag + createShip() pattern as m_unattached and fleet ships.
  /// Deliberately has no public getter (unlike m_unattached/m_fleets, see
  /// getShipList()/getFleetList()) since FPlayer is meant to remain its sole
  /// owner and accessor; consequently its save/load round trip cannot be
  /// asserted directly against a fetched list and is instead verified by
  /// re-save byte-equality against the original stream (see
  /// FPlayerTest::testFullRoundTripPreservesUnattachedFleetsAndDestroyedShips).
  VehicleList m_destroyed;
  /// counter for number of instances of this class
  static unsigned int m_classCount;
};

};

#endif //_FPLAYER_H_
