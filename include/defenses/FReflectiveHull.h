/**
 * @file FReflectiveHull.h
 * @brief Header file for FReflectiveHull class
 * @date Created: Feb 16, 2010
 * @author Tom Stephens
 */

#ifndef FReflectiveHull_H_
#define FReflectiveHull_H_

#include "defenses/FDefense.h"

namespace Frontier {

/**
 * @class FReflectiveHull
 *
 * This class implements the ReflectiveHull defense.
 *
 * @author Tom Stephens
 * @date Created:  Feb 16, 2010
 * @date Last Modified:  Feb 16, 2010
 */

class FReflectiveHull: public Frontier::FDefense {
public:
	/// default constructor
	FReflectiveHull();
	/// default destructor
	~FReflectiveHull();
};

}

#endif /* FReflectiveHull_H_ */
