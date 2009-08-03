/**
 * @file FFleet.cpp
 * @brief Implementation file for FFleet class
 * @author Tom Stephens
 * @date Created:  Jan 12, 2005
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
	m_icon = NULL;
	m_isHolding = false;
	m_speed = 0;
	m_transitTime = 0;
	m_classCount++;
	m_pos[0]=0;
	m_pos[1]=0;
	m_home="";
	m_destination = 0;
	m_jumpRouteID = 0;

}

FFleet::FFleet(const FFleet & f){
	FGameConfig &gc = FGameConfig::create();
	m_ID = m_nextID++;
	m_classCount++;
	m_location = f.m_location;
	m_destination = f.m_destination;
	m_inTransit = f.m_inTransit;
	m_isMilitia = f.m_isMilitia;
	m_iconFile = f.m_iconFile;
	m_icon = new wxImage(gc.getBasePath()+m_iconFile);
	m_isHolding = f.m_isHolding;
	m_speed = f.m_speed;
	m_transitTime = f.m_transitTime;
	m_pos[0]=f.m_pos[0];
	m_pos[1]=f.m_pos[1];
	m_owner = f.m_owner;
	m_ships = f.m_ships;
	m_jumpLength = f.m_jumpLength;
	m_name = "Copy of " + f.m_name;
	m_speed = f.m_speed;
	m_jumpRouteID = f.m_jumpRouteID;
	m_home = f.m_home;
	m_dx = f.m_dx;
	m_dy = f.m_dy;

}

FFleet::~FFleet() {
	delete m_icon;
//	std::cerr << "In fleet " << m_name << ", there are " << m_ships.size() << " ships" << std::endl;
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
			m_jumpRouteID=0;
			m_location = m_destination;
			m_destination = -1;
			m_speed = 1;
		}
		int change = before-m_transitTime;
		m_pos[0]+=m_dx*change;
		m_pos[1]+=m_dy*change;
//		std::cerr << "Change is " << change << "  New position is " << m_pos[0] << ", " << m_pos[1] << std::endl;
	}
	return (int)m_transitTime;
}

const int FFleet::save(std::ostream &os) const {
	write(os,m_ID);
	writeString(os,m_name);
	write(os,m_owner);
	write(os,m_location);
	write(os,m_inTransit);
	write(os,m_destination);
	write(os,m_transitTime);
	write(os,m_jumpLength);
	write(os,m_speed);
	write(os,m_jumpRouteID);
	writeString(os,m_iconFile);
	write(os,m_isMilitia);
	writeString(os,m_home);
	write(os,m_isHolding);
	write(os,m_pos[0]);
	write(os,m_pos[1]);
	write(os,m_dx);
	write(os,m_dy);
//	write(os,m_garrison);
	write(os,m_ships.size());
	for (unsigned int i = 0; i < m_ships.size(); i++){
		m_ships[i]->save(os);
	}
	return 0;
}

int FFleet::load(std::istream &is){
	FGameConfig &gc = FGameConfig::create();
//	std::cerr << "Entering FFleet::load" << std::endl;
	read(is,m_ID);
	readString(is, m_name);
	read(is,m_owner);
	read(is,m_location);
	read(is,m_inTransit);
	read(is,m_destination);
	read(is,m_transitTime);
	read(is,m_jumpLength);
	read(is,m_speed);
	read(is,m_jumpRouteID);
	readString(is,m_iconFile);
	m_icon = new wxImage(gc.getBasePath()+m_iconFile);
	read(is,m_isMilitia);
	readString(is,m_home);
	read(is,m_isHolding);
	read(is,m_pos[0]);
	read(is,m_pos[1]);
	read(is,m_dx);
	read(is,m_dy);
//	read(is,m_garrison);
	unsigned int sCount;
	read(is,sCount);
	for(unsigned int i = 0; i < sCount; i++){
		std::string type;
		readString(is,type);
		FVehicle *v = createShip(type);
		v->load(is);
		m_ships.push_back(v);
	}

	return 0;
}

void FFleet::setIcon(std::string icon){
	FGameConfig &gc = FGameConfig::create();
	if(m_icon != NULL){
		delete m_icon;
	}
	m_iconFile = icon;
	m_icon = new wxImage(gc.getBasePath()+m_iconFile);
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
//	std::cerr << "dx = " << m_dx << "  dy = " << m_dy << std::endl;
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
