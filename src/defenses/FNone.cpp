/**
 * @file FNone.h
 * @brief Implementation file for FNone class
 * @date Created: Feb 16, 2010
 * @author Tom Stephens
 */

#include "defenses/FNone.h"

namespace Frontier {

FNone::FNone() {
	m_name="N/A";
	m_fullName = "None";
	m_type=FDefense::NONE;
	m_maxAmmo=0;
	m_currentAmmo=m_maxAmmo;
	m_isDamaged = false;

}

FNone::~FNone() {
	// TODO Auto-generated destructor stub
}

int FNone::getAttackModifier (FWeapon::Weapon wType){
	int modifier;
	switch(wType){
	case FWeapon::LC:
	case FWeapon::LB:
		modifier = 15;
		break;
	case FWeapon::PB:
	case FWeapon::EB:
	case FWeapon::DC:
	case FWeapon::SM:
	case FWeapon::T:
	case FWeapon::AR:
	case FWeapon::M:
	case FWeapon::RB:
	default:
		modifier = 0;
		break;
	}
	return modifier;
}

}
