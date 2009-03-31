/*
 * @file FPoint.h
 * @brief Header file for the FPoint class
 * @author Tom Stephens
 * @date Created: Mar 30, 2009
 */

#ifndef FPOINT_H_
#define FPOINT_H_

#include <vector>

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
	FPoint() { m_x=0; m_y=0; }

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

private:
	/// x coordinate
	int m_x;
	/// y coordinate
	int m_y;
};

/// define type for a list of points
typedef std::vector<FPoint> PointList;

}

#endif /* FPOINT_H_ */
