/*
 * @file FPoint.cpp
 * @brief Implementation file for the FPoint class
 * @author Tom Stephens
 * @date Created: Mar 30, 2009
 */

#include "core/FPoint.h"
#include <cmath>

namespace Frontier {

FPoint::~FPoint() {
	// TODO Auto-generated destructor stub
}

float FPoint::getDistance(const FPoint &p){
	float dx = m_x-p.getX();
	float dy = m_y-p.getY();
	return sqrt(dx*dx+dy*dy);

}

bool FPoint::operator== (const FPoint & p) {
	return ((m_x==p.getX()) && (m_y==p.getY()));
}

bool FPoint::operator< (const FPoint &p) const {
	if (m_y < p.getY()){
		return true;
	} else if ( m_y == p.getY() && m_x < p.getX() ){
		return true;
	} else {
		return false;
	}
}

//******  Related Friend Function Definitions ********//
std::ostream& operator<< (std::ostream& os, const FPoint& p){
  os << "(" << p.m_x << ", " << p.m_y << ")";
  return os;
}

}
