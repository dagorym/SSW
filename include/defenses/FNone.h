/**
 * @file FNone.h
 * @brief Header file for FNone class
 * @date Created: Feb 16, 2010
 * @author Tom Stephens
 */

#ifndef FNone_H_
#define FNone_H_

#include "defenses/FDefense.h"

namespace Frontier {

/**
 * @class FNone
 *
 * This class implements the case of no defense.
 *
 * @author Tom Stephens
 * @date Created:  Feb 16, 2010
 * @date Last Modified:  Feb 16, 2010
 */

class FNone: public Frontier::FDefense {
public:
	/// default constructor
	FNone();
	/// default destructor
	~FNone();
	/**
	 * @brief Returns the to hit modifier for the defense
	 *
	 * This method implements to attack modifier for the
	 * FNone class.
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

#endif /* FNone_H_ */
