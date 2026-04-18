/**
 * @file WXTacticalUIAdapterTest.h
 * @brief Header file for WXTacticalUI adapter acceptance tests
 */

#ifndef WXTACTICALUIADAPTERTEST_H_
#define WXTACTICALUIADAPTERTEST_H_

#include <cppunit/extensions/HelperMacros.h>
#include <string>

namespace FrontierTests {

class WXTacticalUIAdapterTest : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( WXTacticalUIAdapterTest );
CPPUNIT_TEST( testAdapterConstructsAsITacticalUI );
CPPUNIT_TEST( testRequestRedrawIsSafeWithAndWithoutParent );
CPPUNIT_TEST( testRunICMSelectionValidatesInputs );
CPPUNIT_TEST( testNotifyWinnerUsesNoParentFallbackSafely );
CPPUNIT_TEST_SUITE_END();

public:
void setUp();
void tearDown();

void testAdapterConstructsAsITacticalUI();
void testRequestRedrawIsSafeWithAndWithoutParent();
void testRunICMSelectionValidatesInputs();
void testNotifyWinnerUsesNoParentFallbackSafely();
};

} // namespace FrontierTests

#endif
