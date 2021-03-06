/**
 * @file FAssaultScout.cpp
 * @brief Implementation file for Assault Scout class
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 *
 */
#include "ships/FAssaultScout.h"
#include <sstream>

namespace Frontier
{

FAssaultScout::FAssaultScout() {
	FGameConfig &gc = FGameConfig::create();
	m_maxMR = 4;
	m_maxADF = 5;
	m_maxHP = 15;
	m_currentMR = 4;
	m_currentADF = 5;
	m_currentHP = 15;
	std::ostringstream os;
	os << "Assault Scout " << m_ID;
	m_name = os.str();
	m_iconName = "icons/AS.png";
	m_icon = new wxImage(gc.getBasePath()+m_iconName);
	m_type = "AssaultScout";
	m_maxDCR=50;
	m_currentDCR=m_maxDCR;
	FWeapon *w = createWeapon(FWeapon::AR);
	w->setParent(this);
	w->setMaxAmmo(4);
	w->setCurrentAmmo(4);
	m_weapons.push_back(w);
	w = createWeapon(FWeapon::LB);
	w->setParent(this);
	m_weapons.push_back(w);
	m_defenses.clear();
	FDefense *d = createDefense(FDefense::RH);
	m_defenses.push_back(d);
	m_currentDefense = m_defenses[0];
}

FAssaultScout::~FAssaultScout(){
}

}
