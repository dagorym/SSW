/**
 * @file BattleSim.cpp
 * @brief main() for the BattleSim game
 * @author Tom Stephens
 * @date Created:  May 31, 2010
 * @date Last Modified:  May 31, 2010
 *
 */

#include "FBattleSimApp.h"
#include "BattleSim.h"
#include "battleSim/BattleSimFrame.h"
#include <wx/wx.h>

BEGIN_EVENT_TABLE(BattleSimFrame, wxFrame)
	// menu events
	EVT_MENU (ID_Quit,            BattleSimFrame::onQuit)
END_EVENT_TABLE()

IMPLEMENT_APP(FBattleSimApp)
