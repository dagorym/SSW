/**
 * @file FArmedStation.cpp
 * @brief Implementation file for Armed Station class
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 *
 */
#include "ships/FArmedStation.h"
#include <sstream>

namespace Frontier
{

FArmedStation::FArmedStation() {
	FGameConfig &gc = FGameConfig::create();
	m_maxMR = 0;
	m_maxADF = 0;
	m_maxHP = 80;
	m_currentMR = 0;
	m_currentADF = 0;
	m_currentHP = 80;
	std::ostringstream os;
	os << "Armed Station " << m_ID;
	m_name = os.str();
	m_iconName = "icons/UPFArmedStation.png";
	m_icon = new wxImage(gc.getBasePath()+m_iconName);
	m_type = "ArmedStation";
	m_maxDCR=75;
	m_currentDCR=m_maxDCR;
	FWeapon *w = createWeapon(FWeapon::LB);
	m_weapons.push_back(w);
	w = createWeapon(FWeapon::RB);
	w->setMaxAmmo(6);
	w->setCurrentAmmo(6);
	m_weapons.push_back(w);
	FDefense *d = createDefense(FDefense::RH);
	m_defenses.push_back(d);
	d = createDefense(FDefense::MS);
	d->setMaxAmmo(2);
	d->setCurrentAmmo(2);
	m_defenses.push_back(d);
	d = createDefense(FDefense::ICM);
	d->setMaxAmmo(6);
	d->setCurrentAmmo(6);
	m_defenses.push_back(d);
	m_currentDefense = m_defenses[0];
}

FArmedStation::~FArmedStation(){
}

}
