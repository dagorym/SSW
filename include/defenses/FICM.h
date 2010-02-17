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
};

}

#endif /* FICM_H_ */
