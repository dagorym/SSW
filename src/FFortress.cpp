/**
 * @file FFortress.cpp
 * @brief Implementation file for Fortress class
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 *
 */
#include "FFortress.h"
#include "Frontier.h"
#include <sstream>

namespace Frontier
{

FFortress::FFortress() {
	m_maxMR = 0;
	m_maxADF = 0;
	m_maxHP = 300;
	m_currentMR = 0;
	m_currentADF = 0;
	m_currentHP = 300;
	std::ostringstream os;
	os << "Fortress " << m_ID;
	m_name = os.str();
	m_iconName = "../icons/UPFFortress.png";
	m_icon = new wxImage(m_iconName);
	m_type = "Fortress";
	m_maxDCR=200;
	m_currentDCR=m_maxDCR;
	FWeapon *w = createWeapon(LB);
	m_weapons.push_back(w);
	w = createWeapon(LB);
	m_weapons.push_back(w);
	w = createWeapon(LB);
	m_weapons.push_back(w);
	w = createWeapon(EB);
	m_weapons.push_back(w);
	w = createWeapon(PB);
	m_weapons.push_back(w);
	w = createWeapon(RB);
	w->setMaxAmmo(12);
	w->setCurrentAmmo(12);
	m_weapons.push_back(w);

}

FFortress::~FFortress(){
}

}
