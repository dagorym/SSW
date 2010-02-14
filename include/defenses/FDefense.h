/**
 * @file FDefense.h
 * @brief Header file for FDefense class
 * @date Created: Oct 22, 2009
 * @author Tom Stephens
 */

#ifndef FDEFENSE_H_
#define FDEFENSE_H_

#include "core/FPObject.h"

namespace Frontier {
/// types of defenses
enum {NONE,LB,LC,DC,EB,PB,RB,AR,T,SM,M};


/**
 * @class FDefense
 *
 * This class is the base class for all the defense objects
 * that will represent the defenses on the ship.  It is an
 * abstract base class that defines the interface for the
 * defenses.
 *
 * @author Tom Stephens
 * @date Created:  Oct 22, 2009
 * @date Last Modified:  Oct 22,2009
 */
class FDefense: public Frontier::FPObject {
public:
	/// default constructor
	FDefense();
	/// default destructor
	virtual ~FDefense() = 0;

private:
	/// defense's abbreviated name
	std::string m_name;
	/// full name of defense
	std::string m_fullName;
	/// defense type
	unsigned int m_type;

};

}

#endif /* FDEFENSE_H_ */
