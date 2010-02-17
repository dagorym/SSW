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
};

}

#endif /* FStasisScreen_H_ */
