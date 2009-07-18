/**
 * @file FSystemTest.h
 * @brief Header file for the FSystemTest class
 * @author Tom Stephens
 * @date Created: Jul 17, 2009
 */

#ifndef FSystemTest_H_
#define FSystemTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "strategic/FSystem.h"

namespace FrontierTests {
using namespace Frontier;

class FSystemTest : public CppUnit::TestFixture{
	CPPUNIT_TEST_SUITE( FSystemTest );
	CPPUNIT_TEST( testConstructor );
	CPPUNIT_TEST( testAddPlanet );
	CPPUNIT_TEST( testAddRemoveFleet );
	CPPUNIT_TEST( testSerialize );
	CPPUNIT_TEST_SUITE_END();

private:
	FSystem *m_s1;

public:
	void setUp();
	void tearDown();

	void testConstructor();
	void testAddPlanet();
	void testAddRemoveFleet();
	void testSerialize();
};

}

#endif /* FSystemTest_H_ */
