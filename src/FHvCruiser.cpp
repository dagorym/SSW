/**
 * @file FHvCruiser.cpp
 * @brief Implementation file for Heavy Cruiser class
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 *
 */
#include "FHvCruiser.h"
#include "Frontier.h"
#include <sstream>

namespace Frontier
{

FHvCruiser::FHvCruiser() {
	m_maxMR = 1;
	m_maxADF = 2;
	m_maxHP = 80;
	m_currentMR = 1;
	m_currentADF = 2;
	m_currentHP = 80;
	std::ostringstream os;
	os << "Heavy Cruiser " << m_ID;
	m_name = os.str();
	m_type = "HvCruiser";
	m_maxDCR=120;
	m_currentDCR=m_maxDCR;
	FWeapon *w = createWeapon(DC);
	m_weapons.push_back(w);
	w = createWeapon(LB);
	m_weapons.push_back(w);
	w = createWeapon(LB);
	m_weapons.push_back(w);
	w = createWeapon(T);
	w->setMaxAmmo(4);
	w->setCurrentAmmo(4);
	m_weapons.push_back(w);
	w = createWeapon(RB);
	w->setMaxAmmo(8);
	w->setCurrentAmmo(8);
	m_weapons.push_back(w);
	w = createWeapon(EB);
	m_weapons.push_back(w);
	w = createWeapon(PB);
	m_weapons.push_back(w);
	w = createWeapon(SM);
	w->setMaxAmmo(2);
	w->setCurrentAmmo(2);
	m_weapons.push_back(w);

}

FHvCruiser::~FHvCruiser(){
}

}
