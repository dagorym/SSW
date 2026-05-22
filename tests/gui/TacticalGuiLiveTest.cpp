/**
 * @file TacticalGuiLiveTest.cpp
 * @brief Implementation file for live tactical GUI regression coverage
 */

#include "TacticalGuiLiveTest.h"

#include <wx/button.h>
#include <wx/display.h>
#include <wx/dialog.h>
#include <wx/menu.h>
#include <wx/panel.h>
#include <wx/spinctrl.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/toplevel.h>
#include <wx/uiaction.h>
#include <wx/utils.h>
#include <wx/window.h>

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "gui/ICMSelectionGUI.h"
#include "gui/TacticalDamageSummaryGUI.h"
#include "gui/WXTacticalUI.h"
#include "ships/FVehicle.h"
#include "strategic/FFleet.h"
#include "tactical/FBattleDisplay.h"
#include "tactical/FBattleScreen.h"
#include "tactical/FTacticalCombatReport.h"
#include "weapons/FWeapon.h"
#include "wxWidgets.h"

namespace FrontierTests {
using namespace Frontier;

namespace {

class InspectableWeapon : public FWeapon {
public:
InspectableWeapon() {
m_fullName = "Inspectable Torpedo";
m_type = FWeapon::T;
m_ICMMod = -10;
}

int assignedICMCount() const {
return m_assignedICMCount;
}
};

class ICMSelectionGUITestPeer : public ICMSelectionGUI {
public:
ICMSelectionGUITestPeer(wxWindow * parent, std::vector<ICMData *> * icmData)
: ICMSelectionGUI(parent, icmData) {
}

void selectWeaponRow(int row) {
wxGridEvent event(m_grid1->GetId(), wxEVT_GRID_SELECT_CELL, m_grid1, row, 0);
onPickWeapon(event);
}

size_t appliedControlCount() const {
return m_ICMsApplied.size();
}

wxSpinCtrl * appliedControl(int index) const {
CPPUNIT_ASSERT(index >= 0);
CPPUNIT_ASSERT(static_cast<size_t>(index) < m_ICMsApplied.size());
return m_ICMsApplied[index];
}

void setAppliedICMValue(int index, int value) {
CPPUNIT_ASSERT(index >= 0);
CPPUNIT_ASSERT(static_cast<size_t>(index) < m_ICMsApplied.size());
m_ICMsApplied[index]->SetValue(value);
wxSpinEvent event(wxEVT_COMMAND_SPINCTRL_UPDATED, m_ICMsApplied[index]->GetId());
event.SetEventObject(m_ICMsApplied[index]);
updateICMCount(event);
}

wxString assignedCellText(int row) const {
return m_grid1->GetCellValue(row, 2);
}

	void clickDone() {
		wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, m_button1->GetId());
		event.SetEventObject(m_button1);
		finalizeAssignments(event);
	}
};

class FBattleDisplayTestPeer : public FBattleDisplay {
public:
static int actionPromptTopMargin() {
	return ACTION_PROMPT_TOP_MARGIN;
}

static int actionPromptLineHeight() {
	return ACTION_PROMPT_LINE_HEIGHT;
}

static int actionPromptLineY(int lineIndex) {
	return ACTION_PROMPT_TOP_MARGIN + (lineIndex * ACTION_PROMPT_LINE_HEIGHT);
}

static int actionPromptReservedBottomY() {
	return ACTION_PROMPT_TOP_MARGIN
		+ (ACTION_PROMPT_LINE_HEIGHT * ACTION_PROMPT_MAX_LINES)
		+ ACTION_PROMPT_BUTTON_GAP;
}
};

wxTextCtrl * findFirstTextCtrl(wxWindow * root) {
if (root == NULL) {
return NULL;
}

wxTextCtrl * textCtrl = dynamic_cast<wxTextCtrl *>(root);
if (textCtrl != NULL) {
return textCtrl;
}

const wxWindowList & children = root->GetChildren();
for (wxWindowList::const_iterator itr = children.begin(); itr != children.end(); ++itr) {
wxTextCtrl * found = findFirstTextCtrl(*itr);
if (found != NULL) {
return found;
}
}
return NULL;
}

wxSpinCtrl * findFirstSpinCtrl(wxWindow * root) {
if (root == NULL) {
return NULL;
}

wxSpinCtrl * spinCtrl = dynamic_cast<wxSpinCtrl *>(root);
if (spinCtrl != NULL) {
return spinCtrl;
}

const wxWindowList & children = root->GetChildren();
for (wxWindowList::const_iterator itr = children.begin(); itr != children.end(); ++itr) {
wxSpinCtrl * found = findFirstSpinCtrl(*itr);
if (found != NULL) {
return found;
}
}
return NULL;
}

FBattleBoard * findFirstBattleBoard(wxWindow * root) {
if (root == NULL) {
return NULL;
}

FBattleBoard * battleBoard = dynamic_cast<FBattleBoard *>(root);
if (battleBoard != NULL) {
return battleBoard;
}

const wxWindowList & children = root->GetChildren();
for (wxWindowList::const_iterator itr = children.begin(); itr != children.end(); ++itr) {
FBattleBoard * found = findFirstBattleBoard(*itr);
if (found != NULL) {
return found;
}
}
return NULL;
}

FBattleDisplay * findFirstBattleDisplay(wxWindow * root) {
if (root == NULL) {
return NULL;
}

FBattleDisplay * battleDisplay = dynamic_cast<FBattleDisplay *>(root);
if (battleDisplay != NULL) {
return battleDisplay;
}

const wxWindowList & children = root->GetChildren();
for (wxWindowList::const_iterator itr = children.begin(); itr != children.end(); ++itr) {
FBattleDisplay * found = findFirstBattleDisplay(*itr);
if (found != NULL) {
return found;
}
}
return NULL;
}

wxStaticText * findStaticTextContaining(wxWindow * root, const wxString & token) {
if (root == NULL) {
return NULL;
}

wxStaticText * staticText = dynamic_cast<wxStaticText *>(root);
if (staticText != NULL && staticText->GetLabel().Find(token) != wxNOT_FOUND) {
return staticText;
}

const wxWindowList & children = root->GetChildren();
for (wxWindowList::const_iterator itr = children.begin(); itr != children.end(); ++itr) {
wxStaticText * found = findStaticTextContaining(*itr, token);
if (found != NULL) {
return found;
}
}
return NULL;
}

wxButton * findButtonByLabel(wxWindow * root, const wxString & label) {
if (root == NULL) {
return NULL;
}

wxButton * button = dynamic_cast<wxButton *>(root);
if (button != NULL && button->GetLabel() == label) {
return button;
}

const wxWindowList & children = root->GetChildren();
for (wxWindowList::const_iterator itr = children.begin(); itr != children.end(); ++itr) {
wxButton * found = findButtonByLabel(*itr, label);
if (found != NULL) {
return found;
}
}
return NULL;
}

wxButton * findButtonById(wxWindow * root, int id) {
if (root == NULL) {
return NULL;
}

wxButton * button = dynamic_cast<wxButton *>(root);
if (button != NULL && button->GetId() == id) {
return button;
}

const wxWindowList & children = root->GetChildren();
for (wxWindowList::const_iterator itr = children.begin(); itr != children.end(); ++itr) {
wxButton * found = findButtonById(*itr, id);
if (found != NULL) {
return found;
}
}
return NULL;
}

bool sourceContainsLineToken(const std::vector<std::string> & candidatePaths, const std::string & token) {
for (std::vector<std::string>::const_iterator itr = candidatePaths.begin(); itr != candidatePaths.end(); ++itr) {
std::ifstream source((*itr).c_str());
if (!source.is_open()) {
continue;
}

std::string line;
while (std::getline(source, line)) {
if (line.find(token) != std::string::npos) {
return true;
}
}
}
return false;
}

void assertDialogCenteredOnParent(wxDialog * dialog, wxWindow * parent, int tolerance = 120) {
	CPPUNIT_ASSERT(dialog != NULL);
	CPPUNIT_ASSERT(parent != NULL);
	bool centered = false;
	for (int attempt = 0; attempt < 70 && !centered; ++attempt) {
		if (wxTheApp != NULL) {
			wxTheApp->ProcessPendingEvents();
		}
		const wxRect parentBounds = parent->GetScreenRect();
		const wxRect dialogBounds = dialog->GetScreenRect();
		const wxPoint parentCenter(parentBounds.GetX() + (parentBounds.GetWidth() / 2),
		                           parentBounds.GetY() + (parentBounds.GetHeight() / 2));
		const wxPoint dialogCenter(dialogBounds.GetX() + (dialogBounds.GetWidth() / 2),
		                           dialogBounds.GetY() + (dialogBounds.GetHeight() / 2));
		const bool centeredOnParent = (std::abs(parentCenter.x - dialogCenter.x) <= tolerance)
		                           && (std::abs(parentCenter.y - dialogCenter.y) <= tolerance);
		bool centeredOnTopLevelParent = false;
		wxTopLevelWindow * topLevelParent = wxDynamicCast(wxGetTopLevelParent(parent), wxTopLevelWindow);
		if (topLevelParent != NULL && topLevelParent != parent) {
			const wxRect topLevelBounds = topLevelParent->GetScreenRect();
			const wxPoint topLevelCenter(topLevelBounds.GetX() + (topLevelBounds.GetWidth() / 2),
			                            topLevelBounds.GetY() + (topLevelBounds.GetHeight() / 2));
			centeredOnTopLevelParent = (std::abs(topLevelCenter.x - dialogCenter.x) <= tolerance)
			                        && (std::abs(topLevelCenter.y - dialogCenter.y) <= tolerance);
		}
		centered = centeredOnParent || centeredOnTopLevelParent;
		if (!centered) {
			wxMilliSleep(5);
		}
	}
	CPPUNIT_ASSERT(centered);
}

FTacticalCombatReportSummary buildSummaryWithLines() {
FTacticalCombatReportSummary summary;
summary.context.reportType = TRT_OffensiveFire;
summary.context.immediate = true;
summary.context.phase = 2;

FTacticalShipReportSummary shipSummary;
shipSummary.ship.shipID = 11;
shipSummary.ship.ownerID = 22;
shipSummary.ship.shipName = "Destroyer Alpha";
shipSummary.displayLines.push_back("Destroyer Alpha took 4 hull damage.");
shipSummary.displayLines.push_back("Destroyer Alpha suffered 1 internal hit.");
summary.ships.push_back(shipSummary);

FTacticalHitDetailSummary hitDetail;
hitDetail.displayLine = "Destroyer Alpha [Laser Battery] -> Sathar Frigate: 4 hull damage";
summary.hitDetails.push_back(hitDetail);

	return summary;
}

wxString stripMenuMnemonic(const wxString & label) {
	wxString stripped = label;
	stripped.Replace("&", "");
	return stripped;
}

}

CPPUNIT_TEST_SUITE_REGISTRATION( TacticalGuiLiveTest );

void TacticalGuiLiveTest::setUp() {
CPPUNIT_ASSERT(m_harness.bootstrap());
}

void TacticalGuiLiveTest::tearDown() {
	m_harness.cleanupOrphanTopLevels(10);
}

void TacticalGuiLiveTest::testWXTacticalUIParentBackedModalAndRedrawPaths() {
	std::cerr << "TACTICAL1:start" << std::endl;
	wxFrame * parent = new wxFrame(NULL, wxID_ANY, "WXTacticalUI Parent", wxDefaultPosition, wxSize(540, 420));
	wxPanel * redrawPanel = new wxPanel(parent, wxID_ANY);
	wxBoxSizer * parentSizer = new wxBoxSizer(wxVERTICAL);
	parentSizer->Add(redrawPanel, 1, wxEXPAND);
	parent->SetSizer(parentSizer);
	parent->Layout();
	parent->Show();
	redrawPanel->Show();
	m_harness.pumpEvents();

WXTacticalUI ui(redrawPanel);
	const FTacticalCombatReportSummary summary = buildSummaryWithLines();

	std::cerr << "TACTICAL1:damage" << std::endl;
	const int damageResult = m_harness.runModalFunctionWithAction([&]() {
		return ui.showDamageSummary(summary);
	}, [&]() {
		wxDialog * modal = m_harness.waitForModalDialog();
		CPPUNIT_ASSERT(modal != NULL);
		wxButton * closeButton = findButtonByLabel(modal, wxT("Close"));
		if (closeButton == NULL) {
			closeButton = findButtonByLabel(modal, wxT("OK"));
		}
		if (closeButton != NULL) {
			wxCommandEvent click(wxEVT_COMMAND_BUTTON_CLICKED, closeButton->GetId());
			click.SetEventObject(closeButton);
			closeButton->Command(click);
		} else {
			modal->EndModal(wxID_OK);
		}
	}, wxID_CANCEL, 150);
	CPPUNIT_ASSERT(damageResult == static_cast<int>(wxID_OK)
	    || damageResult == static_cast<int>(wxID_CANCEL));

	FTacticalCombatReportSummary noDetailSummary = buildSummaryWithLines();
	noDetailSummary.showHitDetails = false;
	bool noDetailCloseActionRan = false;
	bool noDetailDialogFound = false;
	bool noDetailCloseButtonFound = false;
	const int noDetailResult = m_harness.runModalFunctionWithAction([&]() {
		return ui.showDamageSummary(noDetailSummary);
	}, [&]() {
		noDetailCloseActionRan = true;
		wxDialog * modal = m_harness.waitForModalDialog();
		noDetailDialogFound = (modal != NULL);
		if (modal != NULL) {
			wxButton * closeButton = findButtonByLabel(modal, wxT("Close"));
			noDetailCloseButtonFound = (closeButton != NULL);
			if (closeButton != NULL) {
				wxCommandEvent click(wxEVT_COMMAND_BUTTON_CLICKED, closeButton->GetId());
				click.SetEventObject(closeButton);
				closeButton->Command(click);
			}
		}
	}, wxID_CANCEL, 150);
	CPPUNIT_ASSERT(noDetailCloseActionRan);
	CPPUNIT_ASSERT(noDetailDialogFound);
	CPPUNIT_ASSERT(noDetailCloseButtonFound);
	CPPUNIT_ASSERT_EQUAL(static_cast<int>(wxID_OK), noDetailResult);

	FTacticalCombatReportSummary emptySummary;
	emptySummary.context.reportType = TRT_None;
	bool emptyCloseActionRan = false;
	bool emptyDialogFound = false;
	bool emptyCloseButtonFound = false;
	const int emptyResult = m_harness.runModalFunctionWithAction([&]() {
		return ui.showDamageSummary(emptySummary);
	}, [&]() {
		emptyCloseActionRan = true;
		wxDialog * modal = m_harness.waitForModalDialog();
		emptyDialogFound = (modal != NULL);
		if (modal != NULL) {
			wxButton * closeButton = findButtonByLabel(modal, wxT("Close"));
			emptyCloseButtonFound = (closeButton != NULL);
			if (closeButton != NULL) {
				wxCommandEvent click(wxEVT_COMMAND_BUTTON_CLICKED, closeButton->GetId());
				click.SetEventObject(closeButton);
				closeButton->Command(click);
			}
		}
	}, wxID_CANCEL, 150);
	CPPUNIT_ASSERT(emptyCloseActionRan);
	CPPUNIT_ASSERT(emptyDialogFound);
	CPPUNIT_ASSERT(emptyCloseButtonFound);
	CPPUNIT_ASSERT_EQUAL(static_cast<int>(wxID_OK), emptyResult);

	WXTacticalUI noParentUI(NULL);
	const int parentlessResult = m_harness.runModalFunctionWithAction([&]() {
		return noParentUI.showDamageSummary(summary);
	}, [&]() {
		wxDialog * modal = m_harness.waitForModalDialog();
		CPPUNIT_ASSERT(modal != NULL);
		CPPUNIT_ASSERT(wxDisplay::GetFromWindow(modal) != wxNOT_FOUND);
		wxButton * closeButton = findButtonByLabel(modal, wxT("Close"));
		CPPUNIT_ASSERT(closeButton != NULL);
		wxCommandEvent click(wxEVT_COMMAND_BUTTON_CLICKED, closeButton->GetId());
		click.SetEventObject(closeButton);
		closeButton->Command(click);
	}, wxID_CANCEL, 150);
	CPPUNIT_ASSERT_EQUAL(static_cast<int>(wxID_OK), parentlessResult);

	FVehicle * attacker = createShip("Destroyer");
	FVehicle * target = createShip("Destroyer");
	FVehicle * defender = createShip("Frigate");
	CPPUNIT_ASSERT(attacker != NULL && target != NULL && defender != NULL);
	attacker->setOwner(1);
	target->setOwner(2);
	defender->setOwner(2);

	FWeapon * weapon = attacker->getWeapon(0);
	CPPUNIT_ASSERT(weapon != NULL);
	weapon->setTarget(target, 5);

VehicleList defenders;
defenders.push_back(target);
defenders.push_back(defender);
ICMData row;
row.weapon = weapon;
row.vehicles = &defenders;
std::vector<ICMData *> icmData;
icmData.push_back(&row);

	VehicleList noDefenders;
	std::cerr << "TACTICAL1:icm" << std::endl;
	const int icmResult = ui.runICMSelection(icmData, &noDefenders);
	CPPUNIT_ASSERT_EQUAL(1, icmResult);

	const int icmModalResult = m_harness.runModalFunctionWithAction([&]() {
		return ui.runICMSelection(icmData, &defenders);
	}, [&]() {
		wxDialog * modal = m_harness.waitForModalDialog();
		CPPUNIT_ASSERT(modal != NULL);
		wxButton * doneButton = findButtonByLabel(modal, wxT("Done"));
		CPPUNIT_ASSERT(doneButton != NULL);
		wxCommandEvent click(wxEVT_COMMAND_BUTTON_CLICKED, doneButton->GetId());
		click.SetEventObject(doneButton);
		doneButton->Command(click);
	}, wxID_CANCEL, 450);
	CPPUNIT_ASSERT_EQUAL(0, icmModalResult);

	std::cerr << "TACTICAL1:msg" << std::endl;
	bool messageActionRan = false;
	bool messageBodyFound = false;
	wxString messageTitle;
	m_harness.runVoidFunctionWithAction([&]() {
		ui.showMessage("Tactical Smoke", "Parent-backed tactical message");
	}, [&]() {
		messageActionRan = true;
		wxDialog * modal = m_harness.waitForModalDialog();
		CPPUNIT_ASSERT(modal != NULL);
		messageTitle = modal->GetTitle();
		messageBodyFound = (findStaticTextContaining(modal, wxT("Parent-backed tactical message")) != NULL);
		wxButton * okButton = findButtonById(modal, wxID_OK);
		if (okButton == NULL) {
			okButton = findButtonByLabel(modal, wxT("OK"));
		}
		CPPUNIT_ASSERT(okButton != NULL);
		wxCommandEvent click(wxEVT_COMMAND_BUTTON_CLICKED, okButton->GetId());
		click.SetEventObject(okButton);
		okButton->Command(click);
	}, wxID_CANCEL, 450);
	CPPUNIT_ASSERT(messageActionRan);
	CPPUNIT_ASSERT_EQUAL(wxString::FromUTF8("Tactical Smoke"), messageTitle);
	CPPUNIT_ASSERT(messageBodyFound);

	std::cerr << "TACTICAL1:winner" << std::endl;
	bool winnerActionRan = false;
	bool satharWinnerBodyFound = false;
	wxString winnerTitle;
	m_harness.runVoidFunctionWithAction([&]() {
		ui.notifyWinner(true);
	}, [&]() {
		winnerActionRan = true;
		wxDialog * modal = m_harness.waitForModalDialog();
		CPPUNIT_ASSERT(modal != NULL);
		winnerTitle = modal->GetTitle();
		satharWinnerBodyFound =
			(findStaticTextContaining(modal, wxT("The winner of the battle is")) != NULL)
			&& (findStaticTextContaining(modal, wxT("Player Sathar")) != NULL);
		wxButton * okButton = findButtonById(modal, wxID_OK);
		if (okButton == NULL) {
			okButton = findButtonByLabel(modal, wxT("OK"));
		}
		CPPUNIT_ASSERT(okButton != NULL);
		wxCommandEvent click(wxEVT_COMMAND_BUTTON_CLICKED, okButton->GetId());
		click.SetEventObject(okButton);
		okButton->Command(click);
	}, wxID_CANCEL, 450);
	CPPUNIT_ASSERT(winnerActionRan);
	CPPUNIT_ASSERT_EQUAL(wxString::FromUTF8("Enemy Defeated!"), winnerTitle);
	CPPUNIT_ASSERT(satharWinnerBodyFound);

	bool upfWinnerBodyFound = false;
	m_harness.runVoidFunctionWithAction([&]() {
		ui.notifyWinner(false);
	}, [&]() {
		wxDialog * modal = m_harness.waitForModalDialog();
		CPPUNIT_ASSERT(modal != NULL);
		upfWinnerBodyFound = (findStaticTextContaining(modal, wxT("Player UPF")) != NULL);
		wxButton * okButton = findButtonById(modal, wxID_OK);
		if (okButton == NULL) {
			okButton = findButtonByLabel(modal, wxT("OK"));
		}
		CPPUNIT_ASSERT(okButton != NULL);
		wxCommandEvent click(wxEVT_COMMAND_BUTTON_CLICKED, okButton->GetId());
		click.SetEventObject(okButton);
		okButton->Command(click);
	}, wxID_CANCEL, 450);
	CPPUNIT_ASSERT(upfWinnerBodyFound);

	std::cerr << "TACTICAL1:redraw" << std::endl;

	ui.requestRedraw();
	redrawPanel->Update();
	m_harness.pumpEvents(5);

	FFleet * attackFleet = new FFleet();
	FFleet * defendFleet = new FFleet();
	FVehicle * setupAttacker = createShip("Destroyer");
	FVehicle * setupDefender = createShip("Frigate");
	CPPUNIT_ASSERT(setupAttacker != NULL && setupDefender != NULL);
	setupAttacker->setOwner(1);
	setupDefender->setOwner(2);
	attackFleet->addShip(setupAttacker);
	defendFleet->addShip(setupDefender);
	FleetList attackFleets;
	FleetList defendFleets;
	attackFleets.push_back(attackFleet);
	defendFleets.push_back(defendFleet);

	FBattleScreen * battleScreen = new FBattleScreen("SpinCtrl Layout Smoke");
	battleScreen->setupFleets(&attackFleets, &defendFleets, false, NULL);
	battleScreen->Layout();
	m_harness.pumpEvents(3);
	wxSpinCtrl * speedCtrl = findFirstSpinCtrl(battleScreen);
	CPPUNIT_ASSERT(speedCtrl != NULL);
	CPPUNIT_ASSERT(speedCtrl->GetParent() != battleScreen);
	CPPUNIT_ASSERT(speedCtrl->GetSize().GetWidth() > 0);
	CPPUNIT_ASSERT(speedCtrl->GetSize().GetHeight() > 0);
	battleScreen->Destroy();
	m_harness.pumpEvents(3);

	delete attackFleet;
	delete defendFleet;

	delete attacker;
	delete target;
	delete defender;
	parent->Destroy();
	m_harness.pumpEvents(10);
	m_harness.cleanupOrphanTopLevels(10);
	std::cerr << "TACTICAL1:done" << std::endl;
}

void TacticalGuiLiveTest::testBattleScreenMenuBarLabelsAndDisabledItems() {
	FBattleScreen * battleScreen = new FBattleScreen("Menu Bar Regression");
	battleScreen->Show();
	m_harness.pumpEvents(5);

	wxMenuBar * menuBar = battleScreen->GetMenuBar();
	CPPUNIT_ASSERT(menuBar != NULL);
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), static_cast<size_t>(menuBar->GetMenuCount()));
	CPPUNIT_ASSERT_EQUAL(wxString::FromUTF8("File"), stripMenuMnemonic(menuBar->GetMenuLabel(0)));
	CPPUNIT_ASSERT_EQUAL(wxString::FromUTF8("Settings"), stripMenuMnemonic(menuBar->GetMenuLabel(1)));
	CPPUNIT_ASSERT_EQUAL(wxString::FromUTF8("Help"), stripMenuMnemonic(menuBar->GetMenuLabel(2)));

	wxMenu * fileMenu = menuBar->GetMenu(0);
	wxMenu * settingsMenu = menuBar->GetMenu(1);
	wxMenu * helpMenu = menuBar->GetMenu(2);
	CPPUNIT_ASSERT(fileMenu != NULL);
	CPPUNIT_ASSERT(settingsMenu != NULL);
	CPPUNIT_ASSERT(helpMenu != NULL);
	CPPUNIT_ASSERT_EQUAL(wxString::FromUTF8("Load Game"), stripMenuMnemonic(fileMenu->FindItemByPosition(0)->GetItemLabelText()));
	CPPUNIT_ASSERT_EQUAL(wxString::FromUTF8("Save Game"), stripMenuMnemonic(fileMenu->FindItemByPosition(1)->GetItemLabelText()));
	CPPUNIT_ASSERT_EQUAL(wxString::FromUTF8("Quit"), stripMenuMnemonic(fileMenu->FindItemByPosition(3)->GetItemLabelText()));
	CPPUNIT_ASSERT_EQUAL(wxString::FromUTF8("Damage Details"), stripMenuMnemonic(settingsMenu->FindItemByPosition(0)->GetItemLabelText()));
	CPPUNIT_ASSERT_EQUAL(wxString::FromUTF8("User's Guide"), stripMenuMnemonic(helpMenu->FindItemByPosition(0)->GetItemLabelText()));
	CPPUNIT_ASSERT_EQUAL(wxString::FromUTF8("About"), stripMenuMnemonic(helpMenu->FindItemByPosition(1)->GetItemLabelText()));

	wxMenuItem * loadItem = menuBar->FindItem(ID_TacticalLoadGame);
	wxMenuItem * saveItem = menuBar->FindItem(ID_TacticalSaveGame);
	wxMenuItem * quitItem = menuBar->FindItem(ID_TacticalQuit);
	wxMenuItem * damageDetailsItem = menuBar->FindItem(ID_TacticalDamageDetails);
	wxMenuItem * usersGuideItem = menuBar->FindItem(ID_TacticalUsersGuide);
	wxMenuItem * aboutItem = menuBar->FindItem(ID_TacticalAbout);
	CPPUNIT_ASSERT(loadItem != NULL);
	CPPUNIT_ASSERT(saveItem != NULL);
	CPPUNIT_ASSERT(quitItem != NULL);
	CPPUNIT_ASSERT(damageDetailsItem != NULL);
	CPPUNIT_ASSERT(usersGuideItem != NULL);
	CPPUNIT_ASSERT(aboutItem != NULL);
	CPPUNIT_ASSERT(!loadItem->IsEnabled());
	CPPUNIT_ASSERT(!saveItem->IsEnabled());
	CPPUNIT_ASSERT(quitItem->IsEnabled());
	CPPUNIT_ASSERT(!damageDetailsItem->IsEnabled());
	CPPUNIT_ASSERT(!usersGuideItem->IsEnabled());
	CPPUNIT_ASSERT(!aboutItem->IsEnabled());

	const int baselineDestroyed = FBattleScreen::getDestroyedCount();
	wxCommandEvent loadEvent(wxEVT_MENU, ID_TacticalLoadGame);
	battleScreen->ProcessWindowEvent(loadEvent);
	wxCommandEvent saveEvent(wxEVT_MENU, ID_TacticalSaveGame);
	battleScreen->ProcessWindowEvent(saveEvent);
	wxCommandEvent damageEvent(wxEVT_MENU, ID_TacticalDamageDetails);
	battleScreen->ProcessWindowEvent(damageEvent);
	wxCommandEvent usersGuideEvent(wxEVT_MENU, ID_TacticalUsersGuide);
	battleScreen->ProcessWindowEvent(usersGuideEvent);
	wxCommandEvent aboutEvent(wxEVT_MENU, ID_TacticalAbout);
	battleScreen->ProcessWindowEvent(aboutEvent);
	m_harness.pumpEvents(3);
	CPPUNIT_ASSERT(battleScreen->IsShown());
	CPPUNIT_ASSERT_EQUAL(baselineDestroyed, FBattleScreen::getDestroyedCount());

	battleScreen->Destroy();
	m_harness.pumpEvents(5);
	m_harness.cleanupOrphanTopLevels(10);
}

void TacticalGuiLiveTest::testBattleScreenMenuQuitClosesViaSharedClosePath() {
	FBattleScreen::resetLifecycleCounters();
	FBattleScreen * battleScreen = new FBattleScreen("Menu Quit Close Path");
	CPPUNIT_ASSERT(battleScreen->GetMenuBar() != NULL);
	battleScreen->SetReturnCode(wxID_OK);
	battleScreen->Show();
	m_harness.pumpEvents(5);
	CPPUNIT_ASSERT_EQUAL(1, FBattleScreen::getConstructedCount());
	CPPUNIT_ASSERT_EQUAL(0, FBattleScreen::getDestroyedCount());
	CPPUNIT_ASSERT_EQUAL(1, FBattleScreen::getLiveInstanceCount());

	wxCommandEvent quitEvent(wxEVT_MENU, ID_TacticalQuit);
	battleScreen->ProcessWindowEvent(quitEvent);
	bool stillPresent = true;
	for (int attempt = 0; attempt < 40 && stillPresent; ++attempt) {
		stillPresent = (m_harness.findTopLevelWindow([&](wxTopLevelWindow * window) {
			return window == battleScreen;
		}, true) != NULL);
		if (!stillPresent) {
			break;
		}
		m_harness.pumpEvents(1);
		wxMilliSleep(5);
	}
	wxTopLevelWindow * screenTopLevel = m_harness.findTopLevelWindow([&](wxTopLevelWindow * window) {
		return window == battleScreen;
	}, true);
	if (screenTopLevel != NULL) {
		CPPUNIT_ASSERT(!screenTopLevel->IsShown());
		screenTopLevel->Destroy();
		m_harness.pumpEvents(5);
	}
	m_harness.cleanupOrphanTopLevels(10);
}

void TacticalGuiLiveTest::testBattleScreenTitleBarCloseClosesViaSharedClosePath() {
	FBattleScreen::resetLifecycleCounters();
	FBattleScreen * battleScreen = new FBattleScreen("Title Bar Close Path");
	battleScreen->SetReturnCode(wxID_OK);
	battleScreen->Show();
	m_harness.pumpEvents(5);
	CPPUNIT_ASSERT_EQUAL(1, FBattleScreen::getConstructedCount());
	CPPUNIT_ASSERT_EQUAL(0, FBattleScreen::getDestroyedCount());
	CPPUNIT_ASSERT_EQUAL(1, FBattleScreen::getLiveInstanceCount());

	const bool closeAccepted = battleScreen->Close();
	bool stillPresent = true;
	for (int attempt = 0; attempt < 40 && stillPresent; ++attempt) {
		stillPresent = (m_harness.findTopLevelWindow([&](wxTopLevelWindow * window) {
			return window == battleScreen;
		}, true) != NULL);
		if (!stillPresent) {
			break;
		}
		m_harness.pumpEvents(1);
		wxMilliSleep(5);
	}
	CPPUNIT_ASSERT(closeAccepted);
	wxTopLevelWindow * screenTopLevel = m_harness.findTopLevelWindow([&](wxTopLevelWindow * window) {
		return window == battleScreen;
	}, true);
	if (screenTopLevel != NULL) {
		CPPUNIT_ASSERT(!screenTopLevel->IsShown());
		screenTopLevel->Destroy();
		m_harness.pumpEvents(5);
	}
	m_harness.cleanupOrphanTopLevels(10);
}

void TacticalGuiLiveTest::testTacticalActionButtonsRemainSizerPositionedWhenShown() {
const int expectedLeftOffset = 40;
const wxRect zoomRect(0, 0, 30, 120);

struct Scenario {
	int state;
	int phase;
	wxString label;
} scenarios[] = {
	{BS_Battle, PH_MOVE, wxT("Movement Done")},
	{BS_Battle, PH_DEFENSE_FIRE, wxT("Defensive Fire Done")},
	{BS_Battle, PH_ATTACK_FIRE, wxT("Offensive Fire Done")},
	{BS_PlaceMines, PH_NONE, wxT("Mine Placement Done")}
};

for (unsigned int i = 0; i < sizeof(scenarios) / sizeof(scenarios[0]); i++) {
	FFleet * attackFleet = new FFleet();
	FFleet * defendFleet = new FFleet();
	FVehicle * setupAttacker = createShip("Destroyer");
	FVehicle * setupDefender = createShip("Frigate");
	CPPUNIT_ASSERT(setupAttacker != NULL && setupDefender != NULL);
	setupAttacker->setOwner(1);
	setupDefender->setOwner(2);
	attackFleet->addShip(setupAttacker);
	defendFleet->addShip(setupDefender);
	FleetList attackFleets;
	FleetList defendFleets;
	attackFleets.push_back(attackFleet);
	defendFleets.push_back(defendFleet);

	FBattleScreen * battleScreen = new FBattleScreen("Action Button Layout Regression");
	battleScreen->setupFleets(&attackFleets, &defendFleets, false, NULL);
	battleScreen->Show();
	m_harness.pumpEvents(2);
	battleScreen->setState(scenarios[i].state);
	battleScreen->setPhase(scenarios[i].phase);
	battleScreen->setMoveComplete(true);
	battleScreen->reDraw();
	battleScreen->Layout();
	m_harness.pumpEvents(5);

	wxButton * actionButton = findButtonByLabel(battleScreen, scenarios[i].label);
	CPPUNIT_ASSERT(actionButton != NULL);
	// Live wx paint timing for FBattleScreen is not deterministic in the harness without
	// driving the full tactical flow, so the source-contract tactical test locks down the
	// runtime Show()/Layout() order while this live check verifies the sizer-managed geometry.
	actionButton->Show();
	actionButton->GetParent()->Layout();
	battleScreen->Layout();
	m_harness.pumpEvents(2);
	CPPUNIT_ASSERT(actionButton->IsShown());
	const wxRect buttonRect = actionButton->GetRect();
	CPPUNIT_ASSERT(buttonRect.GetWidth() > 0);
	CPPUNIT_ASSERT(buttonRect.GetHeight() > 0);
	CPPUNIT_ASSERT(buttonRect.GetX() >= expectedLeftOffset);
	CPPUNIT_ASSERT(buttonRect.GetY() > 0);
	CPPUNIT_ASSERT(!buttonRect.Intersects(zoomRect));

	battleScreen->Destroy();
	m_harness.pumpEvents(3);
	delete attackFleet;
	delete defendFleet;
}

m_harness.cleanupOrphanTopLevels(10);
}

void TacticalGuiLiveTest::testTacticalActionButtonsStayBelowPromptReservationAcrossPhases() {
const int expectedLeftOffset = 40;
CPPUNIT_ASSERT_MESSAGE(
	"Movement reminder text must retain the quoted button label.",
	sourceContainsLineToken(
		std::vector<std::string>(1, "../../src/tactical/FBattleDisplay.cpp"),
		"Press the 'Movement Done' button when all ships have been assigned their movement instructions."));
CPPUNIT_ASSERT_MESSAGE(
	"Movement prompts should use wrapped prompt drawing in constrained widths.",
	sourceContainsLineToken(
		std::vector<std::string>(1, "../../src/tactical/FBattleDisplay.cpp"),
		"drawWrappedActionPrompt("));
CPPUNIT_ASSERT_MESSAGE(
	"Movement prompt line reservation should be calculated from wrapped line counts.",
	sourceContainsLineToken(
		std::vector<std::string>(1, "../../src/tactical/FBattleDisplay.cpp"),
		"countWrappedActionPromptLines("));
CPPUNIT_ASSERT_MESSAGE(
	"Movement prompt rendering must reserve dynamic action-prompt lines.",
	sourceContainsLineToken(
		std::vector<std::string>(1, "../../src/tactical/FBattleDisplay.cpp"),
		"reserveActionPromptLines("));
CPPUNIT_ASSERT_MESSAGE(
	"Movement-phase ship stats margin fallback must derive from available width.",
	sourceContainsLineToken(
		std::vector<std::string>(1, "../../src/tactical/FBattleDisplay.cpp"),
		"largestMarginWithStatsRoom"));

struct Scenario {
	int state;
	int phase;
	int promptLineCount;
	wxString label;
} scenarios[] = {
	{BS_Battle, PH_MOVE, 2, wxT("Movement Done")},
	{BS_Battle, PH_DEFENSE_FIRE, 3, wxT("Defensive Fire Done")},
	{BS_Battle, PH_ATTACK_FIRE, 3, wxT("Offensive Fire Done")},
	{BS_PlaceMines, PH_NONE, 3, wxT("Mine Placement Done")}
};

for (unsigned int i = 0; i < sizeof(scenarios) / sizeof(scenarios[0]); i++) {
	FFleet * attackFleet = new FFleet();
	FFleet * defendFleet = new FFleet();
	FVehicle * setupAttacker = createShip("Destroyer");
	FVehicle * setupDefender = createShip("Frigate");
	CPPUNIT_ASSERT(setupAttacker != NULL && setupDefender != NULL);
	setupAttacker->setOwner(1);
	setupDefender->setOwner(2);
	attackFleet->addShip(setupAttacker);
	defendFleet->addShip(setupDefender);
	FleetList attackFleets;
	FleetList defendFleets;
	attackFleets.push_back(attackFleet);
	defendFleets.push_back(defendFleet);

	FBattleScreen * battleScreen = new FBattleScreen("Action Prompt Spacing Regression");
	battleScreen->setupFleets(&attackFleets, &defendFleets, false, NULL);
	battleScreen->Show();
	m_harness.pumpEvents(2);
	battleScreen->setState(scenarios[i].state);
	battleScreen->setPhase(scenarios[i].phase);
	battleScreen->setMoveComplete(true);
	battleScreen->reDraw();
	battleScreen->Layout();
	m_harness.pumpEvents(5);

	wxButton * actionButton = findButtonByLabel(battleScreen, scenarios[i].label);
	CPPUNIT_ASSERT(actionButton != NULL);
	actionButton->Show();
	actionButton->GetParent()->Layout();
	battleScreen->Layout();
	m_harness.pumpEvents(2);

	const wxRect buttonRect = actionButton->GetRect();
	CPPUNIT_ASSERT(buttonRect.GetWidth() > 0);
	CPPUNIT_ASSERT(buttonRect.GetHeight() > 0);
	CPPUNIT_ASSERT(buttonRect.GetX() >= expectedLeftOffset);
	const int lastPromptLineIndex = scenarios[i].promptLineCount - 1;
	const int promptTextBottomY = FBattleDisplayTestPeer::actionPromptLineY(lastPromptLineIndex)
		+ FBattleDisplayTestPeer::actionPromptLineHeight();
	const wxRect promptTextRegion(
		0,
		FBattleDisplayTestPeer::actionPromptTopMargin(),
		battleScreen->GetClientSize().GetWidth(),
		promptTextBottomY - FBattleDisplayTestPeer::actionPromptTopMargin());
	CPPUNIT_ASSERT_MESSAGE(
		std::string("Button intersects instruction-text block for label: ") + scenarios[i].label.ToStdString(),
		!buttonRect.Intersects(promptTextRegion));
	CPPUNIT_ASSERT_MESSAGE(
		std::string("Button overlapped prompt reservation for label: ") + scenarios[i].label.ToStdString(),
		buttonRect.GetTop() >= FBattleDisplayTestPeer::actionPromptReservedBottomY());
	CPPUNIT_ASSERT_MESSAGE(
		std::string("Button is not below full instruction-text block for label: ") + scenarios[i].label.ToStdString(),
		buttonRect.GetTop() >= promptTextBottomY);

	battleScreen->Destroy();
	m_harness.pumpEvents(3);
	delete attackFleet;
	delete defendFleet;
}

m_harness.cleanupOrphanTopLevels(10);
}

void TacticalGuiLiveTest::testBattleScreenDefaultSizeAndLayoutPolicyRuntime() {
FBattleScreen * battleScreen = new FBattleScreen();
battleScreen->Show();
m_harness.pumpEvents(8);

const wxSize defaultSize = battleScreen->GetSize();
CPPUNIT_ASSERT_EQUAL(1200, defaultSize.GetWidth());
CPPUNIT_ASSERT_EQUAL(900, defaultSize.GetHeight());

FBattleBoard * mapPanel = findFirstBattleBoard(battleScreen);
FBattleDisplay * displayPanel = findFirstBattleDisplay(battleScreen);
CPPUNIT_ASSERT(mapPanel != NULL);
CPPUNIT_ASSERT(displayPanel != NULL);

	const int clientHeight = battleScreen->GetClientSize().GetHeight();
	CPPUNIT_ASSERT(clientHeight > 0);
	const int mapFloor = (clientHeight * 60) / 100;

	CPPUNIT_ASSERT_EQUAL(mapFloor, mapPanel->GetMinSize().GetHeight());
	CPPUNIT_ASSERT_EQUAL(120, displayPanel->GetMinSize().GetHeight());

	battleScreen->Destroy();
	m_harness.pumpEvents(5);
	m_harness.cleanupOrphanTopLevels(10);
}

void TacticalGuiLiveTest::testBattleDisplayLowerPanelLayoutStatePersistsAcrossPhaseAndGeometryChanges() {
const int expectedLeftOffset = 40;
FFleet * attackFleet = new FFleet();
FFleet * defendFleet = new FFleet();
FVehicle * setupAttacker = createShip("Destroyer");
FVehicle * setupDefender = createShip("Frigate");
CPPUNIT_ASSERT(setupAttacker != NULL && setupDefender != NULL);
setupAttacker->setOwner(1);
setupDefender->setOwner(2);
attackFleet->addShip(setupAttacker);
defendFleet->addShip(setupDefender);
FleetList attackFleets;
FleetList defendFleets;
attackFleets.push_back(attackFleet);
defendFleets.push_back(defendFleet);

FBattleScreen * wideScreen = new FBattleScreen("Lower Panel Layout State Persistence (wide)");
wideScreen->setupFleets(&attackFleets, &defendFleets, false, NULL);
wideScreen->Show();
m_harness.pumpEvents(5);

FBattleDisplay * wideDisplay = findFirstBattleDisplay(wideScreen);
CPPUNIT_ASSERT(wideDisplay != NULL);

wideScreen->setState(BS_Battle);
wideScreen->setPhase(PH_DEFENSE_FIRE);
wideScreen->setMoveComplete(true);
wideScreen->reDraw();
wideScreen->Layout();
m_harness.pumpEvents(5);
const int wideDefenseHeight = wideDisplay->GetMinSize().GetHeight();
CPPUNIT_ASSERT_EQUAL(120, wideDefenseHeight);

wideScreen->setPhase(PH_ATTACK_FIRE);
wideScreen->reDraw();
wideScreen->Layout();
m_harness.pumpEvents(5);
const int wideAttackHeight = wideDisplay->GetMinSize().GetHeight();
CPPUNIT_ASSERT_EQUAL(wideDefenseHeight, wideAttackHeight);

const wxSize wideSize = wideScreen->GetSize();
wideScreen->SetSize(wxSize(wideSize.GetWidth() + 180, wideSize.GetHeight() + 120));
wideScreen->SendSizeEvent();
m_harness.pumpEvents(5);
wideScreen->setPhase(PH_DEFENSE_FIRE);
wideScreen->reDraw();
wideScreen->Layout();
m_harness.pumpEvents(5);
const int resizedWideHeight = wideDisplay->GetMinSize().GetHeight();
CPPUNIT_ASSERT_EQUAL(wideDefenseHeight, resizedWideHeight);

wideScreen->setPhase(PH_MOVE);
wideScreen->setShip(setupAttacker);
wideScreen->setMoveComplete(true);
wideScreen->reDraw();
wideScreen->Layout();
m_harness.pumpEvents(5);

wxButton * moveDoneButton = findButtonByLabel(wideScreen, wxT("Movement Done"));
CPPUNIT_ASSERT(moveDoneButton != NULL);
moveDoneButton->Show();
moveDoneButton->GetParent()->Layout();
wideScreen->Layout();
m_harness.pumpEvents(2);

	const wxRect beforeResizeRect = moveDoneButton->GetRect();
	const int beforeResizeHeight = wideDisplay->GetMinSize().GetHeight();
	const wxSize beforeResizeSize = wideScreen->GetSize();
	wideScreen->SetSize(wxSize(760, beforeResizeSize.GetHeight()));
	wideScreen->SendSizeEvent();
	m_harness.pumpEvents(5);

CPPUNIT_ASSERT(moveDoneButton->IsShown());
	const wxRect afterResizeRect = moveDoneButton->GetRect();
	CPPUNIT_ASSERT(afterResizeRect.GetWidth() > 0);
	CPPUNIT_ASSERT(afterResizeRect.GetHeight() > 0);
	CPPUNIT_ASSERT(afterResizeRect.GetX() >= expectedLeftOffset);
	CPPUNIT_ASSERT(afterResizeRect.GetTop() >= FBattleDisplayTestPeer::actionPromptReservedBottomY());
	CPPUNIT_ASSERT(wideDisplay->GetMinSize().GetHeight() >= beforeResizeHeight);
	CPPUNIT_ASSERT(afterResizeRect.GetTop() > beforeResizeRect.GetTop() - 80);

	wideScreen->SetSize(beforeResizeSize);
	wideScreen->SendSizeEvent();
	m_harness.pumpEvents(5);
	const int restoredHeight = wideDisplay->GetMinSize().GetHeight();
	CPPUNIT_ASSERT_EQUAL(beforeResizeHeight, restoredHeight);

	wideScreen->Destroy();
	m_harness.pumpEvents(5);
	m_harness.cleanupOrphanTopLevels(10);
	delete attackFleet;
delete defendFleet;
}

void TacticalGuiLiveTest::testBattleDisplayNarrowWidthStacksShipStatsBelowButtons() {
const int expectedLeftOffset = 40;
CPPUNIT_ASSERT_MESSAGE(
	"Stacked fallback should trigger when split mode cannot preserve full prompt-width padding.",
	sourceContainsLineToken(
		std::vector<std::string>(1, "../../src/tactical/FBattleDisplay.cpp"),
		"largestMarginWithStatsRoom < leftOffset + ACTION_PROMPT_MIN_WIDTH + BORDER"));
FFleet * attackFleet = new FFleet();
FFleet * defendFleet = new FFleet();
FVehicle * setupAttacker = createShip("Destroyer");
FVehicle * setupDefender = createShip("Frigate");
CPPUNIT_ASSERT(setupAttacker != NULL && setupDefender != NULL);
setupAttacker->setOwner(1);
setupDefender->setOwner(2);
attackFleet->addShip(setupAttacker);
defendFleet->addShip(setupDefender);
FleetList attackFleets;
FleetList defendFleets;
attackFleets.push_back(attackFleet);
defendFleets.push_back(defendFleet);

FBattleScreen * battleScreen = new FBattleScreen("Narrow Width Stacked Ship Stats");
battleScreen->setupFleets(&attackFleets, &defendFleets, false, NULL);
battleScreen->Show();
m_harness.pumpEvents(5);

FBattleDisplay * display = findFirstBattleDisplay(battleScreen);
CPPUNIT_ASSERT(display != NULL);
battleScreen->setState(BS_Battle);
battleScreen->setPhase(PH_DEFENSE_FIRE);
battleScreen->setShip(setupAttacker);
battleScreen->setMoveComplete(true);
battleScreen->reDraw();
battleScreen->Layout();
m_harness.pumpEvents(5);

wxButton * defenseDoneButton = findButtonByLabel(battleScreen, wxT("Defensive Fire Done"));
CPPUNIT_ASSERT(defenseDoneButton != NULL);
defenseDoneButton->Show();
defenseDoneButton->GetParent()->Layout();
battleScreen->Layout();
m_harness.pumpEvents(3);
const int wideHeight = display->GetMinSize().GetHeight();
const wxRect wideRect = defenseDoneButton->GetRect();
CPPUNIT_ASSERT(wideRect.GetX() >= expectedLeftOffset);
CPPUNIT_ASSERT(wideRect.GetTop() >= FBattleDisplayTestPeer::actionPromptReservedBottomY());

const wxSize baselineSize = battleScreen->GetSize();
battleScreen->SetSize(wxSize(650, baselineSize.GetHeight()));
battleScreen->SendSizeEvent();
m_harness.pumpEvents(6);
battleScreen->setPhase(PH_DEFENSE_FIRE);
battleScreen->setShip(setupAttacker);
battleScreen->reDraw();
battleScreen->Layout();
m_harness.pumpEvents(5);

defenseDoneButton = findButtonByLabel(battleScreen, wxT("Defensive Fire Done"));
CPPUNIT_ASSERT(defenseDoneButton != NULL);
defenseDoneButton->Show();
defenseDoneButton->GetParent()->Layout();
battleScreen->Layout();
m_harness.pumpEvents(3);

const int narrowHeight = display->GetMinSize().GetHeight();
const wxRect narrowRect = defenseDoneButton->GetRect();
CPPUNIT_ASSERT(narrowRect.GetWidth() > 0);
CPPUNIT_ASSERT(narrowRect.GetHeight() > 0);
CPPUNIT_ASSERT(narrowRect.GetX() >= expectedLeftOffset);
CPPUNIT_ASSERT(narrowRect.GetTop() >= FBattleDisplayTestPeer::actionPromptReservedBottomY());
CPPUNIT_ASSERT(narrowRect.GetTop() >= wideRect.GetTop());
CPPUNIT_ASSERT(narrowHeight > wideHeight);
CPPUNIT_ASSERT(narrowHeight > 120);

battleScreen->Destroy();
m_harness.pumpEvents(5);
m_harness.cleanupOrphanTopLevels(10);
delete attackFleet;
delete defendFleet;
}

void TacticalGuiLiveTest::testTacticalDamageSummaryDialogDisplaysContextAndCloseBehavior() {
	wxFrame * parent = new wxFrame(NULL, wxID_ANY, "Damage Summary Parent", wxPoint(120, 120), wxSize(540, 420));
parent->Show();
m_harness.pumpEvents();

const FTacticalCombatReportSummary summary = buildSummaryWithLines();
TacticalDamageSummaryGUI * dialog = new TacticalDamageSummaryGUI(parent, summary);

CPPUNIT_ASSERT_EQUAL(
wxString::FromUTF8("Tactical Damage Summary - Offensive Fire Results"),
dialog->GetTitle());

wxStaticText * contextText = findStaticTextContaining(dialog, wxT("Report:"));
CPPUNIT_ASSERT(contextText != NULL);
CPPUNIT_ASSERT(contextText->GetLabel().Find(wxT("Immediate report")) != wxNOT_FOUND);
CPPUNIT_ASSERT(contextText->GetLabel().Find(wxT("Phase: 2")) != wxNOT_FOUND);

wxTextCtrl * summaryText = findFirstTextCtrl(dialog);
CPPUNIT_ASSERT(summaryText != NULL);
CPPUNIT_ASSERT(summaryText->GetValue().Find(wxT("Ship Damage Summary")) != wxNOT_FOUND);
CPPUNIT_ASSERT(summaryText->GetValue().Find(wxT("Destroyer Alpha took 4 hull damage.")) != wxNOT_FOUND);
CPPUNIT_ASSERT(summaryText->GetValue().Find(wxT("Hit Details")) != wxNOT_FOUND);
CPPUNIT_ASSERT(summaryText->GetValue().Find(wxT("Destroyer Alpha [Laser Battery] -> Sathar Frigate: 4 hull damage")) != wxNOT_FOUND);

bool closeActionRan = false;
bool closeButtonFound = false;
bool closeButtonFocused = false;
bool closeButtonIsDefault = false;
const int closeResult = m_harness.showModalWithAction(*dialog, [&]() {
	closeActionRan = true;
	assertDialogCenteredOnParent(dialog, parent, 200);
	wxButton * closeButton = findButtonByLabel(dialog, wxT("Close"));
	closeButtonFound = (closeButton != NULL);
	if (closeButton != NULL) {
		m_harness.pumpEvents(2);
		closeButtonIsDefault = (dialog->GetDefaultItem() == closeButton);
		dialog->Raise();
		closeButton->SetFocus();
		m_harness.pumpEvents(2);
		closeButtonFocused = (wxWindow::FindFocus() == closeButton);
		wxUIActionSimulator simulator;
		simulator.Char(WXK_RETURN);
		m_harness.pumpEvents(2);
		if (dialog->IsModal()) {
			wxCommandEvent click(wxEVT_COMMAND_BUTTON_CLICKED, closeButton->GetId());
			click.SetEventObject(closeButton);
			closeButton->Command(click);
		}
	}
}, wxID_CANCEL, 2000);
CPPUNIT_ASSERT(closeActionRan);
CPPUNIT_ASSERT(closeButtonFound);
CPPUNIT_ASSERT(closeButtonIsDefault);
CPPUNIT_ASSERT(closeResult == static_cast<int>(wxID_OK) || closeResult == static_cast<int>(wxID_CANCEL));
dialog->Destroy();
m_harness.pumpEvents(3);

FTacticalCombatReportSummary noDetailSummary = buildSummaryWithLines();
noDetailSummary.showHitDetails = false;
TacticalDamageSummaryGUI * noDetailDialog = new TacticalDamageSummaryGUI(parent, noDetailSummary);
wxTextCtrl * noDetailText = findFirstTextCtrl(noDetailDialog);
CPPUNIT_ASSERT(noDetailText != NULL);
CPPUNIT_ASSERT(noDetailText->GetValue().Find(wxT("Ship Damage Summary")) != wxNOT_FOUND);
CPPUNIT_ASSERT(noDetailText->GetValue().Find(wxT("Destroyer Alpha took 4 hull damage.")) != wxNOT_FOUND);
CPPUNIT_ASSERT(noDetailText->GetValue().Find(wxT("Hit Details")) == wxNOT_FOUND);
CPPUNIT_ASSERT(noDetailText->GetValue().Find(wxT("Destroyer Alpha [Laser Battery] -> Sathar Frigate: 4 hull damage")) == wxNOT_FOUND);
noDetailDialog->Destroy();
m_harness.pumpEvents(3);

FTacticalCombatReportSummary emptySummary;
emptySummary.context.reportType = TRT_None;
TacticalDamageSummaryGUI * emptyDialog = new TacticalDamageSummaryGUI(parent, emptySummary);
wxTextCtrl * emptyText = findFirstTextCtrl(emptyDialog);
CPPUNIT_ASSERT(emptyText != NULL);
CPPUNIT_ASSERT(emptyText->GetValue().Find(wxT("No ships sustained damage in this report.")) != wxNOT_FOUND);
bool emptyCloseActionRan = false;
bool emptyCloseButtonFound = false;
const int emptyCloseResult = m_harness.runModalFunctionWithAction([&]() {
	return emptyDialog->ShowModal();
}, [&]() {
	emptyCloseActionRan = true;
	wxButton * closeButton = findButtonByLabel(emptyDialog, wxT("Close"));
	emptyCloseButtonFound = (closeButton != NULL);
	if (closeButton != NULL) {
		m_harness.pumpEvents(2);
		wxCommandEvent click(wxEVT_COMMAND_BUTTON_CLICKED, closeButton->GetId());
		click.SetEventObject(closeButton);
		closeButton->Command(click);
	}
}, wxID_CANCEL, 450);
CPPUNIT_ASSERT(emptyCloseActionRan);
CPPUNIT_ASSERT(emptyCloseButtonFound);
CPPUNIT_ASSERT_EQUAL(static_cast<int>(wxID_OK), emptyCloseResult);
emptyDialog->Destroy();
m_harness.pumpEvents(3);

TacticalDamageSummaryGUI * parentlessDialog = new TacticalDamageSummaryGUI(NULL, summary);
bool parentlessCloseActionRan = false;
bool parentlessCloseButtonFound = false;
const int parentlessCloseResult = m_harness.runModalFunctionWithAction([&]() {
	return parentlessDialog->ShowModal();
}, [&]() {
	parentlessCloseActionRan = true;
	CPPUNIT_ASSERT(wxDisplay::GetFromWindow(parentlessDialog) != wxNOT_FOUND);
	wxButton * closeButton = findButtonByLabel(parentlessDialog, wxT("Close"));
	parentlessCloseButtonFound = (closeButton != NULL);
	if (closeButton != NULL) {
		wxCommandEvent click(wxEVT_COMMAND_BUTTON_CLICKED, closeButton->GetId());
		click.SetEventObject(closeButton);
		closeButton->Command(click);
	}
}, wxID_CANCEL, 150);
CPPUNIT_ASSERT(parentlessCloseActionRan);
CPPUNIT_ASSERT(parentlessCloseButtonFound);
CPPUNIT_ASSERT_EQUAL(static_cast<int>(wxID_OK), parentlessCloseResult);
parentlessDialog->Destroy();
m_harness.pumpEvents(3);

parent->Destroy();
m_harness.pumpEvents(10);
m_harness.cleanupOrphanTopLevels(10);
}

void TacticalGuiLiveTest::testICMSelectionDialogInteractionFinalizesAssignedCountsAndAmmo() {
FVehicle * target = createShip("Destroyer");
FVehicle * attacker = createShip("Destroyer");
FVehicle * defenderA = createShip("Destroyer");
FVehicle * defenderB = createShip("Frigate");
CPPUNIT_ASSERT(target != NULL && attacker != NULL && defenderA != NULL && defenderB != NULL);
target->setOwner(2);
attacker->setOwner(1);
defenderA->setOwner(2);
defenderB->setOwner(2);

FWeapon * weapon = attacker->getWeapon(0);
CPPUNIT_ASSERT(weapon != NULL);
weapon->setTarget(target, 4);

VehicleList nearbyShips;
nearbyShips.push_back(defenderA);
nearbyShips.push_back(defenderB);

ICMData row;
row.weapon = weapon;
row.vehicles = &nearbyShips;
std::vector<ICMData *> icmRows;
icmRows.push_back(&row);

const unsigned int defenseIndexA = defenderA->hasDefense(FDefense::ICM);
const unsigned int defenseIndexB = defenderB->hasDefense(FDefense::ICM);
CPPUNIT_ASSERT(defenderA->getDefense(defenseIndexA) != NULL);
CPPUNIT_ASSERT(defenderB->getDefense(defenseIndexB) != NULL);

const int defenderAStartingAmmo = defenderA->getDefense(defenseIndexA)->getAmmo();
const int defenderBStartingAmmo = defenderB->getDefense(defenseIndexB)->getAmmo();

ICMSelectionGUITestPeer * dialog = new ICMSelectionGUITestPeer(NULL, &icmRows);
size_t appliedControlCount = 0;
wxSize firstAppliedControlSize;
wxSize secondAppliedControlSize;
long firstAppliedControlStyle = 0;
long secondAppliedControlStyle = 0;
wxSize initialDialogSize;
wxSize postRebuildDialogSize;
wxString assignedCountText;
bool modalActionRan = false;
CPPUNIT_ASSERT_MESSAGE(
	"ICMSelectionGUI spinner constructor must keep explicit vertical style token.",
	sourceContainsLineToken(
		std::vector<std::string>(1, "../../src/gui/ICMSelectionGUI.cpp"),
		"wxSP_ARROW_KEYS | wxSP_VERTICAL"));
const int modalResult = m_harness.runModalFunctionWithAction([&]() {
	return dialog->ShowModal();
}, [&]() {
	modalActionRan = true;
	initialDialogSize = dialog->GetSize();
	wxButton * doneButton = findButtonByLabel(dialog, wxT("Done"));
	CPPUNIT_ASSERT(doneButton != NULL);
	const wxRect initialClientRect(wxPoint(0, 0), dialog->GetClientSize());
	CPPUNIT_ASSERT(initialClientRect.Contains(doneButton->GetRect().GetTopLeft()));
	CPPUNIT_ASSERT(initialClientRect.Contains(doneButton->GetRect().GetBottomRight()));
	dialog->selectWeaponRow(0);
	appliedControlCount = dialog->appliedControlCount();
	dialog->Layout();
	dialog->Update();
	postRebuildDialogSize = dialog->GetSize();
	firstAppliedControlSize = dialog->appliedControl(0)->GetSize();
	secondAppliedControlSize = dialog->appliedControl(1)->GetSize();
	firstAppliedControlStyle = dialog->appliedControl(0)->GetWindowStyleFlag();
	secondAppliedControlStyle = dialog->appliedControl(1)->GetWindowStyleFlag();
	const wxRect rebuiltClientRect(wxPoint(0, 0), dialog->GetClientSize());
	CPPUNIT_ASSERT(rebuiltClientRect.Contains(dialog->appliedControl(0)->GetRect().GetTopLeft()));
	CPPUNIT_ASSERT(rebuiltClientRect.Contains(dialog->appliedControl(0)->GetRect().GetBottomRight()));
	CPPUNIT_ASSERT(rebuiltClientRect.Contains(dialog->appliedControl(1)->GetRect().GetTopLeft()));
	CPPUNIT_ASSERT(rebuiltClientRect.Contains(dialog->appliedControl(1)->GetRect().GetBottomRight()));
	dialog->setAppliedICMValue(0, 2);
	dialog->setAppliedICMValue(1, 1);
	assignedCountText = dialog->assignedCellText(0);
	dialog->clickDone();
}, wxID_CANCEL, 150);

CPPUNIT_ASSERT(modalActionRan);
CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), appliedControlCount);
CPPUNIT_ASSERT(firstAppliedControlSize.GetWidth() > 0);
CPPUNIT_ASSERT(firstAppliedControlSize.GetHeight() > 0);
CPPUNIT_ASSERT(secondAppliedControlSize.GetWidth() > 0);
CPPUNIT_ASSERT(secondAppliedControlSize.GetHeight() > 0);
CPPUNIT_ASSERT((firstAppliedControlStyle & wxSP_ARROW_KEYS) != 0);
CPPUNIT_ASSERT((firstAppliedControlStyle & wxSP_VERTICAL) != 0);
CPPUNIT_ASSERT((secondAppliedControlStyle & wxSP_ARROW_KEYS) != 0);
CPPUNIT_ASSERT((secondAppliedControlStyle & wxSP_VERTICAL) != 0);
CPPUNIT_ASSERT(initialDialogSize.GetWidth() > 0);
CPPUNIT_ASSERT(initialDialogSize.GetHeight() > 0);
CPPUNIT_ASSERT(postRebuildDialogSize.GetWidth() >= initialDialogSize.GetWidth());
CPPUNIT_ASSERT(postRebuildDialogSize.GetHeight() >= initialDialogSize.GetHeight());
CPPUNIT_ASSERT_EQUAL(wxString::FromUTF8("3"), assignedCountText);
CPPUNIT_ASSERT_EQUAL(0, modalResult);
CPPUNIT_ASSERT_EQUAL(defenderAStartingAmmo - 2, defenderA->getDefense(defenseIndexA)->getAmmo());
CPPUNIT_ASSERT_EQUAL(defenderBStartingAmmo - 1, defenderB->getDefense(defenseIndexB)->getAmmo());
dialog->Destroy();
m_harness.pumpEvents(3);

m_harness.cleanupOrphanTopLevels(10);
}

}
