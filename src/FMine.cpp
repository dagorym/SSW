/**
 * @file FMine.h
 * @brief Implementation file for FMine class
 * @date Created: Mar 3, 2009
 * @author Tom Stephens
 */

#include "FMine.h"

namespace Frontier {

FMine::FMine() {
	m_name="M";
	m_fullName = "Mine";
	m_type=M;
	m_range=0;
	m_nDice=3;
	m_dMod=5;
	m_ICMMod=-5;
	m_damageTableMod=-20;
}

FMine::~FMine() {
	// TODO Auto-generated destructor stub
}

}
