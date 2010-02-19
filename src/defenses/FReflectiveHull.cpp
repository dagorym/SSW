/**
 * @file FReflectiveHull.h
 * @brief Implementation file for FReflectiveHull class
 * @date Created: Feb 16, 2010
 * @author Tom Stephens
 */

#include "defenses/FReflectiveHull.h"

namespace Frontier {

FReflectiveHull::FReflectiveHull() {
	m_name="RH";
	m_fullName = "Reflective Hull";
	m_type=FDefense::RH;
	m_maxAmmo=0;
	m_currentAmmo=m_maxAmmo;
	m_isDamaged = false;

}

FReflectiveHull::~FReflectiveHull() {
	// TODO Auto-generated destructor stub
}

int FReflectiveHull::getAttackModifier (FWeapon::Weapon wType){
	return 0;
}

}
