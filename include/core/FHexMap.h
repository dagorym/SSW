/*
 * @file FHexPath.h
 * @brief Header file for the FHexMap class
 * @author Tom Stephens
 * @date Created: Oct 04, 2010
 */

#ifndef FHEXMAP_H_
#define FHEXMAP_H_

#include "core/FPObject.h"
#include "core/FPoint.h"
#include "ships/FVehicle.h"

#include <map>

namespace Frontier {

/**
 * @brief HexMap object
 *
 * This class holds the details of a hex map along with functions
 * related to computing distances and line of sight on the map
 *
 * @author Tom Stephens
 * @date Created:  Oct 04, 2010
 * @date Last Modified:  Mar 18, 2011
 */
class FHexMap: public Frontier::FPObject {
public:
	/// Default Constructor
	FHexMap();
	/// Default Destructor
	virtual ~FHexMap();

	/**
	 * @brief Computes the distance between two hexes on the map
	 *
	 * This method determines the number of hexes in the shortest path
	 * between two specified hexes on the map
	 *
	 * @param sx The horizontal hex coordinate of the starting hex
	 * @param sy The vertical hex coordinate of the starting hex
	 * @param ex The horizontal hex coordinate of the ending hex
	 * @param ey The vertical hex coordinate of the ending hex
	 *
	 * @author Tom Stephens
	 * @date Created:  Sep 14, 2008
	 * @date Last Modified:  Sep 14, 2008
	 */
	static int computeHexDistance(int sx, int sy, int ex, int ey);

	/**
	 * @brief Computes the distance between two hexes on the map
	 *
	 * This method determines the number of hexes in the shortest path
	 * between two specified hexes on the map.  This method just
	 * provides an alternate interface using the FPoint objects to the
	 * computeHexDistance(int sx, int sy, int ex, int ey) method.
	 *
	 * @param s The starting point
	 * @param e The ending point
	 *
	 * @author Tom Stephens
	 * @date Created:  Sep 14, 2008
	 * @date Last Modified:  Sep 14, 2008
	 */
	static int computeHexDistance(FPoint s, FPoint e);

	/**
	 * @brief Finds the next hex in the specified direction
	 *
	 * Given the current hex array coordinates (passed in a FPoint object)
	 * and a heading, this method computes the next hex along the path.
	 *
	 * @param h FPoint object containing array coordinates of hex ship is in
	 * @param heading Hexside the ship is facing
	 *
	 * @author Tom Stephens
	 * @date Created:  Nov 21, 2008
	 * @date Last Modified:  Mar 30, 2009
	 */
	static FPoint findNextHex(FPoint h, int heading);

	/**
	 * @brief Returns heading from one hex to another
	 *
	 * This method takes as input a source and destination hex (grid positions) and computes
	 * the closest heading (hex facing) from the source to the destination.  The computed
	 * heading is returned to the user
	 *
	 * @param s source hex
	 * @param d destination hex
	 *
	 * @author Tom Stephens
	 * @date Created:  Oct 26, 2008
	 * @date Last Modified:  Oct 26, 2008
	 */
	static int computeHeading(FPoint s, FPoint d);

	/**
	 * @brief Computes angle between two hexes
	 *
	 * This returns the angle in degrees between the two passed hexes
	 *
	 * @param s the source hex
	 * @param d the destination hex
	 *
	 * @author Tom Stephens
	 * @date Created:  Nov 1, 2008
	 * @date Last Modified:  Nov 1, 2008
	 */
	static double computeHexAngle(FPoint s, FPoint d);

	/**
	 * @brief  Add a ship to the map
	 *
	 * This method adds the pointer to the ship passed as the second parameter
	 * to the list of ships in the position passed in as the first parameter.
	 *
	 * This method is only to be used when adding completely new ships to
	 * the map.  If you are moving a ship from one point to another use the
	 * moveShip() method.
	 *
	 * @param p the hex to place the ship in
	 * @param v pointer to the ship being added
	 *
	 * @author Tom Stephens
	 * @date Created:  Mar 18, 2011
	 * @date Last Modified:  Mar 18, 2011
	 */
	void addShip(FPoint p, FVehicle * v);

	const virtual int save(std::ostream &os) const {return 0;}
	virtual int load(std::istream &is) { return 0; }

private:
	std::map<FPoint,VehicleList> m_hexList;

};

}

#endif /* FHEXMAP_H_ */
