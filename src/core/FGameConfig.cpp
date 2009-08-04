/**
 * @file FGameConfig.cpp
 * @brief Implementation file for FGameConfig class
 * @author Tom Stephens
 * @date Created:  Aug 2, 2009
 *
 */

#include "core/FGameConfig.h"

#include <iostream>
#include <unistd.h>

namespace Frontier
{

FGameConfig * FGameConfig::m_config = NULL;

FGameConfig & FGameConfig::create(){
	if (!m_config) {
		m_config = new FGameConfig();
	}
	return *m_config;
}

FGameConfig & FGameConfig::getGameConfig(){
	return *m_config;
}

FGameConfig::FGameConfig(){
	// determine the base path to the game's directory structure
	// setup the buffer
	int bufsize = 1000;
	char buf[1000];
	// read in the path.  They way to do this varies by OS.
#ifdef LINUX
	ssize_t size = readlink("/proc/self/exe", buf, bufsize);
#else
	GetModuleFileName(NULL, buf, bufsize)
#endif
	buf[size]=0;  // add the null termination
	std::string path(buf);
	path = path.substr(0,path.find_last_of('/'));
	m_basePath = path.substr(0,(path.find_last_of('/'))+1);
//	std::cerr << "The path returned was " << m_basePath << std::endl;

}

FGameConfig::~FGameConfig(){
	m_config = NULL;
}

const int FGameConfig::save(std::ostream &os) const{
	return 0;
}

int FGameConfig::load(std::istream &is){
	return 0;
}

};
