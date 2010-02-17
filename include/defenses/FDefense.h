/**
 * @file FDefense.h
 * @brief Header file for FDefense class
 * @date Created: Oct 22, 2009
 * @author Tom Stephens
 */

#ifndef FDEFENSE_H_
#define FDEFENSE_H_

#include "core/FPObject.h"
#include <vector>

namespace Frontier {

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
 * @date Last Modified:  Feb 15, 2010
 */
class FDefense: public Frontier::FPObject {
public:
	/// types of defenses
	enum Defense {UNDEF,NONE,RH,MS,ICM,SS,PS,ES};

	/// default constructor
	FDefense();
	/// default destructor
	virtual ~FDefense() = 0;

	/// return the weapon type
	const FDefense::Defense & getType() const { return m_type;}
	/// gets a text string with the abbreviated name of the weapon
	const std::string getName() const;
    /// gets a text string with the full name of the weapon
	const std::string & getLongName() const { return m_fullName; }
	/// gets the damage status of the weapon
	const bool & isDamaged() const { return m_isDamaged; }
	/// sets the damage status
	void setDamageStatus(bool s) { m_isDamaged = s; }
	/// check to see if weapon has limited ammunition
	bool hasAmmo() { return m_maxAmmo; }
	/// get the weapon's max ammo
	const int & getMaxAmmo() { return m_maxAmmo; }
	/// get the weapon's current ammo count
	const int & getAmmo() { return m_currentAmmo; }
	/// set the maximum ammunition value
	void setMaxAmmo(int m) { m_maxAmmo = m; }
	/// set the current ammunition value
	void setCurrentAmmo(int c) { m_currentAmmo = (c<=m_maxAmmo)?c:m_maxAmmo; }

	/**
	 * @brief Method to save the defense data
	 *
	 * This method implements the FPObject base class virtual write method to
	 * save all the defense's data
	 *
	 * @param os The output stream to write to
	 *
	 * @author Tom Stephens
	 * @date Created:  Feb 15, 2010
	 * @date Last Modified:  Feb 15, 2010
	 */
	const int save(std::ostream &os) const;

	/**
	 * @brief Method to read data contents
	 *
	 * This method is the inverse of the save method.  It reads the data for
	 * the class from the designated input stream.  This method returns 0 if
	 * everything is okay and a positive integer error code if there is a
	 * failure
	 *
	 * @author Tom Stephens
	 * @date Created:  Feb 15, 2010
	 * @date Last Modified:  Feb 15, 2010
	 */
	int load(std::istream &is);

	/**
	 * @brief Reloads the defense's ammo
	 *
	 * This method resets the defense's ammo to full if it has a limited supply
	 *
	 * @author Tom Stephens
	 * @date Created:  Feb 15, 2010
	 * @date Last Modified:  Feb 15, 2010
	 */
	void reload();



protected:
	/// defense's abbreviated name
	std::string m_name;
	/// full name of defense
	std::string m_fullName;
	/// defense type
	FDefense::Defense m_type;
    /// damage status
	bool m_isDamaged;
	/// maximum number of charges/rockets in defense
	int m_maxAmmo;
	/// current number of charges/rockets in defense
	int m_currentAmmo;
};

typedef std::vector<FDefense *> DefenseList;

/// This factory method takes the defense type and returns a pointer to an
/// object of that type
FDefense * createDefense (FDefense::Defense type);

};
#endif /* FDEFENSE_H_ */
