/*
 * @file FPoint.h
 * @brief Header file for the FPoint class
 * @author Tom Stephens
 * @date Created: Mar 30, 2009
 */

#ifndef FPOINT_H_
#define FPOINT_H_

#include <vector>
#include <set>
#include <iostream>

namespace Frontier {

/**
 * @brief Basic point object
 *
 * This class is used to store coordinates.  It has an x and y
 * position and stores them as integers.
 *
 * @author Tom Stephens
 * @date Created:  Mar 30, 2009
 * @date Last Modified:  Mar 30, 2009
 */
class FPoint {
public:
	/**
	 * @brief Default constructor
	 *
	 * The default constructor just initialized the point to the origin (0,0)
	 *
	 * @author Tom Stephens
	 * @date Created:  Mar 30, 2009
	 * @date Last Modified:  Mar 30, 2009
	 */
	FPoint() { m_x=-9999; m_y=-9999; }

	/**
	 * @brief Initializing constructor
	 *
	 * The initializing constructor  initialized the point to the x and y
	 * values provided.
	 *
	 * @author Tom Stephens
	 * @date Created:  Mar 30, 2009
	 * @date Last Modified:  Mar 30, 2009
	 */
	FPoint(int x, int y):m_x(x),m_y(y) {}

	/// Default destructor
	virtual ~FPoint();

	/// get the x value
	const int getX() const { return m_x; }
	/// get the y value
	const int getY() const { return m_y; }
	/// set the x value
	void setX(int x){ m_x = x; }
	/// set the y value
	void setY(int y){ m_y = y; }
	/// set both points simultaneously
	void setPoint(int x, int y) { m_x = x; m_y = y; }

	/**
	 * @brief compute distance between this point and the specified point
	 *
	 * This method takes in another point object and computes the distance
	 * between the provided point and the current one.  It returns the
	 * compute distance as a float.
	 *
	 * @param p The point to compute the distance to
	 *
	 * @author Tom Stephens
	 * @date Created:  Mar 30, 2009
	 * @date Last Modified:  Mar 30, 2009
	 */
	float getDistance(const FPoint &p);

	/**
	 * @brief Overloaded comparison operator
	 *
	 * This method overloads the comparison (==) operator.  It returns
	 * true if the coordinates of the two points are the same and false
	 * otherwise
	 *
	 * @param p The point to compare to
	 *
	 * @author Tom Stephens
	 * @date Created:  Mar 30, 2009
	 * @date Last Modified:  Mar 30, 2009
	 */
	bool operator== (const FPoint & p);

	/**
	 * @brief Overloaded not-equal-to operator
	 *
	 * This method overloads the not-equal-to (!=) operator.  It returns
	 * false if the coordinates of the two points are the same and true
	 * otherwise
	 *
	 * @param p The point to compare to
	 *
	 * @author Tom Stephens
	 * @date Created:  Mar 30, 2009
	 * @date Last Modified:  Mar 30, 2009
	 */
	bool operator!= (const FPoint & p) { return !((*this)==p); }

	/**
	 * @brief Overloaded less than operator
	 *
	 * This overloads the less than operator for the point class.  it first check by
	 * y value and then by x value
	 *
	 * @param p The point to compare to
	 *
	 * @author Tom Stephens
	 * @date Created:  Apr 16, 2009
	 * @date Last Modified:  Apr 16, 2009
	 */
	bool operator< (const FPoint &p) const;

	/**
	 * @brief Overloaded << operator for printing the data contents of the object.
	 *
	 * This function overloads the iostream inserter operator (<<) for the
	 * FPoint class.  It prints out the point tuple in the format (x, y) to
	 * the output stream.
	 *
	 * @author Tom Stephens
	 * @date Created:  May 13, 2009
	 * @date Last Modified:  May 13, 2009
	 */
	friend std::ostream& operator<< (std::ostream &os, const FPoint &p);

private:
	/// x coordinate
	int m_x;
	/// y coordinate
	int m_y;
};

/// define type for a list of points
typedef std::vector<FPoint> PointList;
typedef std::set<FPoint> PointSet;

}

#endif /* FPOINT_H_ */
