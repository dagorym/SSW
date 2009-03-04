/**
 * @file FSeekerMissileLauncher.h
 * @brief Header file for FSeekerMissileLauncher class
 * @date Created: Mar 3, 2009
 * @author Tom Stephens
 */

#ifndef FSEEKERMISSILELAUNCHER_H_
#define FSEEKERMISSILELAUNCHER_H_

#include "FWeapon.h"

namespace Frontier {

/**
 * @brief SeekerMissileLauncher class
 *
 * This class implements the Seeker Missile launcher weapon
 *
 * @author Tom Stephens
 * @date Created:  Mar 3, 2009
 * @date Last Modified:  Mar 3, 2009
 */
class FSeekerMissileLauncher: public Frontier::FWeapon {
public:
	FSeekerMissileLauncher();
	virtual ~FSeekerMissileLauncher();
};

}

#endif /* FSEEKERMISSILELAUNCHER_H_ */
