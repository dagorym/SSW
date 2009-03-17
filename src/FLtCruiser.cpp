/**
 * @file FLtCruiser.cpp
 * @brief Implementation file for Light Cruiser class
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 *
 */
#include "FLtCruiser.h"
#include "Frontier.h"
#include <sstream>

namespace Frontier
{

FLtCruiser::FLtCruiser() {
	m_maxMR = 2;
	m_maxADF = 3;
	m_maxHP = 70;
	m_currentMR = 2;
	m_currentADF = 3;
	m_currentHP = 70;
	std::ostringstream os;
	os << "Light Cruiser " << m_ID;
	m_name = os.str();
	m_type = "LtCruiser";
	m_maxDCR=100;
	m_currentDCR=m_maxDCR;
	FWeapon *w = createWeapon(DC);
	m_weapons.push_back(w);
	w = createWeapon(LB);
	m_weapons.push_back(w);
	w = createWeapon(T);
	w->setMaxAmmo(4);
	w->setCurrentAmmo(4);
	m_weapons.push_back(w);
	w = createWeapon(RB);
	w->setMaxAmmo(6);
	w->setCurrentAmmo(6);
	m_weapons.push_back(w);
	w = createWeapon(EB);
	m_weapons.push_back(w);
	w = createWeapon(PB);
	m_weapons.push_back(w);

}

FLtCruiser::~FLtCruiser(){
}

}
