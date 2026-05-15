/**
 * @file FTacticalDestroyedShipCleanupLifecycleTest.h
 * @brief Header file for the FTacticalDestroyedShipCleanupLifecycleTest class
 */

#ifndef FTacticalDestroyedShipCleanupLifecycleTest_H_
#define FTacticalDestroyedShipCleanupLifecycleTest_H_

#include <cppunit/extensions/HelperMacros.h>

namespace FrontierTests {

/**
 * @brief CppUnit fixture for FTacticalDestroyedShipCleanupLifecycleTest tactical regression coverage.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 03, 2026
 * @date Last Modified: Apr 03, 2026
 */
class FTacticalDestroyedShipCleanupLifecycleTest : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( FTacticalDestroyedShipCleanupLifecycleTest );
CPPUNIT_TEST( testClearsSelectedShipAndRedrawsWhenSelectedShipDestroyed );
CPPUNIT_TEST( testBookkeepingClearedExactlyOncePerCleanup );
CPPUNIT_TEST( testWinnerHandlingRunsAfterBookkeepingClear );
CPPUNIT_TEST_SUITE_END();

public:
void setUp();
void tearDown();

/**
 * @brief Verifies clears Selected Ship And Redraws When Selected Ship Destroyed.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 03, 2026
 * @date Last Modified: Apr 03, 2026
 */
void testClearsSelectedShipAndRedrawsWhenSelectedShipDestroyed();
/**
 * @brief Verifies bookkeeping Cleared Exactly Once Per Cleanup.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 03, 2026
 * @date Last Modified: Apr 03, 2026
 */
void testBookkeepingClearedExactlyOncePerCleanup();
/**
 * @brief Verifies winner Handling Runs After Bookkeeping Clear.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 03, 2026
 * @date Last Modified: Apr 03, 2026
 */
void testWinnerHandlingRunsAfterBookkeepingClear();
};

}

#endif
