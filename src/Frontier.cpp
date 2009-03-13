/**
 * @file Frontier.cpp
 * @brief main() for the Frontier game
 * @author Tom Stephens
 * @date Created:  Jan 12, 2005
 * @date Last Modified:  Feb 19, 2008
 *
 */

#include "FApp.h"
#include "FMainFrame.h"
#include "FBattleScreen.h"
#include "Frontier.h"
#include "ships.h"
#include "weapons.h"
#include <wx/wx.h>

BEGIN_EVENT_TABLE(FMainFrame, wxFrame)
	// menu events
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

    // Mouse Events
    EVT_LEFT_DCLICK(FMainFrame::onLeftDClick)
    EVT_LEFT_UP    (FMainFrame::onLeftUp)
	EVT_SIZE       (FMainFrame::onSize)

END_EVENT_TABLE()

IMPLEMENT_APP(FApp)

using namespace Frontier;
FVehicle * createShip(std::string type){
	FVehicle *v;
	if(type == "AssaultScout"){
		v = new FAssaultScout;
	} else if (type == "Fighter") {
		v = new FFighter;
	} else if (type == "AssaultCarrier") {
		v = new FAssaultCarrier;
	} else if (type == "Battleship") {
		v = new FBattleship;
	} else if (type == "Destroyer") {
		v = new FDestroyer;
	} else if (type == "Frigate") {
		v = new FFrigate;
	} else if (type == "HvCruiser") {
		v = new FHvCruiser;
	} else if (type == "LtCruiser") {
		v = new FLtCruiser;
	} else if (type == "Minelayer") {
		v = new FMinelayer;
	} else if (type == "Fortress") {
		v = new FFortress;
	} else if (type == "FortifiedStation") {
		v = new FFortifiedStation;
	} else if (type == "ArmedStation") {
		v = new FArmedStation;
	} else {
		// there was an error
	}
	return v;
}

FWeapon * createWeapon(int type){
	FWeapon *w;
	if(type == LB){
		w = new FLaserBattery;
	} else if (type == LC) {
		w = new FLaserCannon;
	} else if (type == EB) {
		w = new FElectronBattery;
	} else if (type == PB) {
		w = new FProtonBattery;
	} else if (type == RB) {
		w = new FRocketBattery;
	} else if (type == DC) {
		w = new FDisruptorCannon;
	} else if (type == T) {
		w = new FTorpedo;
	} else if (type == AR) {
		w = new FAssaultRocket;
	} else if (type == SM) {
		w = new FSeekerMissileLauncher;
	} else if (type == M) {
		w = new FMineLauncher;
	} else {
		// there was an error
	}
	return w;
}
