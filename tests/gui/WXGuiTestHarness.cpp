/**
 * @file WXGuiTestHarness.cpp
 * @brief Shared wx bootstrap, event pump, and modal-dismiss utility for GUI tests
 */

#include "WXGuiTestHarness.h"

#include <wx/app.h>
#include <wx/debug.h>
#include <wx/dialog.h>
#include <wx/toplevel.h>
#include <wx/timer.h>
#include <wx/utils.h>

namespace FrontierTests {

class WXGuiHarnessApp : public wxApp {
public:
	virtual bool OnInit() wxOVERRIDE {
		return true;
	}
};

wxIMPLEMENT_APP_NO_MAIN(WXGuiHarnessApp);

namespace {

void IgnoreWxAssertHandler(const wxString &,
                           int,
                           const wxString &,
                           const wxString &,
                           const wxString &) {
}

class ModalDismissTimer : public wxTimer {
private:
	wxDialog * m_dialog;
	int m_returnCode;

public:
	ModalDismissTimer(wxDialog * dialog, int returnCode) : m_dialog(dialog), m_returnCode(returnCode) {
	}

	virtual void Notify() wxOVERRIDE;
};

class AnyModalDismissTimer : public wxTimer {
private:
	int m_returnCode;
	wxDialog * m_lastClosedDialog;

public:
	explicit AnyModalDismissTimer(int returnCode) : m_returnCode(returnCode), m_lastClosedDialog(NULL) {
	}

	virtual void Notify() wxOVERRIDE;
};

void ModalDismissTimer::Notify() {
	if (m_dialog != NULL && m_dialog->IsModal()) {
		m_dialog->EndModal(m_returnCode);
	}
}

void AnyModalDismissTimer::Notify() {
	wxWindow * active = wxGetActiveWindow();
	wxDialog * activeDialog = dynamic_cast<wxDialog *>(active);
	if (activeDialog != NULL && activeDialog->IsModal()) {
		if (activeDialog == m_lastClosedDialog) {
			return;
		}
		m_lastClosedDialog = activeDialog;
		activeDialog->EndModal(m_returnCode);
		return;
	}

	wxWindowList::compatibility_iterator node = wxTopLevelWindows.GetFirst();
	wxDialog * lastModalDialog = NULL;
	while (node != NULL) {
		wxTopLevelWindow * topLevel = dynamic_cast<wxTopLevelWindow *>(node->GetData());
		if (topLevel != NULL) {
			wxDialog * dialog = dynamic_cast<wxDialog *>(topLevel);
			if (dialog != NULL && dialog->IsModal()) {
				lastModalDialog = dialog;
			}
		}
		node = node->GetNext();
	}
	if (lastModalDialog != NULL) {
		if (lastModalDialog == m_lastClosedDialog) {
			return;
		}
		m_lastClosedDialog = lastModalDialog;
		lastModalDialog->EndModal(m_returnCode);
	} else {
		m_lastClosedDialog = NULL;
	}
}

}

WXGuiTestHarness::WXGuiTestHarness() : m_bootstrapped(false), m_startedWx(false) {
}

WXGuiTestHarness::~WXGuiTestHarness() {
	shutdown();
}

bool WXGuiTestHarness::bootstrap() {
	if (m_bootstrapped) {
		return true;
	}

	static bool assertHandlerInstalled = false;
	if (!assertHandlerInstalled) {
		wxSetAssertHandler(IgnoreWxAssertHandler);
		assertHandlerInstalled = true;
	}

	if (wxTheApp != NULL) {
		m_bootstrapped = true;
		m_startedWx = false;
		return true;
	}

	int argc = 0;
	char ** argv = NULL;
	if (!wxEntryStart(argc, argv)) {
		return false;
	}

	if (wxTheApp == NULL || !wxTheApp->CallOnInit()) {
		wxEntryCleanup();
		return false;
	}

	m_bootstrapped = true;
	m_startedWx = true;
	return true;
}

void WXGuiTestHarness::shutdown() {
	if (!m_bootstrapped) {
		return;
	}

	cleanupOrphanTopLevels(5);

	if (m_startedWx && wxTheApp != NULL) {
		wxTheApp->OnExit();
	}
	if (m_startedWx) {
		wxEntryCleanup();
	}
	m_bootstrapped = false;
	m_startedWx = false;
}

void WXGuiTestHarness::pumpEvents(int iterations) {
	if (!m_bootstrapped || wxTheApp == NULL) {
		return;
	}

	for (int i = 0; i < iterations; i++) {
		while (wxTheApp->Pending()) {
			wxTheApp->Dispatch();
		}
		wxTheApp->ProcessPendingEvents();
	}
}

std::vector<wxTopLevelWindow *> WXGuiTestHarness::getTopLevelWindows(bool includeBeingDeleted) const {
	std::vector<wxTopLevelWindow *> topLevels;
	wxWindowList::compatibility_iterator node = wxTopLevelWindows.GetFirst();
	while (node != NULL) {
		wxTopLevelWindow * topLevel = dynamic_cast<wxTopLevelWindow *>(node->GetData());
		if (topLevel != NULL && (includeBeingDeleted || !topLevel->IsBeingDeleted())) {
			topLevels.push_back(topLevel);
		}
		node = node->GetNext();
	}
	return topLevels;
}

wxTopLevelWindow * WXGuiTestHarness::findTopLevelWindow(const std::function<bool(wxTopLevelWindow *)> & predicate,
                                                        bool includeBeingDeleted) const {
	const std::vector<wxTopLevelWindow *> topLevels = getTopLevelWindows(includeBeingDeleted);
	for (std::vector<wxTopLevelWindow *>::const_iterator itr = topLevels.begin();
	     itr != topLevels.end();
	     ++itr) {
		if (predicate(*itr)) {
			return *itr;
		}
	}
	return NULL;
}

wxTopLevelWindow * WXGuiTestHarness::waitForTopLevelWindow(const std::function<bool(wxTopLevelWindow *)> & predicate,
                                                           int timeoutMs,
                                                           int pollMs) {
	if (!m_bootstrapped && !bootstrap()) {
		return NULL;
	}

	const int safePollMs = (pollMs > 0) ? pollMs : 1;
	int elapsedMs = 0;
	while (elapsedMs <= timeoutMs) {
		wxTopLevelWindow * found = findTopLevelWindow(predicate);
		if (found != NULL) {
			return found;
		}
		pumpEvents(1);
		if (elapsedMs < timeoutMs) {
			wxMilliSleep(static_cast<unsigned long>(safePollMs));
		}
		elapsedMs += safePollMs;
	}
	return NULL;
}

wxDialog * WXGuiTestHarness::findModalDialog() const {
	wxTopLevelWindow * topLevel = findTopLevelWindow([](wxTopLevelWindow * window) {
		wxDialog * dialog = dynamic_cast<wxDialog *>(window);
		return dialog != NULL && dialog->IsModal();
	});
	return dynamic_cast<wxDialog *>(topLevel);
}

wxDialog * WXGuiTestHarness::waitForModalDialog(int timeoutMs, int pollMs) {
	wxTopLevelWindow * topLevel = waitForTopLevelWindow([](wxTopLevelWindow * window) {
		wxDialog * dialog = dynamic_cast<wxDialog *>(window);
		return dialog != NULL && dialog->IsModal();
	}, timeoutMs, pollMs);
	return dynamic_cast<wxDialog *>(topLevel);
}

int WXGuiTestHarness::cleanupOrphanTopLevels(int pumpIterations) {
	const std::vector<wxTopLevelWindow *> topLevels = getTopLevelWindows(false);
	for (std::vector<wxTopLevelWindow *>::const_iterator itr = topLevels.begin();
	     itr != topLevels.end();
	     ++itr) {
		if (*itr != NULL && !(*itr)->IsBeingDeleted() && (*itr)->IsShown()) {
			(*itr)->Destroy();
		}
	}
	pumpEvents(pumpIterations);

	int shownTopLevels = 0;
	const std::vector<wxTopLevelWindow *> remainingTopLevels = getTopLevelWindows(false);
	for (std::vector<wxTopLevelWindow *>::const_iterator itr = remainingTopLevels.begin();
	     itr != remainingTopLevels.end();
	     ++itr) {
		if (*itr != NULL && (*itr)->IsShown()) {
			shownTopLevels++;
		}
	}
	return shownTopLevels;
}

int WXGuiTestHarness::showModalWithAutoDismiss(wxDialog & dialog, int returnCode, int timeoutMs) {
	if (!m_bootstrapped && !bootstrap()) {
		return wxID_NONE;
	}

	ModalDismissTimer dismissTimer(&dialog, returnCode);
	dismissTimer.Start(timeoutMs, true);
	const int result = dialog.ShowModal();
	dismissTimer.Stop();
	pumpEvents(2);
	return result;
}

int WXGuiTestHarness::showModalWithAction(wxDialog & dialog,
                                          const std::function<void()> & action,
                                          int fallbackCode,
                                          int timeoutMs) {
	if (!m_bootstrapped && !bootstrap()) {
		return wxID_NONE;
	}

	ModalDismissTimer dismissTimer(&dialog, fallbackCode);
	dismissTimer.Start(timeoutMs, true);
	dialog.CallAfter(action);
	const int result = dialog.ShowModal();
	dismissTimer.Stop();
	pumpEvents(2);
	return result;
}

int WXGuiTestHarness::runModalFunctionWithAutoDismiss(const std::function<int()> & callback,
                                                      int returnCode,
                                                      int timeoutMs) {
	if (!m_bootstrapped && !bootstrap()) {
		return wxID_NONE;
	}

	AnyModalDismissTimer dismissTimer(returnCode);
	dismissTimer.Start(timeoutMs, false);
	const int result = callback();
	dismissTimer.Stop();
	pumpEvents(2);
	return result;
}

int WXGuiTestHarness::runModalFunctionWithAction(const std::function<int()> & callback,
                                                 const std::function<void()> & action,
                                                 int fallbackCode,
                                                 int timeoutMs) {
	if (!m_bootstrapped && !bootstrap()) {
		return wxID_NONE;
	}

	AnyModalDismissTimer dismissTimer(fallbackCode);
	dismissTimer.Start(timeoutMs, false);
	wxTheApp->CallAfter(action);
	const int result = callback();
	dismissTimer.Stop();
	pumpEvents(2);
	return result;
}

void WXGuiTestHarness::runVoidFunctionWithAutoDismiss(const std::function<void()> & callback,
                                                      int returnCode,
                                                      int timeoutMs) {
	if (!m_bootstrapped && !bootstrap()) {
		return;
	}

	AnyModalDismissTimer dismissTimer(returnCode);
	dismissTimer.Start(timeoutMs, false);
	callback();
	dismissTimer.Stop();
	pumpEvents(2);
}

void WXGuiTestHarness::runVoidFunctionWithAction(const std::function<void()> & callback,
                                                 const std::function<void()> & action,
                                                 int fallbackCode,
                                                 int timeoutMs) {
	if (!m_bootstrapped && !bootstrap()) {
		return;
	}

	AnyModalDismissTimer dismissTimer(fallbackCode);
	dismissTimer.Start(timeoutMs, false);
	wxTheApp->CallAfter(action);
	callback();
	dismissTimer.Stop();
	pumpEvents(2);
}

}
