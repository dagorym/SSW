/**
 * @file FTacticalShipPlacementOrientationTest.h
 * @brief Runtime regression tests for two-step ship placement orientation
 * @author Tom Stephens
 * @date Created: Apr 05, 2026
 */

#ifndef FTacticalShipPlacementOrientationTest_H_
#define FTacticalShipPlacementOrientationTest_H_

#include <cppunit/extensions/HelperMacros.h>

namespace FrontierTests {

class FTacticalShipPlacementOrientationTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE( FTacticalShipPlacementOrientationTest );
	CPPUNIT_TEST( testPreviewByHexUpdatesHeadingWithoutAdvancingPlacement );
	CPPUNIT_TEST( testDefenderSetupSecondClickCommitsLastPreviewedHeading );
	CPPUNIT_TEST( testAttackerSetupSecondClickCommitsLastPreviewedHeading );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

	void testPreviewByHexUpdatesHeadingWithoutAdvancingPlacement();
	void testDefenderSetupSecondClickCommitsLastPreviewedHeading();
	void testAttackerSetupSecondClickCommitsLastPreviewedHeading();
};

}

#endif
