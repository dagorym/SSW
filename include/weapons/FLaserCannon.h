/**
 * @file FLaserCannon.h
 * @brief Header file for FLaserCannon class
 * @date Created: Mar 3, 2009
 * @author Tom Stephens
 */

#ifndef FLASERCANNON_H_
#define FLASERCANNON_H_

#include "weapons/FWeapon.h"

namespace Frontier {

/**
 * @brief LaserCannon class
 *
 * This class implements the Laser Cannon weapon
 *
 * @author Tom Stephens
 * @date Created:  Mar 3, 2009
 * @date Last Modified:  Mar 3, 2009
 */
class FLaserCannon: public Frontier::FWeapon {
public:
	FLaserCannon();
	virtual ~FLaserCannon();
};

}

#endif /* FLASERCANNON_H_ */
