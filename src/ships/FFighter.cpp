/**
 * @file FFighter.cpp
 * @brief Implementation file for Fighter class
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 *
 */
#include "ships/FFighter.h"
#include <sstream>

namespace Frontier
{

FFighter::FFighter() {
	m_maxMR = 5;
	m_maxADF = 5;
	m_maxHP = 8;
	m_currentMR = 5;
	m_currentADF = 5;
	m_currentHP = 8;
	std::ostringstream os;
	os << "Fighter " << m_ID;
	m_name = os.str();
	m_type = "Fighter";
	m_maxDCR=30;
	m_currentDCR=m_maxDCR;
	FWeapon *w = createWeapon(FWeapon::AR);
	w->setParent(this);
	w->setMaxAmmo(3);
	w->setCurrentAmmo(3);
	m_weapons.push_back(w);
	FDefense *d = createDefense(FDefense::RH);
	m_defenses.clear();
	m_defenses.push_back(d);
	m_currentDefense = m_defenses[0];
}

FFighter::~FFighter(){
}

}
