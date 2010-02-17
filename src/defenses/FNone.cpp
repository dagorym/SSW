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
	m_type=NONE;
	m_maxAmmo=0;
	m_currentAmmo=m_maxAmmo;
	m_isDamaged = false;

}

FNone::~FNone() {
	// TODO Auto-generated destructor stub
}

}
