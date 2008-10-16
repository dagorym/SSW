/**
 * @file FFrigate.h
 * @brief Header file for Frigate class
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 * 
 */
#ifndef FFRIGATE_H_
#define FFRIGATE_H_

#include "FVehicle.h"

namespace Frontier
{

/**
 * @brief Class for the Frigate
 * 
 * This class implements the code for the Frigate
 * 
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 * @date Last Modified:  Feb 21, 2008
 */
class FFrigate : public Frontier::FVehicle
{
public:
	/// Default Constructor
	FFrigate();
	/// Default Destructor
	virtual ~FFrigate();
};

}

#endif /*FFRIGATE_H_*/
