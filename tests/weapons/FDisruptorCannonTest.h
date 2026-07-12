/**
 * @file FDisruptorCannonTest.h
 * @brief Header file for the FDisruptorCannonTest class
 * @author Tom Stephens, Claude Sonnet 5 (medium)
 * @date Created: Jun 19, 2009
 * @date Last Modified: Jul 11, 2026
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
	CPPUNIT_TEST( testSetTargetAcceptsRangeAtMax );
	CPPUNIT_TEST( testSetTargetRejectsRangeBeyondMax );
	CPPUNIT_TEST_SUITE_END();

private:
	FWeapon * m_w1;

public:
	void setUp();
	void tearDown();

	void testConstructor();
	void testFireCoverage();

	/// P2-1 AC: setTarget(target, 9, ...) is accepted because 9 is the corrected max range (RA 9).
	void testSetTargetAcceptsRangeAtMax();
	/// P2-1 AC: setTarget(target, r, ...) for r in 10..12 is rejected now that m_range is 9 (previously accepted up to 12).
	void testSetTargetRejectsRangeBeyondMax();
};

}

#endif /* FDisruptorCannonTest_H_ */
