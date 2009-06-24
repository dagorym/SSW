/**
 * @file FFortifiedStation.h
 * @brief Header file for the FFortifiedStationTest class
 * @author Tom Stephens
 * @date Created: Jun 23, 2009
 */

#ifndef FFortifiedStationTest_H_
#define FFortifiedStationTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "ships/FFortifiedStation.h"

namespace FrontierTests {
using namespace Frontier;

class FFortifiedStationTest : public CppUnit::TestFixture{
	CPPUNIT_TEST_SUITE( FFortifiedStationTest );
	CPPUNIT_TEST( testConstructor );
	CPPUNIT_TEST_SUITE_END();

private:
	FVehicle * m_v1;

public:
	void setUp();
	void tearDown();

	void testConstructor();
};

}

#endif /* FFortifiedStationTest_H_ */
