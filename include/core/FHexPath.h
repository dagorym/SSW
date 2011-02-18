/*
 * @file FHexPath.h
 * @brief Header file for the FHexPath class
 * @author Tom Stephens
 * @date Created: Oct 04, 2010
 */

#ifndef FHEXPATH_H_
#define FHEXPATH_H_

#include "core/FPObject.h"
#include "core/FPoint.h"

#include <vector>
#include <map>

namespace Frontier {

typedef std::map<FPoint,int> FlagMap;

/**
 * @brief Hex Path object
 *
 * This class is used to store a path through a hex map.  It
 * records each hex traversed as well as any special flags for
 * events in each hex.
 *
 * The flags are stored as bits in an integer and defined by the
 * calling classes.
 *
 * @author Tom Stephens
 * @date Created:  Oct 04, 2010
 * @date Last Modified:  Feb 18, 2011
 */
class FHexPath: public Frontier::FObject {
public:
	/// Default Constructor
	FHexPath();
	/// Default destructor
	virtual ~FHexPath();

	/**
	 * @ brief Add a point to the list
	 *
	 * This method takes a FPoint object and adds it as the next point
	 * on the path.  (It validates that the specified point is adjacent
	 * to the last point on the path.  If it is, it adds the point and
	 * returns the total number of points in the path.  If not,
	 * it returns a zero indicating that the point was not added.
	 *
	 * @param point The point to add to the list
	 *
	 * @author Tom Stephens
	 * @date Created:  Oct 04, 2010
	 * @date Last Modified:  Oct 04, 2010
	 */
	int addPoint(FPoint point);

	/**
	 * @ brief Add a flag to a point in the list
	 *
	 * This method takes a FPoint object and an integer representing
	 * the flag to add to the path.  The point object is used as a key
	 * in a set holding all the flags for the path.
	 *
	 * (Eventually) The method verifies that the point specified is on the path
	 * before adding the flag.  If not it returns an error.
	 *
	 * @param point The point to add to the list
	 * @param flag The flag to store with the point
	 *
	 * @author Tom Stephens
	 * @date Created:  Oct 04, 2010
	 * @date Last Modified:  Oct 04, 2010
	 */
	int addFlag(FPoint point, int flag);

	/**
	 * @brief Gets the flag for the specified point
	 *
	 * This method checks to see if the specified point is in the
	 * list.  If it is, it returns the associated flag.  If not,
	 * it returns a 0 to indicate that there is no flag specified
	 *
	 * @param point The point to add to the list
	 *
	 * @author Tom Stephens
	 * @date Created:  Oct 04, 2010
	 * @date Last Modified:  Oct 04, 2010
	 */
	int getFlag(FPoint point);

	/**
	 * @brief Check to see if point is on the path
	 *
	 * This method checks to see if the specified point is on
	 * the path stored by the object.  If it is, it returns true
	 * otherwise it returns false
	 *
	 * @param point The point to add to the list
	 *
	 * @author Tom Stephens
	 * @date Created:  Oct 04, 2010
	 * @date Last Modified:  Oct 04, 2010
	 */
	bool isPointOnPath(FPoint point);

	/// returns the path length
	unsigned int getPathLength() { return m_hexList.size(); }
	/// clears the object data
	void clear() { m_hexList.clear(); m_flags.clear(); }

	/**
	 * @brief returns the end point of the path
	 *
	 * This method returns an FPoint object corresponding to the
	 * last point on the current path
	 *
	 * @author Tom Stephens
	 * @date Created:  Feb 18, 2011
	 * @date Last Modified:  Feb 18, 2011
	 */
	FPoint endPoint();

	/**
	 * @brief returns the start point of the path
	 *
	 * This method returns an FPoint object corresponding to the
	 * first point on the current path
	 *
	 * @author Tom Stephens
	 * @date Created:  Feb 18, 2011
	 * @date Last Modified:  Feb 18, 2011
	 */
	FPoint startPoint();

	/**
	 * @brief Count number of occurrences of a given flag
	 *
	 * This method looks through the flag list and looks to see how many times
	 * the specified flag occurs.
	 *
	 * @author Tom Stephens
	 * @date Created:  Feb 18, 2011
	 * @date Last Modified:  Feb 18, 2011
	 */
	unsigned int countFlags(int flag);

	/**
	 * @brief removes all points in path beyond the one specified
	 *
	 * This method removes all the points in the path beyond
	 * the one specified in the input parameter.  It also removes any flags
	 * associated with those points.  The method returns the remaining size
	 * of the path once finished.
	 *
	 * @param point The point that should become the last point on the path
	 *
	 * @author Tom Stephens
	 * @date Created:  Feb 18, 2011
	 * @date Last Modified:  Feb 18, 2011
	 */
	unsigned int removeTrailingPoints(FPoint point);

	/**
	 * @brief Determines the heading based on the last two points in the path
	 *
	 * This method determines the heading based on the last two points in the
	 * path by calling the FHexMap::computeHeading function.
	 *
	 * @author Tom Stephens
	 * @date Created:  Feb 18, 2011
	 * @date Last Modified:  Feb 18, 2011
	 */
	unsigned int getLastHeading();

	/// returns a copy of the path list
	PointList getFullPath() const { return m_hexList; }

private:

	// List of hexes in the path
	PointList m_hexList;
	// Special flags for specified hex
	FlagMap m_flags;
};

}

#endif /* FHEXPATH_H_ */
