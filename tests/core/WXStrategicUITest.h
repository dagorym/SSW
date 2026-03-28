/**
 * @file WXStrategicUITest.h
 * @brief Header file for the WXStrategicUITest class
 */

#ifndef WXStrategicUITest_H_
#define WXStrategicUITest_H_

#include <cppunit/extensions/HelperMacros.h>

namespace FrontierTests {

class WXStrategicUITest : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( WXStrategicUITest );
CPPUNIT_TEST( testGuardedDialogMethodsReturnNonZeroWhenNoUIContext );
CPPUNIT_TEST( testRequestRedrawIsNullSafe );
CPPUNIT_TEST_SUITE_END();

public:
void setUp();
void tearDown();

void testGuardedDialogMethodsReturnNonZeroWhenNoUIContext();
void testRequestRedrawIsNullSafe();
};

}

#endif
