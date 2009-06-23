/**
 * @file FLaserBatteryTest.h
 * @brief Header file for the FLaserBatteryTest class
 * @author Tom Stephens
 * @date Created: Jun 19, 2009
 */

#ifndef FLaserBatteryTest_H_
#define FLaserBatteryTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "weapons/FLaserBattery.h"

namespace FrontierTests {
using namespace Frontier;

class FLaserBatteryTest : public CppUnit::TestFixture{
	CPPUNIT_TEST_SUITE( FLaserBatteryTest );
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

#endif /* FLaserBatteryTest_H_ */
