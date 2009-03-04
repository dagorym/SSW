/**
 * @file FMine.h
 * @brief Header file for FMine class
 * @date Created: Mar 3, 2009
 * @author Tom Stephens
 */

#ifndef FMINE_H_
#define FMINE_H_

#include "FWeapon.h"

namespace Frontier {

/**
 * @brief Mine class
 *
 * This class implements the Mine weapon
 *
 * @author Tom Stephens
 * @date Created:  Mar 3, 2009
 * @date Last Modified:  Mar 3, 2009
 */
class FMine: public Frontier::FWeapon {
public:
	FMine();
	virtual ~FMine();
};

}

#endif /* FMINE_H_ */
