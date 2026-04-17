/**
 * @file FApp.cpp
 * @brief Implementation file for FApp class
 * @author Tom Stephens
 * @date Created:  Feb 28, 2005
 *
 */
#include "FApp.h"
#include "FMainFrame.h"
#include "gui/WXStartupLaunch.h"
#include <wx/wx.h>

using namespace Frontier;

FApp::FApp() {
}

FApp::~FApp() {
}

bool FApp::OnInit() {
	FMainFrame *frame = static_cast<FMainFrame*>(createStartupSplashAndFrame(
			*this,
			[]() -> wxFrame* {
				return new FMainFrame("Frontier - Second Sathar War",
						wxPoint(50, 50),
						wxSize(760, 800));
			}));
	if (frame == nullptr) {
		return false;
	}

	// Ensure File->Exit menu stops the main loop even if Close() alone doesn't.
	// Bind the standard exit id (wxID_EXIT) on the frame to explicitly close the frame
	// and call ExitMainLoop to guarantee the process will exit.
	frame->Bind(wxEVT_MENU,
		[frame](wxCommandEvent& evt){
			// Ask frame to close (fires Close event and runs any veto logic)
			frame->Close(true);
			// Ensure the main loop stops if it wasn't already stopped by frame destruction
			if (wxTheApp) wxTheApp->ExitMainLoop();
			evt.Skip();
		},
		wxID_EXIT);

	return true;
}
