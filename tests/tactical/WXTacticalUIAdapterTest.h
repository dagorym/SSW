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
CPPUNIT_TEST( testHasPendingDialogAndDismissActiveDialogAreSafeNoOpsWithEmptyStack );
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
/**
 * @brief Verifies hasPendingDialog()/dismissActiveDialog() are safe no-ops on a fresh
 *        WXTacticalUI instance whose dialog stack has never had a dialog pushed onto it.
 *
 * TMFR-01: m_activeDialog (a single pointer) was replaced with m_dialogStack (a
 * std::vector<wxDialog*>). This behaviorally confirms the empty-stack edge case: a
 * freshly constructed adapter (no showMessage()/showDamageSummary()/runICMSelection()
 * call has ever pushed a dialog) reports hasPendingDialog() == false, and calling
 * dismissActiveDialog() on that empty stack does not crash or throw. Real nested,
 * multi-dialog stack-ordering coverage (which requires driving live, nested wx modal
 * dialogs) lives in tests/gui/TacticalGuiLiveTest.cpp, which has the wx runtime bootstrap
 * this standalone tactical test runner does not provide.
 *
 * @author Claude Sonnet 5 (medium)
 * @date Created: Jul 03, 2026
 * @date Last Modified: Jul 03, 2026
 */
void testHasPendingDialogAndDismissActiveDialogAreSafeNoOpsWithEmptyStack();
};

} // namespace FrontierTests

#endif
