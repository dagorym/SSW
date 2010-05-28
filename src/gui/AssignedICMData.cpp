/**
 * @file AssignedICMData.cpp
 * @brief Implementation file for AssignedICMData class
 * @author Tom Stephens
 * @date Created:  Apr 30, 2010
 *
 */

#include "gui/AssignedICMData.h"

namespace Frontier {

AssignedICMData::AssignedICMData(FVehicle * ship) {
	m_id = ship->getID();
	m_maxICMs = ship->getDefense(ship->hasDefense(FDefense::ICM))->getAmmo();
	m_allocatedICMs = 0;

}

AssignedICMData::~AssignedICMData() {
}

unsigned int AssignedICMData::getICMsAllocatedToWeapon(FWeapon * wep){
	std::map<unsigned int, unsigned int>::iterator itr = m_ICMAssignments.find(wep->getID());
	if (itr != m_ICMAssignments.end()){
		return m_ICMAssignments.find(wep->getID())->second;
	}
	return 0;
}

void AssignedICMData::setICMsAllocatedToWeapon(FWeapon *wep, int ICMCount){
	/// do we need to do error checking?
	int currentICMs = m_ICMAssignments[wep->getID()];
	m_ICMAssignments[wep->getID()]=ICMCount;
	m_allocatedICMs += ICMCount-currentICMs;
}

}
