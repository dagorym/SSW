/**
 * @file WXGuiTestHarness.cpp
 * @brief Shared wx bootstrap, event pump, and modal-dismiss utility for GUI tests
 */

#include "WXGuiTestHarness.h"

#include <wx/app.h>
#include <wx/dialog.h>
#include <wx/toplevel.h>
#include <wx/timer.h>

namespace FrontierTests {

class WXGuiHarnessApp : public wxApp {
public:
	virtual bool OnInit() wxOVERRIDE {
		return true;
	}
};

wxIMPLEMENT_APP_NO_MAIN(WXGuiHarnessApp);

namespace {

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

public:
	explicit AnyModalDismissTimer(int returnCode) : m_returnCode(returnCode) {
	}

	virtual void Notify() wxOVERRIDE;
};

void ModalDismissTimer::Notify() {
	if (m_dialog != NULL && m_dialog->IsModal()) {
		m_dialog->EndModal(m_returnCode);
	}
}

void AnyModalDismissTimer::Notify() {
	wxWindowList::compatibility_iterator node = wxTopLevelWindows.GetFirst();
	while (node != NULL) {
		wxTopLevelWindow * topLevel = dynamic_cast<wxTopLevelWindow *>(node->GetData());
		if (topLevel != NULL) {
			wxDialog * dialog = dynamic_cast<wxDialog *>(topLevel);
			if (dialog != NULL && dialog->IsModal()) {
				dialog->EndModal(m_returnCode);
				return;
			}
		}
		node = node->GetNext();
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

	wxWindowList::compatibility_iterator node = wxTopLevelWindows.GetFirst();
	while (node != NULL) {
		wxTopLevelWindow * topLevel = dynamic_cast<wxTopLevelWindow *>(node->GetData());
		node = node->GetNext();
		if (topLevel != NULL && !topLevel->IsBeingDeleted()) {
			topLevel->Destroy();
		}
	}
	pumpEvents(5);

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

}
