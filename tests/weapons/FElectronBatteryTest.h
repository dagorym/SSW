/**
 * @file FElectronBatteryTest.h
 * @brief Header file for the FElectronBatteryTest class
 * @author Tom Stephens
 * @date Created: Jun 19, 2009
 */

#ifndef FElectronBatteryTest_H_
#define FElectronBatteryTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "weapons/FElectronBattery.h"

namespace FrontierTests {
using namespace Frontier;

class FElectronBatteryTest : public CppUnit::TestFixture{
	CPPUNIT_TEST_SUITE( FElectronBatteryTest );
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

#endif /* FElectronBatteryTest_H_ */
