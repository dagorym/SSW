/**
 * @file FFrigate.cpp
 * @brief Implementation file for Frigate class
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 *
 */
#include "ships/FFrigate.h"
#include <sstream>

namespace Frontier
{

FFrigate::FFrigate() {
	m_maxMR = 3;
	m_maxADF = 4;
	m_maxHP = 40;
	m_currentMR = 3;
	m_currentADF = 4;
	m_currentHP = 40;
	std::ostringstream os;
	os << "Frigate " << m_ID;
	m_name = os.str();
	m_type = "Frigate";
	m_maxDCR=70;
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

}

FFrigate::~FFrigate(){
}

}
