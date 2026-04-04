/**
 * @file FTacticalForwardFireFinalOrientationTest.h
 * @brief Runtime regression tests for forward-fire final-orientation behavior
 * @author Tom Stephens
 * @date Created: Apr 04, 2026
 */

#ifndef FTacticalForwardFireFinalOrientationTest_H_
#define FTacticalForwardFireFinalOrientationTest_H_

#include <cppunit/extensions/HelperMacros.h>

namespace FrontierTests {

class FTacticalForwardFireFinalOrientationTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE( FTacticalForwardFireFinalOrientationTest );
	CPPUNIT_TEST( testFinalTurnEndpointUsesFinalHeadingForHighlightsAndTargetValidation );
	CPPUNIT_TEST( testStraightLineForwardFireRemainsUnchanged );
	CPPUNIT_TEST( testBatteryRangeRemainsHeadingIndependent );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

	void testFinalTurnEndpointUsesFinalHeadingForHighlightsAndTargetValidation();
	void testStraightLineForwardFireRemainsUnchanged();
	void testBatteryRangeRemainsHeadingIndependent();
};

}

#endif
