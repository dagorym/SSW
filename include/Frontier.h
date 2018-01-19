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
#include <string>
#include <cstdlib>
#include <vector>

typedef std::vector<wxImage> ImageList;

enum {
	BS_Unknown = 0,
	BS_SetupPlanet,
	BS_SetupStation,
	BS_SetupDefendFleet,
	BS_PlaceMines,
	BS_SetupAttackFleet,
	BS_Battle
};

enum {
	PH_NONE = 0,
	PH_SET_SPEED,
	PH_MOVE,
	PH_SELECT_SHIP,
	PH_FINALIZE_MOVE,
	PH_DEFENSE_FIRE,
	PH_ATTACK_FIRE
};

/// integer random number generator.  Parameter range is the maximum value
inline int irand(unsigned int range){
        return (int)(range*(rand()/(RAND_MAX+1.0)))+1;
}

/// implements the sign function
template < typename T >
inline bool sign( T const &value ){
return value < 0;
}

/// returns the integer floor of x/2
inline int Floor2 (int x){
	return (x>=0) ? (x>>1) : (x-1)/2;
}

/// returns the integer ceiling of x/2
inline int Ceil2 (int x){
	return (x >= 0) ? (x+1)>>1 : x/2;
}

/// round function since it doesn't exist in VC++
//#ifndef LINUX
//inline double round(const double & v){
//	return floor( v + 0.5 );
//}
//#endif


#endif //_FRONTIER_H_
