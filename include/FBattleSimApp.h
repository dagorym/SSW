/**
 * @file FBattleSimApp.h
 * @brief Header file for FBattleSimApp class
 * @author Tom Stephens
 * @date Created:  May 31, 2010
 *
 */

#ifndef _FBATTLESIMAPP_H_
#define _FBATTLESIMAPP_H_

#include "core/FObject.h"
#include <wx/wx.h>

/**
 * @brief Main game class for the SF BattleSim game
 *
 * This class is the main GUI for the SF BattleSim game.
 *
 * @author Tom Stephens
 * @date Created:  May 31, 2010
 * @date Last Modified:  May 31, 2010
 */
class FBattleSimApp : public wxApp {
public:
  // Default constuctor
	FBattleSimApp();
  /// Default destructor
	virtual ~FBattleSimApp();
  /**
   * @brief Method to initalize the program
   *
   * This method handles all the setup of the program
   *
   * @author Tom Stephens
   * @date Created:  Feb May 31, 2010
   * @date Last Modified:  May 31, 2010
   */
  virtual bool OnInit();
};


#endif //_FBATTLESIMAPP_H_
