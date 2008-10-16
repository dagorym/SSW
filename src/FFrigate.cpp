/**
 * @file FFrigate.cpp
 * @brief Implementation file for Frigate class
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 * 
 */
#include "FFrigate.h"
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
}

FFrigate::~FFrigate(){
}

}
