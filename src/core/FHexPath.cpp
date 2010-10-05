/*
 * @file FHexPath.cpp
 * @brief Implementation file for the FHexPath class
 * @author Tom Stephens
 * @date Created: Oct 04, 2010
 */

#include "core/FHexPath.h"
#include "core/FHexMap.h"

namespace Frontier {

FHexPath::FHexPath() {
	// TODO Auto-generated constructor stub

}

FHexPath::~FHexPath() {
	// TODO Auto-generated destructor stub
}

int FHexPath::addPoint(FPoint point){
//	FPoint last = m_hexList.back();
	if (( m_hexList.size() == 0 ) || ( FHexMap::computeHexDistance(m_hexList.back().getX(),m_hexList.back().getY(),point.getX(),point.getY())==1)){
		m_hexList.push_back(point);
		return m_hexList.size();
	} else {
		return 0;
	}
}

int FHexPath::addFlag(FPoint point, int flag){
	m_flags[point]=flag;
	return 0;
}

int FHexPath::getFlag(FPoint point){
	FlagMap::iterator itr = m_flags.find(point);
	if (itr == m_flags.end()){
		return 0;
	} else {
		return m_flags[point];
	}
}

bool FHexPath::isPointOnPath(FPoint point){
	for (HexPathList::iterator itr = m_hexList.begin(); itr < m_hexList.end(); itr++){
		if (*itr == point){
			return true;
		}
	}
	return false;
}

}
