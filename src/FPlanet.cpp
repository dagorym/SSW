/**
 * @file FPlanet.cpp
 * @brief Implementation file for FPlanet class
 * @author Tom Stephens
 * @date Created:  Jan 12, 2005
 * 
 */

#include "FPlanet.h"
#include "Frontier.h"
#include <iostream>

namespace Frontier
{

int FPlanet::m_nextID = 0;
unsigned int FPlanet::m_classCount = 0;

FPlanet::FPlanet(){
	m_ID = m_nextID++;
	m_classCount++;
	m_station = NULL;
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
		write(os,1);
		m_station->save(os);
	} else {
		write (os,0);
	}
	return 0;
}

int FPlanet::load(std::istream &is){
//	std::cerr << "Entering FPlanet::load" << std::endl;
	read(is,m_ID);
	readString(is,m_name);
	int stationCount;
	read(is,stationCount);
	if (stationCount){
		std::string type;
		readString(is,type);
		m_station = createShip(type);
		m_station->load(is);
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
