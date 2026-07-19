/**
 * @file FFleet.cpp
 * @brief Implementation file for FFleet class
 * @author Tom Stephens, Claude Sonnet 5 (medium)
 * @date Created:  Jan 12, 2005
 * @date Last Modified:  Jul 19, 2026
 *
 */

#include "strategic/FFleet.h"
#include "Frontier.h"
#include "strategic/FJumpRoute.h"
#include "strategic/FSystem.h"
#include "ships/FVehicle.h"

namespace Frontier
{

int FFleet::m_nextID = 0;
unsigned int FFleet::m_classCount = 0;

FFleet::FFleet() {
	m_ID = m_nextID++;
	m_location = 0;
	m_inTransit = false;
	m_isMilitia = false;
	m_isHolding = false;
	m_speed = 0;
	m_transitTime = 0;
	m_classCount++;
	m_pos[0]=0;
	m_pos[1]=0;
	m_home="";
	m_destination = NO_DESTINATION;
	m_jumpRouteID = NO_ROUTE;
	m_dx = 0;
	m_dy = 0;
	m_owner = -1;
	m_jumpLength = -1;

}

FFleet::~FFleet() {
	for (unsigned int i = 0; i < m_ships.size(); i++){
		delete m_ships[i];
	}
	m_classCount--;
	if (m_classCount==0){  // if all players have been deleted
		m_nextID=0;  // reset the id counter
	}
}

int FFleet::setLocation( FSystem * loc, bool transit ,int time, unsigned int dest, int speed, unsigned int route){
	m_location = loc->getID();
	m_destination = dest;
	m_inTransit = transit;
	m_transitTime = time;
	m_jumpLength = time;
	m_speed = speed;
	m_jumpRouteID = route;
	m_pos[0] = loc->getCoord(0);
	m_pos[1] = loc->getCoord(1);
	return 0;
}

int FFleet::addShip( FVehicle * ship ){
	m_ships.push_back(ship);
	return 0;
}

FVehicle * FFleet::removeShip( unsigned int id ) {
	VehicleList::iterator itr;
	for (itr = m_ships.begin(); itr < m_ships.end(); itr++)
		if (id == (*itr)->getID()){
			FVehicle * s = (*itr);
			m_ships.erase(itr);
			return s;
		}
	return NULL;
}

FVehicle * FFleet::getShip( unsigned int id ) {
	VehicleList::iterator itr;
	for (itr = m_ships.begin(); itr < m_ships.end(); itr++)
		if (id == (*itr)->getID()){
			return (*itr);
		}
	return NULL;
}

int FFleet::decTransitTime(){
	if(!m_isHolding){
		int before = m_transitTime;
		if(m_transitTime>m_speed){
			m_transitTime-=m_speed;
			if (m_location != m_destination && m_transitTime <= m_jumpLength/2){  // check to see if we made the jump
				m_transitTime = m_jumpLength/2;
				if (irand(100) > getRJChance()){  // failed the jump
					return -1;
				}
			}

		} else {
			m_transitTime=0;
			m_inTransit = false;
			m_jumpRouteID = NO_ROUTE;
			m_location = m_destination;
			m_destination = NO_DESTINATION;
			m_speed = 1;
		}
		int change = before-m_transitTime;
		m_pos[0]+=m_dx*change;
		m_pos[1]+=m_dy*change;
	}
	return (int)m_transitTime;
}

const int FFleet::save(std::ostream &os) const {
	writeU32(os,(uint32_t)m_ID);
	writeString(os,m_name);
	writeU32(os,(uint32_t)m_owner);
	writeU32(os,(uint32_t)m_location);
	write(os,m_inTransit);
	writeU32(os,(uint32_t)m_destination);
	write(os,m_transitTime);
	write(os,m_jumpLength);
	write(os,m_speed);
	writeU32(os,(uint32_t)m_jumpRouteID);
	writeString(os,m_iconFile);
	write(os,m_isMilitia);
	writeString(os,m_home);
	write(os,m_isHolding);
	write(os,m_pos[0]);
	write(os,m_pos[1]);
	write(os,m_dx);
	write(os,m_dy);
//	write(os,m_garrison);
	writeU32(os,(uint32_t)m_ships.size());
	for (unsigned int i = 0; i < m_ships.size(); i++){
		m_ships[i]->save(os);
	}
	return 0;
}

int FFleet::load(std::istream &is){
	uint32_t id = 0;
	readU32(is,id);
	m_ID = id;
	// H3: advance the static next-ID counter past any loaded ID so a
	// freshly-constructed fleet never reuses an ID restored from a save
	// file. m_nextID is signed (int); compare/assign through an unsigned
	// view of it to avoid a signed/unsigned comparison warning.
	if (m_ID >= (unsigned int)m_nextID){
		m_nextID = (int)(m_ID + 1);
	}
	readString(is, m_name);
	uint32_t owner = 0;
	readU32(is,owner);
	m_owner = owner;
	uint32_t location = 0;
	readU32(is,location);
	m_location = location;
	read(is,m_inTransit);
	uint32_t destination = 0;
	readU32(is,destination);
	m_destination = destination;
	read(is,m_transitTime);
	read(is,m_jumpLength);
	read(is,m_speed);
	uint32_t jumpRouteID = 0;
	readU32(is,jumpRouteID);
	m_jumpRouteID = jumpRouteID;
	// H4: the legacy "route 0 means no route" normalization has been
	// removed. Jump route ID 0 is a valid, distinct route ID and is now
	// preserved exactly as saved; only the distinguished NO_ROUTE sentinel
	// value means "not on a route".
	readString(is,m_iconFile);
	read(is,m_isMilitia);
	readString(is,m_home);
	read(is,m_isHolding);
	read(is,m_pos[0]);
	read(is,m_pos[1]);
	read(is,m_dx);
	read(is,m_dy);
//	read(is,m_garrison);
	uint32_t sCount = 0;
	readU32(is,sCount);
	for(uint32_t i = 0; i < sCount; i++){
		std::string type;
		readString(is,type);
		FVehicle *v = createShip(type);
		if (v == NULL){
			// unknown/corrupt ship type on the wire: abort the load rather
			// than dereference a NULL factory result.
			return 1;
		}
		if (v->load(is) != 0){
			// FR-1 (SF-nested-load-returns): the stream truncated/failed
			// partway through this ship's own record. v is not yet in
			// m_ships, so freeing it here cannot leak or dangle; propagate
			// the failure so the ultimate caller's (FGame::load())
			// aggregate-abort check fires instead of committing a
			// half-built ship.
			delete v;
			return 1;
		}
		m_ships.push_back(v);
	}

	return 0;
}

void FFleet::setIcon(std::string icon){
	m_iconFile = icon;
}

void FFleet::setSpeed(unsigned int s ) {
		m_speed = s;
}

void FFleet::cancelJump() {
	if(m_location!=m_destination){
		m_destination = m_location;
		m_transitTime = m_jumpLength-m_transitTime;
		if (m_transitTime==0) {
			m_inTransit = false;
		}
	}
}

void FFleet::setJumpRoute(int i, const FSystem * s, const FSystem * e, unsigned int length) {
	m_jumpRouteID = i;
	m_transitTime = length;
	m_jumpLength = length;
	m_dx = (e->getCoord(0)-s->getCoord(0))/(float)length;
	m_dy = (e->getCoord(1)-s->getCoord(1))/(float)length;
}

int FFleet::getMaxSpeed(){
	int low = 5;
	for (unsigned int i = 0; i < m_ships.size(); i++){
		if (m_ships[i]->getADF() < low) {
			low = m_ships[i]->getADF();
		}
	}
	return low;
}

int FFleet::getRJChance(){
	if (m_speed == 1) {
		return 100;
	}
	if (m_isMilitia){
		if (m_speed == 2){
			return 70;
		} else {
			return 50;
		}
	}
	bool hasBattleship = false;
	for (unsigned int i = 0; i < m_ships.size(); i++){
		if (m_ships[i]->getType() == "Battleship"){
			hasBattleship = true;
			break;
		}
	}
	if (hasBattleship){
		if (m_speed == 2){
			return 95;
		} else {
			return 90;
		}
	} else {
		if (m_speed == 2){
			return 90;
		} else {
			return 70;
		}
	}
}

};
