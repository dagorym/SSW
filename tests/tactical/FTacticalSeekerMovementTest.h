/**
 * @file FTacticalSeekerMovementTest.h
 * @brief Header file for seeker movement model-side regression tests
 */

#ifndef _FTACTICALSEEKERMOVEMENTTEST_H_
#define _FTACTICALSEEKERMOVEMENTTEST_H_

#include <cppunit/extensions/HelperMacros.h>
#include <string>

namespace FrontierTests {

/**
 * @brief CppUnit fixture for model-side seeker targeting and movement helpers.
 *
 * Verifies source-level tactical seeker helper contracts remain deterministic,
 * non-wx, and aligned with planner acceptance criteria.
 *
 * @author gpt-5.4 (high)
 * @date Created: May 25, 2026
 * @date Last Modified: May 25, 2026
 */
class FTacticalSeekerMovementTest : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( FTacticalSeekerMovementTest );
CPPUNIT_TEST( testSeekerTargetSelectionFiltersAndTieBreaksAcrossSides );
CPPUNIT_TEST( testSeekerHeadingAndMovementGreedyTurnLimits );
CPPUNIT_TEST( testSeekerMovementAllowanceProgression );
CPPUNIT_TEST( testSeekerHelpersRemainModelOnlyAndFHexMapGeneric );
CPPUNIT_TEST_SUITE_END();

private:
std::string readFile(const std::string & path);
std::string extractFunctionBody(const std::string & source, const std::string & signature);
void assertContains(const std::string & haystack, const std::string & needle);

public:
void setUp();
void tearDown();

/**
 * @brief Verifies closest-target seeker selection filters and random tie-breaking.
 *
 * @author gpt-5.4 (high)
 * @date Created: May 25, 2026
 * @date Last Modified: May 25, 2026
 */
void testSeekerTargetSelectionFiltersAndTieBreaksAcrossSides();

/**
 * @brief Verifies heading clamp and per-hex greedy turn behavior.
 *
 * @author gpt-5.4 (high)
 * @date Created: May 25, 2026
 * @date Last Modified: May 25, 2026
 */
void testSeekerHeadingAndMovementGreedyTurnLimits();

/**
 * @brief Verifies seeker movement allowance progression and cap behavior.
 *
 * @author gpt-5.4 (high)
 * @date Created: May 25, 2026
 * @date Last Modified: May 25, 2026
 */
void testSeekerMovementAllowanceProgression();

/**
 * @brief Verifies seeker helpers and FHexMap surface remain non-wx model code.
 *
 * @author gpt-5.4 (high)
 * @date Created: May 25, 2026
 * @date Last Modified: May 25, 2026
 */
void testSeekerHelpersRemainModelOnlyAndFHexMapGeneric();
};

}

#endif
