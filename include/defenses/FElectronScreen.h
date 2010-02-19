/**
 * @file FElectronScreen.h
 * @brief Header file for FElectronScreen class
 * @date Created: Feb 16, 2010
 * @author Tom Stephens
 */

#ifndef FElectronScreen_H_
#define FElectronScreen_H_

#include "defenses/FDefense.h"

namespace Frontier {

/**
 * @class FElectronScreen
 *
 * This class implements the ElectronScreen defense.
 *
 * @author Tom Stephens
 * @date Created:  Feb 16, 2010
 * @date Last Modified:  Feb 16, 2010
 */

class FElectronScreen: public Frontier::FDefense {
public:
	/// default constructor
	FElectronScreen();
	/// default destructor
	~FElectronScreen();
	/**
	 * @brief Returns the to hit modifier for the defense
	 *
	 * This method implements to attack modifier for the
	 * FElectronScreen class.
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

#endif /* FElectronScreen_H_ */
