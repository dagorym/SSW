/**
 * @file FPlanetTest.h
 * @brief Header file for the FPlanetTest class
 * @author Tom Stephens
 * @date Created: Jun 24, 2009
 */

#ifndef FPlanetTest_H_
#define FPlanetTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "strategic/FPlanet.h"

namespace FrontierTests {
using namespace Frontier;

class FPlanetTest : public CppUnit::TestFixture{
	CPPUNIT_TEST_SUITE( FPlanetTest );
	CPPUNIT_TEST( testConstructor );
	CPPUNIT_TEST( testAddDestroyStation );
	CPPUNIT_TEST( testOverwriteStation );
	CPPUNIT_TEST( testSerialize );
	CPPUNIT_TEST_SUITE_END();

private:
	FPlanet *m_p1;

public:
	void setUp();
	void tearDown();

	void testConstructor();
	void testAddDestroyStation();
	void testOverwriteStation();
	void testSerialize();
};

}

#endif /* FPlanetTest_H_ */
