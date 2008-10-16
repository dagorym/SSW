/**
 * @file FObject.h
 * @brief Header file for FObject class
 * @author Tom Stephens
 * @date Created:  Jan 12, 2005
 * 
 */

#ifndef _FOBJECT_H_
#define _FOBJECT_H_

/**
 * @brief Namespace for all game objects
 * 
 * The Frontier namespace will hold all the classes, typedefs, structures,
 * etc. to be used by the game code.
 * 
 * @author Tom Stephens
 * @date Created:  Jan 12, 2005
 */
namespace Frontier
{

/**
 * @brief Base object for all classes used in the game.
 * 
 * The FObject class forms the base object of a single rooted class
 * heirarchy that all the classes in the Frontier namespace will 
 * derive from.  It provides some basic virtual interface functions that
 * all other classes can/will implement.
 * 
 * @author Tom Stephens
 * @date Created:  Jan 12, 2005
 * @date Last Modified:  Jan 12, 2005
 */
class FObject
{
public:
  /// Default constructor
	FObject();
  /// Default destrutor
	virtual ~FObject();

};

};

#endif //_FOBJECT_H_
