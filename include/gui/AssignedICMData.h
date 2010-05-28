/**
 * @file AssignedICMData.h
 * @brief Header file for AssignedICMData class
 * @author Tom Stephens
 * @date Created:  Apr 30, 2010
 *
 */

#ifndef ASSIGNEDICMDATA_H_
#define ASSIGNEDICMDATA_H_

#include "ships/FVehicle.h"
#include <map>

namespace Frontier {

/**
 * @brief Object to track data on ICM's assigned to weapons
 *
 * This class keeps track of all the ICM's from a particular ship
 * and which weapons they are assigned to
 *
 * @author Tom Stephens
 * @date Created:  Apr 30, 2010
 * @date Last Modified:  Apr 30, 2010
 */
class AssignedICMData {
public:
	/// default Constructor
	AssignedICMData(FVehicle * ship);
	/// default Destructor
	virtual ~AssignedICMData();

	/// returns max available ICMs
	unsigned int getMaxICMs() { return m_maxICMs; }
	/// returns current number of allocated ICMs
	unsigned int getAllocatedICMs() { return m_allocatedICMs; }

	/**
	 * @brief get the number of ICMs currently allocated to the specified weapon
	 *
	 * This method returns the number of ICMs that have been
	 * allocated so far to the specified weapon.  It looks through
	 * the m_ICMAssignments map to find the assigned number if any.
	 * If not found it returns 0
	 *
	 * @param wep The weapon to look for
	 *
	 * @author Tom Stephens
	 * @date Created:  Apr 30, 2010
	 * @date Last Modified:  Apr 30, 2010
	 */
	unsigned int getICMsAllocatedToWeapon(FWeapon * wep);

	/*
	 * @brief set the number of ICM's allocated to a weapon
	 *
	 * This method takes a weapon pointer and an integer as input
	 * and assigns that number of ICM to the specified weapon.
	 *
	 * @author Tom Stephens
	 * @date Created:  May 26, 2010
	 * @date Last Modified:  Apr 26, 2010
	 */
	void setICMsAllocatedToWeapon(FWeapon *wep, int ICMCount);

private:
	/// ship's id
	unsigned int m_id;
	/// Maximum available ICMs for this ship this round
	unsigned int m_maxICMs;
	/// Total number of ICM's allocated this round from this ship
	unsigned int m_allocatedICMs;
	/// list of weapons and number of ICM's assigned to them
	std::map<unsigned int, unsigned int> m_ICMAssignments;

};

}

#endif /* ASSIGNEDICMDATA_H_ */
