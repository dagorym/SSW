/**
 * @file FVehicle.cpp
 * @brief Implementation file for FVehicle class
 * @author Tom Stephens
 * @date Created:  Jan 12, 2005
 *
 */
#include "FVehicle.h"
#include <sstream>

namespace Frontier
{
unsigned int FVehicle::m_nextID = 0;
unsigned int FVehicle::m_classCount = 0;

FVehicle::FVehicle(){
	m_ID = m_nextID++;
	m_maxMR = 0;
	m_maxADF = 0;
	m_maxHP = 0;
	m_currentMR = 0;
	m_currentADF = 0;
	m_currentHP = 0;
	std::ostringstream os;
	os << "Vehicle " << m_ID;
	m_name = os.str();
	m_iconName = "../icons/ufo.png";
	m_icon = new wxImage(m_iconName);
	m_type = "none";
	m_classCount++;
	m_speed = 0;
	m_heading = 0;
}

FVehicle::~FVehicle(){
//	std::cerr << "Deleting " << m_name << std::endl;
	delete m_icon;
	m_classCount--;
	if (m_classCount==0){  // if all players have been deleted
		m_nextID=0;  // reset the id counter
	}
}

const int FVehicle::save(std::ostream &os) const {
	writeString(os,m_type);
	write(os,m_ID);
	writeString(os,m_name);
	writeString(os,m_iconName);
	write(os,m_currentMR);
	write(os,m_currentADF);
	write(os,m_currentHP);
	write(os,m_speed);
	write(os,m_heading);
	return 0;
}

void FVehicle::setIcon(std::string icon) {
	m_iconName = icon;
	m_icon->LoadFile(icon);
}

int FVehicle::load(std::istream &is) {
//	std::cerr << "Entering FVehicle::load" << std::endl;
	read(is,m_ID);
	readString(is,m_name);
	readString(is,m_iconName);
	delete m_icon;
	m_icon = new wxImage(m_iconName);
	read(is,m_currentMR);
	read(is,m_currentADF);
	read(is,m_currentHP);
	read(is,m_speed);
	read(is,m_heading);
	return 0;
}

void FVehicle::setMR(int mr){
	if (mr < 0){
		m_currentMR = 0;
	} else if (mr > m_maxMR){
		m_currentMR = m_maxMR;
	} else {
		m_currentMR = mr;
	}
}

void FVehicle::setADF(int adf){
	if (adf < 0){
		m_currentADF = 0;
	} else if (adf > m_maxADF){
		m_currentADF = m_maxADF;
	} else {
		m_currentADF = adf;
	}
}

void FVehicle::setHP(int hp){
	if (hp < 0){
		m_currentHP = 0;
	} else if (hp > m_maxHP){
		m_currentHP = m_maxHP;
	} else {
		m_currentHP = hp;
	}
}

};
