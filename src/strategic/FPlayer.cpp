/**
 * @file FPlayer.cpp
 * @brief Implementation file for FPlayer class
 * @author Tom Stephens, Claude Sonnet 5 (medium)
 * @date Created:  Jan 17, 2005
 * @date Last Modified:  Jul 17, 2026
 *
 */

#include "strategic/FPlayer.h"
#include "strategic/FJumpRoute.h"
#include "ships/FVehicle.h"
#include "core/FGameConfig.h"
#include <cmath>
#include <iostream>

namespace Frontier
{
unsigned int FPlayer::m_nextID = 1;
unsigned int FPlayer::m_classCount = 0;

FPlayer::FPlayer(){
  m_name = "";
  m_ID = m_nextID++;
  m_classCount++;
}

// See FPlayer::~FPlayer() Doxygen block in FPlayer.h for the ownership
// contract this destructor implements (m_fleets, m_unattached, m_destroyed).
FPlayer::~FPlayer(){
	if (m_fleets.size() >0 ) {  // delete the fleets
		for (unsigned int i = 0; i<m_fleets.size(); i++) {
			delete m_fleets[i];
		}
		m_fleets.clear();
	} else {
		m_fleets.clear();
	}
	if (m_unattached.size() > 0){
		for (unsigned int i = 0; i < m_unattached.size(); i++) {
			delete m_unattached[i];
		}
		m_unattached.clear();
	} else {
		m_unattached.clear();
	}
	// FPlayer is the sole owner of ships in m_destroyed (see addDestroyedShip()); they were
	// already removed -- not deleted -- from their owning fleet/unattached list by the caller
	// before being handed off here, so freeing them now cannot double-delete a ship still held
	// by m_fleets or m_unattached.
	if (m_destroyed.size() > 0){
		for (unsigned int i = 0; i < m_destroyed.size(); i++) {
			delete m_destroyed[i];
		}
		m_destroyed.clear();
	} else {
		m_destroyed.clear();
	}
	m_classCount--;
	if (m_classCount==0){  // if all players have been deleted
		m_nextID=1;  // reset the id counter
	}
}

FFleet * FPlayer::getFleet(std::string name) const {
	for (unsigned int i = 0; i < m_fleets.size(); i++){
		if (m_fleets[i]->getName()==name){
			return (m_fleets[i]);
		}
	}
	return NULL;
}

void FPlayer::setFleetIcon(std::string file){
	m_iconName = file;
}

int FPlayer::addShip( FVehicle * ship ){
	m_unattached.push_back(ship);
	return 0;
}

int FPlayer::addDestroyedShip( FVehicle * ship ){
	m_destroyed.push_back(ship);
	return 0;
}

//FVehicle * FPlayer::removeShip( unsigned int id ) {
//	VehicleList::iterator itr;
//	for (itr = m_unattached.begin(); itr < m_unattached.end(); itr++)
//		if (id == (*itr)->getID()){
//			FVehicle * s = (*itr);
//			m_unattached.erase(itr);
//			return s;
//		}
//	return NULL;
//}

//FVehicle * FPlayer::getShip( unsigned int id ) {
//	VehicleList::iterator itr;
//	for (itr = m_unattached.begin(); itr < m_unattached.end(); itr++)
//		if (id == (*itr)->getID()){
//			return (*itr);
//		}
//	return NULL;
//}

const int FPlayer::save(std::ostream &os) const{
	writeU32(os,(uint32_t)m_ID);
	writeString(os,m_name);
	writeString(os,m_iconName);
//	write(os,m_credits);
	writeU32(os,(uint32_t)m_unattached.size());
	for (unsigned int i = 0; i < m_unattached.size(); i++){
		m_unattached[i]->save(os);
	}
	writeU32(os,(uint32_t)m_fleets.size());
	for (unsigned int i = 0; i < m_fleets.size(); i++){
		m_fleets[i]->save(os);
	}
	// F2-serialization: persist the destroyed-ship list using the same
	// type-tag + FVehicle::save() pattern as m_unattached (the ship's own
	// save() writes its type string first).
	writeU32(os,(uint32_t)m_destroyed.size());
	for (unsigned int i = 0; i < m_destroyed.size(); i++){
		m_destroyed[i]->save(os);
	}
	return 0;
}

int FPlayer::load(std::istream &is){
	uint32_t id = 0;
	readU32(is,id);
	m_ID = id;
	// H3: advance the static next-ID counter past any loaded ID so a
	// freshly-constructed player never reuses an ID restored from a save
	// file.
	if (m_ID >= m_nextID){
		m_nextID = m_ID + 1;
	}
	readString(is,m_name);
	readString(is,m_iconName);
	uint32_t uSize = 0, fSize = 0, dSize = 0;
	readU32(is,uSize);
	for(uint32_t i = 0; i < uSize; i++){
		std::string type;
		readString(is,type);
		FVehicle *v = createShip(type);
		if (v == NULL){
			// unknown/corrupt ship type on the wire: abort the load rather
			// than dereference a NULL factory result.
			return 1;
		}
		v->load(is);
		m_unattached.push_back(v);
	}
	readU32(is,fSize);
	for(uint32_t i = 0; i < fSize; i++){
		FFleet *f = new FFleet;
		f->load(is);
		m_fleets.push_back(f);
	}
	// F2-serialization: restore the destroyed-ship list with the same
	// type-tag + createShip() + v->load() pattern and null-check as
	// m_unattached, preserving FPlayer's sole-ownership contract (freed in
	// ~FPlayer()).
	readU32(is,dSize);
	for(uint32_t i = 0; i < dSize; i++){
		std::string type;
		readString(is,type);
		FVehicle *v = createShip(type);
		if (v == NULL){
			// unknown/corrupt ship type on the wire: abort the load rather
			// than dereference a NULL factory result.
			return 1;
		}
		v->load(is);
		m_destroyed.push_back(v);
	}
	return 0;
}

FFleet * FPlayer::getFleet (double x, double y) const {
	for (unsigned int i=0; i < m_fleets.size(); i++){
		double x2 = m_fleets[i]->getCoord(0);
		double y2 = m_fleets[i]->getCoord(1);
		if(sqrt((double)(x-x2)*(x-x2)+(y-y2)*(y-y2))<0.5){
			return m_fleets[i];
		}
	}
	return NULL;
}

unsigned int FPlayer::getLargestFleetID(){
	unsigned int max = 0;
	if (m_fleets.size()>0){
		for (unsigned int i = 0; i< m_fleets.size(); i++){
			if (m_fleets[i]->getID()>max){
				max = m_fleets[i]->getID();
			}
		}
	}
	return max;
}

FFleet * FPlayer::removeFleet(unsigned int id) {
	FleetList::iterator itr;
	for (itr = m_fleets.begin(); itr < m_fleets.end(); itr++)
		if (id == (*itr)->getID()){
			FFleet * f = (*itr);
			m_fleets.erase(itr);
			return f;
		}
	return NULL;
}

};
