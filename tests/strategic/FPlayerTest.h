/**
 * @file FPlayerTest.h
 * @brief Header file for the FPlayerTest class
 * @author Tom Stephens
 * @date Created: Jul 18, 2009
 */

#ifndef FPlayerTest_H_
#define FPlayerTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "strategic/FPlayer.h"

namespace FrontierTests {
using namespace Frontier;

class FPlayerTest : public CppUnit::TestFixture{
	CPPUNIT_TEST_SUITE( FPlayerTest );
	CPPUNIT_TEST( testConstructor );
	CPPUNIT_TEST( testSetName );
	CPPUNIT_TEST( testAddFleet );
	CPPUNIT_TEST( testGetFleetByName );
	CPPUNIT_TEST( testGetFleetList );
	CPPUNIT_TEST( testRemoveFleet );
	CPPUNIT_TEST( testSetIcon );
	CPPUNIT_TEST( testAddUnattachedShips );
	CPPUNIT_TEST( testAddDestroyedShips );
	CPPUNIT_TEST( testSerialize );
	CPPUNIT_TEST_SUITE_END();

private:
	FPlayer *m_p1;

public:
	void setUp();
	void tearDown();

	void testConstructor();
	void testSetName();
	void testAddFleet();
	void testGetFleetByName();
	void testGetFleetList();
	void testRemoveFleet();
	void testSetIcon();
	void testAddUnattachedShips();
	void testAddDestroyedShips();
	void testSerialize();
};

}

#endif /* FPlayerTest_H_ */
