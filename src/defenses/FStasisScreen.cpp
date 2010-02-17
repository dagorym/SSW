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
	m_type=SS;
	m_maxAmmo=0;
	m_currentAmmo=m_maxAmmo;
	m_isDamaged = false;

}

FStasisScreen::~FStasisScreen() {
	// TODO Auto-generated destructor stub
}

}
