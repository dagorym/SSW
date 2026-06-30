/**
 * @file Frontier.h
 * @brief Header for the Frontier game
 * @author Tom Stephens, gpt-5.4 (high), claude-sonnet-4-6 (medium)
 * @date Created:  Jan 12, 2005
 * @date Last Modified:  Jun 02, 2026
 *
 */
#ifndef _FRONTIER_H_
#define _FRONTIER_H_

#include <string>
#include <cstdlib>
#include <vector>

// ImageList is now defined in include/gui/GuiTypes.h

enum {
	BS_Unknown = 0,
	BS_SetupPlanet,
	BS_SetupStation,
	BS_SetupDefendFleet,
	BS_PlaceMines,
	BS_PlaceSeekers,
	BS_SetupAttackFleet,
	BS_Battle
};

/**
 * @brief Tactical battle phase ordering.
 *
 * `PH_SEEKER_ACTIVATION` is an additive pre-movement phase that allows the
 * tactical model to expose inactive seeker stacks, activate individual seekers,
 * and resolve the active-seeker seam before normal ship movement begins.
 */
enum {
	PH_NONE = 0,
	PH_SET_SPEED,
	PH_SEEKER_ACTIVATION,
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
