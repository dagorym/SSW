/**
 * @file FDisruptorCannonTest.h
 * @brief Header file for the FDisruptorCannonTest class
 * @author Tom Stephens
 * @date Created: Jun 19, 2009
 */

#ifndef FDisruptorCannonTest_H_
#define FDisruptorCannonTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "weapons/FDisruptorCannon.h"

namespace FrontierTests {
using namespace Frontier;

class FDisruptorCannonTest : public CppUnit::TestFixture{
	CPPUNIT_TEST_SUITE( FDisruptorCannonTest );
	CPPUNIT_TEST( testConstructor );
	CPPUNIT_TEST( testFireCoverage );
	CPPUNIT_TEST_SUITE_END();

private:
	FWeapon * m_w1;

public:
	void setUp();
	void tearDown();

	void testConstructor();
	void testFireCoverage();
};

}

#endif /* FDisruptorCannonTest_H_ */
