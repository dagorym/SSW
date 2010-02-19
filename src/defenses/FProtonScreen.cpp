/**
 * @file FProtonScreen.h
 * @brief Implementation file for FProtonScreen class
 * @date Created: Feb 16, 2010
 * @author Tom Stephens
 */

#include "defenses/FProtonScreen.h"

namespace Frontier {

FProtonScreen::FProtonScreen() {
	m_name="PS";
	m_fullName = "Proton Screen";
	m_type=FDefense::PS;
	m_maxAmmo=0;
	m_currentAmmo=m_maxAmmo;
	m_isDamaged = false;

}

FProtonScreen::~FProtonScreen() {
	// TODO Auto-generated destructor stub
}

int FProtonScreen::getAttackModifier (FWeapon::Weapon wType){
	int modifier;
	switch(wType){
	case FWeapon::LC:
	case FWeapon::LB:
		modifier = 15;
		break;
	case FWeapon::PB:
		modifier = -35;
		break;
	case FWeapon::EB:
		modifier = 10;
		break;
	case FWeapon::DC:
		modifier = -10;
		break;
	case FWeapon::M:
	case FWeapon::SM:
	case FWeapon::T:
	case FWeapon::AR:
	case FWeapon::RB:
	default:
		modifier = 0;
		break;
	}
	return modifier;
}

}
