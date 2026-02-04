/**
 * @file FApp.cpp
 * @brief Implementation file for FApp class
 * @author Tom Stephens
 * @date Created:  Feb 28, 2005
 *
 */
#include "FApp.h"
#include "FMainFrame.h"
#include "core/FGameConfig.h"
#include <wx/splash.h>
#include <wx/wx.h>

using namespace Frontier;

FApp::FApp() {
}

FApp::~FApp() {
}

bool FApp::OnInit() {
	// Draw splash screen
	wxInitAllImageHandlers(); // is no longer needed in wxWidgets 3.1 +
	wxBitmap bitmap;
	FGameConfig &gc = FGameConfig::create();
	if (bitmap.LoadFile(gc.getBasePath()+"data/splash.png", wxBITMAP_TYPE_PNG))
	{
		wxSplashScreen* splash = new wxSplashScreen(bitmap,
				wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT,
				//          100
				2000
				, nullptr, wxID_ANY, wxDefaultPosition, wxDefaultSize,
				wxSIMPLE_BORDER|wxSTAY_ON_TOP);
		splash->GetTimeout();  // do something to use the variable to get rid of compiler warning.
	}

	FMainFrame *frame = new FMainFrame( "Frontier - Second Sathar War"
			, wxPoint(50,50), wxSize(760,800) );
	frame->Show( true );
	SetTopWindow( frame );

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
