/**
 * @file Frontier.h
 * @brief Header for the Frontier game
 * @author Tom Stephens
 * @date Created:  Jan 12, 2005
 * @date Last Modified:  Feb 19, 2008
 *
 */
#ifndef _FRONTIER_H_
#define _FRONTIER_H_

#include "wxWidgets.h"
#include "FVehicle.h"
#include <string>
#include <cstdlib>

typedef std::vector<wxImage> ImageList;

enum {
	BS_Unknown = 0,
	BS_SetupPlanet,
	BS_SetupStation,
	BS_SetupDefendFleet,
	BS_SetupAttackFleet,
	BS_Battle
};

enum {
	PH_NONE = 0,
	PH_SET_SPEED,
	PH_MOVE,
	PH_DEFENSE_FIRE,
	PH_ATTACK_FIRE
};

/// integer random number generator.  Parameter range is the maximum value
inline int irand(unsigned int range){
        return (int)(range*(rand()/(RAND_MAX+1.0)))+1;
}

/// round function since it doesn't exist in VC++
#ifndef LINUX
inline double round(const double & v){
	return floor( v + 0.5 );
}
#endif

/// This method take the ship type name and returns a pointer to an
/// object of that type
Frontier::FVehicle * createShip(std::string type);


#endif //_FRONTIER_H_
