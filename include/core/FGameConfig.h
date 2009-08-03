/**
 * @file FGameConfig.h
 * @brief Header file for FGameConfig class
 * @author Tom Stephens
 * @date Created:  Jan 15, 2005
 *
 */

#ifndef _FGAMECONFIG_H_
#define _FGAMECONFIG_H_

#include "core/FPObject.h"
#include <string>
#include <vector>

namespace Frontier
{

/**
 * @brief Class to hold game config information
 *
 * This class holds the information about a the game config and provides
 * methods to get information on the configuration
 *
 * @author Tom Stephens
 * @date Created:  Aug 2, 2009
 * @date Last Modified:  Aug 2, 2009
 */
class FGameConfig : public Frontier::FPObject {
public:
	/// Default destructor
	virtual ~FGameConfig();

	/**
	 * @brief Method to actually create the GameConfig
	 *
	 * This method creates the config object the first time it is called
	 * Subsequent calls do nothing but return a reference to the FGameConfig
	 * object.
	 *
	 * @author Tom Stephens
	 * @date Created:  May 30, 2008
	 * @date Last Modified:  May 30, 2008
	 */
	static FGameConfig & create();

	/**
	 * @brief Get a reference to the config object
	 *
	 * This method just returns a reference to the config object if it
	 * exists if it doesn't exist, it returns a null reference.
	 *
	 * @author Tom Stephens
	 * @date Created:  May 30, 2008
	 * @date Last Modified:  May 30, 2008
	 */
	static FGameConfig & getGameConfig();

	/**
	 * @brief Method to save the config data
	 *
	 * This method implements the FPObject base class virtual write method to
	 * save all the config's data
	 *
	 * @param os The output stream to write to
	 *
	 * @author Tom Stephens
	 * @date Created:  Mar 05, 2008
	 * @date Last Modified:  Mar 05, 2008
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
	 * @date Created:  Mar 07, 2008
	 * @date Last Modified:  Mar 07, 2008
	 */
	virtual int load(std::istream &is);

	/// returns the base path for the game installation
	std::string getBasePath() { return m_basePath; }

private:
	/// Static instance of the FGameConfig class
	static FGameConfig * m_config;
	/// Private copy construtor to prevent copies
	FGameConfig(const FGameConfig&);
	/// Private assignment operator to prevent copies
	FGameConfig& operator=(FGameConfig&);

protected:
	/**
	 * @brief Default constructor
	 *
	 * This initalized the new config.  If the random variable set to true,
	 * nothing currently happens other than creating an empty config of
	 * size 0.  However, if set to false the Frontier systems are
	 * set up as described in the Alpha Dawn rules.
	 *
	 * @author Tom Stephens
	 * @date Created:  Jan 28, 2008
	 * @date Last Modified:  Feb 19, 2008
	 */
	FGameConfig();

	/// string to store games base directory in
	std::string m_basePath;



private:

};

};

#endif //_FGAMECONFIG_H_
