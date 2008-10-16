/**
 * @file FFortress.cpp
 * @brief Implementation file for Fortress class
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 * 
 */
#include "FFortress.h"
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
}

FFortress::~FFortress(){
}

}
