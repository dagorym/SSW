/**
 * @file FSeekerMissile.h
 * @brief Implementation file for FSeekerMissile class
 * @date Created: Mar 3, 2009
 * @author Tom Stephens
 */

#include "FSeekerMissile.h"

namespace Frontier {

FSeekerMissile::FSeekerMissile() {
	m_name="SM";
	m_fullName = "Seeker Missile";
	m_type=SM;
	m_range=-1;
	m_nDice=5;
	m_ICMMod=-8;
	m_damageTableMod=-20;
}

FSeekerMissile::~FSeekerMissile() {
	// TODO Auto-generated destructor stub
}

}
