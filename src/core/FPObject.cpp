/**
 * @file FPObject.cpp
 * @brief Implementation file for FPObject class
 * @author Tom Stephens, Claude Sonnet 5 (medium)
 * @date Created:  Jan 12, 2005
 * @date Last Modified: Jul 17, 2026
 *
 */
#include "core/FPObject.h"
#include <vector>

namespace Frontier
{

FPObject::FPObject(){
}

FPObject::~FPObject(){
}

const int FPObject::writeU32(std::ostream &os, uint32_t v) const {
	unsigned char bytes[4];
	bytes[0] = static_cast<unsigned char>( v         & 0xFFu);
	bytes[1] = static_cast<unsigned char>((v >>  8u) & 0xFFu);
	bytes[2] = static_cast<unsigned char>((v >> 16u) & 0xFFu);
	bytes[3] = static_cast<unsigned char>((v >> 24u) & 0xFFu);
	os.write(reinterpret_cast<const char*>(bytes), sizeof(bytes));
	return os.fail() ? 1 : 0;
}

int FPObject::readU32(std::istream &is, uint32_t &v){
	unsigned char bytes[4];
	is.read(reinterpret_cast<char*>(bytes), sizeof(bytes));
	if (is.fail()){
		return 1;
	}
	v = static_cast<uint32_t>(bytes[0])
	  | (static_cast<uint32_t>(bytes[1]) <<  8u)
	  | (static_cast<uint32_t>(bytes[2]) << 16u)
	  | (static_cast<uint32_t>(bytes[3]) << 24u);
	return 0;
}

const int FPObject::writeString(std::ostream &os, const std::string &s) const {
	if (s.size() > kMaxSerializedStringBytes){
		// oversize guard: refuse to write a string longer than the cap
		return 1;
	}
	if (writeU32(os, static_cast<uint32_t>(s.size())) != 0){
		return 1;
	}
	if (!s.empty()){
		os.write(s.data(), static_cast<std::streamsize>(s.size()));
	}
	return os.fail() ? 1 : 0;
}

int FPObject::readString(std::istream &is, std::string &s){
	// get the size of the string via the fixed-width little-endian helper
	uint32_t size = 0;
	if (readU32(is, size) != 0){
		return 1;
	}
	// validate against the cap BEFORE allocating anything, so a corrupt or
	// adversarial length value cannot trigger an unbounded allocation
	if (size > kMaxSerializedStringBytes){
		return 1;
	}
	// now read in the string itself
	if (size > 0){ // but only if there is data to read
		std::vector< char > rawString(size);
		is.read(&rawString[0], static_cast<std::streamsize>(size));
		if (is.fail()){
			return 1;
		}
		s.assign(rawString.begin(), rawString.end());
	} else {
		s.clear();
	}
	return 0;
}

};
