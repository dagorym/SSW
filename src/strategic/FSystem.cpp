/**
 * @file FSystem.cpp
 * @brief Implementation file for FSystem class
 * @author Tom Stephens, Claude Sonnet 5 (medium)
 * @date Created:  Jan 17, 2005
 * @date Last Modified: Jul 17, 2026
 *
 */

#include "strategic/FSystem.h"

namespace Frontier
{

int FSystem::m_nextID=0;
unsigned int FSystem::m_classCount = 0;

FSystem::FSystem(){
	m_ID = ++m_nextID;
	m_classCount++;
	m_owner=-1;
}

FSystem::~FSystem(){
	for(unsigned int i = 0; i < m_planets.size(); i++){
		delete m_planets[i];
	}
	m_classCount--;
	if (m_classCount==0){  // if all players have been deleted
		m_nextID=0;  // reset the id counter
	}
}

FSystem::FSystem(std::string name,float x, float y, float z, unsigned int owner){
	m_ID = ++m_nextID;
	m_name = name;
	setCoords(x,y,z);
	m_owner = owner;
	m_classCount++;
}

FFleet * FSystem::removeFleet(unsigned int id) {
	FleetList::iterator itr;
	for (itr = m_fleets.begin(); itr < m_fleets.end(); itr++)
		if (id == (*itr)->getID()){
			FFleet * f = (*itr);
			m_fleets.erase(itr);
			return f;
		}
	return NULL;
}

//FPlanet * FSystem::getPlanet(unsigned int id) const {
//		for (unsigned int i=0; i<m_planets.size(); i++){
//			if (m_planets[i]->getID() == id){
//				return m_planets[i];
//			}
//		}
//		return NULL;
//	return NULL;
//}

FPlanet * FSystem::getPlanet(std::string name) const {
	for (unsigned int i=0; i<m_planets.size(); i++){
		if (m_planets[i]->getName() == name){
			return m_planets[i];
		}
	}
	return NULL;
}

const int FSystem::save(std::ostream &os) const{
	// save basic information using the fixed-width little-endian helpers for
	// the ID and owner fields
	int rc = 0;
	if (writeU32(os,m_ID) != 0) rc = 1;
	if (writeString(os,m_name) != 0) rc = 1;
	if (writeU32(os,m_owner) != 0) rc = 1;
	if (write(os,m_coords) != 0) rc = 1;
	// Save the stars in the system
	// Save the planets in the system
	if (writeU32(os,static_cast<uint32_t>(m_planets.size())) != 0) rc = 1;
	for (unsigned int i = 0; i < m_planets.size(); i++) {
		if (m_planets[i]->save(os) != 0) rc = 1;
	}
//Note:  we don't have to save the fleets as they will be
//	  saved as part of the player data and we'll add them back
//	  in once the players are loaded.
	return rc;
}

int FSystem::load(std::istream &is){
	uint32_t idVal = 0;
	if (readU32(is,idVal) != 0) return 1;
	m_ID = idVal;
	if (readString(is,m_name) != 0) return 1;
	uint32_t ownerVal = 0;
	if (readU32(is,ownerVal) != 0) return 1;
	m_owner = ownerVal;
	if (read(is,m_coords) != 0) return 1;
	uint32_t planetCount = 0;
	if (readU32(is,planetCount) != 0) return 1;
	for (uint32_t i = 0; i < planetCount; i++){
		FPlanet *p = new FPlanet;
		if (p->load(is) != 0){
			delete p;
			return 1;
		}
		m_planets.push_back(p);
	}

	// H3: advance the ID counter past the loaded ID (non-colliding guard),
	// respecting FSystem's pre-increment allocation convention
	// (m_ID = ++m_nextID), so the next constructed FSystem's ID is
	// guaranteed to be strictly greater than every ID loaded so far.
	if (m_ID > static_cast<unsigned int>(m_nextID)) {
		m_nextID = static_cast<int>(m_ID);
	}

	return 0;
}

};
