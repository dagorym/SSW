/**
 * @file FFleet.h
 * @brief Header file for FFleet class
 * @author Tom Stephens
 * @date Created:  Jan 14, 2005
 *
 */

#ifndef _FFLEET_H_
#define _FFLEET_H_

#include "core/FPObject.h"
#include "StrategicTypes.h"
#include "ships/FVehicle.h"
#include <string>
#include <vector>

namespace Frontier
{

class FSystem;

/**
 * @brief Class to hold a fleet of ships
 *
 * This class holds all the information pertaining to a fleet of ships,
 * including its location, composition, owner, etc.  FFleet is non-copyable
 * because it exclusively owns the FVehicle objects in m_ships; copying would
 * alias that ownership.  Use the NO_DESTINATION and NO_ROUTE sentinel
 * constants (both equal to `static_cast<unsigned int>(-1)`) to represent an
 * unset destination or jump-route ID respectively.  FFleet does not own or
 * cache any wxImage state; icon identity is stored as a filename string and
 * retrieved via getIconName().
 *
 * @author Tom Stephens, gpt-5.3-codex (medium), Claude Sonnet 5 (medium)
 * @date Created:  Jan 14, 2005
 * @date Last Modified:  Jul 17, 2026
 */
class FFleet : public Frontier::FPObject {
public:
  static constexpr unsigned int NO_DESTINATION = static_cast<unsigned int>(-1);
  static constexpr unsigned int NO_ROUTE = static_cast<unsigned int>(-1);

  /// Default constructor
	FFleet();
  /// Default destructor
	virtual ~FFleet();

  /**
   * @brief Set the location of the fleet
   *
   * This method takes as input a pointer to the FSystem the fleet is
   * currently at and optionally a transit flag, remaining transit time in
   * days, destination system ID, jump speed, and jump route ID.  The system's
   * ID is extracted and stored in m_location; the system's map coordinates
   * are copied into m_pos.  When not specified, dest defaults to
   * NO_DESTINATION and route defaults to NO_ROUTE, indicating that no
   * destination or active jump route is set.
   *
   * If there is a problem the method will return a non-zero error code,
   * otherwise it returns zero.
   *
   * @param loc     Pointer to the FSystem the fleet is currently at
   * @param transit Flag indicating whether the fleet is in transit on a jump route
   * @param time    Remaining transit time in days (default 0)
   * @param dest    ID of the destination system; use NO_DESTINATION when none (default)
   * @param speed   Jump speed for the transit (default 0)
   * @param route   ID of the jump route the fleet is on; use NO_ROUTE when none (default)
   *
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created:  Jan 14, 2004
   * @date Last Modified:  Mar 24, 2026
   */
  int setLocation( FSystem * loc, bool transit, int time = 0, unsigned int dest = NO_DESTINATION , int speed = 0, unsigned int route = NO_ROUTE);

  /**
   * @brief Add a ship to the fleet
   *
   * This method takes a point to a FVehicle object and adds that object
   * to the m_ships member variable which contains list of the ships assigned
   * to the fleet.
   *
   * If there is a problem adding the ship to the fleet the method will
   * return a non zero error code.  Otherwise it will return a 0.
   *
   * @param ship Pointer a FVehicle object that holds the ship information
   *
   * @author Tom Stephens
   * @date Created:  Jan 14, 2004
   * @date Last Modified:  Jan 14, 2004
   */
  int addShip( FVehicle * ship );

  /**
   * @brief Remove a ship from the fleet
   *
   * This method removes a ship from the fleet.  It takes as input the
   * ID of the ship to remove.  If all goes well, the method
   * returns a pointer to the ship that was removed.  If there is a problem
   * it returns a NULL pointer.
   *
   * @param id The ID code of the ship to remove.
   *
   * @author Tom Stephens
   * @date Created:  Jan 14, 2004
   * @date Last Modified:  Jan 14, 2004
   */
  FVehicle * removeShip( unsigned int id );

  /**
   * @brief Get a pointer to a ship in the fleet
   *
   * This method returns a pointer to the ith ship indicated by the id
   * input parameter.  If there is a problem
   * it returns a NULL pointer.
   *
   * @param id The ID code of the ship to return a pointer to.
   *
   * @author Tom Stephens
   * @date Created:  Jan 14, 2004
   * @date Last Modified:  Jan 14, 2004
   */
  FVehicle * getShip( unsigned int id );

  /**
   * @brief Decrement the transit time
   *
   * This method reduces the distance left in the jump by the appropriate
   * value based on the fleet's holding status and jump speed.  It takes
   * the shortened step at high jump speeds into account when transitioning
   * from one system to another.  When the transit completes, m_inTransit is
   * cleared, m_jumpRouteID is reset to NO_ROUTE, m_destination is reset to
   * NO_DESTINATION, and m_location is updated to the destination system.
   * It returns the amount of time left in the jump as its return value.
   *
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created:  Jan 14, 2004
   * @date Last Modified:  Mar 24, 2026
   */
  int decTransitTime();

  /**
   * @brief Method to return the ID number of the fleet
   *
   * @author Tom Stephens
   * @date Created:  Jan 14, 2004
   * @date Last Modified:  Jan 14, 2004
   */
  const unsigned int & getID() const { return m_ID; }

  /**
   * @brief Method to return the location of the fleet
   *
   * @author Tom Stephens
   * @date Created:  Jan 14, 2004
   * @date Last Modified:  Jan 14, 2004
   */
  const unsigned int & getLocation() const { return m_location; }

  /**
   * @brief Method to return the transit status of the fleet
   *
   * @author Tom Stephens
   * @date Created:  Jan 14, 2004
   * @date Last Modified:  Jan 14, 2004
   */
  const bool & getInTransit() const { return m_inTransit; }

  /**
   * @brief Method to return the id of the fleets owner
   *
   * @author Tom Stephens
   * @date Created:  Jan 14, 2004
   * @date Last Modified:  Jan 14, 2004
   */
  const unsigned int & getOwner() const  { return m_owner; }

  /**
   * @brief Method to return the name of the fleet
   *
   * @author Tom Stephens
   * @date Created:  Jan 14, 2004
   * @date Last Modified:  Jan 14, 2004
   */
  const std::string & getName() const { return m_name; }

  /**
   * @brief Set whether or not the fleet is on a jump
   *
   * This method sets the status of the m_inTranist variable to indicate
   * whether the fleet is in transit between systems.  If the value is
   * false, the fleet is in a system.  If true, the fleet is on a jump
   * route.
   *
   * @param flag Flag indicating the status of the fleet.
   *
   * @author Tom Stephens
   * @date Created:  Jan 14, 2004
   * @date Last Modified:  Jan 14, 2004
   */
  void setInTransit( bool flag ) { m_inTransit = flag; }

  /**
   * @brief Sets the ID of the owner
   *
   * This method take as input the ID value of the FPlayer object
   * corresponding to the owner of the fleet and sets the m_owner
   * member variable accordingly.
   *
   * @param owner The ID of the owning player
   *
   * @author Tom Stephens
   * @date Created:  Jan 14, 2004
   * @date Last Modified:  Jan 14, 2004
   */
  void setOwner( unsigned int owner ) { m_owner = owner; }

  /**
   * @brief Set the name of the fleet
   *
   * This method takes as input a string containing a name for the fleet
   * and stores it in the m_name variable
   *
   * @param name The name to give the fleet
   *
   * @author Tom Stephens
   * @date Created:  Jan 14, 2004
   * @date Last Modified:  Jan 14, 2004
   */
  void setName( std::string name ) { m_name = name; }

  /**
   * @brief Method to save the fleet data
   *
   * This method implements the FPObject base class virtual write method to
   * save all the fleet's data. The fleet ID, owner ID, location/destination/
   * jump-route system IDs, and the ship-list count are written via the
   * fixed-width little-endian writeU32() helper; each ship is delegated to
   * its own save().
   *
   * @param os The output stream to write to
   *
   * @author Tom Stephens, Claude Sonnet 5 (medium)
   * @date Created:  Mar 06, 2008
   * @date Last Modified:  Jul 17, 2026
   */
  const virtual int save(std::ostream &os) const;

	/**
	 * @brief Method to read data contents
	 *
	 * This method is the inverse of the save method.  It reads the data for
	 * the class from the designated input stream and reconstructs the fleet's
	 * ships via createShip(). The fleet ID, owner ID, location/destination/
	 * jump-route system IDs, and the ship-list count are read via the
	 * fixed-width little-endian readU32() helper. After restoring the fleet
	 * ID, the static m_nextID counter is advanced past it (H3) so a
	 * freshly-constructed fleet never reuses an ID restored from a save file.
	 * H4: the legacy normalization that rewrote a loaded m_jumpRouteID of 0 to
	 * the NO_ROUTE sentinel has been removed -- jump route ID 0 is a valid,
	 * distinct route ID and is now preserved exactly as saved; only the
	 * distinguished NO_ROUTE sentinel value itself means "not on a route".
	 * Each ship-list entry's type tag is resolved via createShip(); an
	 * unknown/NULL type aborts the load by returning nonzero without
	 * dereferencing the NULL result. This method returns 0 if everything is
	 * okay and a positive integer error code if there is a failure.
	 *
	 * @param is The input stream to read from
	 *
	 * @author Tom Stephens, gpt-5.3-codex (medium), Claude Sonnet 5 (medium)
	 * @date Created:  Mar 07, 2008
	 * @date Last Modified:  Jul 17, 2026
	 */
	virtual int load(std::istream &is);

	/**
	 * @brief Method to cancel a jump order
	 *
	 * This method cancels a jump order by setting the fleet's destination to
	 * its current location and recomputing the remaining transit time as
	 * (m_jumpLength - m_transitTime) so the fleet travels back to the origin.
	 * If the resulting transit time is zero, m_inTransit is cleared and the
	 * fleet is considered to have arrived.  If m_location already equals
	 * m_destination the method does nothing.
	 *
	 * @author Tom Stephens, gpt-5.3-codex (medium)
	 * @date Created:  Mar 14, 2008
	 * @date Last Modified:  Mar 24, 2026
	 */
	void cancelJump();

	/**
	 * @brief Detemine fleet's max jump speed.
	 *
	 * This method loops through the fleet's ships and returns the
	 * fastest risk jump speed based on the slowest ships ADF.
	 *
	 * @author Tom Stephens
	 * @date Created:  Apr 14, 2008
	 * @date Last Modified:  Apr 14, 2008
	 */
	int getMaxSpeed();

  /// sets the fleet's speed
  void setSpeed(unsigned int s );
  /// get the fleet's speed
  const unsigned int & getSpeed() const { return m_speed; }
  /// set the jump route the fleet is on
  void setJumpRoute(int i, const FSystem * s, const FSystem * e, unsigned int length);
  /// get the ID of the active jump route; returns NO_ROUTE when the fleet is not on a jump route
  unsigned int getJumpRoute() const { return m_jumpRouteID; }
  /// get the time left in the transit
  const int getTransitTime() const { return m_transitTime; }
  /// set the fleet's location
  int setLocation(unsigned int loc) { m_location = loc; return 0;}
  /// set the fleet's destination
  void setDestination(unsigned int d) { m_destination = d; }
  /// get the fleet's destination
  const unsigned int & getDestination() const { return m_destination; }

  /// get the ship count
  unsigned int getShipCount() { return m_ships.size(); }

//  /// get ship directly from the vector
//  FVehicle * getShip (int i) { return m_ships[i]; }

  /// get reference to the list of ships
  const VehicleList & getShipList() const { return m_ships; }

  /// Set the fleet icon
  void setIcon(std::string icon);
  /// get the fleet icon name
  const std::string & getIconName() const { return m_iconFile; }

  /// set the milita flag
  void setMilitia(bool flag, std::string home) { m_isMilitia = flag; m_home = home;}
  /// check to see if the fleet is a milita fleet
  bool isMilitia() { return m_isMilitia; }
  /// get the fleet's home system
  const std::string & getHomeSystem() const { return m_home; }
  /// get the flag for whether or not the ship is holding in its jump
  bool isHolding() { return m_isHolding; }
  /// set the flag for whether or not the fleet is holding in its jump
  void setHolding(bool f) { m_isHolding = f; }

  float getCoord(int i) { return m_pos[i];}

  /**
   * @brief Detemine the fleet's jump chance
   *
   * This method detemines the fleet's chance to successfully make the
   * desired jump.  It is 100% for speed 1 jumps and decreases as a function
   * of ship type and speed.  See the SSW game description for values
   *
   * @author Tom Stephens
   * @date Created:  Apr 14, 2008
   * @date Last Modified:  Apr 14, 2008
   */
  int getRJChance();

private:
  // Fleets exclusively own ships in m_ships, so copying would alias ownership.
  FFleet(const FFleet &);
  FFleet & operator=(const FFleet &);

  /// ID value of the fleet
  unsigned int m_ID;
  /// ID of the system, planet or jump the fleet is currently at
  unsigned int m_location;
  /// ID of the system fleet is headed to
  unsigned int m_destination;
  /// Flag indicating whether or not the fleet is in transit
  bool m_inTransit;
  /// ID of the fleets owner
  unsigned int m_owner;
  /// Vector of pointers to all the objects in the fleet
  VehicleList m_ships;
  /// Number of days left in the jump
  unsigned int m_transitTime;
  /// Length of jump in light years
  unsigned int m_jumpLength;
  /// Name of the fleet
  std::string m_name;
  /// current jump speed (1=standard, 2=RJ2, 3=RJ3)
  unsigned int m_speed;
  /// pointer to the jump the fleet is on
  unsigned int m_jumpRouteID;
  /// counter for next fleet id
  static int m_nextID;
  /// name of icon file
  std::string m_iconFile;
  /// flag for whether the fleet is a militia fleet or not
  bool m_isMilitia;
  /// Name of the home system of the fleet (only used for milita)
  std::string m_home;
  /// flag for whether the fleet is holding in its jump or not.
  bool m_isHolding;
  /// counter for number of instances of this class
  static unsigned int m_classCount;
  /// Position of fleet (x,y) on map in real (unscaled) coordinates
  float m_pos[2];
  /// x change for one ly of the jump
  float m_dx;
  /// y change for one ly of the jump
  float m_dy;

};
};

#endif //_FFLEET_H_
