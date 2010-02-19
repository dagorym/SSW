/**
 * @file FMaskingScreen.h
 * @brief Implementation file for FMaskingScreen class
 * @date Created: Feb 16, 2010
 * @author Tom Stephens
 */

#include "defenses/FMaskingScreen.h"

namespace Frontier {

FMaskingScreen::FMaskingScreen() {
	m_name="MS";
	m_fullName = "Masking Screen";
	m_type=FDefense::MS;
	m_maxAmmo=2;
	m_currentAmmo=m_maxAmmo;
	m_isDamaged = false;

}

FMaskingScreen::~FMaskingScreen() {
	// TODO Auto-generated destructor stub
}

int FMaskingScreen::getAttackModifier (FWeapon::Weapon wType){
	int modifier;
	switch(wType){
	case FWeapon::LC:
		modifier = -35;
		break;
	case FWeapon::LB:
		modifier = -30;
		break;
	case FWeapon::PB:
	case FWeapon::EB:
	case FWeapon::DC:
		modifier = -10;
		break;
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
