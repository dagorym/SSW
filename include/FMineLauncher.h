/**
 * @file FMineLauncher.h
 * @brief Header file for FMineLauncher class
 * @date Created: Mar 3, 2009
 * @author Tom Stephens
 */

#ifndef FMINELAUNCHER_H_
#define FMINELAUNCHER_H_

#include "FWeapon.h"

namespace Frontier {

/**
 * @brief MineLauncher class
 *
 * This class implements the Mine Launcher weapon
 *
 * @author Tom Stephens
 * @date Created:  Mar 3, 2009
 * @date Last Modified:  Mar 3, 2009
 */
class FMineLauncher: public Frontier::FWeapon {
public:
	FMineLauncher();
	virtual ~FMineLauncher();
};

}

#endif /* FMINELAUNCHER_H_ */
