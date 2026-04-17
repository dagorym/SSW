/**
 * @file WXIconCacheTest.cpp
 * @brief Implementation file for the WXIconCacheTest class
 */

#include "WXIconCacheTest.h"

#include <fstream>
#include <iterator>

#include <wx/image.h>

#include "core/FGameConfig.h"
#include "gui/WXIconCache.h"

namespace FrontierTests {
using namespace Frontier;

namespace {

std::string repoFile(const std::string & relativePath) {
	return std::string(CORE_TEST_REPO_ROOT) + "/" + relativePath;
}

void assertContains(const std::string & haystack, const std::string & needle) {
	CPPUNIT_ASSERT_MESSAGE(
		std::string("Expected to find '") + needle + "' in WXIconCache.cpp",
		haystack.find(needle) != std::string::npos);
}

}

CPPUNIT_TEST_SUITE_REGISTRATION( WXIconCacheTest );

void WXIconCacheTest::setUp() {
	wxInitAllImageHandlers();
	FGameConfig::reset();
}

void WXIconCacheTest::tearDown() {
	FGameConfig::reset();
}

std::string WXIconCacheTest::readFile(const std::string & path) {
	std::ifstream file(path.c_str());
	CPPUNIT_ASSERT_MESSAGE(path, file.is_open());
	return std::string((std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());
}

void WXIconCacheTest::testGetResolvesFilenameThroughSharedAssetResolver() {
	// AC1: get() resolves icon paths through FGameConfig::resolveAssetPath before image load.
	const std::string source = readFile(repoFile("src/gui/WXIconCache.cpp"));
	assertContains(source, "FGameConfig & gc = FGameConfig::getGameConfig();");
	assertContains(source, "std::string fullPath = gc.resolveAssetPath(filename);");
	assertContains(source, "wxImage img(fullPath);");
	assertContains(source, "wxLogWarning(\"WXIconCache: failed to load image '%s'\", fullPath.c_str());");
	CPPUNIT_ASSERT(source.find("gc.getBasePath() + filename;") == std::string::npos);

	FGameConfig & gc = FGameConfig::getGameConfig();
	const std::string resolvedPath = gc.resolveAssetPath("icons/UPFFighter.png");
	CPPUNIT_ASSERT(!resolvedPath.empty());
	wxImage directLoad(resolvedPath);
	CPPUNIT_ASSERT_MESSAGE(
		std::string("Expected image to load from resolved path: ") + resolvedPath,
		directLoad.IsOk());
}

void WXIconCacheTest::testCacheKeyRemainsOriginalFilename() {
	// AC2: cache key remains original short filename while loading uses resolved asset path.
	WXIconCache & cache = WXIconCache::instance();
	const std::string shortFilename = "icons/UPFFighter.png";

	const wxImage & first = cache.get(shortFilename);
	CPPUNIT_ASSERT_MESSAGE("First load through cache should succeed for known icon", first.IsOk());

	const wxImage & second = cache.get(shortFilename);
	CPPUNIT_ASSERT_MESSAGE("Second fetch should return same cached image reference", &first == &second);
}

}
