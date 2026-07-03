/**
 * @file WXTacticalUIAdapterTest.h
 * @brief Header file for WXTacticalUI adapter acceptance tests
 */

#ifndef WXTACTICALUIADAPTERTEST_H_
#define WXTACTICALUIADAPTERTEST_H_

#include <cppunit/extensions/HelperMacros.h>
#include <string>

namespace FrontierTests {

/**
 * @brief CppUnit fixture for WXTacticalUIAdapterTest tactical regression coverage.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Mar 29, 2026
 * @date Last Modified: Apr 18, 2026
 */
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

/**
 * @brief Verifies adapter Constructs As ITactical UI.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 18, 2026
 * @date Last Modified: Apr 18, 2026
 */
void testAdapterConstructsAsITacticalUI();
/**
 * @brief Verifies request Redraw Is Safe With And Without Parent.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 18, 2026
 * @date Last Modified: Apr 18, 2026
 */
void testRequestRedrawIsSafeWithAndWithoutParent();
/**
 * @brief Verifies run ICMSelection Validates Inputs.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 18, 2026
 * @date Last Modified: Apr 18, 2026
 */
void testRunICMSelectionValidatesInputs();
/**
 * @brief Verifies notify Winner Uses No Parent Fallback Safely.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 18, 2026
 * @date Last Modified: Apr 18, 2026
 */
void testNotifyWinnerUsesNoParentFallbackSafely();
};

} // namespace FrontierTests

#endif
