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
};

}

#endif /* FMaskingScreen_H_ */
