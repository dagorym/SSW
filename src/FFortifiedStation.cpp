/**
 * @file FFortifiedStation.cpp
 * @brief Implementation file for Fortified Station class
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 *
 */
#include "FFortifiedStation.h"
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
}

FFortifiedStation::~FFortifiedStation(){
}

}
