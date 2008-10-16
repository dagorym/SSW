/**
 * @file FVehicle.h
 * @brief Header file for FVehicle class
 * @author Tom Stephens
 * @date Created:  Jan 12, 2005
 *
 */
#ifndef _FVEHICLE_H_
#define _FVEHICLE_H_

#include "FPObject.h"
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
 * @date Last Modified:  Mar 06, 2008
 */
class FVehicle : public Frontier::FPObject
{
public:
	/// Default constructor
	FVehicle();
	/// Default destructor
	virtual ~FVehicle();
	/// returns the vehicle's undamaged MR
	const int & getMaxMR() const { return m_maxMR; }
	/// return the vehicle's undamaged ADF
	const int & getMaxADF() const { return m_maxADF; }
	/// return the vehicle's undamaged HP
	const int & getMaxHP() const { return m_maxHP; }
	/// returns the vehicle's current MR
	const int & getMR() const { return m_currentMR; }
	/// return the vehicle's current ADF
	const int & getADF() const { return m_currentADF; }
	/// return the vehicle's current HP
	const int & getHP() const { return m_currentHP; }
	/// set the vehicle's current MR
	void setMR(int mr);
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
	 * @aate Last Modified:  Mar 06, 2008
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

protected:
	/// counter for total number of vehicles and next ship ID
	static unsigned int m_nextID;
	/// vehicle ID number
	unsigned int m_ID;
	/// vehicle's MR
	int m_maxMR;
	/// vehicle's ADF
	int m_maxADF;
	/// vehicle's Hull points
	int m_maxHP;
	/// vehicle's MR
	int m_currentMR;
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
};

typedef std::vector<FVehicle *> VehicleList;

};

#endif //_FVEHICLE_H_
