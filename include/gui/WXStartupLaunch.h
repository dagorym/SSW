/**
 * @file WXStartupLaunch.h
 * @brief Shared startup splash/frame launch helpers for wx apps.
 * @author GitHub Copilot, Tom Stephens
 * @date Created: Apr 16, 2026
 * @date Last Modified: Apr 17, 2026
 */

#ifndef _WXSTARTUPLAUNCH_H_
#define _WXSTARTUPLAUNCH_H_

#include "core/FGameConfig.h"
#include <wx/app.h>
#include <wx/frame.h>
#include <wx/splash.h>

namespace Frontier {

typedef wxFrame* (*FStartupFrameFactory)();

/**
 * @brief Create the startup splash and initial frame for a wx app.
 *
 * Loads the splash image through FGameConfig::resolveAssetPath(...), shows
 * the splash when available, constructs the app's startup frame through the
 * factory callback, centers and shows that frame, and registers it as the
 * app's top-level window.
 *
 * @param app wxApp instance that owns the startup frame.
 * @param createFrame Callback that constructs the startup frame.
 * @param splashTimeout Splash timeout in milliseconds.
 * @return The created startup frame, or nullptr when frame creation fails.
 *
 * @author GitHub Copilot, Tom Stephens
 * @date Created: Apr 16, 2026
 * @date Last Modified: Apr 17, 2026
 */
inline wxFrame* createStartupSplashAndFrame(wxApp& app,
			FStartupFrameFactory createFrame,
			const int splashTimeout = 2000) {
	wxInitAllImageHandlers(); // despite the documentation saying this is not needed, it actually is.
	wxBitmap bitmap;
	FGameConfig &gc = FGameConfig::create();
	if (bitmap.LoadFile(gc.resolveAssetPath("data/splash.png"), wxBITMAP_TYPE_PNG)) {
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
