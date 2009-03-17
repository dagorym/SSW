/**
 * @file FArmedStation.cpp
 * @brief Implementation file for Armed Station class
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 *
 */
#include "FArmedStation.h"
#include "Frontier.h"
#include <sstream>

namespace Frontier
{

FArmedStation::FArmedStation() {
	m_maxMR = 0;
	m_maxADF = 0;
	m_maxHP = 80;
	m_currentMR = 0;
	m_currentADF = 0;
	m_currentHP = 80;
	std::ostringstream os;
	os << "Armed Station " << m_ID;
	m_name = os.str();
	m_iconName = "../icons/UPFArmedStation.png";
	m_icon = new wxImage(m_iconName);
	m_type = "ArmedStation";
	m_maxDCR=75;
	m_currentDCR=m_maxDCR;
	FWeapon *w = createWeapon(LB);
	m_weapons.push_back(w);
	w = createWeapon(RB);
	w->setMaxAmmo(6);
	w->setCurrentAmmo(6);
	m_weapons.push_back(w);
}

FArmedStation::~FArmedStation(){
}

}
