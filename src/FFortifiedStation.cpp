/**
 * @file FFortifiedStation.cpp
 * @brief Implementation file for Fortified Station class
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 *
 */
#include "FFortifiedStation.h"
#include "Frontier.h"
#include <sstream>

namespace Frontier
{

FFortifiedStation::FFortifiedStation() {
	m_maxMR = 0;
	m_maxADF = 0;
	m_maxHP = 140;
	m_currentMR = 0;
	m_currentADF = 0;
	m_currentHP = 140;
	std::ostringstream os;
	os << "Fortified Station " << m_ID;
	m_name = os.str();
	m_iconName = "../icons/UPFFortifiedStation.png";
	m_icon = new wxImage(m_iconName);
	m_type = "FortifiedStation";
	m_maxDCR=100;
	m_currentDCR=m_maxDCR;
	FWeapon *w = createWeapon(LB);
	m_weapons.push_back(w);
	w = createWeapon(LB);
	m_weapons.push_back(w);
	w = createWeapon(RB);
	w->setMaxAmmo(8);
	w->setCurrentAmmo(8);
	m_weapons.push_back(w);

}

FFortifiedStation::~FFortifiedStation(){
}

}
