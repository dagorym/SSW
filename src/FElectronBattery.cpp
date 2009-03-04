/**
 * @file FElectronBattery.h
 * @brief Implementation file for FElectronBattery class
 * @date Created: Mar 3, 2009
 * @author Tom Stephens
 */

#include "FElectronBattery.h"

namespace Frontier {

FElectronBattery::FElectronBattery() {
	m_name="EB";
	m_fullName = "Electron Battery";
	m_type=EB;
	m_range=12;
	m_nDice=1;
	m_RD=true;
	m_damageTableMod=10;
}

FElectronBattery::~FElectronBattery() {
	// TODO Auto-generated destructor stub
}

}
