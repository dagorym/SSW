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

/**
 * @brief CppUnit fixture for FTacticalShipPlacementOrientationTest tactical regression coverage.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 05, 2026
 * @date Last Modified: Apr 05, 2026
 */
class FTacticalShipPlacementOrientationTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE( FTacticalShipPlacementOrientationTest );
	CPPUNIT_TEST( testPreviewByHexUpdatesHeadingWithoutAdvancingPlacement );
	CPPUNIT_TEST( testDefenderSetupSecondClickCommitsLastPreviewedHeading );
	CPPUNIT_TEST( testAttackerSetupSecondClickCommitsLastPreviewedHeading );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

	/**
	 * @brief Verifies preview By Hex Updates Heading Without Advancing Placement.
	 *
	 * Exercises the tactical regression behavior covered by this fixture case.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 05, 2026
	 * @date Last Modified: Apr 05, 2026
	 */
	void testPreviewByHexUpdatesHeadingWithoutAdvancingPlacement();
	/**
	 * @brief Verifies defender Setup Second Click Commits Last Previewed Heading.
	 *
	 * Exercises the tactical regression behavior covered by this fixture case.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 05, 2026
	 * @date Last Modified: Apr 05, 2026
	 */
	void testDefenderSetupSecondClickCommitsLastPreviewedHeading();
	/**
	 * @brief Verifies attacker Setup Second Click Commits Last Previewed Heading.
	 *
	 * Exercises the tactical regression behavior covered by this fixture case.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 05, 2026
	 * @date Last Modified: Apr 05, 2026
	 */
	void testAttackerSetupSecondClickCommitsLastPreviewedHeading();
};

}

#endif
