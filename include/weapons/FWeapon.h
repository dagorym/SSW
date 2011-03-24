/**
 * @file FWeapon.h
 * @brief Header file for FWeapon class
 * @date Created: Feb 27, 2009
 * @author Tom Stephens
 */

#ifndef FWEAPON_H_
#define FWEAPON_H_

#include "core/FPObject.h"
#include <string>
#include <vector>

namespace Frontier
{
class FVehicle;

/**
 * @brief Base class for all weapon objects
 *
 * This class forms the base class for all the weapon objects used in
 * the game.
 *
 * @author Tom Stephens
 * @date Created:  Feb 27, 2009
 * @date Last Modified:  May 28, 2010
 */
class FWeapon: public Frontier::FPObject {
public:
	/// types of weapons
	enum Weapon {NONE,LB,LC,DC,EB,PB,RB,AR,T,SM,M};

	/// default constructor
	FWeapon();
	/// default destructor;
	virtual ~FWeapon();

	/// get the weapon's ID
	const unsigned int & getID() const { return m_ID; }
/// return the weapon type
	const FWeapon::Weapon & getType() const { return m_type;}
	/// return the weapon's range
	const unsigned int & getRange() const { return m_range; }
	/// return the weapon's number of damage dice
	const unsigned int & getDice() const { return m_nDice; }
	/// return the weapon's damage modifier
	const unsigned int & getDamageMod() const { return m_dMod; }
	/// check to see if weapon is Moving Player Only (MPO)
	const bool & isMPO() const { return m_MPO; }
	/// check to see if weapon is Forward Firing (FF)
	const bool & isFF() const { return m_FF; }
	/// check to see if the weapon suffers from Range Diffusion (RD)
	const bool & isRD() const { return m_RD; }
	/// check to see if weapon has limited ammunition
	bool hasAmmo() { return m_maxAmmo; }
	/// get the weapon's max ammo
	const int & getMaxAmmo() { return m_maxAmmo; }
	/// get the weapon's current ammo count
	const int & getAmmo() { return m_currentAmmo; }
	/// get the weapon's ICM modifier
	const int & getICMMod() const { return m_ICMMod; }
	/// gets a text string with the abbreviated name of the weapon
	/// @todo expand this comment last updated 4/10/09
	const std::string getName() const;
    /// gets a text string with the full name of the weapon
	const std::string & getLongName() const { return m_fullName; }
	/// gets the damage status of the weapon
	const bool & isDamaged() const { return m_isDamaged; }
	/// sets the damage status
	void setDamageStatus(bool s) { m_isDamaged = s; }

	/// set the maximum ammunition value
	void setMaxAmmo(int m) { m_maxAmmo = m; }
	/// set the current ammunition value
	void setCurrentAmmo(int c) { m_currentAmmo = (c<=m_maxAmmo)?c:m_maxAmmo; }
	/// Gets the currently selected target, returns null if not target set
	FVehicle * getTarget() { return m_target; }
	/// gets the range to the current target
	int getTargetRange() { return m_targetRange; }
	/// set pointer to ship the weapon is a part of
	void setParent(FVehicle *v) { m_parent = v; }
	/// get pointer to the ship the weapon is a part of
	const FVehicle * getParent() const { return m_parent; }
	/// set the number of ICMs defending against this weapon
	void setAssignedICMCount(int count){ m_assignedICMCount=count; }

	/**
	 * @brief Method to save the weapon data
	 *
	 * This method implements the FPObject base class virtual write method to
	 * save all the weapon's data
	 *
	 * @param os The output stream to write to
	 *
	 * @author Tom Stephens
	 * @date Created:  Mar 12, 2009
	 * @date Last Modified:  Mar 12, 2009
	 */
	const virtual int save(std::ostream &os) const;

	/**
	 * @brief Method to read data contents
	 *
	 * This method is the inverse of the save method.  It reads the data for
	 * the class from the designated input stream.  This method returns 0 if
	 * everything is okay and a positive integer error code if there is a
	 * failure
	 *
	 * @author Tom Stephens
	 * @date Created:  Mar 12, 2009
	 * @date Last Modified:  Mar 12, 2009
	 */
	virtual int load(std::istream &is);

	/**
	 * @brief Fires the weapon
	 *
	 * This method fires the weapon at the specified target.  It currently just
	 * implements the basic damage system (hull damage only).  It rolls the d100
	 * and compares that to the To Hit probability modified by the range if
	 * appropriate.  If the attack is a success, it computes the hull damage and
	 * passes that along to the target vessel.
	 *
	 * @param v The FVehicle object to shoot at
	 *
	 * @author Tom Stephens
	 * @date Created:  Feb 27, 2009
	 * @date Last Modified:  Mar 24, 2011
	 */
	void fire();

	/**
	 * @brief Set the target vessel for this weapon
	 *
	 * This method sets the target vessel that the weapon will fire
	 * at during the weapon firing phase
	 *
	 *
	 * @param v The FVehicle object to shoot at
	 * @param r The range to the selected target
	 * @param headOn Flag for whether or not the target is head on
	 *
	 * @author Tom Stephens
	 * @date Created:  Feb 27, 2009
	 * @date Last Modified:  Apr 27, 2009
	 */
	void setTarget(FVehicle * v,int r, bool headOn = false);

	/**
	 * @brief Reloads the weapon's ammo
	 *
	 * This method loops resets the weapon's ammo to full if it has a limited supply
	 *
	 * @author Tom Stephens
	 * @date Created:  Jun 03, 2009
	 * @date Last Modified:  Jun 03, 2009
	 */
	void reload();

protected:
	/// counter for total number of weapons and next weapon ID
	static unsigned int m_nextID;
	/// weapon ID number
	unsigned int m_ID;
	/// pointer to ship weapon is a part of
	FVehicle * m_parent;
	/// weapon's abbreviated name
	std::string m_name;
	/// full name of weapon
	std::string m_fullName;
	/// weapon type
	FWeapon::Weapon m_type;
	/// weapon range
	unsigned int m_range;
	/// number of dice of damage
	unsigned int m_nDice;
	/// damage modifier
	unsigned int m_dMod;
	/// Moving player only flag
	bool m_MPO;
	/// Forward firing flag
	bool m_FF;
	/// range diffusion flag
	bool m_RD;
	/// maximium ammo count (O if unlimited)
	int m_maxAmmo;
	/// ammo count
	int m_currentAmmo;
	/// ICM modifier
	int m_ICMMod;
	/// damage table modifier
	int m_damageTableMod;
	/// Pointer to the weapon's target vessel
	FVehicle *m_target;
	/// range to selected target
	int m_targetRange;
	/// flag for whether or not the weapon is damaged
	bool m_isDamaged;
	/// flag for whether or not the current target gets head-on shot bonus.
	bool m_isHeadOn;
	/// base probability to hit a RH
	int m_baseToHitProb;
	/// number of assigned ICMs
	int m_assignedICMCount;
};

typedef std::vector<FWeapon *> WeaponList;

/// This factory method takes the weapon type and returns a pointer to an
/// object of that type
FWeapon * createWeapon (FWeapon::Weapon type);

};
#endif /* FWEAPON_H_ */
