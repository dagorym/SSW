/**
 * @file FMineLauncher.cpp
 * @brief Implementation file for FMineLauncher class
 * @date Created: Mar 3, 2009
 * @author Tom Stephens
 */

#include "weapons/FMineLauncher.h"

namespace Frontier {

FMineLauncher::FMineLauncher() {
	m_name="M";
	m_fullName = "Mine";
	m_type=M;
	m_range=0;
	m_nDice=3;
	m_dMod=5;
	m_ICMMod=-5;
	m_damageTableMod=-20;
	m_baseToHitProb = 60;
	m_MPO=true;
}

FMineLauncher::~FMineLauncher() {
	// TODO Auto-generated destructor stub
}

}
