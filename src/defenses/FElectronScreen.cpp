/**
 * @file FElectronScreen.h
 * @brief Implementation file for FElectronScreen class
 * @date Created: Feb 16, 2010
 * @author Tom Stephens
 */

#include "defenses/FElectronScreen.h"

namespace Frontier {

FElectronScreen::FElectronScreen() {
	m_name="ES";
	m_fullName = "Electron Screen";
	m_type=FDefense::ES;
	m_maxAmmo=0;
	m_currentAmmo=m_maxAmmo;
	m_isDamaged = false;

}

FElectronScreen::~FElectronScreen() {
	// TODO Auto-generated destructor stub
}

int FElectronScreen::getAttackModifier (FWeapon::Weapon wType){
	int modifier;
	switch(wType){
	case FWeapon::LC:
	case FWeapon::LB:
		modifier = 15;
		break;
	case FWeapon::PB:
		modifier = 10;
		break;
	case FWeapon::EB:
		modifier = -35;
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
