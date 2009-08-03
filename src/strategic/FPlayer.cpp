/**
 * @file FPlayer.cpp
 * @brief Implementation file for FPlayer class
 * @author Tom Stephens
 * @date Created:  Jan 17, 2005
 *
 */

#include "strategic/FPlayer.h"
#include "strategic/FJumpRoute.h"
#include "ships/FVehicle.h"
#include "core/FGameConfig.h"
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

FPlayer::~FPlayer(){
//	std::cerr << "Entering FPlayer destructor" << std::endl;
//	std::cerr << "There are " << m_fleets.size() << " fleets to remove" << std::endl;
	if (m_fleets.size() >0 ) {  // delete the fleets
		for (unsigned int i = 0; i<m_fleets.size(); i++) {
//			std::cerr << "Deleting fleet " << i << std::endl;
			delete m_fleets[i];
		}
		m_fleets.clear();
	} else {
		m_fleets.clear();
	}
//	std::cerr << "There are " << m_unattached.size() << " ships to remove" << std::endl;
	if (m_unattached.size() > 0){
//		std::cerr << "Deleting unattached ships" << std::endl;
		for (unsigned int i = 0; i < m_unattached.size(); i++) {
//			std::cerr << "Deleting ship " << i << std::endl;
			delete m_unattached[i];
		}
		m_unattached.clear();
	} else {
		m_unattached.clear();
	}
	m_classCount--;
	if (m_classCount==0){  // if all players have been deleted
		m_nextID=1;  // reset the id counter
	}
//	std::cerr << "Leaving Player destructor" << std::endl;
}

FFleet * FPlayer::getFleet(std::string name) const {
	for (unsigned int i = 0; i < m_fleets.size(); i++){
		if (m_fleets[i]->getName()==name){
			return (m_fleets[i]);
		}
	}
	return NULL;
}

void FPlayer::drawFleets(wxDC &dc, FMap *map){
	const double scale = map->getScale(dc);
	if (m_fleets.size()>0){
		for (FleetList::iterator itr = m_fleets.begin(); itr < m_fleets.end(); itr++){
			wxBitmap b((*itr)->getIcon()->Scale((int)scale,(int)scale));
			if((*itr)->getInTransit()){  // it's in a jump
				FJumpRoute *j = map->getJumpRoute((*itr)->getJumpRoute());
				unsigned int start,end;
				if(j->getStart()->getID()==(*itr)->getDestination()){
					start = j->getEnd()->getID();
					end = j->getStart()->getID();
				} else {
					start = j->getStart()->getID();
					end = j->getEnd()->getID();
				}
				float dx = (map->getSystem(start)->getCoord(0)-map->getSystem(end)->getCoord(0));
				float dy = (map->getSystem(start)->getCoord(1)-map->getSystem(end)->getCoord(1));
				float frac = 0;
//				std::cerr << "Fraction is " << frac << std::endl;
				if ((*itr)->getTransitTime()==j->getLength()){ // we only need to offset when we assign the jump
					frac = 0.1;
				}
				wxCoord x = (wxCoord)(((*itr)->getCoord(0)-0.5-frac*dx) * scale);
				wxCoord y = (wxCoord)(((*itr)->getCoord(1)-0.5-frac*dy) * scale);
//				std::cerr << "x = " << x << "  y = " << y << std::endl;
				dc.DrawBitmap(b,x,y);
			} else {  // it's in a system
				double xoffset = -0.2*scale;
				double yoffset = -0.2*scale;
				if(m_ID!=1){
					xoffset = 0.2*scale;
					yoffset = -0.2*scale;
				} else if((*itr)->isMilitia()){
					xoffset = 0;
					yoffset = 0.2*scale;
				}
				if((*itr)->getLocation() != 0) {
					wxCoord x = (wxCoord)(map->getSystem((*itr)->getLocation())->getCoord(0)*scale-0.5*scale+xoffset);
					wxCoord y = (wxCoord)(map->getSystem((*itr)->getLocation())->getCoord(1)*scale-0.5*scale+yoffset);
					dc.DrawBitmap(b,x,y);
				}
			}
		}
	}
}

void FPlayer::setFleetIcon(std::string file){
	FGameConfig &gc = FGameConfig::create();
	m_iconName = file;
	m_fleetIcon.LoadFile(gc.getBasePath()+m_iconName);
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
	write(os,m_ID);
	writeString(os,m_name);
	writeString(os,m_iconName);
//	write(os,m_credits);
	write(os,m_unattached.size());
	for (unsigned int i = 0; i < m_unattached.size(); i++){
		m_unattached[i]->save(os);
	}
	write(os,m_fleets.size());
	for (unsigned int i = 0; i < m_fleets.size(); i++){
		m_fleets[i]->save(os);
	}
	return 0;
}

int FPlayer::load(std::istream &is){
	FGameConfig &gc = FGameConfig::create();
//	std::cerr << "Entering FPlayer::load" << std::endl;
	read(is,m_ID);
	readString(is,m_name);
	readString(is,m_iconName);
	m_fleetIcon.LoadFile(gc.getBasePath()+m_iconName);
	unsigned int uSize, fSize;
	read(is,uSize);
	for(unsigned int i = 0; i < uSize; i++){
		std::string type;
		readString(is,type);
		FVehicle *v = createShip(type);
		v->load(is);
		m_unattached.push_back(v);
	}
	read(is,fSize);
	for(unsigned int i = 0; i < fSize; i++){
		FFleet *f = new FFleet;
		f->load(is);
		m_fleets.push_back(f);
	}
	return 0;
}

FFleet * FPlayer::getFleet (int x, int y , FMap * map, wxDC &dc) const {
	const double scale = map->getScale(dc);
	for (unsigned int i=0; i < m_fleets.size(); i++){
		wxCoord x2 = (wxCoord)((m_fleets[i]->getCoord(0))*scale);
		wxCoord y2 = (wxCoord)((m_fleets[i]->getCoord(1))*scale);
		if(sqrt((double)(x-x2)*(x-x2)+(y-y2)*(y-y2))<(0.5*scale)){
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
