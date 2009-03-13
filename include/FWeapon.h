/**
 * @file FWeapon.h
 * @brief Header file for FWeapon class
 * @date Created: Feb 27, 2009
 * @author Tom Stephens
 */

#ifndef FWEAPON_H_
#define FWEAPON_H_

#include "FPObject.h"
#include "FVehicle.h"
#include <string>
#include <vector>

namespace Frontier
{
/// types of weapons
enum {NONE,LB,LC,DC,EB,PB,RB,AR,T,SM,M};

/**
 * @brief Base class for all weapon objects
 *
 * This class forms the base class for all the weapon objects used in
 * the game.
 *
 * @author Tom Stephens
 * @date Created:  Feb 27, 2009
 * @date Last Modified:  Feb 27, 2009
 */
class FWeapon: public Frontier::FPObject {
public:
	/// default constructor
	FWeapon();
	/// default destructor;
	virtual ~FWeapon();

	/// return the weapon type
	const unsigned int & getType() const { return m_type;}
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
	const unsigned int & getICMMod() const { return m_ICMMod; }
	/// gets a text string with the abbreviated name of the weapon
	const std::string & getName() const { return m_name; }
    /// gets a text string with the full name of the weapon
	const std::string & getLongName() const { return m_fullName; }

	/// set the maximum ammunition value
	void setMaxAmmo(int m) { m_maxAmmo = m; }
	/// set the current ammunition value
	void setCurrentAmmo(int c) { m_currentAmmo = c; }

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
	 * @aate Last Modified:  Mar 12, 2009
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
	 * This method fires the weapon at the specified target.  It starts by
	 * querying the target ship object to get a list of active defenses.
	 *
	 * @param v The FVehicle object to shoot at
	 *
	 * @author Tom Stephens
	 * @date Created:  Feb 27, 2009
	 * @date Last Modified:  Feb 27, 2009
	 */
	void fire(FVehicle * v);

protected:
	/// weapon's abbreviated name
	std::string m_name;
	/// full name of weapon
	std::string m_fullName;
	/// weapon type
	unsigned int m_type;
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
	unsigned int m_ICMMod;
	/// damage table modifier
	int m_damageTableMod;
};

typedef std::vector<FWeapon *> WeaponList;

};
#endif /* FWEAPON_H_ */
