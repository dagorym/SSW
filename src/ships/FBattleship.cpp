/**
 * @file FBattleship.cpp
 * @brief Implementation file for Battleship class
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 *
 */
#include "ships/FBattleship.h"
#include <sstream>

namespace Frontier
{

FBattleship::FBattleship() {
	FGameConfig &gc = FGameConfig::create();
	m_maxMR = 2;
	m_maxADF = 2;
	m_maxHP = 120;
	m_currentMR = 2;
	m_currentADF = 2;
	m_currentHP = 120;
	std::ostringstream os;
	os << "Battleship " << m_ID;
	m_name = os.str();
	m_iconName = "icons/UPFBattleship.png";
	m_icon = new wxImage(gc.getBasePath()+m_iconName);
	m_type = "Battleship";
	m_maxDCR=200;
	m_currentDCR=m_maxDCR;
	FWeapon *w = createWeapon(FWeapon::DC);
	w->setParent(this);
	m_weapons.push_back(w);
	w = createWeapon(FWeapon::LB);
	w->setParent(this);
	m_weapons.push_back(w);
	w = createWeapon(FWeapon::LB);
	w->setParent(this);
	m_weapons.push_back(w);
	w = createWeapon(FWeapon::LB);
	w->setParent(this);
	m_weapons.push_back(w);
	w = createWeapon(FWeapon::T);
	w->setParent(this);
	w->setMaxAmmo(8);
	w->setCurrentAmmo(8);
	m_weapons.push_back(w);
	w = createWeapon(FWeapon::RB);
	w->setParent(this);
	w->setMaxAmmo(10);
	w->setCurrentAmmo(10);
	m_weapons.push_back(w);
	w = createWeapon(FWeapon::EB);
	w->setParent(this);
	m_weapons.push_back(w);
	w = createWeapon(FWeapon::EB);
	w->setParent(this);
	m_weapons.push_back(w);
	w = createWeapon(FWeapon::PB);
	w->setParent(this);
	m_weapons.push_back(w);
	w = createWeapon(FWeapon::SM);
	w->setParent(this);
	w->setMaxAmmo(4);
	w->setCurrentAmmo(4);
	m_weapons.push_back(w);
	FDefense *d = createDefense(FDefense::RH);
	m_defenses.push_back(d);
	d = createDefense(FDefense::ES);
	m_defenses.push_back(d);
	d = createDefense(FDefense::PS);
	m_defenses.push_back(d);
	d = createDefense(FDefense::SS);
	m_defenses.push_back(d);
	d = createDefense(FDefense::ICM);
	d->setMaxAmmo(12);
	d->setCurrentAmmo(12);
	m_defenses.push_back(d);
	m_currentDefense = m_defenses[0];

}

FBattleship::~FBattleship(){
}

}
