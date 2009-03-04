/**
 * @file FMinelayer.cpp
 * @brief Implementation file for Minelayer class
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 *
 */
#include "FMinelayer.h"
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
}

FMinelayer::~FMinelayer(){
}

}
