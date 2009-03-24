/**
 * @file FLtCruiser.h
 * @brief Header file for Light Cruiser class
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 * 
 */
#ifndef FLTCRUISER_H_
#define FLTCRUISER_H_

#include "FVehicle.h"

namespace Frontier
{

/**
 * @brief Class for the Light Cruiser
 * 
 * This class implements the code for the Light Cruiser
 * 
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 * @date Last Modified:  Feb 21, 2008
 */
class FLtCruiser : public Frontier::FVehicle
{
public:
	/// Default Constructor
	FLtCruiser();
	/// Default Destructor
	virtual ~FLtCruiser();
};

}

#endif /*FLTCRUISER_H_*/
