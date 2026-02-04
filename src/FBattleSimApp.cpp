/**
 * @file FBattleSimApp.cpp
 * @brief Implementation file for FApp class
 * @author Tom Stephens
 * @date Created:  May 31, 2010
 *
 */
#include "FBattleSimApp.h"
#include "battleSim/BattleSimFrame.h"
#include "core/FGameConfig.h"
#include <wx/splash.h>
using namespace Frontier;

FBattleSimApp::FBattleSimApp() {
}

FBattleSimApp::~FBattleSimApp() {
}

bool FBattleSimApp::OnInit() {
	// Draw splash screen
	wxInitAllImageHandlers();
	wxBitmap bitmap;
	FGameConfig &gc = FGameConfig::create();
	if (bitmap.LoadFile(gc.getBasePath() + "data/splash.png", wxBITMAP_TYPE_PNG))
	{
		wxSplashScreen* splash = new wxSplashScreen(bitmap,
				wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT,
				//          100
				2000
				, nullptr, wxID_ANY, wxDefaultPosition, wxDefaultSize,
				wxSIMPLE_BORDER|wxSTAY_ON_TOP);
		splash->GetTimeout();  // do something to use the variable to get rid of compiler warning.
	}

	BattleSimFrame *frame = new BattleSimFrame();
	frame->Show( true );
	SetTopWindow( frame );


	return true;
}
