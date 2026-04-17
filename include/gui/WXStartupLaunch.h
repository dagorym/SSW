/**
 * @file WXStartupLaunch.h
 * @brief Shared startup splash/frame launch helpers for wx apps
 * @author GitHub Copilot
 * @date Created: Apr 16, 2026
 */

#ifndef _WXSTARTUPLAUNCH_H_
#define _WXSTARTUPLAUNCH_H_

#include "core/FGameConfig.h"
#include <wx/app.h>
#include <wx/frame.h>
#include <wx/splash.h>

namespace Frontier {

typedef wxFrame* (*FStartupFrameFactory)();

inline wxFrame* createStartupSplashAndFrame(wxApp& app,
		FStartupFrameFactory createFrame,
		const int splashTimeout = 2000) {
	wxInitAllImageHandlers();
	wxBitmap bitmap;
	FGameConfig &gc = FGameConfig::create();
	if (bitmap.LoadFile(gc.getBasePath() + "data/splash.png", wxBITMAP_TYPE_PNG)) {
		wxSplashScreen* splash = new wxSplashScreen(bitmap,
				wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_TIMEOUT,
				splashTimeout,
				nullptr,
				wxID_ANY,
				wxDefaultPosition,
				wxDefaultSize,
				wxSIMPLE_BORDER | wxFRAME_NO_TASKBAR | wxSTAY_ON_TOP);
		splash->GetTimeout();
	}

	if (createFrame == nullptr) {
		return nullptr;
	}

	wxFrame *frame = createFrame();
	if (frame == nullptr) {
		return nullptr;
	}

	frame->CentreOnScreen(wxBOTH);
	frame->Show(true);
	app.SetTopWindow(frame);
	return frame;
}

} // namespace Frontier

#endif // _WXSTARTUPLAUNCH_H_
