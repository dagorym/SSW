/**
 * @file FFortress.cpp
 * @brief Implementation file for Fortress class
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 *
 */
#include "ships/FFortress.h"
#include <sstream>

namespace Frontier
{

FFortress::FFortress() {
	FGameConfig &gc = FGameConfig::create();
	m_maxMR = 0;
	m_maxADF = 0;
	m_maxHP = 300;
	m_currentMR = 0;
	m_currentADF = 0;
	m_currentHP = 300;
	std::ostringstream os;
	os << "Fortress " << m_ID;
	m_name = os.str();
	m_iconName = "icons/UPFFortress.png";
	m_icon = new wxImage(gc.getBasePath()+m_iconName);
	m_type = "Fortress";
	m_maxDCR=200;
	m_currentDCR=m_maxDCR;
	FWeapon *w = createWeapon(FWeapon::LB);
	m_weapons.push_back(w);
	w = createWeapon(FWeapon::LB);
	m_weapons.push_back(w);
	w = createWeapon(FWeapon::LB);
	m_weapons.push_back(w);
	w = createWeapon(FWeapon::EB);
	m_weapons.push_back(w);
	w = createWeapon(FWeapon::PB);
	m_weapons.push_back(w);
	w = createWeapon(FWeapon::RB);
	w->setMaxAmmo(12);
	w->setCurrentAmmo(12);
	m_weapons.push_back(w);
	FDefense *d = createDefense(FDefense::RH);
	m_defenses.push_back(d);
	d = createDefense(FDefense::MS);
	d->setMaxAmmo(3);
	d->setCurrentAmmo(3);
	m_defenses.push_back(d);
	d = createDefense(FDefense::ES);
	m_defenses.push_back(d);
	d = createDefense(FDefense::PS);
	m_defenses.push_back(d);
	d = createDefense(FDefense::ICM);
	d->setMaxAmmo(20);
	d->setCurrentAmmo(20);
	m_defenses.push_back(d);

}

FFortress::~FFortress(){
}

}
