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
};

}

#endif /* FElectronScreen_H_ */
