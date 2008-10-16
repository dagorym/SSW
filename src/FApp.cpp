/**
 * @file FApp.cpp
 * @brief Implementation file for FApp class
 * @author Tom Stephens
 * @date Created:  Feb 28, 2005
 * 
 */
#include "FApp.h"
//#include "FGame.h"
#include "FMainFrame.h"
#include <wx/splash.h>
using namespace Frontier;

FApp::FApp() {
}

FApp::~FApp() {
}

bool FApp::OnInit() {
	// Draw splash screen
	wxInitAllImageHandlers();
	wxBitmap bitmap;
	if (bitmap.LoadFile("../data/splash.png", wxBITMAP_TYPE_PNG))
	{
		wxSplashScreen* splash = new wxSplashScreen(bitmap,
				wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT,
				//          100
				2000
				, NULL, -1, wxDefaultPosition, wxDefaultSize,
				wxSIMPLE_BORDER|wxSTAY_ON_TOP);
		splash->GetTimeout();  // do something to use the variable to get rid of compiler warning.
	}

	FMainFrame *frame = new FMainFrame( "Frontier - Second Sathar War"
			, wxPoint(50,50), wxSize(760,800) );
	frame->Show( TRUE );
	SetTopWindow( frame );


	return TRUE;
}
