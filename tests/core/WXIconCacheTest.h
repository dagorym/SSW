/**
 * @file WXIconCacheTest.h
 * @brief Header file for the WXIconCacheTest class
 */

#ifndef WXIconCacheTest_H_
#define WXIconCacheTest_H_

#include <cppunit/extensions/HelperMacros.h>
#include <string>

namespace FrontierTests {

class WXIconCacheTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE( WXIconCacheTest );
	CPPUNIT_TEST( testGetResolvesFilenameThroughSharedAssetResolver );
	CPPUNIT_TEST( testCacheKeyRemainsOriginalFilename );
	CPPUNIT_TEST_SUITE_END();

private:
	std::string readFile(const std::string & path);

public:
	void setUp();
	void tearDown();

	void testGetResolvesFilenameThroughSharedAssetResolver();
	void testCacheKeyRemainsOriginalFilename();
};

}

#endif
