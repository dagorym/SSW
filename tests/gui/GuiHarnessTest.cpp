/**
 * @file GuiHarnessTest.cpp
 * @brief Implementation file for GUI test harness regression coverage
 */

#include "GuiHarnessTest.h"

#include <wx/dialog.h>
#include <wx/window.h>

namespace FrontierTests {

CPPUNIT_TEST_SUITE_REGISTRATION( GuiHarnessTest );

void GuiHarnessTest::setUp() {
	CPPUNIT_ASSERT(m_harness.bootstrap());
}

void GuiHarnessTest::tearDown() {
	m_harness.shutdown();
}

void GuiHarnessTest::testPumpEventsAfterBootstrap() {
	m_harness.pumpEvents();
	CPPUNIT_ASSERT(true);
}

void GuiHarnessTest::testShowModalWithAutoDismissReturnsExpectedCode() {
	wxDialog dialog(static_cast<wxWindow*>(NULL), wxID_ANY, wxT("GuiHarnessTest"));
	const int result = m_harness.showModalWithAutoDismiss(dialog, wxID_CANCEL, 25);
	CPPUNIT_ASSERT_EQUAL(static_cast<int>(wxID_CANCEL), result);
}

}
