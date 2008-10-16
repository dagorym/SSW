/**
 * @file FHvCruiser.h
 * @brief Header file for Heavy Cruiser class
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 * 
 */
#ifndef FHVCRUISER_H_
#define FHVCRUISER_H_

#include "FVehicle.h"

namespace Frontier
{

/**
 * @brief Class for the Heavy Cruiser
 * 
 * This class implements the code for the Heavy Cruiser
 * 
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 * @date Last Modified:  Feb 21, 2008
 */
class FHvCruiser : public Frontier::FVehicle
{
public:
	/// Default Constructor
	FHvCruiser();
	/// Default Destructor
	virtual ~FHvCruiser();
};

}

#endif /*FHVCRUISER_H_*/
