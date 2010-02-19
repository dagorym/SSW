/**
 * @file FICM.h
 * @brief Implementation file for FICM class
 * @date Created: Feb 16, 2010
 * @author Tom Stephens
 */

#include "defenses/FICM.h"

namespace Frontier {

FICM::FICM() {
	m_name="ICM";
	m_fullName = "ICM";
	m_type=FDefense::ICM;
	m_maxAmmo=4;
	m_currentAmmo=m_maxAmmo;
	m_isDamaged = false;

}

FICM::~FICM() {
	// TODO Auto-generated destructor stub
}

int FICM::getAttackModifier (FWeapon::Weapon wType){
	int modifier;
	switch(wType){
	case FWeapon::SM:
		modifier = -8;
		break;
	case FWeapon::T:
		modifier = -10;
		break;
	case FWeapon::AR:
	case FWeapon::M:
		modifier = -5;
		break;
	case FWeapon::RB:
		modifier = -3;
		break;
	case FWeapon::LC:
	case FWeapon::LB:
	case FWeapon::PB:
	case FWeapon::EB:
	case FWeapon::DC:
	default:
		modifier = 0;
		break;
	}
	return modifier;
}

}
