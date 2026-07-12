/**
 * @file FWeapon.h
 * @brief Header file for FWeapon class
 * @date Created: Feb 27, 2009
 * @author Tom Stephens
 */

#ifndef FWEAPON_H_
#define FWEAPON_H_

#include "core/FPObject.h"
#include "tactical/FTacticalAttackResult.h"
#include <string>
#include <vector>

namespace Frontier
{
class FVehicle;

/**
 * @brief Base class for all weapon objects
 *
 * This class stores shared weapon stats, target-selection state, ammo and
 * damage state, and attack-resolution behavior for all concrete weapon types.
 *
 * @author Tom Stephens, gpt-5.3-codex (medium)
 * @date Created:  Feb 27, 2009
 * @date Last Modified:  Mar 20, 2026
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
	 * This method resolves a complete attack cycle against the currently assigned
	 * target, including skip conditions, to-hit logic, defensive modifiers,
	 * optional damage-table effects, and ammunition consumption, then returns a
	 * structured tactical attack result.
	 *
	 * To-hit defensive modifier resolution: the masking screen "fired out of
	 * the screen" override is laser-only (LC/LB) and only applies when this
	 * weapon has a parent (mines and seekers have no firing parent) whose
	 * raised defense is a Masking Screen -- in that laser/MS-parent case the
	 * to-hit modifier comes from the attacker's raised Masking Screen instead
	 * of the target's defense. Every other weapon type, and every laser fired
	 * by a parent without a raised Masking Screen, resolves the to-hit
	 * modifier against the target's most-effective OPERATING defense via
	 * FVehicle::resolveToHitModifier(), which also applies the
	 * attracting-screen override (Proton Screen attracts Electron Beam,
	 * Electron Screen attracts Proton Beam, Stasis Screen attracts
	 * Torpedo/Seeker Missile/Mine). Damage-halving for a laser fired at (or
	 * out of) a raised Masking Screen remains laser-only and independent of
	 * the to-hit resolution above.
	 *
	 * @return Structured attack outcome and damage/effect details
	 *
	 * @author Tom Stephens, gpt-5.3-codex (medium), Claude Sonnet 5 (medium)
	 * @date Created:  Mar 24, 2009
	 * @date Last Modified:  Jul 11, 2026
	 */
	FTacticalAttackResult fire();

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
