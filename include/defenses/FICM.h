/**
 * @file FICM.h
 * @brief Header file for FICM class
 * @date Created: Feb 16, 2010
 * @author Tom Stephens
 */

#ifndef FICM_H_
#define FICM_H_

#include "defenses/FDefense.h"

namespace Frontier {

/**
 * @class FICM
 *
 * This class implements the ICM defense.
 *
 * @author Tom Stephens
 * @date Created:  Feb 16, 2010
 * @date Last Modified:  Feb 16, 2010
 */

class FICM: public Frontier::FDefense {
public:
	/// default constructor
	FICM();
	/// default destructor
	~FICM();
	/**
	 * @brief Returns the to hit modifier for the defense
	 *
	 * This method implements to attack modifier for the
	 * FICM class.
	 *
	 * @param weapon The weapon type that is firing at the defense
	 *
	 * @author Tom Stephens
	 * @date Created:  Feb 18, 2010
	 * @date Last Modified:  Feb 18, 2010
	 */
	int getAttackModifier (FWeapon::Weapon wType);

};

}

#endif /* FICM_H_ */
