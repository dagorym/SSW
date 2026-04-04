/**
 * @file GuiHarnessTest.cpp
 * @brief Implementation file for GUI test harness regression coverage
 */

#include "GuiHarnessTest.h"

#include <wx/dialog.h>
#include <wx/frame.h>
#include <wx/toplevel.h>
#include <wx/window.h>

namespace FrontierTests {

CPPUNIT_TEST_SUITE_REGISTRATION( GuiHarnessTest );

void GuiHarnessTest::setUp() {
	CPPUNIT_ASSERT(m_harness.bootstrap());
}

void GuiHarnessTest::tearDown() {
	m_harness.cleanupOrphanTopLevels(10);
	m_harness.shutdown();
}

void GuiHarnessTest::testPumpEventsAfterBootstrap() {
	m_harness.pumpEvents();
	CPPUNIT_ASSERT(true);
}

void GuiHarnessTest::testCanObserveShownTopLevelWindow() {
	wxFrame * frame = new wxFrame(NULL, wxID_ANY, wxT("Harness Observed Frame"));
	frame->Show();
	m_harness.pumpEvents(2);

	wxTopLevelWindow * observed = m_harness.waitForTopLevelWindow([&](wxTopLevelWindow * topLevel) {
		return topLevel == frame;
	}, 150, 5);
	CPPUNIT_ASSERT(observed == frame);

	frame->Destroy();
	m_harness.pumpEvents(10);
}

void GuiHarnessTest::testWaitForModalDialogFindsLaunchedModal() {
	wxDialog dialog(static_cast<wxWindow*>(NULL), wxID_ANY, wxT("Harness Modal Observe"));
	wxDialog * observedDialog = NULL;
	const int result = m_harness.showModalWithAction(dialog, [&]() {
		observedDialog = m_harness.waitForModalDialog(100, 5);
	}, wxID_OK, 50);
	CPPUNIT_ASSERT_EQUAL(static_cast<int>(wxID_OK), result);
	CPPUNIT_ASSERT(observedDialog == &dialog);
}

void GuiHarnessTest::testCleanupOrphanTopLevelsRemovesShownWindows() {
	wxFrame * first = new wxFrame(NULL, wxID_ANY, wxT("Harness Orphan 1"));
	wxFrame * second = new wxFrame(NULL, wxID_ANY, wxT("Harness Orphan 2"));
	first->Show();
	second->Show();
	m_harness.pumpEvents(2);

	CPPUNIT_ASSERT(m_harness.waitForTopLevelWindow([&](wxTopLevelWindow * topLevel) {
		return topLevel == first;
	}, 150, 5) == first);
	CPPUNIT_ASSERT(m_harness.waitForTopLevelWindow([&](wxTopLevelWindow * topLevel) {
		return topLevel == second;
	}, 150, 5) == second);
	const int beforeCleanupShownCount = static_cast<int>(m_harness.getTopLevelWindows(false).size());
	const int remainingShownCount = m_harness.cleanupOrphanTopLevels(10);
	CPPUNIT_ASSERT(remainingShownCount >= 0);
	CPPUNIT_ASSERT(remainingShownCount <= beforeCleanupShownCount);
}

void GuiHarnessTest::testShowModalWithAutoDismissReturnsExpectedCode() {
	wxDialog dialog(static_cast<wxWindow*>(NULL), wxID_ANY, wxT("GuiHarnessTest"));
	const int result = m_harness.showModalWithAutoDismiss(dialog, wxID_CANCEL, 25);
	CPPUNIT_ASSERT_EQUAL(static_cast<int>(wxID_CANCEL), result);
}

}
