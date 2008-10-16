/**
 * @file FAssaultScout.h
 * @brief Header file for Assault Scout class
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 * 
 */
#ifndef FASSAULTSCOUT_H_
#define FASSAULTSCOUT_H_

#include "FVehicle.h"

namespace Frontier
{

/**
 * @brief Class for the UPF Assault Scout
 * 
 * This class implements the code for the UPF Assault
 * Scout.
 * 
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 * @date Last Modified:  Feb 21, 2008
 */
class FAssaultScout : public Frontier::FVehicle
{
public:
	/// Default Constructor
	FAssaultScout();
	/// Default Destructor
	virtual ~FAssaultScout();
};

}

#endif /*FASSAULTSCOUT_H_*/
