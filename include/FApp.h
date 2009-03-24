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
 * @author Tom Stephens
 * @date Created:  Feb 28, 2005
 * @date Last Modified:  Feb 28, 2005
 */
class FApp : public wxApp {
public:
  // Default constuctor
	FApp();
  /// Default destructor
	virtual ~FApp();
  /**
   * @brief Method to initalize the program
   *
   * This method handles all the setup of the program
   *
   * @author Tom Stephens
   * @date Created:  Feb 28, 2005
   * @date Last Modified:  Mar 01, 2005
   */
  virtual bool OnInit();
};


#endif //_FAPP_H_
