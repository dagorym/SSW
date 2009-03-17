/**
 * @file FMinelayer.cpp
 * @brief Implementation file for Minelayer class
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 *
 */
#include "FMinelayer.h"
#include "Frontier.h"
#include <sstream>

namespace Frontier
{

FMinelayer::FMinelayer() {
	m_maxMR = 2;
	m_maxADF = 1;
	m_maxHP = 50;
	m_currentMR = 2;
	m_currentADF = 1;
	m_currentHP = 50;
	std::ostringstream os;
	os << "Minelayer " << m_ID;
	m_name = os.str();
	m_iconName = "../icons/UPFMinelayer.png";
	m_icon = new wxImage(m_iconName);
	m_type = "Minelayer";
	m_maxDCR=75;
	m_currentDCR=m_maxDCR;
	FWeapon *w = createWeapon(LB);
	m_weapons.push_back(w);
	w = createWeapon(LB);
	m_weapons.push_back(w);
	w = createWeapon(SM);
	w->setMaxAmmo(4);
	w->setCurrentAmmo(4);
	m_weapons.push_back(w);
	w = createWeapon(M);
	w->setMaxAmmo(20);
	w->setCurrentAmmo(20);
	m_weapons.push_back(w);

}

FMinelayer::~FMinelayer(){
}

}
