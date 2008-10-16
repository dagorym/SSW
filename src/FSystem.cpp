/**
 * @file FSystem.cpp
 * @brief Implementation file for FSystem class
 * @author Tom Stephens
 * @date Created:  Jan 17, 2005
 * 
 */

#include "FSystem.h"

namespace Frontier
{

int FSystem::m_nextID=0;
unsigned int FSystem::m_classCount = 0;

FSystem::FSystem(){
	m_ID = ++m_nextID;
	m_classCount++;
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
	// save basic information
	write(os,m_ID);
	writeString(os,m_name);
	write(os,m_owner);
	write(os,m_coords);
	// Save the stars in the system
	// Save the planets in the system
	write(os,m_planets.size());
	for (unsigned int i = 0; i < m_planets.size(); i++) {
		m_planets[i]->save(os);
	}
//Note:  we don't have to save the fleets as they will be
//	  saved as part of the player data and we'll add them back
//	  in once the players are loaded.
	return 0;
}

int FSystem::load(std::istream &is){
//	std::cerr << "Entering FSystem::load" << std::endl;
	read(is,m_ID);
	readString(is,m_name);
	read(is,m_owner);
	read(is,m_coords);
	unsigned int planetCount;
	read(is,planetCount);
	for (unsigned int i = 0; i < planetCount; i++){
		FPlanet *p = new FPlanet;
		p->load(is);
		m_planets.push_back(p);
	}
	
	return 0;
}

};
