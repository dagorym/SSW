/**
 * @file FJumpRoute.h
 * @brief Header file for FJumpRoute class
 * @author Tom Stephens, Claude Sonnet 5 (medium)
 * @date Created:  Jan 20, 2005
 * @date Last Modified: Jul 17, 2026
 *
 */

#ifndef _FJUMPROUTE_H_
#define _FJUMPROUTE_H_

#include "core/FPObject.h"
#include "FSystem.h"
#include <vector>

namespace Frontier
{

/**
 * @brief Class to hold jump route information
 *
 * This class holds the information about a jump route.  All jumps are one
 * way and have a distinct start and end system.
 *
 * @author Tom Stephens
 * @date Created:  Jan 20, 2005
 * @date Last Modified:  Feb 19, 2008
 */
class FJumpRoute : public Frontier::FPObject
{
public:
  /// Default constructor
	FJumpRoute();
  /// Default destructor
	virtual ~FJumpRoute();

  /**
   * @brief Alternate constructor
   *
   * This constructor takes as input pointers to the start and end systems
   * and a vector of ID values of the players that know the route.  These
   * values are stored in the appropriate member variables and then the
   * length of the jump is computed.
   *
   * @param start Pointer to the sytem at the start of the jump route.
   * @param end Pointer to the sytem at the end of the jump route.
   * @param players vector containing player ID's for players who know the route.
   *
   * @author Tom Stephens
   * @date Created:  Jan 20, 2005
   * @date Last Modified:  Jan 20, 2005
   */
  FJumpRoute(FSystem * start, FSystem * end, std::vector<unsigned int> players);

  /**
   * @brief Get the jump's start point
   *
   * This method returns a pointer to the FSystem object at the start of the
   * jump route.
   *
   * @author Tom Stephens
   * @date Created:  Jan 20, 2005
   * @date Last Modified:  Jan 20, 2005
   */
  const FSystem * getStart() const { return m_start; }

  /**
   * @brief Set the jump route start point
   *
   * This method takes as input a pointer to the FSystem object at the start
   * of the jump route.  The pointer is stored in the m_start member variable
   *
   * @param start Pointer to the sytem at the start of the jump route.
   *
   * @author Tom Stephens
   * @date Created:  Jan 20, 2005
   * @date Last Modified:  Jan 20, 2005
   */
  void setStart(FSystem * start) { m_start = start; }

  /**
   * @brief Get the jump's end point
   *
   * This method returns a pointer to the FSystem object at the end of the
   * jump route.
   *
   * @author Tom Stephens
   * @date Created:  Jan 20, 2005
   * @date Last Modified:  Jan 20, 2005
   */
  const FSystem * getEnd() const { return m_end; }

  /**
   * @brief Set the jump route end point
   *
   * This method takes as input a pointer to the FSystem object at the end
   * of the jump route.  The pointer is stored in the m_end member variable
   *
   * @param end Pointer to the sytem at the end of the jump route.
   *
   * @author Tom Stephens
   * @date Created:  Jan 20, 2005
   * @date Last Modified:  Jan 20, 2005
   */
  void setEnd(FSystem * end) { m_end = end; }

  /**
   * @brief Get the start system's ID as read from the stream by load()
   *
   * This method returns the raw start-system ID that @c load() decoded
   * from the stream as a real fixed-width field (not a pointer-smuggled
   * value). @c FMap::load uses this ID with @c getSystem(id) to resolve
   * the actual @c FSystem pointer and calls @c setStart() with the
   * result; the value is meaningless before @c load() has been called and
   * is not touched by the pointer-based alternate constructor.
   *
   * @author Claude Sonnet 5 (medium)
   * @date Created: Jul 17, 2026
   */
  const unsigned int & getStartSystemID() const { return m_startSystemID; }

  /**
   * @brief Get the end system's ID as read from the stream by load()
   *
   * This method returns the raw end-system ID that @c load() decoded
   * from the stream as a real fixed-width field (not a pointer-smuggled
   * value). @c FMap::load uses this ID with @c getSystem(id) to resolve
   * the actual @c FSystem pointer and calls @c setEnd() with the result;
   * the value is meaningless before @c load() has been called and is not
   * touched by the pointer-based alternate constructor.
   *
   * @author Claude Sonnet 5 (medium)
   * @date Created: Jul 17, 2026
   */
  const unsigned int & getEndSystemID() const { return m_endSystemID; }

  /**
   * @brief Get the jump route's ID number
   *
   * This method returns the ID value for the jump route
   *
   * @author Tom Stephens
   * @date Created:  Jan 20, 2005
   * @date Last Modified:  Jan 20, 2005
   */
  const unsigned int & getID() const { return m_ID; }

  /**
   * @brief Get the jump length
   *
   * This method returns the length of the jump
   *
   * @author Tom Stephens
   * @date Created:  Jan 20, 2005
   * @date Last Modified:  Jan 20, 2005
   */
  const float & getLength() { return m_length; }

  /**
   * @brief Determine whether a player has explored the jump or not
   *
   * This method takes as input the ID of a player and searches to see
   * if that player is in the list of players having explored the route.
   * If they are it returns true, otherwise it returns false.
   *
   * @param id The ID of the player to check
   *
   * @author Tom Stephens
   * @date Created:  Jan 20, 2005
   * @date Last Modified:  Jan 20, 2005
   */
  const bool isKnown(unsigned int id) const;

  /**
   * @brief returns the time in days to complete the jump
   *
   * This method takes as input the jump speed and return the length of
   * time needed to complete the jump at that speed.
   *
   * @param s The jump speed
   * @param  l length of jump to calculate for.  If 0 just the jump's acutual length is used
   *
   * @author Tom Stephens
   * @date Created:  Feb 19, 2008
   * @date Last Modified:  Mar 14, 2008
   */
  int getJumpTime(int s, int l=0);

  /**
   * @brief Method to save the jump route data
   *
   * This method implements the FPObject base class virtual write method to
   * save all the jump route's data. The route ID, the start/end system
   * IDs, the player-ID-list count, and each player ID are written as real
   * fixed-width little-endian fields via @c writeU32 (there is no more
   * pointer-value smuggling of the start/end system IDs into a native-word
   * field), so the wire format is portable across host word size and
   * endianness and IDs above 65535 round-trip correctly.
   *
   * @param os The output stream to write to
   *
   * @author Tom Stephens, Claude Sonnet 5 (medium)
   * @date Created:  Mar 05, 2008
   * @date Last Modified: Jul 17, 2026
   */
  const virtual int save(std::ostream &os) const;

	/**
	 * @brief Method to read data contents
	 *
	 * This method is the inverse of the save method.  It reads the data for
	 * the class from the designated input stream, using the fixed-width
	 * little-endian @c readU32 helper for the ID, start/end system ID, and
	 * player-list fields. The start/end system IDs are stored in
	 * m_startSystemID/m_endSystemID (see getStartSystemID()/
	 * getEndSystemID()) rather than smuggled into the m_start/m_end
	 * pointers; m_start and m_end are left NULL by this method and are
	 * resolved to real FSystem pointers only by FMap::load, which looks
	 * them up via getSystem(id) and calls setStart()/setEnd(). This method
	 * returns 0 if everything is okay and a positive integer error code if
	 * there is a failure.
	 *
	 * After the ID is read, m_nextID is advanced past it (H3 non-colliding
	 * guard, respecting FJumpRoute's post-increment allocation convention
	 * m_ID = m_nextID++) so a subsequently constructed FJumpRoute always
	 * receives an ID strictly greater than every ID loaded so far.
	 *
	 * @author Tom Stephens, Claude Sonnet 5 (medium)
	 * @date Created:  Mar 07, 2008
	 * @date Last Modified: Jul 17, 2026
	 */
	virtual int load(std::istream &is);

private:
  /// Pointer to system at start of jump
  FSystem * m_start;
  /// Pointer to system at end of jump
  FSystem * m_end;
  /// Start system ID as read from the stream by load(); resolved to
  /// m_start by FMap::load via getSystem(id)/setStart(). See
  /// getStartSystemID().
  unsigned int m_startSystemID;
  /// End system ID as read from the stream by load(); resolved to m_end
  /// by FMap::load via getSystem(id)/setEnd(). See getEndSystemID().
  unsigned int m_endSystemID;
  /// ID of jump route
  unsigned int m_ID;
  /// Length of jump in days.
  float m_length;
  /// List of player ID's that know about the route.
  std::vector<unsigned int> m_players;
  /// counter for ID and number of jumps
  static unsigned int m_nextID;
  /// counter for number of instances of this class
  static unsigned int m_classCount;

  /**
   * @brief Returns the time to make the jump at Risk Jump 2 speed
   *
   * This method determines the time it takes to make the jump when using
   * risk jump 2 speeds.  It recreates the times based on the FDM.  However,
   * it uses the times based on the real distance instead of the distances on
   * the FDM which are typically 1 ly longer than reality but in a couple of
   * case are the same or smaller than the real values.
   *
   * @param  l length of jump to calculate for.  If 0 just the jump's acutual length is used
   *
   * @author Tom Stephens
   * @date Created:  Feb 15, 2008
   * @date Last Modified:  Mar 14, 2008
   */
  int getRJ2Time(int l=0);

  /**
   * @brief Returns the time to make the jump at Risk Jump 3 speed
   *
   * This method determines the time it takes to make the jump when using
   * risk jump 3 speeds.  It recreates the times based on the FDM.  However,
   * it uses the times based on the real distance instead of the distances on
   * the FDM which are typically 1 ly longer than reality but in a couple of
   * case are the same or smaller than the real values.
   *
   * @param  l length of jump to calculate for.  If 0 just the jump's acutual length is used
   *
   * @author Tom Stephens
   * @date Created:  Feb 15, 2008
   * @date Last Modified:  Mar 14, 2008
   */
  int getRJ3Time(int l=0);
};

/// define the JumpRouteList type to be a vector of FJumpRoute pointers
typedef std::vector<FJumpRoute *> JumpRouteList;
};

#endif //_FJUMPROUTE_H_
