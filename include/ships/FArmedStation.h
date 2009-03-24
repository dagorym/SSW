/**
 * @file FArmedStation.h
 * @brief Header file for ArmedStation class
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 * 
 */
#ifndef FARMEDSTATION_H_
#define FARMEDSTATION_H_

#include "FVehicle.h"

namespace Frontier
{

/**
 * @brief Class for the ArmedStation
 * 
 * This class implements the code for the ArmedStation
 * 
 * @author Tom Stephens
 * @date Created:  Mar 21, 2008
 * @date Last Modified:  Mar 21, 2008
 */
class FArmedStation : public Frontier::FVehicle
{
public:
	/// Default Constructor
	FArmedStation();
	/// Default Destructor
	virtual ~FArmedStation();
};

}

#endif /*FARMEDSTATION_H_*/
