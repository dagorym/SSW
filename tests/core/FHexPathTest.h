/**
 * @file FHexPathTest.h
 * @brief Header file for the FHexPathTest class
 * @author Tom Stephens
 * @date Created: Oct 04, 2010
 */

#ifndef FHexPathTEST_H_
#define FHexPathTEST_H_

#include <cppunit/extensions/HelperMacros.h>

#include "core/FHexPath.h"

namespace FrontierTests {
using namespace Frontier;

class FHexPathTest : public CppUnit::TestFixture{
	CPPUNIT_TEST_SUITE( FHexPathTest );
	CPPUNIT_TEST( testConstructor );
	CPPUNIT_TEST( testAddPoint );
	CPPUNIT_TEST( testAddPointWithGap );
	CPPUNIT_TEST( testAddFlag );
	CPPUNIT_TEST( testGetFlag );
	CPPUNIT_TEST( testGetFlagNoPoint );
	CPPUNIT_TEST( testFindPoint );
	CPPUNIT_TEST( testFindPointNoPoint );
	CPPUNIT_TEST_SUITE_END();

private:
	FHexPath * m_hl1;

public:
	void setUp();
	void tearDown();

	void testConstructor();
	void testAddPoint();
	void testAddPointWithGap();
	void testAddFlag();
	void testGetFlag();
	void testGetFlagNoPoint();
	void testFindPoint();
	void testFindPointNoPoint();

};

}

#endif /* FHexPathTEST_H_ */
