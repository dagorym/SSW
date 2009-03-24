/**
 * @file FAssaultRocket.h
 * @brief Header file for FAssaultRocket class
 * @date Created: Mar 3, 2009
 * @author Tom Stephens
 */

#ifndef FASSAULTROCKET_H_
#define FASSAULTROCKET_H_

#include "weapons/FWeapon.h"

namespace Frontier {

/**
 * @brief AssaultRocket class
 *
 * This class implements the Assault Rocket weapon
 *
 * @author Tom Stephens
 * @date Created:  Mar 3, 2009
 * @date Last Modified:  Mar 3, 2009
 */
class FAssaultRocket: public Frontier::FWeapon {
public:
	FAssaultRocket();
	virtual ~FAssaultRocket();
};

}

#endif /* FASSAULTROCKET_H_ */
