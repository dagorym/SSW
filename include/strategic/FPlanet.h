/**
 * @file FPlanet.h
 * @brief Header file for FPlanet class
 * @author Tom Stephens
 * @date Created:  Jan 15, 2005
 *
 */

#ifndef _FPLANET_H_
#define _FPLANET_H_

#include "FAstroBody.h"
#include "ships/FVehicle.h"

namespace Frontier
{

/**
 * @brief Class to hold details of a planetary body
 *
 * This class holds all the information pertaining to a single planetary
 * body, whether planet, moon or asteroid, including the population, government,
 * surface type and production
 *
 * @author Tom Stephens
 * @date Created:  Jan 15, 2005
 * @date Last Modified:  Feb 03, 2008
 */
class FPlanet : public Frontier::FAstroBody
{
public:
  /// Default constructor
	FPlanet();
  /// Default destructor
	virtual ~FPlanet();
	/**
	 * @brief Alternate constructor
	 *
	 * This version of the constructor allows you to specify many of the planet's
	 * characteristics immediately upon creation instead of doing it randomly
	 *
	 * @param name The system name
	 *
	 * @author Tom Stephens
	 * @date Created:  Feb 03, 2008
	 * @date Last Modified:  Feb 03, 2008
	 */
	FPlanet(std::string name);

//  /// set planet name
//  void setName(std::string name) { m_name = name; }

  /// get planet name
  const std::string & getName() const { return m_name; }

  /**
   * @brief Method to save the planet data
   *
   * This method implements the FPObject base class virtual write method to
   * save all the planet's data
   *
   * @param os The output stream to write to
   *
   * @author Tom Stephens
   * @date Created:  Mar 05, 2008
   * @date Last Modified:  Mar 05, 2008
   */
  const virtual int save(std::ostream &os) const;

	/**
	 * @brief Method to read data contents
	 *
	 * This method is the inverse of the save method.  It reads the data for
	 * the class from the designated input stream.  This method returns 0 if
	 * everything is okay and a positive integer error code if there is a
	 * failure. The return of every own read (@c m_ID, @c m_name, the
	 * station count, and, when a station is present, its serialized type
	 * tag) is checked and a nonzero code is returned on the first failure.
	 * When a station is present, the station type is looked up via
	 * @c createShip(type) and the result is NULL-checked before use: an
	 * unknown/corrupt station type aborts the load with a nonzero return
	 * instead of dereferencing a NULL pointer. The nested
	 * @c m_station->load(is) return is likewise checked, and a failure
	 * there also aborts with a nonzero return. This makes the abort
	 * propagate through @c FSystem::load()'s existing
	 * <tt>if (p->load(is) != 0)</tt> check (which deletes the not-yet-owned
	 * @c FPlanet, freeing any partially-loaded @c m_station via
	 * ~FPlanet()) and on through @c FMap::load() to @c FGame::load()'s
	 * aggregate-abort/no-live-singleton path.
	 *
	 * @author Tom Stephens, Claude Sonnet 5 (medium)
	 * @date Created:  Mar 07, 2008
	 * @date Last Modified:  Jul 19, 2026
	 */
	virtual int load(std::istream &is);

	/// Adds a station to the system
	void addStation(FVehicle *s);
	/// get a pointer to the station
	FVehicle * getStation() const { return m_station; }
	/// clear station pointer
	void destroyStation() { delete m_station; m_station = NULL; }

private:
  /// Planet Name
  std::string m_name;
  /// running number of planets (used for id)
  static int m_nextID;
  /// global planet id in game
  int m_ID;
  /// counter for number of instances of this class
  static unsigned int m_classCount;
  /// pointer to the planet's space station
  FVehicle * m_station;
};

/// Define the PlanetList type to be a vector of FPlanet pointers
typedef std::vector<FPlanet *> PlanetList;

};

#endif //_FPLANET_H_
