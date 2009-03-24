/**
 * @file FProtonBattery.h
 * @brief Implementation file for FProtonBattery class
 * @date Created: Mar 3, 2009
 * @author Tom Stephens
 */

#include "weapons/FProtonBattery.h"

namespace Frontier {

FProtonBattery::FProtonBattery() {
	m_name="PB";
	m_fullName = "Proton Battery";
	m_type=PB;
	m_range=12;
	m_nDice=1;
	m_RD=true;
	m_damageTableMod=10;
}

FProtonBattery::~FProtonBattery() {
	// TODO Auto-generated destructor stub
}

}
