/**
 * @file FDestroyer.h
 * @brief Header file for Destroyer class
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 * 
 */
#ifndef FDESTROYER_H_
#define FDESTROYER_H_

#include "FVehicle.h"

namespace Frontier
{

/**
 * @brief Class for the Destroyer
 * 
 * This class implements the code for the Destroyer
 * 
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 * @date Last Modified:  Feb 21, 2008
 */
class FDestroyer : public Frontier::FVehicle
{
public:
	/// Default Constructor
	FDestroyer();
	/// Default Destructor
	virtual ~FDestroyer();
};

}

#endif /*FDESTROYER_H_*/
