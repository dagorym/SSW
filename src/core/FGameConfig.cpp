/**
 * @file FGameConfig.cpp
 * @brief Implementation file for FGameConfig class
 * @author Tom Stephens
 * @date Created:  Aug 2, 2009
 *
 */

#include "core/FGameConfig.h"

#include <iostream>
#ifdef LINUX
#include <unistd.h>
#else
#include <Windows.h>
#include <locale>
#include <codecvt>
#endif
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
	// read in the path.  They way to do this varies by OS.
	int bufsize = 1000;
#ifdef LINUX
	// setup the buffer
	char buf[1000];
	size_t size = readlink("/proc/self/exe", buf, bufsize);
	buf[size] = 0;  // add the null termination
	std::string path(buf);
	path = path.substr(0, path.find_last_of('/'));
	m_basePath = path.substr(0, (path.find_last_of('/')) + 1);
#else
	wchar_t buf[1000];
	DWORD size = GetModuleFileName(NULL, buf, bufsize);
	std::wstring wpath(buf);
	wpath = wpath.substr(0, wpath.find_last_of('/'));
	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;

	//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
	std::string path = converter.to_bytes(wpath);
	m_basePath = path.substr(0, (path.find_last_of('/')) + 1);
#endif
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
