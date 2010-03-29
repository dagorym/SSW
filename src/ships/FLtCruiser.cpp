/**
 * @file FLtCruiser.cpp
 * @brief Implementation file for Light Cruiser class
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 *
 */
#include "ships/FLtCruiser.h"
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
	FWeapon *w = createWeapon(FWeapon::DC);
	m_weapons.push_back(w);
	w = createWeapon(FWeapon::LB);
	m_weapons.push_back(w);
	w = createWeapon(FWeapon::T);
	w->setMaxAmmo(4);
	w->setCurrentAmmo(4);
	m_weapons.push_back(w);
	w = createWeapon(FWeapon::RB);
	w->setMaxAmmo(6);
	w->setCurrentAmmo(6);
	m_weapons.push_back(w);
	w = createWeapon(FWeapon::EB);
	m_weapons.push_back(w);
	w = createWeapon(FWeapon::PB);
	m_weapons.push_back(w);
	FDefense *d = createDefense(FDefense::RH);
	m_defenses.push_back(d);
	d = createDefense(FDefense::ES);
	m_defenses.push_back(d);
	d = createDefense(FDefense::SS);
	m_defenses.push_back(d);
	d = createDefense(FDefense::ICM);
	d->setMaxAmmo(8);
	d->setCurrentAmmo(8);
	m_defenses.push_back(d);
	m_currentDefense = m_defenses[0];

}

FLtCruiser::~FLtCruiser(){
}

}
