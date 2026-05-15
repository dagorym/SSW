/**
 * @file GuiHarnessTest.h
 * @brief Header file for GUI test harness regression coverage
 */

#ifndef _GUIHARNESSTEST_H_
#define _GUIHARNESSTEST_H_

#include <cppunit/extensions/HelperMacros.h>

#include "WXGuiTestHarness.h"

namespace FrontierTests {

/**
 * @brief Exercises shared GUI harness bootstrap, modal, and cleanup helpers.
 *
 * The fixture verifies that harness utilities can observe top-level windows, find modal
 * dialogs, and clean residual UI state so other live GUI suites remain deterministic.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 04, 2026
 * @date Last Modified: Apr 04, 2026
 */
class GuiHarnessTest : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( GuiHarnessTest );
CPPUNIT_TEST( testPumpEventsAfterBootstrap );
CPPUNIT_TEST( testCanObserveShownTopLevelWindow );
CPPUNIT_TEST( testWaitForModalDialogFindsLaunchedModal );
CPPUNIT_TEST( testCleanupOrphanTopLevelsRemovesShownWindows );
CPPUNIT_TEST( testShowModalWithAutoDismissReturnsExpectedCode );
CPPUNIT_TEST_SUITE_END();

private:
	WXGuiTestHarness m_harness;

public:
	/**
	 * @brief Bootstraps the shared wx GUI harness before each test.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	void setUp();
	/**
	 * @brief Cleans and shuts down harness-managed wx state after each test.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	void tearDown();

	/**
	 * @brief Verifies event pumping succeeds after harness bootstrap.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	void testPumpEventsAfterBootstrap();
	/**
	 * @brief Confirms shown top-level windows are discoverable through harness polling.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	void testCanObserveShownTopLevelWindow();
	/**
	 * @brief Confirms modal wait helpers can locate a launched modal dialog.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	void testWaitForModalDialogFindsLaunchedModal();
	/**
	 * @brief Verifies orphan top-level cleanup destroys shown windows and reports leftovers.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	void testCleanupOrphanTopLevelsRemovesShownWindows();
	/**
	 * @brief Verifies timed auto-dismiss helpers return the requested modal code.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	void testShowModalWithAutoDismissReturnsExpectedCode();
};

}

#endif
