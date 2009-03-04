/**
 * @file FRocketBattery.h
 * @brief Header file for FRocketBattery class
 * @date Created: Mar 3, 2009
 * @author Tom Stephens
 */

#ifndef FROCKETBATTERY_H_
#define FROCKETBATTERY_H_

#include "FWeapon.h"

namespace Frontier {

/**
 * @brief RocketBattery class
 *
 * This class implements the Rocket Battery weapon
 *
 * @author Tom Stephens
 * @date Created:  Mar 3, 2009
 * @date Last Modified:  Mar 3, 2009
 */
class FRocketBattery: public Frontier::FWeapon {
public:
	FRocketBattery();
	virtual ~FRocketBattery();
};

}

#endif /* FROCKETBATTERY_H_ */
