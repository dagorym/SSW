/**
 * @file FLaserCannon.cpp
 * @brief Implementation file for FLaserCannon class
 * @date Created: Mar 3, 2009
 * @author Tom Stephens
 */

#include "weapons/FLaserCannon.h"

namespace Frontier {

FLaserCannon::FLaserCannon() {
	m_name="LC";
	m_fullName = "Laser Cannon";
	m_type=LC;
	m_range=10;
	m_FF=true;
	m_nDice=2;
	m_RD=true;
}

FLaserCannon::~FLaserCannon() {
	// TODO Auto-generated destructor stub
}

}
