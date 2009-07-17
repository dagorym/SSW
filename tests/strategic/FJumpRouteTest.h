/**
 * @file FJumpRouteTest.h
 * @brief Header file for the FJumpRouteTest class
 * @author Tom Stephens
 * @date Created: DATE
 */

#ifndef FJumpRouteTest_H_
#define FJumpRouteTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "strategic/FJumpRoute.h"

namespace FrontierTests {
using namespace Frontier;

class FJumpRouteTest : public CppUnit::TestFixture{
	CPPUNIT_TEST_SUITE( FJumpRouteTest );
	CPPUNIT_TEST( testConstructor );
	CPPUNIT_TEST( testFullConstructor );
	CPPUNIT_TEST( testGetJumpTimeSpeed0 );
	CPPUNIT_TEST( testGetJumpTimeSpeed1 );
	CPPUNIT_TEST( testGetJumpTimeSpeed2 );
	CPPUNIT_TEST( testGetJumpTimeSpeed3 );
	CPPUNIT_TEST( testSerialize );
	CPPUNIT_TEST_SUITE_END();

private:
	FJumpRoute *m_j1;

public:
	void setUp();
	void tearDown();

	void testConstructor();
	void testFullConstructor();
	void testGetJumpTimeSpeed0();
	void testGetJumpTimeSpeed1();
	void testGetJumpTimeSpeed2();
	void testGetJumpTimeSpeed3();
	void testSerialize();
};

}

#endif /* FJumpRouteTest_H_ */
