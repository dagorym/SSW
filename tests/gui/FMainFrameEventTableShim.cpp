/**
 * @file FMainFrameEventTableShim.cpp
 * @brief Provides FMainFrame event-table symbols for GUI test linking.
 */

#include <wx/wx.h>

#include "FMainFrame.h"
#include "wxWidgets.h"

BEGIN_EVENT_TABLE(FMainFrame, wxFrame)
	EVT_MENU (ID_Quit,            FMainFrame::onQuit)
	EVT_MENU (ID_About,           FMainFrame::onAbout)
	EVT_MENU (ID_Open,            FMainFrame::onOpen)
	EVT_MENU (ID_Save,            FMainFrame::onSave)
	EVT_MENU (ID_New,             FMainFrame::onNew)
	EVT_MENU (ID_ShowPlayers,     FMainFrame::onShowPlayers)
	EVT_MENU (ID_EndUPFTurn,      FMainFrame::onEndUPFTurn)
	EVT_MENU (ID_EndSatharTurn,   FMainFrame::onEndSatharTurn)
	EVT_MENU (ID_PlaceNova,       FMainFrame::onPlaceNova)
	EVT_MENU (ID_Close,           FMainFrame::onClose)
	EVT_MENU (ID_AddSatharShips,  FMainFrame::onAddSatharShips)
	EVT_MENU (ID_ShowRetreatCond, FMainFrame::onShowRetreatConditions)
	EVT_MENU (ID_ShowBattleScreen, FMainFrame::onShowBattleScreen)
	EVT_CLOSE(FMainFrame::onCloseWindow)
	EVT_LEFT_DCLICK(FMainFrame::onLeftDClick)
	EVT_LEFT_UP(FMainFrame::onLeftUp)
	EVT_SIZE(FMainFrame::onSize)
END_EVENT_TABLE()
