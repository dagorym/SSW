/**
 * @file FArmedStation.h
 * @brief Header file for the FArmedStationTest class
 * @author Tom Stephens
 * @date Created: Jun 22, 2009
 */

#ifndef FArmedStationTest_H_
#define FArmedStationTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "ships/FArmedStation.h"

namespace FrontierTests {
using namespace Frontier;

class FArmedStationTest : public CppUnit::TestFixture{
	CPPUNIT_TEST_SUITE( FArmedStationTest );
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

#endif /* FArmedStationTest_H_ */
