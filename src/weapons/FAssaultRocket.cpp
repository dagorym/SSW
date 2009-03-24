/**
 * @file FAssaultRocket.cpp
 * @brief Implementation file for FAssaultRocket class
 * @date Created: Mar 3, 2009
 * @author Tom Stephens
 */

#include "weapons/FAssaultRocket.h"

namespace Frontier {

FAssaultRocket::FAssaultRocket() {
	m_name="AR";
	m_fullName = "Assault Rocket";
	m_type=AR;
	m_range=4;
	m_nDice=2;
	m_dMod=4;
	m_MPO=true;
	m_FF=true;
	m_ICMMod=-5;
	m_damageTableMod=-10;
}

FAssaultRocket::~FAssaultRocket() {
	// TODO Auto-generated destructor stub
}

}
