/**
 * @file FBattleship.cpp
 * @brief Implementation file for Battleship class
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 *
 */
#include "FBattleship.h"
#include <sstream>

namespace Frontier
{

FBattleship::FBattleship() {
	m_maxMR = 2;
	m_maxADF = 2;
	m_maxHP = 120;
	m_currentMR = 2;
	m_currentADF = 2;
	m_currentHP = 120;
	std::ostringstream os;
	os << "Battleship " << m_ID;
	m_name = os.str();
	m_iconName = "../icons/UPFBattleship.png";
	m_icon = new wxImage(m_iconName);
	m_type = "Battleship";
	m_maxDCR=200;
	m_currentDCR=m_maxDCR;
}

FBattleship::~FBattleship(){
}

}
