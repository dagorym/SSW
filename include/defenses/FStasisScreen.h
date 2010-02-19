/**
 * @file FStasisScreen.h
 * @brief Header file for FStasisScreen class
 * @date Created: Feb 16, 2010
 * @author Tom Stephens
 */

#ifndef FStasisScreen_H_
#define FStasisScreen_H_

#include "defenses/FDefense.h"

namespace Frontier {

/**
 * @class FStasisScreen
 *
 * This class implements the StasisScreen defense.
 *
 * @author Tom Stephens
 * @date Created:  Feb 16, 2010
 * @date Last Modified:  Feb 16, 2010
 */

class FStasisScreen: public Frontier::FDefense {
public:
	/// default constructor
	FStasisScreen();
	/// default destructor
	~FStasisScreen();
	/**
	 * @brief Returns the to hit modifier for the defense
	 *
	 * This method implements to attack modifier for the
	 * FStasisScreen class.
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

#endif /* FStasisScreen_H_ */
