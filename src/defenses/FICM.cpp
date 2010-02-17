/**
 * @file FICM.h
 * @brief Implementation file for FICM class
 * @date Created: Feb 16, 2010
 * @author Tom Stephens
 */

#include "defenses/FICM.h"

namespace Frontier {

FICM::FICM() {
	m_name="ICM";
	m_fullName = "ICM";
	m_type=FDefense::ICM;
	m_maxAmmo=4;
	m_currentAmmo=m_maxAmmo;
	m_isDamaged = false;

}

FICM::~FICM() {
	// TODO Auto-generated destructor stub
}

}
