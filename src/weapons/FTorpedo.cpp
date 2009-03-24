/**
 * @file FTorpedo.cpp
 * @brief Implementation file for FTorpedo class
 * @date Created: Mar 3, 2009
 * @author Tom Stephens
 */

#include "weapons/FTorpedo.h"

namespace Frontier {

FTorpedo::FTorpedo() {
	m_name="T";
	m_fullName = "Torpedo";
	m_type=T;
	m_range=4;
	m_nDice=4;
	m_MPO=true;
	m_ICMMod=-10;
	m_damageTableMod=-20;
}

FTorpedo::~FTorpedo() {
	// TODO Auto-generated destructor stub
}

}
