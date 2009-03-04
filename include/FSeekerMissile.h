/**
 * @file FSeekerMissile.h
 * @brief Header file for FSeekerMissile class
 * @date Created: Mar 3, 2009
 * @author Tom Stephens
 */

#ifndef FSEEKERMISSILE_H_
#define FSEEKERMISSILE_H_

#include "FWeapon.h"

namespace Frontier {

/**
 * @brief SeekerMissile class
 *
 * This class implements the Seeker Missile weapon
 *
 * @author Tom Stephens
 * @date Created:  Mar 3, 2009
 * @date Last Modified:  Mar 3, 2009
 */
class FSeekerMissile: public Frontier::FWeapon {
public:
	FSeekerMissile();
	virtual ~FSeekerMissile();
};

}

#endif /* FSEEKERMISSILE_H_ */
