/**
 * @file FHvCruiser.cpp
 * @brief Implementation file for Heavy Cruiser class
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 *
 */
#include "FHvCruiser.h"
#include <sstream>

namespace Frontier
{

FHvCruiser::FHvCruiser() {
	m_maxMR = 1;
	m_maxADF = 2;
	m_maxHP = 80;
	m_currentMR = 1;
	m_currentADF = 2;
	m_currentHP = 80;
	std::ostringstream os;
	os << "Heavy Cruiser " << m_ID;
	m_name = os.str();
	m_type = "HvCruiser";
	m_maxDCR=120;
	m_currentDCR=m_maxDCR;
}

FHvCruiser::~FHvCruiser(){
}

}
