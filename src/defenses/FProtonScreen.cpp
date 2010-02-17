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
	m_type=PS;
	m_maxAmmo=0;
	m_currentAmmo=m_maxAmmo;
	m_isDamaged = false;

}

FProtonScreen::~FProtonScreen() {
	// TODO Auto-generated destructor stub
}

}
