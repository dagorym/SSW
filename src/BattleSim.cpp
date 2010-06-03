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
//    EVT_MENU (ID_About,           FMainBattleSimFrame::onAbout)
//    EVT_MENU (ID_Open,            FMainBattleSimFrame::onOpen)
//    EVT_MENU (ID_Save,            FMainBattleSimFrame::onSave)
//    EVT_MENU (ID_New,             FMainBattleSimFrame::onNew)
//    EVT_MENU (ID_ShowPlayers,     FMainBattleSimFrame::onShowPlayers)
//    EVT_MENU (ID_EndUPFTurn,      FMainBattleSimFrame::onEndUPFTurn)
//    EVT_MENU (ID_EndSatharTurn,   FMainBattleSimFrame::onEndSatharTurn)
//    EVT_MENU (ID_PlaceNova,       FMainBattleSimFrame::onPlaceNova)
//    EVT_MENU (ID_Close,           FMainBattleSimFrame::onClose)
//    EVT_MENU (ID_AddSatharShips,  FMainBattleSimFrame::onAddSatharShips)
//    EVT_MENU (ID_ShowRetreatCond, FMainBattleSimFrame::onShowRetreatConditions)
//    EVT_MENU (ID_ShowBattleScreen, FMainBattleSimFrame::onShowBattleScreen)
//
//    // Mouse Events
//    EVT_LEFT_DCLICK(FMainBattleSimFrame::onLeftDClick)
//    EVT_LEFT_UP    (FMainBattleSimFrame::onLeftUp)
//	EVT_SIZE       (FMainBattleSimFrame::onSize)

END_EVENT_TABLE()

IMPLEMENT_APP(FBattleSimApp)
