/**
 * @file FDisruptorCannon.h
 * @brief Header file for FDisruptorCannon class
 * @date Created: Mar 3, 2009
 * @author Tom Stephens
 */

#ifndef FDISRUPTORCANNON_H_
#define FDISRUPTORCANNON_H_

#include "weapons/FWeapon.h"

namespace Frontier {

/**
 * @brief DisruptorCannon class
 *
 * This class implements the Disruptor Cannon weapon
 *
 * @author Tom Stephens
 * @date Created:  Mar 3, 2009
 * @date Last Modified:  Mar 3, 2009
 */
class FDisruptorCannon: public Frontier::FWeapon {
public:
	FDisruptorCannon();
	virtual ~FDisruptorCannon();
};

}

#endif /* FDISRUPTORCANNON_H_ */
