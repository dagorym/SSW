/**
 * @file FLaserBattery.h
 * @brief Implementation file for FLaserBattery class
 * @date Created: Feb 27, 2009
 * @author Tom Stephens
 */

#include "weapons/FLaserBattery.h"

namespace Frontier {

FLaserBattery::FLaserBattery() {
	m_name="LB";
	m_fullName = "Laser Battery";
	m_type=LB;
	m_range=9;
	m_nDice=1;
	m_RD=true;
}

FLaserBattery::~FLaserBattery() {
	// TODO Auto-generated destructor stub
}

}
