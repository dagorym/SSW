/**
 * @file FProtonBatteryTest.h
 * @brief Header file for the FProtonBatteryTest class
 * @author Tom Stephens
 * @date Created: Jun 19, 2009
 */

#ifndef FProtonBatteryTest_H_
#define FProtonBatteryTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "weapons/FProtonBattery.h"

namespace FrontierTests {
using namespace Frontier;

class FProtonBatteryTest : public CppUnit::TestFixture{
	CPPUNIT_TEST_SUITE( FProtonBatteryTest );
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

#endif /* FProtonBatteryTest_H_ */
