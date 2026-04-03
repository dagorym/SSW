/**
 * @file FTacticalDestroyedShipCleanupLifecycleTest.h
 * @brief Header file for the FTacticalDestroyedShipCleanupLifecycleTest class
 */

#ifndef FTacticalDestroyedShipCleanupLifecycleTest_H_
#define FTacticalDestroyedShipCleanupLifecycleTest_H_

#include <cppunit/extensions/HelperMacros.h>

namespace FrontierTests {

class FTacticalDestroyedShipCleanupLifecycleTest : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( FTacticalDestroyedShipCleanupLifecycleTest );
CPPUNIT_TEST( testClearsSelectedShipAndRedrawsWhenSelectedShipDestroyed );
CPPUNIT_TEST( testBookkeepingClearedExactlyOncePerCleanup );
CPPUNIT_TEST( testWinnerHandlingRunsAfterBookkeepingClear );
CPPUNIT_TEST_SUITE_END();

public:
void setUp();
void tearDown();

void testClearsSelectedShipAndRedrawsWhenSelectedShipDestroyed();
void testBookkeepingClearedExactlyOncePerCleanup();
void testWinnerHandlingRunsAfterBookkeepingClear();
};

}

#endif
