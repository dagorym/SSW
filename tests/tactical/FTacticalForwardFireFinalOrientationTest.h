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

/**
 * @brief CppUnit fixture for FTacticalForwardFireFinalOrientationTest tactical regression coverage.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 04, 2026
 * @date Last Modified: Apr 04, 2026
 */
class FTacticalForwardFireFinalOrientationTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE( FTacticalForwardFireFinalOrientationTest );
	CPPUNIT_TEST( testFinalTurnEndpointUsesFinalHeadingForHighlightsAndTargetValidation );
	CPPUNIT_TEST( testStraightLineForwardFireRemainsUnchanged );
	CPPUNIT_TEST( testBatteryRangeRemainsHeadingIndependent );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

	/**
	 * @brief Verifies final Turn Endpoint Uses Final Heading For Highlights And Target Validation.
	 *
	 * Exercises the tactical regression behavior covered by this fixture case.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	void testFinalTurnEndpointUsesFinalHeadingForHighlightsAndTargetValidation();
	/**
	 * @brief Verifies straight Line Forward Fire Remains Unchanged.
	 *
	 * Exercises the tactical regression behavior covered by this fixture case.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	void testStraightLineForwardFireRemainsUnchanged();
	/**
	 * @brief Verifies battery Range Remains Heading Independent.
	 *
	 * Exercises the tactical regression behavior covered by this fixture case.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	void testBatteryRangeRemainsHeadingIndependent();
};

}

#endif
