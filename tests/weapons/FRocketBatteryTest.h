/**
 * @file FRocketBatteryTest.h
 * @brief Header file for the FRocketBatteryTest class
 * @author Tom Stephens
 * @date Created: Jun 19, 2009
 */

#ifndef FRocketBatteryTest_H_
#define FRocketBatteryTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "weapons/FRocketBattery.h"

namespace FrontierTests {
using namespace Frontier;

class FRocketBatteryTest : public CppUnit::TestFixture{
	CPPUNIT_TEST_SUITE( FRocketBatteryTest );
	CPPUNIT_TEST( testConstructor );
	CPPUNIT_TEST_SUITE_END();

private:
	FWeapon * m_w1;

public:
	void setUp();
	void tearDown();

	void testConstructor();
};

}

#endif /* FRocketBatteryTest_H_ */
