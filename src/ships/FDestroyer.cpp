/**
 * @file FDestroyer.cpp
 * @brief Implementation file for Destroyer class
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 *
 */
#include "ships/FDestroyer.h"
#include <sstream>

namespace Frontier
{

FDestroyer::FDestroyer() {
	m_maxMR = 3;
	m_maxADF = 3;
	m_maxHP = 50;
	m_currentMR = 3;
	m_currentADF = 3;
	m_currentHP = 50;
	std::ostringstream os;
	os << "Destroyer " << m_ID;
	m_name = os.str();
	m_type = "Destroyer";
	m_maxDCR=75;
	m_currentDCR=m_maxDCR;
	FWeapon *w = createWeapon(FWeapon::LC);
	w->setParent(this);
	m_weapons.push_back(w);
	w = createWeapon(FWeapon::LB);
	w->setParent(this);
	m_weapons.push_back(w);
	w = createWeapon(FWeapon::T);
	w->setParent(this);
	w->setMaxAmmo(2);
	w->setCurrentAmmo(2);
	m_weapons.push_back(w);
	w = createWeapon(FWeapon::RB);
	w->setParent(this);
	w->setMaxAmmo(4);
	w->setCurrentAmmo(4);
	m_weapons.push_back(w);
	w = createWeapon(FWeapon::EB);
	w->setParent(this);
	m_weapons.push_back(w);
	FDefense *d = createDefense(FDefense::RH);
	m_defenses.push_back(d);
	d = createDefense(FDefense::MS);
	d->setMaxAmmo(2);
	d->setCurrentAmmo(2);
	m_defenses.push_back(d);
	d = createDefense(FDefense::ICM);
	d->setMaxAmmo(5);
	d->setCurrentAmmo(5);
	m_defenses.push_back(d);
	m_currentDefense = m_defenses[0];
}

FDestroyer::~FDestroyer(){
}

}
