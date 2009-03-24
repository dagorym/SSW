/**
 * @file FMinelayer.h
 * @brief Header file for Minelayer class
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 * 
 */
#ifndef FMINELAYER_H_
#define FMINELAYER_H_

#include "FVehicle.h"

namespace Frontier
{

/**
 * @brief Class for the UPF Assault Scout
 * 
 * This class implements the code for the Minelayer
 * 
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 * @date Last Modified:  Feb 21, 2008
 */
class FMinelayer : public Frontier::FVehicle
{
public:
	/// Default Constructor
	FMinelayer();
	/// Default Destructor
	virtual ~FMinelayer();
};

}

#endif /*FMINELAYER_H_*/
