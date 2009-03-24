/**
 * @file FFortress.h
 * @brief Header file for Fortress class
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 * 
 */
#ifndef FFORTRESS_H_
#define FFORTRESS_H_

#include "FVehicle.h"

namespace Frontier
{

/**
 * @brief Class for the Fortress
 * 
 * This class implements the code for the Fortress
 * 
 * @author Tom Stephens
 * @date Created:  Mar 21, 2008
 * @date Last Modified:  Mar 21, 2008
 */
class FFortress : public Frontier::FVehicle
{
public:
	/// Default Constructor
	FFortress();
	/// Default Destructor
	virtual ~FFortress();
};

}

#endif /*FFORTRESS_H_*/
