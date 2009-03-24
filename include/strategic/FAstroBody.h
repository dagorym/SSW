/**
 * @file FAstroBody.h
 * @brief Header file for FAstroBody class
 * @author Tom Stephens
 * @date Created:  Jan 12, 2005
 *
 */
#ifndef _FASTROBODY_H_
#define _FASTROBODY_H_

#include "core/FPObject.h"
//#include "FFleet.h"
#include <string>
#include <vector>

namespace Frontier
{
/**
 * @brief Class to hold planetary data
 *
 * This class holds the information necessary to describe an astronomical
 * object such as a planet, moon or star
 *
 * @author Tom Stephens
 * @date Created:  Jan 12, 2005
 * @date Last Modified:  Jan 15, 2005
 */
class FAstroBody : public Frontier::FPObject
{
public:
  /// Default constructor
	FAstroBody();
  /// Default destructor
	virtual ~FAstroBody();

private:

};

};

#endif //_FASTROBODY_H_
