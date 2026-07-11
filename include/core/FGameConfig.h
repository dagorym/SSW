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
	 * This method returns a reference to the config object and lazily creates
	 * it if needed by delegating to create().
	 *
	 * @author Tom Stephens
	 * @date Created:  May 30, 2008
	 * @date Last Modified:  May 30, 2008
	 */
	static FGameConfig & getGameConfig();

	/**
	 * @brief Reset the singleton instance.
	 *
	 * Intended for tests only to ensure clean singleton state between runs.
	 */
	static void reset();

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

	/**
	 * @brief Resolve a repository-relative asset path to a usable file path.
	 *
	 * This keeps existing getBasePath() behavior intact while allowing callers
	 * to request paths such as data/zoom.png and icons/MaskingScreen.png in a
	 * way that works for binaries started from different output directories.
	 *
	 * @param assetPath repository-relative path (for example data/splash.png)
	 * @return resolved path candidate suitable for asset loading
	 */
	std::string resolveAssetPath(const std::string &assetPath) const;

	/**
	 * @brief Compute a safe, in-bounds index for null-terminating a readlink() buffer.
	 *
	 * `readlink()` returns the number of bytes written into the destination
	 * buffer on success, or a negative value on failure; it never guarantees
	 * the result is a valid index for null-terminating the buffer (a failure
	 * return of -1 cast to `size_t` becomes a huge value, and a result equal
	 * to the requested length may still need clamping). This helper
	 * centralizes that arithmetic so the constructor can never perform an
	 * out-of-bounds write, and so the clamping logic can be exercised by
	 * behavioral tests without spawning a real process.
	 *
	 * @param readlinkResult raw return value from readlink() (bytes written, or negative/zero on failure)
	 * @param bufferCapacity total capacity (in bytes) of the destination buffer
	 * @return an index in the range [0, bufferCapacity - 1] safe for `buf[index] = 0`;
	 *         returns 0 when bufferCapacity is 0 or readlinkResult indicates failure (<= 0)
	 *
	 * @author Claude Sonnet 5 (medium)
	 * @date Created: Jul 10, 2026
	 * @date Last Modified: Jul 11, 2026
	 */
	static size_t computeSafeTerminatorIndex(long readlinkResult, size_t bufferCapacity);

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
	 * On Linux, the executable's own path is resolved via
	 * `readlink("/proc/self/exe", ...)` into a fixed-size buffer, reserving
	 * room for the null terminator. The raw result is captured in a signed
	 * `ssize_t` and checked before the buffer is ever touched: a
	 * non-positive result (failure, or a degenerate zero-length result)
	 * skips the buffer write entirely and leaves `m_basePath` and
	 * `m_executablePath` as empty strings — a safe, defined state — rather
	 * than reading an undefined/garbage buffer. Only on a positive result is
	 * the null-terminator index computed via `computeSafeTerminatorIndex()`,
	 * which clamps it into range so a result at or above the buffer's
	 * capacity still cannot write out of bounds.
	 *
	 * @author Tom Stephens, Claude Sonnet 5 (medium)
	 * @date Created:  Jan 28, 2008
	 * @date Last Modified:  Jul 11, 2026
	 */
	FGameConfig();

	/// string to store games base directory in
	std::string m_basePath;

	/// string to store executable directory in
	std::string m_executablePath;



private:

};

};

#endif //_FGAMECONFIG_H_
