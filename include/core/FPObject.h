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
	 * @brief method to write a string to the stream
	 * 
	 * This method writes a string to the stream so that it can be
	 * properly read in again later.  It stores the lenght of the
	 * string first and then the data.
	 * 
	 * @param os The output stream
	 * @param s A reference to the string to write
	 * 
	 * @author Tom Stephens
	 * @date Created:  Mar 06, 2008
	 * @date Last Modified:  Mar 06, 2008
	 */
	const int writeString(std::ostream &os, const std::string &s) const;

	/**
	 * @brief method to write data to the stream
	 * 
	 * This method writes the data to the stream in binary format
	 * 
	 * @param os The output stream
	 * @param d The data to write out
	 * 
	 * @author Tom Stephens
	 * @date Created:  Mar 06, 2008
	 * @date Last Modified:  Mar 06, 2008
	 */
	template<typename T> const int write(std::ostream &os, const T &d) const{
//		std::streampos pos = os.tellp();
		os.write((char*)&d,sizeof(T));
//		os.flush();
		return 0;
	}

	/**
	 * @brief method to read a string from the stream
	 * 
	 * This method properly reads a string from the stream so that 
	 * additional data after the string in the stream is not read 
	 * into the string
	 * 
	 * @param is The input stream
	 * @param s A reference to the string to read
	 * 
	 * @author Tom Stephens
	 * @date Created:  Mar 06, 2008
	 * @date Last Modified:  Mar 06, 2008
	 */
	int readString(std::istream &is, std::string &s);

	/**
	 * @brief method to read data from the stream
	 * 
	 * This method reads the data from the stream in binary format
	 * 
	 * @param is The input stream
	 * @param d The data to read out
	 * 
	 * @author Tom Stephens
	 * @date Created:  Mar 06, 2008
	 * @date Last Modified:  Mar 06, 2008
	 */
	template<typename T> int read(std::istream &is, T &d){
//		std::streampos pos = is.tellg();
		is.read((char*)&d,sizeof(T));
		return 0;
	}

};

};

#endif //_FPOBJECT_H_
