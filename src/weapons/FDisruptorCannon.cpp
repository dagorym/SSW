/**
 * @file FDisruptorCannon.cpp
 * @brief Implementation file for FDisruptorCannon class
 * @date Created: Mar 3, 2009
 * @date Last Modified: Jul 11, 2026
 * @author Tom Stephens, Claude Sonnet 5 (medium)
 */

#include "weapons/FDisruptorCannon.h"

namespace Frontier {

FDisruptorCannon::FDisruptorCannon() {
	m_name="DC";
	m_fullName = "Disruptor Cannon";
	m_type=DC;
	m_range=9;
	m_nDice=3;
	m_FF=true;
	m_RD=true;
	m_damageTableMod=20;
	m_baseToHitProb = 60;
}

FDisruptorCannon::~FDisruptorCannon() {
	// TODO Auto-generated destructor stub
}

}
