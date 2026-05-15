/**
 * @file FApp.h
 * @brief Header file for FApp class
 * @author Tom Stephens
 * @date Created:  Feb 28, 2005
 *
 */

#ifndef _FAPP_H_
#define _FAPP_H_

#include "core/FObject.h"
#include <wx/wx.h>

/**
 * @brief Main game class for the SSW game
 *
 * This class is the main GUI for the SSW game.
 *
 * @author Tom Stephens, gpt-5.3-codex (medium)
 * @date Created:  Feb 28, 2005
 * @date Last Modified:  Apr 16, 2026
 */
class FApp : public wxApp {
public:
  /// Default constructor
	FApp();
  /// Default destructor
	virtual ~FApp();
  /**
   * @brief Initialize the application startup flow
   *
   * Creates the startup splash screen and main frame, then installs the exit
   * handler for the main loop.
   *
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created:  Feb 28, 2005
   * @date Last Modified:  Apr 16, 2026
   */
  virtual bool OnInit();
};


#endif //_FAPP_H_
