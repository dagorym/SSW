/**
 * @file FMinelayer.cpp
 * @brief Implementation file for Minelayer class
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 *
 */
#include "ships/FMinelayer.h"
#include <sstream>

namespace Frontier
{

FMinelayer::FMinelayer() {
	FGameConfig &gc = FGameConfig::create();
	m_maxMR = 2;
	m_maxADF = 1;
	m_maxHP = 50;
	m_currentMR = 2;
	m_currentADF = 1;
	m_currentHP = 50;
	std::ostringstream os;
	os << "Minelayer " << m_ID;
	m_name = os.str();
	m_iconName = "icons/UPFMinelayer.png";
	m_icon = new wxImage(gc.getBasePath()+m_iconName);
	m_type = "Minelayer";
	m_maxDCR=75;
	m_currentDCR=m_maxDCR;
	FWeapon *w = createWeapon(FWeapon::LB);
	w->setParent(this);
	m_weapons.push_back(w);
	w = createWeapon(FWeapon::LB);
	w->setParent(this);
	m_weapons.push_back(w);
	w = createWeapon(FWeapon::SM);
	w->setParent(this);
	w->setMaxAmmo(4);
	w->setCurrentAmmo(4);
	m_weapons.push_back(w);
	w = createWeapon(FWeapon::M);
	w->setParent(this);
	w->setMaxAmmo(20);
	w->setCurrentAmmo(20);
	m_weapons.push_back(w);
	m_defenses.clear();
	FDefense *d = createDefense(FDefense::RH);
	m_defenses.push_back(d);
	d = createDefense(FDefense::ICM);
	d->setMaxAmmo(4);
	d->setCurrentAmmo(4);
	m_defenses.push_back(d);
	m_currentDefense = m_defenses[0];

}

FMinelayer::~FMinelayer(){
}

}
