/**
 * @file FLaserBattery.h
 * @brief Header file for FLaserBattery class
 * @date Created: Feb 27, 2009
 * @author Tom Stephens
 */

#ifndef FLASERBATTERY_H_
#define FLASERBATTERY_H_

#include "weapons/FWeapon.h"

namespace Frontier {

/**
 * @brief LaserBattery class
 *
 * This class implements the Laser Battery weapon
 *
 * @author Tom Stephens
 * @date Created:  Feb 27, 2009
 * @date Last Modified:  Feb 27, 2009
 */
class FLaserBattery: public Frontier::FWeapon {
public:
	FLaserBattery();
	virtual ~FLaserBattery();
};

}

#endif /* FLASERBATTERY_H_ */
