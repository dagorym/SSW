/**
 * @file FElectronBattery.h
 * @brief Header file for FElectronBattery class
 * @date Created: Mar 3, 2009
 * @author Tom Stephens
 */

#ifndef FELECTRONBATTERY_H_
#define FELECTRONBATTERY_H_

#include "weapons/FWeapon.h"

namespace Frontier {

/**
 * @brief ElectronBattery class
 *
 * This class implements the Electron Battery weapon
 *
 * @author Tom Stephens
 * @date Created:  Mar 3, 2009
 * @date Last Modified:  Mar 3, 2009
 */
class FElectronBattery: public Frontier::FWeapon {
public:
	FElectronBattery();
	virtual ~FElectronBattery();
};

}

#endif /* FELECTRONBATTERY_H_ */
