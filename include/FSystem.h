/**
 * @file FSystem.h
 * @brief Header file for FSystem class
 * @author Tom Stephens
 * @date Created:  Jan 17, 2005
 * 
 */

#ifndef _FSYSTEM_H_
#define _FSYSTEM_H_

#include "FPObject.h"
#include "FPlanet.h"
#include "FFleet.h"
#include <vector>
#include <string>

namespace Frontier
{

/**
 * @brief Class to hold stellar system information
 * 
 * This class holds the information about a single stellar system
 * 
 * @author Tom Stephens
 * @date Created:  Jan 17, 2005
 * @date Last Modified:  Feb 03, 2008
 */
class FSystem : public Frontier::FPObject
{
public:
  /// Default Constructor
	FSystem();
  /// Default Destructor
	virtual ~FSystem();
	/**
	 * @brief Alternate constructor
	 * 
	 * This version of the constructor allows you to specify many of the system's
	 * characteristics immediately upon creation instead of doing it randomly
	 * 
	 * @param name The system name
	 * @param x The x coordinate value
	 * @param y The y coordinate value
	 * @param x The z coordinate value
	 * @param owner The ID of the owning player
	 * 
	 * @author Tom Stephens
	 * @date Created:  Feb 03, 2008
	 * @date Last Modified:  Feb 03, 2008
	 */
	FSystem(std::string name,float x, float y, float z, unsigned int owner);
	
  /**
   * @brief Get the name of the system
   * 
   * This method returns a string containing the name of the system.
   * 
   * @author Tom Stephens
   * @date Created:  Jan 17, 2005
   * @date Last Modified:  Jan 17, 2005
   */ 
  const std::string & getName() const { return m_name; }

  /**
   * @brief Get the owning player's ID
   * 
   * This method returns the ID value of the player that owns the system.
   * 
   * @author Tom Stephens
   * @date Created:  Jan 17, 2005
   * @date Last Modified:  Jan 17, 2005
   */ 
  const unsigned int & getOwner() const { return m_owner; }

//  /**
//   * @brief Set the owner of the system
//   * 
//   * This method takes as input the ID of the player that controls the system
//   * and stores it in the m_owner member variable.
//   * 
//   * @param owner The ID of the player who owns the system
//   * 
//   * @author Tom Stephens
//   * @date Created:  Jan 17, 2005
//   * @date Last Modified:  Jan 17, 2005
//   */ 
//  void setOwner(unsigned int owner) { m_owner = owner; }

//  /**
//   * @brief Get a fleet in the system
//   * 
//   * This method takes as input the ID value of a fleet in the system and
//   * returns a pointer to that fleet.  If there is an error or the fleet is
//   * not in the system, a NULL pointer is returned.
//   * 
//   * @param id The ID value of the fleet to find
//   * 
//   * @author Tom Stephens
//   * @date Created:  Jan 17, 2005
//   * @date Last Modified:  Jan 17, 2005
//   */ 
//  const FFleet * getFleet(unsigned int id) const;

  /**
   * @brief Add a fleet to the system
   * 
   * This method takes as input a pointer to a FFleet object and adds that
   * fleet to the list of fleets in the system.  The method returns the number
   * of fleets listed in the systems fleet list (not counting those garrisoning
   * planets).  If there is an error the method returns a 0.
   * 
   * @param fleet The fleet to add to the system.
   * 
   * @author Tom Stephens
   * @date Created:  Jan 17, 2005
   * @date Last Modified:  Jan 17, 2005
   */ 
  unsigned int addFleet(FFleet * fleet) { m_fleets.push_back(fleet); return 0;}

  /**
   * @brief Remove a fleet from the system
   * 
   * This method takes as input the ID value of a fleet.  If the fleet is
   * stationed in the system or garrisoning one of the planets in the system
   * the fleet is removed from that objects list of fleets and a pointer
   * to the fleet is returned.  If the fleet is not in the system a NULL
   * pointer is returned.
   * 
   * @param id The ID number of the fleet to find
   * 
   * @author Tom Stephens
   * @date Created:  Jan 17, 2005
   * @date Last Modified:  Feb 19, 2008
   */ 
  FFleet * removeFleet(unsigned int id);

//  /**
//   * @brief Get one of the system's planets
//   * 
//   * This method takes as input the ID value of a planet in the system and
//   * returns a pointer to the specified object.  If there is an error or
//   * the planet is not in the system a NULL pointer is returned.
//   * 
//   * @param id The ID of the planet to find
//   * 
//   * @author Tom Stephens
//   * @date Created:  Jan 17, 2005
//   * @date Last Modified:  Jan 17, 2005
//   */ 
//  FPlanet * getPlanet(unsigned int id) const;

  /**
   * @brief Get one of the system's planets
   * 
   * This method takes as input the name of a planet in the system and
   * returns a pointer to the specified object.  If there is an error or
   * the planet is not in the system a NULL pointer is returned.
   * 
   * @param name The name of the planet
   * 
   * @author Tom Stephens
   * @date Created:  Jan 17, 2005
   * @date Last Modified:  Jan 17, 2005
   */ 
  FPlanet * getPlanet(std::string name) const;

  /**
   * @brief Get the system's ID number
   * 
   * This method returns the system's ID value
   * 
   * @author Tom Stephens
   * @date Created:  Jan 17, 2005
   * @date Last Modified:  Jan 17, 2005
   */ 
  const unsigned int & getID() const { return m_ID; }

  /**
   * @brief Get the system's cooridate
   * 
   * This method takes a coordinate index (0=x, 1=y, 2=z) as input and
   * returns the value of that coordinate.
   * 
   * @author Tom Stephens
   * @date Created:  Jan 17, 2005
   * @date Last Modified:  Jan 17, 2005
   */ 
  const float & getCoord (unsigned int i) const { return m_coords[i]; }

  /**
   * @brief Set system location coordinates
   * 
   * This method takes as input the value of the three coordiantes (x,y,z) and
   * stores them in the m_coords array.
   * 
   * @param x The x coordinate value
   * @param y The y coordinate value
   * @param x The z coordinate value
   * 
   * @author Tom Stephens
   * @date Created:  Jan 17, 2005
   * @date Last Modified:  Feb 03, 2008
   */ 
  void setCoords(float x, float y, float z) { m_coords[0]=x; m_coords[1]=y; m_coords[2]=z; }

//  /**
//   * @brief Draw the system
//   * 
//   * This method draws the system on the display
//   * 
//   * @author Tom Stephens
//   * @date Created:  Jan 17, 2005
//   * @date Last Modified:  Jan 17, 2005
//   */ 
//  void draw();
  
  /**
   * @brief add a planet to the system
   * 
   * This method adds a pregenerated planet to the system
   * 
   * @param p Planet to add
   * 
   * @author Tom Stephens
   * @date Created:  Feb 03, 2008
   * @date Last Modified:  Feb 03, 2008
   */
  void addPlanet(FPlanet* p) { m_planets.push_back(p); }
  
  /// returns reference to the planet list
  const PlanetList & getPlanetList() const { return m_planets; }
  
  /// returns reference to the fleet list
  const FleetList & getFleetList() const { return m_fleets; }
  
  /**
   * @brief Method to save the system data
   * 
   * This method implements the FPObject base class virtual write method to
   * save all the system's data
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
	   
private:
//  /// Stars in the system
//  std::vector<FStar *> m_stars;
  /// Planets in the system
	PlanetList m_planets;
  /// Fleets in the system
  FleetList m_fleets;
  /// ID value of owning player
  unsigned int m_owner;
  /// Cartesian system coordinates
  float m_coords[3];
  /// System name
  std::string m_name;
  /// System ID number
  unsigned int m_ID;
  /// counter for system id's
  static int m_nextID;
  /// counter for number of instances of this class
  static unsigned int m_classCount;
};

/// define the SystemList type to be a vector of FSystem pointers
typedef std::vector<FSystem *> SystemList;
};

#endif //_FSYSTEM_H_
