/**
 * @file FFortifiedStation.cpp
 * @brief Implementation file for Fortified Station class
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 *
 */
#include "ships/FFortifiedStation.h"
#include <sstream>

namespace Frontier
{

FFortifiedStation::FFortifiedStation() {
	FGameConfig &gc = FGameConfig::create();
	m_maxMR = 0;
	m_maxADF = 0;
	m_maxHP = 140;
	m_currentMR = 0;
	m_currentADF = 0;
	m_currentHP = 140;
	std::ostringstream os;
	os << "Fortified Station " << m_ID;
	m_name = os.str();
	m_iconName = "icons/UPFFortifiedStation.png";
	m_icon = new wxImage(gc.getBasePath()+m_iconName);
	m_type = "FortifiedStation";
	m_maxDCR=100;
	m_currentDCR=m_maxDCR;
	FWeapon *w = createWeapon(FWeapon::LB);
	w->setParent(this);
	m_weapons.push_back(w);
	w = createWeapon(FWeapon::LB);
	w->setParent(this);
	m_weapons.push_back(w);
	w = createWeapon(FWeapon::RB);
	w->setParent(this);
	w->setMaxAmmo(8);
	w->setCurrentAmmo(8);
	m_weapons.push_back(w);
	FDefense *d = createDefense(FDefense::RH);
	m_defenses.push_back(d);
	d = createDefense(FDefense::MS);
	d->setMaxAmmo(2);
	d->setCurrentAmmo(2);
	m_defenses.push_back(d);
	d = createDefense(FDefense::ICM);
	d->setMaxAmmo(10);
	d->setCurrentAmmo(10);
	m_defenses.push_back(d);
	m_currentDefense = m_defenses[0];

}

FFortifiedStation::~FFortifiedStation(){
}

}
