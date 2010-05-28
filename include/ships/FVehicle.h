/**
 * @file FVehicle.h
 * @brief Header file for FVehicle class
 * @author Tom Stephens
 * @date Created:  Jan 12, 2005
 *
 */
#ifndef _FVEHICLE_H_
#define _FVEHICLE_H_

#include "core/FPObject.h"
#include "core/FGameConfig.h"
#include "weapons/FWeapon.h"
#include "defenses/FDefense.h"
#include "wx/wx.h"
#include <vector>

namespace Frontier
{
/**
 * @brief Base class for all vehicle objects
 *
 * This class forms the base class for all the vehicle objects used in
 * the game such as space ships and stations.
 *
 * @author Tom Stephens
 * @date Created:  Jan 12, 2005
 * @date Last Modified:  Apr 22, 2010
 */
class FVehicle : public Frontier::FPObject
{
public:
	/// Default constructor
	FVehicle();
	/// Default destructor
	virtual ~FVehicle();
	/// returns the vehicle's undamaged MR
	const unsigned int & getMaxMR() const { return m_maxMR; }
	/// return the vehicle's undamaged ADF
	const int & getMaxADF() const { return m_maxADF; }
	/// return the vehicle's undamaged HP
	const int & getMaxHP() const { return m_maxHP; }
	/// returns the vehicle's current MR
	const unsigned int & getMR() const { return m_currentMR; }
	/// return the vehicle's current ADF
	const int & getADF() const { return m_currentADF; }
	/// return the vehicle's current HP
	const int & getHP() const { return m_currentHP; }
	/// set the vehicle's current MR
	void setMR(unsigned int mr);
	/// set the vehicle's current ADF
	void setADF(int adf);
	/// set the vehicle's current HP
	void setHP(int hp);
	/// get the vehicle's ID
	const unsigned int & getID() const { return m_ID; }
	/// set the vehicle's name
	void setName(std::string n) { m_name = n; }
	/// get the vehicle's name
	const std::string & getName() { return m_name; }
	/// get the vehicle's icon
	const wxImage * getIcon() const { return m_icon; }
	/// set the vehicles icon
	void setIcon(std::string icon);
	/// get the vehicle's type
	const std::string & getType() { return m_type; }
	/// set the vehicle's speed
	void setSpeed(int speed) { m_speed = speed; }
	/// get the vehicle's speed
	const int & getSpeed() const { return m_speed; }
	/// set the vehicle's heading
	void setHeading(int heading) { m_heading = heading; }
	/// get the vehicle's heading
	const int & getHeading() const { return m_heading; }
	/// set the ships owner
	void setOwner(unsigned int owner) { m_owner = owner; }
	/// get the ship's owner
	const unsigned int & getOwner() const { return m_owner; }
	/// get the ship's max DCR
	const unsigned int & getMaxDCR() const { return m_maxDCR; }
	/// get the ship's current DCR
	const unsigned int & getDCR() const { return m_currentDCR; }
	/// set the ship's current DCR
	void setDCR(unsigned int dcr);
	/// get count of ship's weapons
	unsigned int getWeaponCount() { return m_weapons.size(); }
	/// get pointer to specified weapon;
	FWeapon * getWeapon(unsigned int i) { if (i<m_weapons.size()) { return m_weapons[i]; } else { return NULL; } }
	/// get pointer to specified defense;
	FDefense * getDefense(unsigned int i) { if (i<m_defenses.size()) { return m_defenses[i]; } else { return NULL; } }
	/// get pointer to current defense
	FDefense * getCurrentDefense() { return m_currentDefense; }
	/// get count of ship's defenses
	unsigned int getDefenseCount() { return m_defenses.size(); }
	/// get the number of turns left until the masking screen runs out
	int getMSTurnCount(){return m_maskingScreenTurnCount;}


	/**
	 * @brief Method to save the vehicle data
	 *
	 * This method implements the FPObject base class virtual write method to
	 * save all the vehicle's data
	 *
	 * @param os The output stream to write to
	 *
	 * @author Tom Stephens
	 * @date Created:  Mar 06, 2008
	 * @date Last Modified:  Mar 06, 2008
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
	 * @date Created:  Mar 06, 2008
	 * @date Last Modified:  Mar 06, 2008
	 */
	virtual int load(std::istream &is);

	/**
	 * @brief generates a string listing all weapons
	 *
	 * This method cycles through the list of weapons and creates a string showing
	 * all the weapons and their current ammo where appropriate.  It is returned
	 * as a std::string
	 *
	 * @author Tom Stephens
	 * @date Created:  Mar 13, 2009
	 * @date Last Modified: Mar 13, 2009
	 */
	std::string getWeaponString();

	/**
	 * @brief generates a string listing all defenses
	 *
	 * This method cycles through the list of defenses and creates a string showing
	 * all the defenses and their current ammo where appropriate.  It is returned
	 * as a std::string
	 *
	 * @author Tom Stephens
	 * @date Created:  Mar 13, 2009
	 * @date Last Modified: Mar 13, 2009
	 */
	std::string getDefenseString();

	/**
	 * @brief Take damage from a successful hit
	 *
	 * This method applies the damage done by the opponent.  Right now
	 * it just applies the passed in damage to the hull points.
	 *
	 * @todo Implement the full advanced damage table.  Subclasses should
	 * re-implement this virtual function to be specific for the specific
	 * vessel.
	 *
	 * @param damage The amount of hull damage sustained
	 *
	 * @author Tom Stephens
	 * @date Created:  Apr 28, 2009
	 * @date Last Modified:  Apr 28, 2009
	 */
	virtual void takeDamage (int damage);

	/**
	 * @brief Reloads all the ships weapons
	 *
	 * This method loops through all the ships weapons and reloads the ones
	 * with limited ammo.
	 *
	 * @author Tom Stephens
	 * @date Created:  Jun 03, 2009
	 * @date Last Modified:  Jun 03, 2009
	 */
	void reload();

	/*
	 * @ brief sets the pointer to the currently active defense.
	 *
	 * This method sets the current defense to the one specified
	 * and handles any book keeping associated with changing the
	 * defense type such as setting the masking screen flag and
	 * reducing the number of masking screen charges
	 *
	 * @param i the index of the defense to set as current
	 *
	 * @author Tom Stephens
	 * @date Created:  Mar 13, 2010
	 * @date Last Modified:  Apr 1, 2010
	 */
	void setCurrentDefense(unsigned int i);


	/**
	 * Decrease the turns for the masking screen and remove it if zero
	 *
	 * This method decreases the number of turns left for a masking
	 * screen for a station and then removes the masking screen if
	 * the count goes to zero.
	 *
	 * @author Tom Stephens
	 * @date Created:  Apr 08, 2010
	 * @date Last Modified:  Apr 08, 2010
	 */
	void decrementMSTurnCount();

	/**
	 * @brief check to see if ship has specified defense
	 *
	 * This method searches through the ship's defenses to see if
	 * the specified defense is one of them.  If not it returns 0.
	 * Otherwise it returns the index of the defense in the ship's
	 * defense list
	 *
	 * @param d defense type
	 *
	 * @author Tom Stephens
	 * @date Created:  Apr 22, 2010
	 * @date Last Modified:  Apr 22, 2010
	 */
	unsigned int hasDefense(FDefense::Defense d);

protected:
	/// counter for total number of vehicles and next ship ID
	static unsigned int m_nextID;
	/// vehicle ID number
	unsigned int m_ID;
	/// vehicle's MR
	unsigned int m_maxMR;
	/// vehicle's ADF
	int m_maxADF;
	/// vehicle's Hull points
	int m_maxHP;
	/// vehicle's MR
	unsigned int m_currentMR;
	/// vehicle's ADF
	int m_currentADF;
	/// vehicle's Hull points
	int m_currentHP;
	/// pointer to the vehicle's icon
	wxImage * m_icon;
	/// icon file name
	std::string m_iconName;
	/// Vehicle's name
	std::string m_name;
	/// Vehicle Type
	std::string m_type;
	/// counter for number of instances of this class
	static unsigned int m_classCount;
	/// the ship's speed on the hex map
	int m_speed;
	/// the ship's hex facing on the map 0=west increasing counterclockwise from there
	int m_heading;
	/// ID of the player owning the ship
	unsigned int m_owner;
	/// The DCR of an undamaged ship
	unsigned int m_maxDCR;
	/// The current DCR of the vessel;
	unsigned int m_currentDCR;
	/// list of ship's weapons
	WeaponList m_weapons;
	/// List of ship's defenses
	DefenseList m_defenses;
	/// pointer to currently active defense
	FDefense * m_currentDefense;
	/// turns until MS runs out
	int m_maskingScreenTurnCount;
};

typedef std::vector<FVehicle *> VehicleList;

/// This factory method takes the ship type name and returns a pointer to an
/// object of that type
FVehicle * createShip(std::string type);

/**
 * @brief structure to hold ICM targeting information
 *
 * @author Tom Stephens
 * @date Created:  Apr 22, 2010
 * @date Last Modified:  Apr 22, 2010
 */
typedef struct {
	/// Weapon to fire ICM's against
	FWeapon * weapon;
	/// Ships in target hex
	VehicleList * vehicles;
} ICMData;


};

#endif //_FVEHICLE_H_
