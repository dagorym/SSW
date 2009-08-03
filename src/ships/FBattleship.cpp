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
	FWeapon *w = createWeapon(DC);
	m_weapons.push_back(w);
	w = createWeapon(LB);
	m_weapons.push_back(w);
	w = createWeapon(LB);
	m_weapons.push_back(w);
	w = createWeapon(LB);
	m_weapons.push_back(w);
	w = createWeapon(T);
	w->setMaxAmmo(8);
	w->setCurrentAmmo(8);
	m_weapons.push_back(w);
	w = createWeapon(RB);
	w->setMaxAmmo(10);
	w->setCurrentAmmo(10);
	m_weapons.push_back(w);
	w = createWeapon(EB);
	m_weapons.push_back(w);
	w = createWeapon(EB);
	m_weapons.push_back(w);
	w = createWeapon(PB);
	m_weapons.push_back(w);
	w = createWeapon(SM);
	w->setMaxAmmo(4);
	w->setCurrentAmmo(4);
	m_weapons.push_back(w);

}

FBattleship::~FBattleship(){
}

}
