/**
 * @file FMap.h
 * @brief Header file for FMap class
 * @author Tom Stephens
 * @date Created:  Jan 15, 2005
 *
 */

#ifndef _FMAP_H_
#define _FMAP_H_

#include "core/FPObject.h"
#include "FSystem.h"
#include "FJumpRoute.h"
//#include "FPlayer.h"
#include <wx/wx.h>
#include <string>
#include <vector>

namespace Frontier
{

/**
 * @brief Class to hold game map information
 *
 * This class holds the information about a the game map and provides
 * methods to locate and get information on the systems and jump routes.
 *
 * The FMap class is responsible for destroying all of the FSystem and
 * FJumpRoute objects when it is destroyed.
 *
 * May 30, 2008 - Converted this to a singleton to make access easier
 * where it is needed throughout the program.
 *
 * @author Tom Stephens
 * @date Created:  Jan 17, 2005
 * @date Last Modified:  May 30, 2008
 */
class FMap : public Frontier::FPObject {
private:
	/// Static instance of the FMap class
	static FMap * m_map;
	/// Private copy construtor to prevent copies
	FMap(const FMap&);
	/// Private assignment operator to prevent copies
	FMap& operator=(FMap&);


protected:
	/// blank constructor
	FMap(){}
	/**
	 * @brief Default constructor
	 *
	 * This initalized the new map.  If the random variable set to true,
	 * nothing currently happens other than creating an empty map of
	 * size 0.  However, if set to false the Frontier systems are
	 * set up as described in the Alpha Dawn rules.
	 *
	 * @param random Flag for whether to use the true Frontier or a random map
	 * @param pList vector containing all the player ID's
	 *
	 * @author Tom Stephens
	 * @date Created:  Jan 28, 2008
	 * @date Last Modified:  Feb 19, 2008
	 */
	FMap(bool random, std::vector<unsigned int> pList);

public:
	/// Default destructor
	virtual ~FMap();

	/**
	 * @brief Method to actually create the Map
	 *
	 * This method creates the map object the first time it is called
	 * Subsequent calls do nothing but return a reference to the FMap
	 * object.
	 *
	 * @param random Flag for whether to use the true Frontier or a random map
	 * @param pList vector containing all the player ID's
	 *
	 * @author Tom Stephens
	 * @date Created:  May 30, 2008
	 * @date Last Modified:  May 30, 2008
	 */
	static FMap & create(bool random, std::vector<unsigned int> pList );

	/// alternate creation method to get a blank map
	static FMap & create();

	/**
	 * @brief Get a reference to the map object
	 *
	 * This method just returns a reference to the map object if it
	 * exists if it doesn't exist, it returns a null reference.
	 *
	 * @author Tom Stephens
	 * @date Created:  May 30, 2008
	 * @date Last Modified:  May 30, 2008
	 */
	static FMap & getMap();

	/**
	 * @brief Get a pointer to a star system
	 *
	 * This method takes a string containing the name of a star system and
	 * returns a pointer to that system.  If no system is at the given
	 * coordinates or an error occurs, a NULL pointer is returned.
	 *
	 * @param name The name of the star system to find
	 *
	 * @author Tom Stephens
	 * @date Created:  Jan 17, 2005
	 * @date Last Modified:  Feb 19, 2008
	 */
	FSystem * getSystem(std::string name) const;

	/**
	 * @brief Get a pointer to a star system
	 *
	 * This method takes the ID value of a star system and
	 * returns a pointer to that system.  If no system is at the given
	 * coordinates or an error occurs, a NULL pointer is returned.
	 *
	 * @param id The ID value of the system to find.
	 *
	 * @author Tom Stephens
	 * @date Created:  Jan 17, 2005
	 * @date Last Modified:  Feb 19, 2008
	 */
	FSystem * getSystem(unsigned int id) const;

	/**
	 * @brief Draw the map
	 *
	 * This method draws the map on the display
	 *
	 * @param dc the Device Context to draw to
	 *
	 * @author Tom Stephens
	 * @date Created:  Jan 17, 2005
	 * @date Last Modified:  Mar 10, 2008
	 */
	void draw(wxDC &dc/*, unsigned int id*/);

	/**
	 * @brief  Select a system close to a given point
	 *
	 * This method takes as input a pair of x and y screen coordinates.
	 * It then searches through the list of systems to see if any are close
	 * to the specified coordinates.  If so, it returns a pointer to that
	 * system.  It returns NULL if nothing is close.
	 *
	 * @param x The x coordinate to check
	 * @param y The y coordinate to check
	 * @param dc The device context of the window.
	 *
	 * @author Tom Stephens
	 * @date Created:  Feb 11, 2008
	 * @date Last Modified:  Feb 11, 2008
	 */
	FSystem * selectSystem(wxCoord x, wxCoord y,wxDC &dc) const;

	/**
	 * @brief returns the scale for the current window dispaly
	 *
	 * This method determines the current scale based on the map size
	 * and the size of the current window
	 *
	 * @param dc The device context of the window.
	 *
	 * @author Tom Stephens
	 * @date Created:  Feb 11, 2008
	 * @date Last Modified:  Feb 11, 2008
	 */
	const double getScale(wxDC &dc) const;

	/**
	 * @brief returns a list of systems you can jump to
	 *
	 * This method takes as input the name of the system in question and the ID of
	 * the player whose turn it is.  It returns a list of all the systems connected
	 * to the specified systems by jump routes known by the specified player.
	 *
	 * @param system The name of the system being checked
	 * @param player The ID of the player
	 * @param fleet The fleet you are checking jump routes for (militia have limited range)
	 *
	 * @author Tom Stephens
	 * @date Created:  Feb 15, 2008
	 * @date Last Modified:  Feb 15, 2008
	 */
	std::vector<std::string> getConnectedSystems(std::string system, unsigned int player, FFleet * fleet);

	/**
	 * @brief Returns a pointer the jumproute with the specified endpoints
	 *
	 * This method takes as input the the start and end systems and returns a pointer
	 * the the jump route between those two systems.  It returns a NULL pointer if
	 * there are no matches.
	 *
	 * @param start The name of the starting system
	 * @param end The name of the ending system
	 *
	 * @author Tom Stephens
	 * @date Created:  Feb 15, 2008
	 * @date Last Modified:  Feb 15, 2008
	 */
	FJumpRoute * getJumpRoute(std::string start, std::string end);

	/**
	 * @brief Returns a pointer the jumproute with the specified ID
	 *
	 * This method takes as input the the jump route ID and returns a pointer
	 * the the jump route between those two systems.  It returns a NULL pointer if
	 * there are no matches.
	 *
	 * @param id The ID value of the jump route
	 *
	 * @author Tom Stephens
	 * @date Created:  Feb 19, 2008
	 * @date Last Modified:  Feb 19, 2008
	 */
	FJumpRoute * getJumpRoute(unsigned int id);

	/// returns a reference to the list of all the systems
	const SystemList & getSystemList() const { return m_systems; }

	/**
	 * @brief Method to save the map data
	 *
	 * This method implements the FPObject base class virtual write method to
	 * save all the map's data
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

private:
	/// List of all systems in the game
	SystemList m_systems;
	/// Maximum size of game map
	int m_maxCoord;
	/// All explored jump routes
	std::vector<FJumpRoute *> m_jumps;

	/**
	 * @brief Initalized the true Frontier map
	 *
	 * Sets all of the systems for the real frontier map
	 *
	 * @param pList vector containing all the player ID's
	 *
	 * @author Tom Stephens
	 * @date Created:  Jan 28, 2008
	 * @date Last Modified:  Feb 19, 2008
	 */
	void setUpFrontier(std::vector<unsigned int> pList);

};

};

#endif //_FMAP_H_
