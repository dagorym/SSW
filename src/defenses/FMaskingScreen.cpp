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

}
