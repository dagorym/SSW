/**
 * @file FPlanet.cpp
 * @brief Implementation file for FPlanet class
 * @author Tom Stephens
 * @date Created:  Jan 12, 2005
 *
 */

#include "strategic/FPlanet.h"
#include "ships/FVehicle.h"
#include <iostream>

namespace Frontier
{

int FPlanet::m_nextID = 0;
unsigned int FPlanet::m_classCount = 0;

FPlanet::FPlanet(){
	m_ID = m_nextID++;
	m_classCount++;
	m_station = NULL;
	m_name = "Unknown";
}

FPlanet::~FPlanet(){
	m_classCount--;
	if (m_classCount==0){  // if all players have been deleted
		m_nextID=0;  // reset the id counter
	}
	if(m_station!=NULL){
		delete m_station;
	}
}

FPlanet::FPlanet(std::string name){
	m_name = name;
	m_ID = m_nextID++;
	m_classCount++;
	m_station=NULL;
}

const int FPlanet::save(std::ostream &os) const{
	write(os,m_ID);
	writeString(os,m_name);
	if (m_station!=NULL){
		write(os,1); // @todo this might be a problem, check size
		m_station->save(os);
	} else {
		write (os,0);
	}
	return 0;
}

int FPlanet::load(std::istream &is){
	if (read(is,m_ID) != 0) return 1;
	if (readString(is,m_name) != 0) return 1;
	int stationCount;
	if (read(is,stationCount) != 0) return 1;
	if (stationCount){
		std::string type;
		if (readString(is,type) != 0) return 1;
		m_station = createShip(type);
		if (m_station == NULL){
			// unknown/corrupt station type on the wire: abort the load
			// rather than dereference a NULL factory result.
			return 1;
		}
		if (m_station->load(is) != 0){
			// stream truncated/failed partway through the station's own
			// record. m_station is already owned by this FPlanet (via
			// addStation-equivalent assignment above), so it is not
			// leaked here: ~FPlanet() frees it when the caller
			// (FSystem::load()) deletes this not-yet-owned planet on the
			// nonzero return, mirroring the FFleet/FPlayer::load() abort
			// pattern.
			return 1;
		}
	}
	return 0;
}

void FPlanet::addStation(FVehicle *s) {
	if (m_station != NULL){
		delete m_station;
	}
	m_station = s;
}

};
