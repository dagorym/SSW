/**
 * @file FStasisScreen.h
 * @brief Implementation file for FStasisScreen class
 * @date Created: Feb 16, 2010
 * @author Tom Stephens
 */

#include "defenses/FStasisScreen.h"

namespace Frontier {

FStasisScreen::FStasisScreen() {
	m_name="SS";
	m_fullName = "Stasis Screen";
	m_type=FDefense::SS;
	m_maxAmmo=0;
	m_currentAmmo=m_maxAmmo;
	m_isDamaged = false;

}

FStasisScreen::~FStasisScreen() {
	// TODO Auto-generated destructor stub
}

int FStasisScreen::getAttackModifier (FWeapon::Weapon wType){
	int modifier;
	switch(wType){
	case FWeapon::LC:
	case FWeapon::LB:
	case FWeapon::SM:
		modifier = 15;
		break;
	case FWeapon::PB:
	case FWeapon::EB:
	case FWeapon::DC:
		modifier = -20;
		break;
	case FWeapon::T:
		modifier = 25;
		break;
	case FWeapon::M:
		modifier = 20;
		break;
	case FWeapon::AR:
	case FWeapon::RB:
	default:
		modifier = 0;
		break;
	}
	return modifier;
}

}
