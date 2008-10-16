/**
 * @file FAssaultScout.cpp
 * @brief Implementation file for Assault Scout class
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 * 
 */
#include "FAssaultScout.h"
#include <sstream>

namespace Frontier
{

FAssaultScout::FAssaultScout() {
	m_maxMR = 4;
	m_maxADF = 5;
	m_maxHP = 15;
	m_currentMR = 4;
	m_currentADF = 5;
	m_currentHP = 15;
	std::ostringstream os;
	os << "Assault Scout " << m_ID;
	m_name = os.str();
	m_iconName = "../icons/AS.png";
	m_icon = new wxImage(m_iconName);
	m_type = "AssaultScout";
}

FAssaultScout::~FAssaultScout(){
}

}
