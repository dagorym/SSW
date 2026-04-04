/**
 * @file GuiHarnessTest.h
 * @brief Header file for GUI test harness regression coverage
 */

#ifndef _GUIHARNESSTEST_H_
#define _GUIHARNESSTEST_H_

#include <cppunit/extensions/HelperMacros.h>

#include "WXGuiTestHarness.h"

namespace FrontierTests {

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
	void setUp();
	void tearDown();

	void testPumpEventsAfterBootstrap();
	void testCanObserveShownTopLevelWindow();
	void testWaitForModalDialogFindsLaunchedModal();
	void testCleanupOrphanTopLevelsRemovesShownWindows();
	void testShowModalWithAutoDismissReturnsExpectedCode();
};

}

#endif
