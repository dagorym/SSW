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
	for (PointList::iterator itr = m_hexList.begin(); itr < m_hexList.end(); itr++){
		if (*itr == point){
			return true;
		}
	}
	return false;
}

FPoint FHexPath::endPoint(){
//	std::cerr << "The current endpoint is (" << m_hexList.back().getX() << ", " << m_hexList.back().getY() << ")" << std::endl;
	return m_hexList.back();
}

FPoint FHexPath::startPoint(){
	return m_hexList[0];
}

unsigned int FHexPath::countFlags(int flag){
	unsigned int count = 0;
	FlagMap::iterator itr = m_flags.begin();
	while (itr != m_flags.end() ){
		if (itr->second & flag){
			count++;
		}
		itr++;
	}
	return count;
}

unsigned int FHexPath::removeTrailingPoints(FPoint point){
	while (m_hexList.back() != point){ // loop until we get to the point in question
		// remove associated flags
		m_flags.erase(m_hexList.back());
		// remove last point from list
		m_hexList.pop_back();
	}
	return m_hexList.size();
}

unsigned int FHexPath::getLastHeading(){
	unsigned int size = m_hexList.size();
	return FHexMap::computeHeading(m_hexList[size-2], m_hexList[size-1]);
}

unsigned int FHexPath::getPointHeading(FPoint point){
	PointList::iterator itr = m_hexList.begin();
	while (*itr != point && itr != m_hexList.end()) {
		itr++;
	}
	return FHexMap::computeHeading(*(itr-1), *itr);
}

}
