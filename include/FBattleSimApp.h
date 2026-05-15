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
 * @author Tom Stephens, gpt-5.3-codex (medium)
 * @date Created:  May 31, 2010
 * @date Last Modified:  Apr 16, 2026
 */
class FBattleSimApp : public wxApp {
public:
  /// Default constructor
	FBattleSimApp();
  /// Default destructor
	virtual ~FBattleSimApp();
  /**
   * @brief Initialize the BattleSim startup flow
   *
   * Creates the startup splash screen and main BattleSim frame.
   *
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created:  May 31, 2010
   * @date Last Modified:  Apr 16, 2026
   */
  virtual bool OnInit();
};


#endif //_FBATTLESIMAPP_H_
