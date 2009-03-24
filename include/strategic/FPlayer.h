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
 * the player posses when the class is destroyed.
 *
 * @author Tom Stephens
 * @date Created:  Jan 17, 2005
 * @date Last Modified:  Mar 06, 2008
 */
class FPlayer : public Frontier::FPObject
{
public:
  /// Default Constructor
	FPlayer();
  /// Default destructor
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
   * @brief Get a pointer to a player's fleet
   *
   * This method takes as input the position of a fleet (in screen coordinates)
   * and returns a pointer to that fleet.  If the player does not own a fleet
   * at that position or if an error occured, the method returns a NULL pointer.
   *
   * @param x  The x coordinate of the fleet
   * @param y The y coordinate of the fleet
   *
   * @author Tom Stephens
   * @date Created:  Mar 21, 2008
   * @date Last Modified:  Mar 21, 200
   */
  FFleet * getFleet (int x, int y, FMap * map, wxDC &dc) const;

  /**
   * @brief Draws the player's fleets on the map
   *
   * This method takes in the DC from the game object and draws the player's fleets
   *
   * @param dc Device Context to draw to
   * @param map Pointer to the FMap object with the system and jump references
   *
   * @author Tom Stephens
   * @date Created:  Feb 07, 2008
   * @date Last Modified:  Mar 14, 2008
   */
  void drawFleets(wxDC &dc, FMap *map);

  /**
   * @brief Set the image for the fleet icon
   *
   * This message loads the specified filed into the wxImage object and stores it to
   * be used whenever drawing the fleet icons on the map.
   *
   * @param file The file name of the icon to load
   *
   * @author Tom Stephens
   * @date Created:  Feb 10, 2008
   * @date Last Modified:  Feb 10, 2008
   */
  void setFleetIcon(std::string file);

//  // return a pointer to the fleet icon
//  wxImage * getFleetIcon() { return &m_fleetIcon; }

  // return a pointer to the fleet icon
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
     * If there is a problem adding the ship to the fleet the method will
     * return a non zero error code.  Otherwise it will return a 0.
     *
     * @param ship Pointer a FVehicle object that holds the ship information
     *
     * @author Tom Stephens
     * @date Created:  May 30, 2008
     * @date Last Modified:  May 30, 2008
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

  /// get the list of ships
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
   * save all the player's data
   *
   * @param os The output stream to write to
   *
   * @author Tom Stephens
   * @date Created:  Mar 05, 2008
   * @aate Last Modified:  Mar 05, 2008
   */
  const virtual int save(std::ostream &os) const;

	/**
	 * @brief Method to read data contents
	 *
	 * This method is the inverse of the save method.  It reads the data for
	 * the class from the designated input stream.  This method returns 0 if
	 * everything is okay and a positive integer error code if there is a
	 * failure
	 *
	 * @author Tom Stephens
	 * @date Created:  Mar 07, 2008
	 * @date Last Modified:  Mar 07, 2008
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
  /// icon for fleets
  wxImage m_fleetIcon;
  /// Name of icon image file
  std::string m_iconName;
  /// list of unattached ships
  VehicleList m_unattached;
  /// list of destroyed ships
  VehicleList m_destroyed;
  /// counter for number of instances of this class
  static unsigned int m_classCount;
};

// PlayerList typedef
typedef std::vector<FPlayer *> PlayerList;

};

#endif //_FPLAYER_H_
