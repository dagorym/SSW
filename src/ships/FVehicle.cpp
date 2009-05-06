/**
 * @file FVehicle.cpp
 * @brief Implementation file for FVehicle class
 * @author Tom Stephens
 * @date Created:  Jan 12, 2005
 *
 */
#include "ships/FVehicle.h"
#include "weapons/FWeapon.h"
#include "ships/ships.h"
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
	m_owner = 0;
	m_maxDCR = 0;
	m_currentDCR = 0;
}

FVehicle::~FVehicle(){
//	std::cerr << "Deleting " << m_name << std::endl;
	delete m_icon;
	for (unsigned int i=0; i< m_weapons.size(); i++){  // delete weapons
		delete m_weapons[i];
	}
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
	write(os,m_owner);
	write(os,m_currentDCR);
	write(os,m_weapons.size());
	for (WeaponList::const_iterator itr = m_weapons.begin(); itr != m_weapons.end(); itr++){
		(*itr)->save(os);
	}
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
	read(is,m_owner);
	read(is,m_currentDCR);
	unsigned int count = 0;
	read(is,count);
	for (unsigned int i=0; i< m_weapons.size(); i++){	// they were populated with default values
		delete m_weapons[i];				// at creation so we need to clear them this is a bit
	}										// wasteful in resources and could be done better.
	m_weapons.clear();
	for (unsigned int i=0; i<count; i++){
		unsigned int type;
		read(is,type);
		FWeapon *w = createWeapon(type);
		w->load(is);
		m_weapons.push_back(w);
	}
	return 0;
}

void FVehicle::setMR(unsigned int mr){
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
void FVehicle::setDCR(unsigned int dcr){
	if (dcr < 0){
		m_currentDCR = 0;
	} else if (dcr > m_maxDCR){
		m_currentDCR = m_maxDCR;
	} else {
		m_currentDCR = dcr;
	}
}

std::string FVehicle::getWeaponString(){
	std::ostringstream os;
	for (unsigned int i = 0; i < m_weapons.size(); i++){
		FWeapon *w = m_weapons[i];
		os << w->getName() << "  ";
	}
	return os.str();

}

FVehicle * createShip(std::string type){
	FVehicle *v;
	if(type == "AssaultScout"){
		v = new FAssaultScout;
	} else if (type == "Fighter") {
		v = new FFighter;
	} else if (type == "AssaultCarrier") {
		v = new FAssaultCarrier;
	} else if (type == "Battleship") {
		v = new FBattleship;
	} else if (type == "Destroyer") {
		v = new FDestroyer;
	} else if (type == "Frigate") {
		v = new FFrigate;
	} else if (type == "HvCruiser") {
		v = new FHvCruiser;
	} else if (type == "LtCruiser") {
		v = new FLtCruiser;
	} else if (type == "Minelayer") {
		v = new FMinelayer;
	} else if (type == "Fortress") {
		v = new FFortress;
	} else if (type == "FortifiedStation") {
		v = new FFortifiedStation;
	} else if (type == "ArmedStation") {
		v = new FArmedStation;
	} else {
		// there was an error
		v = NULL;
	}
	return v;
}

void FVehicle::takeDamage (int damage){
	m_currentHP -= damage;
	if (m_currentHP < 0) {
		m_currentHP = 0;
	}
//	std::cerr << m_name << " took " << damage << " HP and has "
//			<< m_currentHP << " HP remaining." << std::endl;
}

};
