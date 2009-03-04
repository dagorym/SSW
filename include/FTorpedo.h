/**
 * @file FTorpedo.h
 * @brief Header file for FTorpedo class
 * @date Created: Mar 3, 2009
 * @author Tom Stephens
 */

#ifndef FTORPEDO_H_
#define FTORPEDO_H_

#include "FWeapon.h"

namespace Frontier {

/**
 * @brief Torpedo class
 *
 * This class implements the Torpedo weapon
 *
 * @author Tom Stephens
 * @date Created:  Mar 3, 2009
 * @date Last Modified:  Mar 3, 2009
 */
class FTorpedo: public Frontier::FWeapon {
public:
	FTorpedo();
	virtual ~FTorpedo();
};

}

#endif /* FTORPEDO_H_ */
