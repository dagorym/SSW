/**
 * @file FJumpRoute.cpp
 * @brief Implementation file for FJumpRoute class
 * @author Tom Stephens, Claude Sonnet 5 (medium)
 * @date Created:  Jan 20, 2005
 * @date Last Modified: Jul 17, 2026
 *
 */

#include "strategic/FJumpRoute.h"
#include <cmath>

namespace Frontier
{

unsigned int FJumpRoute::m_nextID=0;
unsigned int FJumpRoute::m_classCount = 0;

FJumpRoute::FJumpRoute(){
//	m_ID = m_nextID++;
	m_classCount++;
	m_start = NULL;
	m_end = NULL;
	m_startSystemID = 0;
	m_endSystemID = 0;
	m_length = 0;
	m_ID = 0;
}

FJumpRoute::~FJumpRoute(){
	m_classCount--;
	if (m_classCount==0){  // if all players have been deleted
		m_nextID=0;  // reset the id counter
	}
}

FJumpRoute::FJumpRoute(FSystem * start, FSystem * end, std::vector<unsigned int> players){
	m_start = start;
	m_end = end;
	m_startSystemID = start->getID();
	m_endSystemID = end->getID();
	m_players = players;
	float dx = start->getCoord(0)-end->getCoord(0);
	float dy = start->getCoord(1)-end->getCoord(1);
	float dz = start->getCoord(2)-end->getCoord(2);
	float dis = sqrt(dx*dx+dy*dy+dz*dz);
	m_length = floor(dis + 0.5);//round(dis);  // Note:  to match the FDM this should be ceil instead of round
	m_ID = m_nextID++;
	m_classCount++;
}

const bool FJumpRoute::isKnown(unsigned int id) const {
	for (unsigned int i = 0; i < m_players.size(); i++){
		if (m_players[i]==id) return true;
	}
	return false;
}

int FJumpRoute::getRJ3Time(int l) {
	int dis;
	if (l){
		dis = l;
	} else {
		dis = (int)m_length;
	}
	if (dis%6 == 0){
		return dis/3;
	}
	if (dis == 8) {
		return 4;
	}
	return dis/3+1;
}

int FJumpRoute::getRJ2Time(int l){
	int dis;
	if (l){
		dis = l;
	} else {
		dis = (int)m_length;
	}
	if (dis%4 == 0){
		return dis/2;
	} else {
		return dis/2+1;
	}
}

int FJumpRoute::getJumpTime(int s, int l) {
	if (!l){
		l=(int)m_length;
	}
	switch(s){
	case 1:
		return l;
		break;
	case 2:
		return getRJ2Time(l);
		break;
	case 3:
		return getRJ3Time(l);
		break;
	default:
		return l;
		break;
	}
}

const int FJumpRoute::save(std::ostream &os) const{
	int rc = 0;
	if (writeU32(os,m_ID) != 0) rc = 1;
	if (write(os,m_length) != 0) rc = 1;
	// store the ID's of the systems at the start and the end as real
	// fixed-width fields; FMap::load resolves them back to FSystem
	// pointers via getSystem(id)
	if (writeU32(os,m_start->getID()) != 0) rc = 1;
	if (writeU32(os,m_end->getID()) != 0) rc = 1;
	// store the list of player IDs that know about the route.
	if (writeU32(os,static_cast<uint32_t>(m_players.size())) != 0) rc = 1;
	for (unsigned int i = 0; i < m_players.size(); i++){
		if (writeU32(os,m_players[i]) != 0) rc = 1;
	}
	return rc;
}

int FJumpRoute::load(std::istream &is){
	uint32_t idVal = 0;
	if (readU32(is,idVal) != 0) return 1;
	m_ID = idVal;
	if (read(is,m_length) != 0) return 1;
	// Read the start/end system IDs as real fixed-width fields (no more
	// smuggling the ID value into the m_start/m_end pointers via a cast).
	// m_start/m_end are left NULL here; FMap::load resolves them to the
	// matching FSystem via getSystem(id) and setStart()/setEnd().
	uint32_t startID = 0, endID = 0;
	if (readU32(is,startID) != 0) return 1;
	if (readU32(is,endID) != 0) return 1;
	m_startSystemID = startID;
	m_endSystemID = endID;
	m_start = NULL;
	m_end = NULL;
	uint32_t pSize = 0;
	if (readU32(is,pSize) != 0) return 1;
	for (uint32_t i = 0; i < pSize; i++){
		uint32_t pID = 0;
		if (readU32(is,pID) != 0) return 1;
		m_players.push_back(pID);
	}

	// H3: advance the ID counter past the loaded ID (non-colliding guard),
	// respecting FJumpRoute's post-increment allocation convention
	// (m_ID = m_nextID++), so the next constructed FJumpRoute's ID is
	// guaranteed to be strictly greater than every ID loaded so far.
	if (m_ID >= m_nextID) {
		m_nextID = m_ID + 1;
	}

	return 0;
}

};
