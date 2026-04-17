/**
 * @file FGameConfig.cpp
 * @brief Implementation file for FGameConfig class
 * @author Tom Stephens
 * @date Created:  Aug 2, 2009
 *
 */

#include "core/FGameConfig.h"

#include <fstream>
#include <iostream>
#include <string>
#ifdef LINUX
#include <unistd.h>
#else
#include <Windows.h>
#include <locale>
#include <codecvt>
#endif
namespace Frontier
{
namespace
{
const char kPathSeparator = '/';

bool hasDrivePrefix(const std::string &path)
{
	return path.size() > 1 && path[1] == ':';
}

bool isAbsolutePath(const std::string &path)
{
	if (path.empty()) {
		return false;
	}
	return path[0] == '/' || path[0] == '\\' || hasDrivePrefix(path);
}

bool pathExists(const std::string &path)
{
	std::ifstream input(path.c_str(), std::ios::binary);
	return input.good();
}

std::string normalizeAssetPath(const std::string &assetPath)
{
	std::string normalized = assetPath;
	while (normalized.size() >= 2 && normalized[0] == '.' &&
			(normalized[1] == '/' || normalized[1] == '\\')) {
		normalized = normalized.substr(2);
	}
	while (!normalized.empty() &&
			(normalized[0] == '/' || normalized[0] == '\\')) {
		normalized = normalized.substr(1);
	}
	return normalized;
}

std::string ensureTrailingSeparator(const std::string &path)
{
	if (path.empty()) {
		return path;
	}
	const char tail = path[path.size() - 1];
	if (tail == '/' || tail == '\\') {
		return path;
	}
	return path + kPathSeparator;
}

std::string joinPath(const std::string &basePath, const std::string &assetPath)
{
	return ensureTrailingSeparator(basePath) + assetPath;
}
}


FGameConfig * FGameConfig::m_config = NULL;

FGameConfig & FGameConfig::create(){
	if (!m_config) {
		m_config = new FGameConfig();
	}
	return *m_config;
}

FGameConfig & FGameConfig::getGameConfig(){
	return create();
}

void FGameConfig::reset(){
	if (m_config) {
		FGameConfig * oldConfig = m_config;
		m_config = NULL;
		delete oldConfig;
	}
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
	m_executablePath = ensureTrailingSeparator(path);
	m_basePath = path.substr(0, (path.find_last_of('/')) + 1);
#else
	wchar_t buf[1000];
	DWORD size = GetModuleFileName(NULL, buf, bufsize);
	std::wstring wpath(buf);
	wpath = wpath.substr(0, wpath.find_last_of('\\'));
	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;

	//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
	std::string path = converter.to_bytes(wpath);
	m_executablePath = ensureTrailingSeparator(path);
	m_basePath = path.substr(0, (path.find_last_of('\\')) + 1);
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

std::string FGameConfig::resolveAssetPath(const std::string &assetPath) const
{
	if (assetPath.empty() || isAbsolutePath(assetPath)) {
		return assetPath;
	}

	const std::string normalizedAssetPath = normalizeAssetPath(assetPath);
	const std::string baseCandidate = joinPath(m_basePath, normalizedAssetPath);
	if (pathExists(baseCandidate)) {
		return baseCandidate;
	}

	const std::string executableCandidate =
			joinPath(m_executablePath, normalizedAssetPath);
	if (pathExists(executableCandidate)) {
		return executableCandidate;
	}

	const std::string executableParentCandidate =
			joinPath(m_executablePath, std::string("../") + normalizedAssetPath);
	if (pathExists(executableParentCandidate)) {
		return executableParentCandidate;
	}

	const std::string baseParentCandidate =
			joinPath(m_basePath, std::string("../") + normalizedAssetPath);
	if (pathExists(baseParentCandidate)) {
		return baseParentCandidate;
	}

	return baseCandidate;
}

};
