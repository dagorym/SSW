/**
 * @file WXGuiTestHarness.cpp
 * @brief Shared wx bootstrap, event pump, and modal-dismiss utility for GUI tests
 */

#include "WXGuiTestHarness.h"

#include <wx/app.h>
#include <wx/dialog.h>
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

void ModalDismissTimer::Notify() {
	if (m_dialog != NULL && m_dialog->IsModal()) {
		m_dialog->EndModal(m_returnCode);
	}
}

}

WXGuiTestHarness::WXGuiTestHarness() : m_bootstrapped(false) {
}

WXGuiTestHarness::~WXGuiTestHarness() {
	shutdown();
}

bool WXGuiTestHarness::bootstrap() {
	if (m_bootstrapped) {
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
	return true;
}

void WXGuiTestHarness::shutdown() {
	if (!m_bootstrapped) {
		return;
	}

	if (wxTheApp != NULL) {
		wxTheApp->OnExit();
	}
	wxEntryCleanup();
	m_bootstrapped = false;
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

}
