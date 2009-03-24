/**
 * @file FAssaultCarrier.h
 * @brief Header file for Assault Carrier class
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 * 
 */
#ifndef FASSAULTCARRIER_H_
#define FASSAULTCARRIER_H_

#include "FVehicle.h"

namespace Frontier
{

/**
 * @brief Class for the Assault Carrier
 * 
 * This class implements the code for the Assault
 * Carrier.
 * 
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 * @date Last Modified:  Feb 21, 2008
 */
class FAssaultCarrier : public Frontier::FVehicle
{
public:
	/// Default Constructor
	FAssaultCarrier();
	/// Default Destructor
	virtual ~FAssaultCarrier();
};

}

#endif /*FASSAULTCARRIER_H_*/
