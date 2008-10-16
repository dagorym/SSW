/**
 * @file FBattleship.h
 * @brief Header file for Battleship class
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 * 
 */
#ifndef FBATTLESHIP_H_
#define FBATTLESHIP_H_

#include "FVehicle.h"

namespace Frontier
{

/**
 * @brief Class for the UPF Battleship
 * 
 * This class implements the code for the UPF Battleship.
 * 
 * @author Tom Stephens
 * @date Created:  Feb 21, 2008
 * @date Last Modified:  Feb 21, 2008
 */
class FBattleship : public Frontier::FVehicle
{
public:
	/// Default Constructor
	FBattleship();
	/// Default Destructor
	virtual ~FBattleship();
};

}

#endif /*FBATTLESHIP_H_*/
