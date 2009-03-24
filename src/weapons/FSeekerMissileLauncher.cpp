/**
 * @file FSeekerMissileLauncher.cpp
 * @brief Implementation file for FSeekerMissileLauncher class
 * @date Created: Mar 3, 2009
 * @author Tom Stephens
 */

#include "weapons/FSeekerMissileLauncher.h"

namespace Frontier {

FSeekerMissileLauncher::FSeekerMissileLauncher() {
	m_name="SM";
	m_fullName = "Seeker Missile";
	m_type=SM;
	m_range=-1;
	m_nDice=5;
	m_ICMMod=-8;
	m_damageTableMod=-20;
}

FSeekerMissileLauncher::~FSeekerMissileLauncher() {
	// TODO Auto-generated destructor stub
}

}
