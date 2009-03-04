/**
 * @file FProtonBattery.h
 * @brief Header file for FProtonBattery class
 * @date Created: Mar 3, 2009
 * @author Tom Stephens
 */

#ifndef FPROTONBATTERY_H_
#define FPROTONBATTERY_H_

#include "FWeapon.h"

namespace Frontier {

/**
 * @brief ProtonBattery class
 *
 * This class implements the Proton Battery weapon
 *
 * @author Tom Stephens
 * @date Created:  Mar 3, 2009
 * @date Last Modified:  Mar 3, 2009
 */
class FProtonBattery: public Frontier::FWeapon {
public:
	FProtonBattery();
	virtual ~FProtonBattery();
};

}

#endif /* FPROTONBATTERY_H_ */
