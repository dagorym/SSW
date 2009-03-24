/**
 * @file FFighter.h
 * @brief Header file for Fighter class
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 * 
 */
#ifndef FFIGHTER_H_
#define FFIGHTER_H_

#include "FVehicle.h"

namespace Frontier
{

/**
 * @brief Class for the Fighter
 * 
 * This class implements the code for the Fighter
 * 
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 * @date Last Modified:  Feb 21, 2008
 */
class FFighter : public Frontier::FVehicle
{
public:
	/// Default Constructor
	FFighter();
	/// Default Destructor
	virtual ~FFighter();
};

}

#endif /*FFIGHTER_H_*/
