/**
 * @file FPObject.h
 * @brief Header file for FPObject class
 * @author Tom Stephens
 * @date Created:  Jan 12, 2005
 * 
 */

#ifndef _FPOBJECT_H_
#define _FPOBJECT_H_

#include "FObject.h"
#include "core/FSaveFormat.h"
#include <cstdint>
#include <fstream>

namespace Frontier
{

/**
 * @brief Persistant Object class
 * 
 * This class forms the base class for all objects that require persistance
 * between session, ie need to be saved to disk or which need to be
 * transmitted to another process.  It proveds the base classes for reading
 * and writing the data to a serial stream.
 * 
 * @author Tom Stephens
 * @date Created:  Jan 12, 2005
 * @date Last Modified:  Jan 12, 2005
 */
class FPObject : public Frontier::FObject {
public:
  /// Default constructor
	FPObject();
  /// Default destructor
	virtual ~FPObject();

  /**
   * @brief Method to save data contents
   * 
   * This method serializes the class data and writes it to the designated
   * output stream.  The method returns 0 if everything is okay and a positive
   * integer error code if there is a failure.
   * 
   * @author Tom Stephens
   * @date Created:  Jan 12, 2005
   * @date Last Modified:  Jan 12, 2005
   */
	const virtual int save(std::ostream &os) const = 0;
  
  /**
   * @brief Method to read data contents
   * 
   * This method is the inverse of the save method.  It reads the data for
   * the class from the designated input stream.  This method returns 0 if
   * everything is okay and a positive integer error code if there is a
   * failure
   * 
   * @author Tom Stephens
   * @date Created:  Jan 12, 2005
   * @date Last Modified:  Jan 12, 2005
   */
	virtual int load(std::istream &is) = 0;
   
protected:
	/**
	 * @brief method to write a fixed-width 32-bit value to the stream
	 *
	 * Emits @c v as exactly 4 bytes in explicit little-endian byte order
	 * (least-significant byte first), built via byte shift/mask rather than
	 * a raw native-order struct copy, so counts and IDs written by this
	 * helper are portable across platforms regardless of host endianness or
	 * native integer width. This is the wire-format primitive that
	 * @c writeString and every hardened @c save() count/ID field builds on.
	 *
	 * @param os The output stream
	 * @param v The 32-bit value to write
	 * @return 0 on success, nonzero if the stream is in a failed state after
	 *         the write
	 *
	 * @author Claude Sonnet 5 (medium)
	 * @date Created: Jul 17, 2026
	 */
	const int writeU32(std::ostream &os, uint32_t v) const;

	/**
	 * @brief method to read a fixed-width 32-bit value from the stream
	 *
	 * Inverse of @c writeU32: consumes exactly 4 bytes and reassembles them
	 * as an explicit little-endian value (byte shift/mask), independent of
	 * host endianness or native integer width.
	 *
	 * @param is The input stream
	 * @param v Reference to receive the decoded 32-bit value; unspecified
	 *          on a nonzero (failure) return
	 * @return 0 on success, nonzero if the stream is in a failed state after
	 *         the read
	 *
	 * @author Claude Sonnet 5 (medium)
	 * @date Created: Jul 17, 2026
	 */
	int readU32(std::istream &is, uint32_t &v);

	/**
	 * @brief method to write a string to the stream
	 *
	 * This method writes a string to the stream so that it can be
	 * properly read in again later.  It stores the length first, via the
	 * fixed-width little-endian @c writeU32 helper, and then writes the raw
	 * string bytes with @c ostream::write(s.data(), s.size()) so embedded
	 * NUL bytes are preserved instead of truncating the string at the first
	 * NUL (as the previous @c c_str()-based implementation did). Strings
	 * longer than @c kMaxSerializedStringBytes are rejected rather than
	 * written.
	 *
	 * @param os The output stream
	 * @param s A reference to the string to write
	 * @return 0 on success, nonzero if @c s exceeds
	 *         @c kMaxSerializedStringBytes or the stream is in a failed
	 *         state after the write
	 *
	 * @author Tom Stephens, Claude Sonnet 5 (medium)
	 * @date Created:  Mar 06, 2008
	 * @date Last Modified: Jul 17, 2026
	 */
	const int writeString(std::ostream &os, const std::string &s) const;

	/**
	 * @brief method to write data to the stream
	 *
	 * This method writes the data to the stream in binary format, in the
	 * type's native representation (unchanged for single-byte and other
	 * scalar fields; use @c writeU32 instead for portable fixed-width
	 * little-endian counts/IDs). The stream state is checked after the
	 * write and a nonzero code is returned on failure.
	 *
	 * @param os The output stream
	 * @param d The data to write out
	 * @return 0 on success, nonzero if the stream is in a failed state
	 *         after the write
	 *
	 * @author Tom Stephens, Claude Sonnet 5 (medium)
	 * @date Created:  Mar 06, 2008
	 * @date Last Modified: Jul 17, 2026
	 */
	template<typename T> int write(std::ostream &os, const T &d) const{
//		std::streampos pos = os.tellp();
		os.write((char*)&d,sizeof(T));
//		os.flush();
		return os.fail() ? 1 : 0;
	}

	/**
	 * @brief method to read a string from the stream
	 *
	 * This method properly reads a string from the stream so that
	 * additional data after the string in the stream is not read
	 * into the string. The length is read via the fixed-width
	 * little-endian @c readU32 helper and validated against the
	 * @c kMaxSerializedStringBytes cap *before* any buffer is allocated, so
	 * a corrupt or adversarial length value cannot trigger an unbounded
	 * allocation; a stream failure or cap violation returns nonzero and
	 * leaves @c s unspecified without allocating on the bad path.
	 *
	 * @param is The input stream
	 * @param s A reference to the string to read
	 * @return 0 on success, nonzero if the encoded length exceeds
	 *         @c kMaxSerializedStringBytes or the stream is in a failed
	 *         state
	 *
	 * @author Tom Stephens, Claude Sonnet 5 (medium)
	 * @date Created:  Mar 06, 2008
	 * @date Last Modified: Jul 17, 2026
	 */
	int readString(std::istream &is, std::string &s);

	/**
	 * @brief method to read data from the stream
	 *
	 * This method reads the data from the stream in binary format, in the
	 * type's native representation (unchanged for single-byte and other
	 * scalar fields; use @c readU32 instead for portable fixed-width
	 * little-endian counts/IDs). The stream state is checked after the
	 * read and a nonzero code is returned on failure.
	 *
	 * @param is The input stream
	 * @param d The data to read out
	 * @return 0 on success, nonzero if the stream is in a failed state
	 *         after the read
	 *
	 * @author Tom Stephens, Claude Sonnet 5 (medium)
	 * @date Created:  Mar 06, 2008
	 * @date Last Modified: Jul 17, 2026
	 */
	template<typename T> int read(std::istream &is, T &d){
//		std::streampos pos = is.tellg();
		is.read((char*)&d,sizeof(T));
		return is.fail() ? 1 : 0;
	}

};

};

#endif //_FPOBJECT_H_
