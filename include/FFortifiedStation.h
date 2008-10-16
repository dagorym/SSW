/**
 * @file FFortifiedStation.h
 * @brief Header file for Fortified Station class
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 * 
 */
#ifndef FFORTIFIEDSTATION_H_
#define FFORTIFIEDSTATION_H_

#include "FVehicle.h"

namespace Frontier
{

/**
 * @brief Class for the FortifiedStation
 * 
 * This class implements the code for the FortifiedStation
 * 
 * @author Tom Stephens
 * @date Created:  Mar 21, 2008
 * @date Last Modified:  Mar 21, 2008
 */
class FFortifiedStation : public Frontier::FVehicle
{
public:
	/// Default Constructor
	FFortifiedStation();
	/// Default Destructor
	virtual ~FFortifiedStation();
};

}

#endif /*FFORTIFIEDSTATION_H_*/
