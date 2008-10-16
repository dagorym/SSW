/**
 * @file FPObject.cpp
 * @brief Implementation file for FPObject class
 * @author Tom Stephens
 * @date Created:  Jan 12, 2005
 * 
 */
#include "FPObject.h"
#include <vector>

namespace Frontier
{

FPObject::FPObject(){
}

FPObject::~FPObject(){
}

const int FPObject::writeString(std::ostream &os, const std::string &s) const {
	
	write(os,s.size());
	os << s.c_str();
	return 0;
}

int FPObject::readString(std::istream &is, std::string &s){
	// get the size of the string
	unsigned int size;
	read(is,size);
	// now read in the string itself
	if (size >0 ){ // but only if there is data to read
		std::vector< char > rawString(size);
		is.read(&rawString[0], size);
		s = std::string(rawString.begin(), rawString.end());
	}
	return 0;
}

};
