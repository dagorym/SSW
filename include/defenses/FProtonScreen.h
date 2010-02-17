/**
 * @file FProtonScreen.h
 * @brief Header file for FProtonScreen class
 * @date Created: Feb 16, 2010
 * @author Tom Stephens
 */

#ifndef FProtonScreen_H_
#define FProtonScreen_H_

#include "defenses/FDefense.h"

namespace Frontier {

/**
 * @class FProtonScreen
 *
 * This class implements the ProtonScreen defense.
 *
 * @author Tom Stephens
 * @date Created:  Feb 16, 2010
 * @date Last Modified:  Feb 16, 2010
 */

class FProtonScreen: public Frontier::FDefense {
public:
	/// default constructor
	FProtonScreen();
	/// default destructor
	~FProtonScreen();
};

}

#endif /* FProtonScreen_H_ */
