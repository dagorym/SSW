/**
 * @file FLaserCannonTest.h
 * @brief Header file for the FLaserCannonTest class
 * @author Tom Stephens
 * @date Created: Jun 19, 2009
 */

#ifndef FLaserCannonTest_H_
#define FLaserCannonTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "weapons/FLaserCannon.h"

namespace FrontierTests {
using namespace Frontier;

class FLaserCannonTest : public CppUnit::TestFixture{
	CPPUNIT_TEST_SUITE( FLaserCannonTest );
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

#endif /* FLaserCannonTest_H_ */
