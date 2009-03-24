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
	 * failure
	 *
	 * @author Tom Stephens
	 * @date Created:  Mar 07, 2008
	 * @date Last Modified:  Mar 07, 2008
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
