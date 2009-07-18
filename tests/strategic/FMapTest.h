/**
 * @file FMapTest.h
 * @brief Header file for the FMapTest class
 * @author Tom Stephens
 * @date Created: Jul 18, 2009
 */

#ifndef FMapTest_H_
#define FMapTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "strategic/FMap.h"

namespace FrontierTests {
using namespace Frontier;

class FMapTest : public CppUnit::TestFixture{
	CPPUNIT_TEST_SUITE( FMapTest );
	CPPUNIT_TEST( testConstructor );
	CPPUNIT_TEST( testCreateFrontier );
	CPPUNIT_TEST( testGetSystemByName );
	CPPUNIT_TEST( testGetSystemByID );
	CPPUNIT_TEST( getJumpRouteBySystems );
	CPPUNIT_TEST( getJumpRouteByID );
	CPPUNIT_TEST( testGetConnectedSystems );
	CPPUNIT_TEST( testSerialize );
//	CPPUNIT_TEST( testDraw );
	CPPUNIT_TEST_SUITE_END();

private:
	std::vector<unsigned int> m_pList;

public:
	void setUp();
	void tearDown();

	void testConstructor();
	void testCreateFrontier();
	void testGetSystemByName();
	void testGetSystemByID();
	void getJumpRouteBySystems();
	void getJumpRouteByID();
	void testGetConnectedSystems();
	void testSerialize();
	void testDraw();
};

}

#endif /* FMapTest_H_ */
