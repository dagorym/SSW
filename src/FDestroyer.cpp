/**
 * @file FDestroyer.cpp
 * @brief Implementation file for Destroyer class
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 *
 */
#include "FDestroyer.h"
#include "Frontier.h"
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
	FWeapon *w = createWeapon(LC);
	m_weapons.push_back(w);
	w = createWeapon(LB);
	m_weapons.push_back(w);
	w = createWeapon(T);
	w->setMaxAmmo(2);
	w->setCurrentAmmo(2);
	m_weapons.push_back(w);
	w = createWeapon(RB);
	w->setMaxAmmo(4);
	w->setCurrentAmmo(4);
	m_weapons.push_back(w);
	w = createWeapon(EB);
	m_weapons.push_back(w);
}

FDestroyer::~FDestroyer(){
}

}
