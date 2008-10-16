/**
 * @file FDestroyer.cpp
 * @brief Implementation file for Destroyer class
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 * 
 */
#include "FDestroyer.h"
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
}

FDestroyer::~FDestroyer(){
}

}
