/**
 * @file FBattleSimApp.cpp
 * @brief Implementation file for FApp class
 * @author Tom Stephens
 * @date Created:  May 31, 2010
 *
 */
#include "FBattleSimApp.h"
#include "battleSim/BattleSimFrame.h"
#include "gui/WXStartupLaunch.h"
using namespace Frontier;

FBattleSimApp::FBattleSimApp() {
}

FBattleSimApp::~FBattleSimApp() {
}

bool FBattleSimApp::OnInit() {
	BattleSimFrame *frame = static_cast<BattleSimFrame*>(createStartupSplashAndFrame(
			*this,
			[]() -> wxFrame* {
				return new BattleSimFrame();
			}));
	if (frame == nullptr) {
		return false;
	}


	return true;
}
