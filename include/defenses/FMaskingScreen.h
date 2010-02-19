/**
 * @file FMaskingScreen.h
 * @brief Header file for FMaskingScreen class
 * @date Created: Feb 16, 2010
 * @author Tom Stephens
 */

#ifndef FMaskingScreen_H_
#define FMaskingScreen_H_

#include "defenses/FDefense.h"

namespace Frontier {

/**
 * @class FMaskingScreen
 *
 * This class implements the MaskingScreen defense.
 *
 * @author Tom Stephens
 * @date Created:  Feb 16, 2010
 * @date Last Modified:  Feb 16, 2010
 */

class FMaskingScreen: public Frontier::FDefense {
public:
	/// default constructor
	FMaskingScreen();
	/// default destructor
	~FMaskingScreen();
	/**
	 * @brief Returns the to hit modifier for the defense
	 *
	 * This method implements to attack modifier for the
	 * FMaskingScreen class.
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

#endif /* FMaskingScreen_H_ */
