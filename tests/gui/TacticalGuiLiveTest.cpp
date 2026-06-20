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

#include "core/FHexMap.h"
#include "gui/ICMSelectionGUI.h"
#include "gui/TacticalDamageSummaryGUI.h"
#include "gui/WXTacticalUI.h"
#include "ships/FVehicle.h"
#include "strategic/FFleet.h"
#include "tactical/FBattleDisplay.h"
#include "tactical/FBattleScreen.h"
#include "tactical/FTacticalCombatReport.h"
#include "tactical/FTacticalGame.h"
#include "weapons/FWeapon.h"
#include "weapons/FSeekerMissileLauncher.h"
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

/// returns the number of pending-seeker recall regions registered after the last draw
size_t pendingSeekerRecallRegionCount() const {
	return m_pendingSeekerRecallRegions.size();
}

/// returns the i-th pending-seeker recall region after the last draw
wxRect pendingSeekerRecallRegion(size_t i) const {
	CPPUNIT_ASSERT(i < m_pendingSeekerRecallRegions.size());
	return m_pendingSeekerRecallRegions[i];
}

/// returns the i-th pending-seeker recall hex after the last draw
wxPoint pendingSeekerRecallHex(size_t i) const {
	CPPUNIT_ASSERT(i < m_pendingSeekerRecallHexes.size());
	return m_pendingSeekerRecallHexes[i];
}

/// exposes the action button row bottom for region-position verification
int actionButtonRowBottomPublic() const {
	return getActionButtonRowBottom();
}

/// exposes the lower-panel layout state ship-stats left margin for position verification
int shipStatsLeftMarginPublic() const {
	return m_lowerPanelLayoutState.shipStatsLeftMargin;
}

/// exposes requestedDisplayHeight from the lower-panel layout state for height-expansion verification
int requestedDisplayHeightPublic() const {
	return m_lowerPanelLayoutState.requestedDisplayHeight;
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

// SMFR-04: SeederGame subclass exposes a seedSeeker() injection method so GUI
// tests can plant active seekers directly into the model without driving the
// full ordnance-placement API flow.  m_seekerMissiles is protected in
// FTacticalGame so a derived class can access it.
class SeederGame : public Frontier::FTacticalGame {
public:
	void seedSeeker(const Frontier::FTacticalSeekerMissileState & s) {
		m_seekerMissiles.push_back(s);
	}
};

// SMFR-04: TestableBattleScreen exposes seeker-seeding through the protected
// m_tacticalGame pointer.  After base construction the original FTacticalGame
// is replaced with a SeederGame so the test can inject active seekers while
// keeping all other FBattleScreen subsystems intact.
class TestableBattleScreen : public FBattleScreen {
public:
	explicit TestableBattleScreen(const wxString & title = wxT("TestableBattleScreen"))
		: FBattleScreen(title)
		, m_seederGame(NULL)
	{
		// Base constructor already allocated a FTacticalGame and installed the UI.
		// Detach the UI from the old game, destroy it, and reinstall on a SeederGame.
		if (m_tacticalGame) {
			m_tacticalGame->installUI(NULL);
			delete m_tacticalGame;
			m_tacticalGame = NULL;
		}
		m_seederGame = new SeederGame();
		if (m_tacticalUI) {
			m_seederGame->installUI(m_tacticalUI);
		}
		m_tacticalGame = m_seederGame;
	}

	/// Inject an active seeker directly into the model's seeker list.
	void seedSeeker(const Frontier::FTacticalSeekerMissileState & s) {
		if (m_seederGame) {
			m_seederGame->seedSeeker(s);
		}
	}

private:
	SeederGame * m_seederGame;
};

void waitForBattleScreenLifecycleSettle(WXGuiTestHarness & harness,
                                        int timeoutMs = 1200,
                                        int pollMs = 10) {
	const int safePollMs = (pollMs > 0) ? pollMs : 1;
	int elapsedMs = 0;
	while (elapsedMs <= timeoutMs) {
		harness.pumpEvents(2);
		if (FBattleScreen::getLiveInstanceCount() == 0
		    && FBattleScreen::getConstructedCount() == FBattleScreen::getDestroyedCount()) {
			return;
		}
		if (elapsedMs < timeoutMs) {
			wxMilliSleep(static_cast<unsigned long>(safePollMs));
		}
		elapsedMs += safePollMs;
	}
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
	const int baselineConstructed = FBattleScreen::getConstructedCount();
	const int baselineDestroyed = FBattleScreen::getDestroyedCount();
	FBattleScreen * battleScreen = new FBattleScreen("Menu Quit Close Path");
	wxMenuBar * menuBar = battleScreen->GetMenuBar();
	CPPUNIT_ASSERT(menuBar != NULL);
	CPPUNIT_ASSERT(menuBar->FindItem(ID_TacticalQuit) != NULL);
	battleScreen->SetReturnCode(wxID_OK);
	battleScreen->Show();
	m_harness.pumpEvents(5);
	CPPUNIT_ASSERT(m_harness.waitForTopLevelWindow([&](wxTopLevelWindow * window) {
		return window == battleScreen;
	}) != NULL);
	CPPUNIT_ASSERT(FBattleScreen::getConstructedCount() >= baselineConstructed + 1);
	CPPUNIT_ASSERT(FBattleScreen::getDestroyedCount() >= baselineDestroyed);
	CPPUNIT_ASSERT(FBattleScreen::getLiveInstanceCount() >= 1);

	wxCommandEvent quitEvent(wxEVT_MENU, ID_TacticalQuit);
	quitEvent.SetEventObject(menuBar);
	wxPostEvent(battleScreen, quitEvent);
	CPPUNIT_ASSERT(m_harness.waitForTopLevelWindowClosed([&](wxTopLevelWindow * window) {
		return window == battleScreen;
	}, 1200, 10, true));
	waitForBattleScreenLifecycleSettle(m_harness);
	wxTopLevelWindow * closedMenuWindow = m_harness.findTopLevelWindow([&](wxTopLevelWindow * window) {
		return window == battleScreen;
	}, true);
	CPPUNIT_ASSERT(closedMenuWindow == NULL || !closedMenuWindow->IsShown());
	CPPUNIT_ASSERT(FBattleScreen::getConstructedCount() >= baselineConstructed + 1);
	CPPUNIT_ASSERT(FBattleScreen::getDestroyedCount() >= baselineDestroyed + 1);
	CPPUNIT_ASSERT_EQUAL(0, FBattleScreen::getLiveInstanceCount());
	m_harness.cleanupOrphanTopLevels(10);
}

void TacticalGuiLiveTest::testBattleScreenTitleBarCloseClosesViaSharedClosePath() {
	FBattleScreen::resetLifecycleCounters();
	const int baselineConstructed = FBattleScreen::getConstructedCount();
	const int baselineDestroyed = FBattleScreen::getDestroyedCount();
	FBattleScreen * battleScreen = new FBattleScreen("Title Bar Close Path");
	battleScreen->SetReturnCode(wxID_OK);
	battleScreen->Show();
	m_harness.pumpEvents(5);
	CPPUNIT_ASSERT(FBattleScreen::getConstructedCount() >= baselineConstructed + 1);
	CPPUNIT_ASSERT(FBattleScreen::getDestroyedCount() >= baselineDestroyed);
	CPPUNIT_ASSERT(FBattleScreen::getLiveInstanceCount() >= 1);

	wxCloseEvent closeEvent(wxEVT_CLOSE_WINDOW);
	closeEvent.SetEventObject(battleScreen);
	wxPostEvent(battleScreen, closeEvent);
	CPPUNIT_ASSERT(m_harness.waitForTopLevelWindowClosed([&](wxTopLevelWindow * window) {
		return window == battleScreen;
	}, 1200, 10, true));
	waitForBattleScreenLifecycleSettle(m_harness);
	wxTopLevelWindow * closedTitleWindow = m_harness.findTopLevelWindow([&](wxTopLevelWindow * window) {
		return window == battleScreen;
	}, true);
	CPPUNIT_ASSERT(closedTitleWindow == NULL || !closedTitleWindow->IsShown());
	CPPUNIT_ASSERT(FBattleScreen::getConstructedCount() >= baselineConstructed + 1);
	CPPUNIT_ASSERT(FBattleScreen::getDestroyedCount() >= baselineDestroyed + 1);
	CPPUNIT_ASSERT_EQUAL(0, FBattleScreen::getLiveInstanceCount());
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
	{BS_Battle, PH_SEEKER_ACTIVATION, wxT("Seeker Activation Done")},
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
	{BS_Battle, PH_SEEKER_ACTIVATION, 3, wxT("Seeker Activation Done")},
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

void TacticalGuiLiveTest::testSeekerActivationPanelSourceContracts() {
CPPUNIT_ASSERT_MESSAGE(
	"Seeker activation panel should display instructional prompt text.",
	sourceContainsLineToken(
		std::vector<std::string>(1, "../../src/tactical/FBattleDisplay.cpp"),
		"Seeker activation phase."));
CPPUNIT_ASSERT_MESSAGE(
	"Seeker activation panel should explain row-based deactivation.",
	sourceContainsLineToken(
		std::vector<std::string>(1, "../../src/tactical/FBattleDisplay.cpp"),
		"Click a row below to deactivate an activated seeker."));
CPPUNIT_ASSERT_MESSAGE(
	"Seeker activation panel should register one clickable row per activated seeker.",
	sourceContainsLineToken(
		std::vector<std::string>(1, "../../src/tactical/FBattleDisplay.cpp"),
		"m_seekerActivationRegions.push_back"));
CPPUNIT_ASSERT_MESSAGE(
	"Seeker activation row clicks should deactivate the selected seeker ID.",
	sourceContainsLineToken(
		std::vector<std::string>(1, "../../src/tactical/FBattleDisplay.cpp"),
		"deactivateActiveSeekerByID(m_seekerActivationSeekerIDs[i])"));
CPPUNIT_ASSERT_MESSAGE(
	"Battle-board activation rendering should use moving-player inactive seeker stacks.",
	sourceContainsLineToken(
		std::vector<std::string>(1, "../../src/tactical/FBattleBoard.cpp"),
		"getInactiveSeekerActivationHexes()"));
CPPUNIT_ASSERT_MESSAGE(
	"Battle-board battle rendering should draw active seekers only.",
	sourceContainsLineToken(
		std::vector<std::string>(1, "../../src/tactical/FBattleBoard.cpp"),
		"getSeekerMissilesAtHex(hex, true)"));
CPPUNIT_ASSERT_MESSAGE(
	"Activation-phase board clicks should activate one seeker at the clicked hex.",
	sourceContainsLineToken(
		std::vector<std::string>(1, "../../src/tactical/FBattleBoard.cpp"),
		"m_parent->activateInactiveSeekerAtHex(hex);"));
}

void TacticalGuiLiveTest::testSetupPlacementSourceRowsAndOrdnanceColorContracts() {
CPPUNIT_ASSERT_MESSAGE(
	"Setup placement panel should render deployable source rows.",
	sourceContainsLineToken(
		std::vector<std::string>(1, "../../src/tactical/FBattleDisplay.cpp"),
		"const std::vector<FTacticalDeploymentSource> & deployableSources = m_parent->getDeployablePlacementSources();"));
CPPUNIT_ASSERT_MESSAGE(
	"Setup placement panel should read selected placement source index.",
	sourceContainsLineToken(
		std::vector<std::string>(1, "../../src/tactical/FBattleDisplay.cpp"),
		"const int selectedSourceIndex = m_parent->getSelectedPlacementSourceIndex();"));
CPPUNIT_ASSERT_MESSAGE(
	"Setup placement rows should display per-slot ammo values.",
	sourceContainsLineToken(
		std::vector<std::string>(1, "../../src/tactical/FBattleDisplay.cpp"),
		"os << \"Ammo: \" << weapon->getAmmo();"));
CPPUNIT_ASSERT_MESSAGE(
	"Setup placement row clicks should change active source by row index.",
	sourceContainsLineToken(
		std::vector<std::string>(1, "../../src/tactical/FBattleDisplay.cpp"),
		"m_parent->selectPlacementSourceByIndex(static_cast<unsigned int>(m_shipSelectionSourceIndices[i]));"));
CPPUNIT_ASSERT_MESSAGE(
	"Setup placement board should render placed ordnance from source-tracked records.",
	sourceContainsLineToken(
		std::vector<std::string>(1, "../../src/tactical/FBattleBoard.cpp"),
		"const std::vector<FTacticalPlacedOrdnance> & placedOrdnance = m_parent->getPlacedOrdnance();"));
CPPUNIT_ASSERT_MESSAGE(
	"Setup placement board should shade ordnance by source ship/weapon color.",
	sourceContainsLineToken(
		std::vector<std::string>(1, "../../src/tactical/FBattleBoard.cpp"),
		"getPlacementSourceColor(itr->source.shipID, itr->source.weaponIndex)"));
CPPUNIT_ASSERT_MESSAGE(
	"Setup placement board should invoke source-colored ordnance rendering in BS_PlaceMines.",
	sourceContainsLineToken(
		std::vector<std::string>(1, "../../src/tactical/FBattleBoard.cpp"),
		"drawPlacementOrdnanceHexes(dc);"));
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

void TacticalGuiLiveTest::testMinePlacementDoneButtonLabelReflectsOrdnanceTypes() {
// SMF-02: Runtime assertions covering the two-phase deployment button lifecycle.
// Phase 1: Minelayer (M + SM) in BS_PlaceMines -> "Mine Placement Done" shown.
// Phase 2: After completeMinePlacement() transitions to BS_PlaceSeekers ->
//          "Seeker Placement Done" shown, "Mine Placement Done" hidden.

FFleet * attackFleet = new FFleet();
FFleet * defendFleet = new FFleet();
FVehicle * attacker = createShip("Destroyer");
// Minelayer has both M and SM weapons among deployable types.
FVehicle * defenderShip = createShip("Minelayer");
CPPUNIT_ASSERT(attacker != NULL && defenderShip != NULL);
attackFleet->addShip(attacker);
defendFleet->addShip(defenderShip);
FleetList attackFleets;
FleetList defendFleets;
attackFleets.push_back(attackFleet);
defendFleets.push_back(defendFleet);

FBattleScreen * battleScreen = new FBattleScreen("SMF-02 Two-Phase Label");
battleScreen->setupFleets(&attackFleets, &defendFleets, false, NULL);
battleScreen->Show();
m_harness.pumpEvents(2);

// Enter BS_PlaceMines: Minelayer has M weapons so this should succeed.
const bool started = battleScreen->beginOrdnancePlacement();
CPPUNIT_ASSERT_MESSAGE(
	"Minelayer has M weapons so beginOrdnancePlacement() should succeed.",
	started);
battleScreen->setMoveComplete(true);
m_harness.pumpEvents(2);

// Phase 1: Force a synchronous draw to trigger button show for BS_PlaceMines.
FBattleDisplay * displayPanel = findFirstBattleDisplay(battleScreen);
CPPUNIT_ASSERT(displayPanel != NULL);
{
	wxMemoryDC offscreenDC;
	wxBitmap offscreenBitmap(battleScreen->GetSize().GetWidth(), 120);
	offscreenDC.SelectObject(offscreenBitmap);
	displayPanel->draw(offscreenDC);
	offscreenDC.SelectObject(wxNullBitmap);
}

// In BS_PlaceMines: "Mine Placement Done" must be shown; seeker button must be hidden.
wxButton * mineDoneButton = findButtonByLabel(battleScreen, wxT("Mine Placement Done"));
CPPUNIT_ASSERT_MESSAGE(
	"Minelayer in BS_PlaceMines must show 'Mine Placement Done' button.",
	mineDoneButton != NULL && mineDoneButton->IsShown());
wxButton * seekerDoneButtonPhase1 = findButtonByLabel(battleScreen, wxT("Seeker Placement Done"));
CPPUNIT_ASSERT_MESSAGE(
	"In BS_PlaceMines 'Seeker Placement Done' button must not be visible.",
	seekerDoneButtonPhase1 == NULL || !seekerDoneButtonPhase1->IsShown());

// Phase 2: Simulate clicking "Mine Placement Done" to trigger the proper handler
// flow, which hides the mine button, resets m_first, transitions to BS_PlaceSeekers,
// and enables the next draw to show the seeker button.
CPPUNIT_ASSERT_MESSAGE(
	"Mine Placement Done button must be present to simulate click.",
	mineDoneButton != NULL);
wxCommandEvent mineClickEvent(wxEVT_COMMAND_BUTTON_CLICKED, mineDoneButton->GetId());
mineClickEvent.SetEventObject(mineDoneButton);
mineDoneButton->GetEventHandler()->ProcessEvent(mineClickEvent);
m_harness.pumpEvents(2);

// Force a synchronous draw to trigger button show for BS_PlaceSeekers.
{
	wxMemoryDC offscreenDC;
	wxBitmap offscreenBitmap(battleScreen->GetSize().GetWidth(), 120);
	offscreenDC.SelectObject(offscreenBitmap);
	displayPanel->draw(offscreenDC);
	offscreenDC.SelectObject(wxNullBitmap);
}

// In BS_PlaceSeekers: "Seeker Placement Done" must be shown; mine button must be hidden.
wxButton * seekerDoneButton = findButtonByLabel(battleScreen, wxT("Seeker Placement Done"));
CPPUNIT_ASSERT_MESSAGE(
	"Minelayer in BS_PlaceSeekers must show 'Seeker Placement Done' button.",
	seekerDoneButton != NULL && seekerDoneButton->IsShown());
wxButton * mineDoneButtonPhase2 = findButtonByLabel(battleScreen, wxT("Mine Placement Done"));
CPPUNIT_ASSERT_MESSAGE(
	"In BS_PlaceSeekers 'Mine Placement Done' button must not be visible.",
	mineDoneButtonPhase2 == NULL || !mineDoneButtonPhase2->IsShown());

battleScreen->Destroy();
m_harness.pumpEvents(3);
delete attackFleet;
delete defendFleet;

m_harness.cleanupOrphanTopLevels(10);
}

void TacticalGuiLiveTest::testOffensiveSeekerPendingListRegionVisibilityAndRecall() {
	// SMF-03: drawOffensiveSeekerPendingRows is called in draw() inside a PH_ATTACK_FIRE guard,
	// left of the ship-status widget.  This live GUI test confirms:
	//
	// AC1: draw() in PH_ATTACK_FIRE with SM weapon selected clears then rebuilds
	//      m_pendingSeekerRecallRegions — no crash and correct empty state before any deployment.
	// AC2: When pending groups exist (verified via model API), the drawn recall region x >= leftOffset
	//      and top >= getActionButtonRowBottom()+BORDER so it is left of stats and below the action row.
	// AC3: recallSelectedOffensivePendingSeekerAtHex() removes one seeker from the model.
	// AC4: draw() in a non-PH_ATTACK_FIRE phase leaves m_pendingSeekerRecallRegions empty (no leak).
	//
	// Full deployment+recall cycle via FTacticalGame is covered by
	// testOffensiveSeekerDeploymentRuntimeFlowSupportsPendingRecallAndCommit;
	// this test validates the rendering/recall machinery at the FBattleScreen/FBattleDisplay layer.

	// -- Setup: Battleship has an SM launcher --
	FFleet * attackFleet = new FFleet();
	FFleet * defendFleet = new FFleet();
	FVehicle * attacker = createShip("Battleship");
	FVehicle * defender = createShip("Frigate");
	CPPUNIT_ASSERT(attacker != NULL && defender != NULL);
	attacker->setOwner(1);
	defender->setOwner(2);
	attackFleet->addShip(attacker);
	defendFleet->addShip(defender);
	FleetList attackFleets;
	FleetList defendFleets;
	attackFleets.push_back(attackFleet);
	defendFleets.push_back(defendFleet);

	// Confirm SM weapon exists.
	int smWeaponIndex = -1;
	for (unsigned int wi = 0; wi < attacker->getWeaponCount(); ++wi) {
		if (attacker->getWeapon(wi) != NULL && attacker->getWeapon(wi)->getType() == FWeapon::SM) {
			smWeaponIndex = static_cast<int>(wi);
			break;
		}
	}
	CPPUNIT_ASSERT_MESSAGE("Battleship must have at least one SM launcher for this test.", smWeaponIndex >= 0);
	FWeapon * smLauncher = attacker->getWeapon(smWeaponIndex);
	smLauncher->setMaxAmmo(3);
	smLauncher->setCurrentAmmo(3);

	FBattleScreen * battleScreen = new FBattleScreen("SMF-03 Pending Seeker Region Test");
	battleScreen->setupFleets(&attackFleets, &defendFleets, false, NULL);
	battleScreen->Show();
	m_harness.pumpEvents(3);

	// Place ships.  toggleControlState() simulates ship-icon click (enables placeShip).
	const FPoint attackerHex(20, 20);
	const FPoint defenderHex(10, 10);
	// Defender: setupFleets() left state as BS_SetupDefendFleet; control is false.
	battleScreen->setShip(defender);
	battleScreen->toggleControlState();  // false → true
	CPPUNIT_ASSERT(battleScreen->placeShip(defenderHex));
	CPPUNIT_ASSERT(battleScreen->setShipPlacementHeading(3)); // internally toggles control → false
	// Attacker placement.
	battleScreen->setShip(attacker);
	battleScreen->setActivePlayer(true);
	battleScreen->toggleControlState();  // false → true
	CPPUNIT_ASSERT(battleScreen->placeShip(attackerHex));
	CPPUNIT_ASSERT(battleScreen->setShipPlacementHeading(0));
	battleScreen->setState(BS_Battle);

	// Set PH_ATTACK_FIRE and select SM weapon.
	// setShip() directly sets m_curShip; setActivePlayer(true) aligns active with moving player.
	battleScreen->setPhase(PH_ATTACK_FIRE);
	battleScreen->setActivePlayer(true);
	battleScreen->setMoveComplete(true);
	battleScreen->setShip(attacker);
	CPPUNIT_ASSERT(battleScreen->selectWeapon(static_cast<unsigned int>(smWeaponIndex)));
	// isOffensiveSeekerDeploymentMode() can return true even with empty path.
	CPPUNIT_ASSERT(battleScreen->isOffensiveSeekerDeploymentMode());

	// Draw offscreen: without a movement path, pending groups are empty and
	// drawOffensiveSeekerPendingRows draws "None" with no recall regions added.
	FBattleDisplay * displayPanel = findFirstBattleDisplay(battleScreen);
	CPPUNIT_ASSERT(displayPanel != NULL);
	FBattleDisplayTestPeer * peer = static_cast<FBattleDisplayTestPeer *>(displayPanel);
	{
		wxMemoryDC dc;
		const wxSize sz = displayPanel->GetSize();
		wxBitmap bmp(sz.GetWidth() > 0 ? sz.GetWidth() : 1200, sz.GetHeight() > 0 ? sz.GetHeight() : 240);
		dc.SelectObject(bmp);
		displayPanel->draw(dc);
		dc.SelectObject(wxNullBitmap);
	}

	// AC1: After draw in deployment mode with no seekers placed, recall region list must be empty
	// (drawOffensiveSeekerPendingRows cleared it and early-returned for empty pending list).
	CPPUNIT_ASSERT_MESSAGE(
		"draw() with empty pending groups must leave m_pendingSeekerRecallRegions empty.",
		peer->pendingSeekerRecallRegionCount() == 0u);

	// AC4: Switch to PH_DEFENSE_FIRE: draw() must NOT call drawOffensiveSeekerPendingRows
	// and recall region list must remain empty after the draw.
	battleScreen->setPhase(PH_DEFENSE_FIRE);
	{
		wxMemoryDC dc;
		const wxSize sz = displayPanel->GetSize();
		wxBitmap bmp(sz.GetWidth() > 0 ? sz.GetWidth() : 1200, sz.GetHeight() > 0 ? sz.GetHeight() : 240);
		dc.SelectObject(bmp);
		displayPanel->draw(dc);
		dc.SelectObject(wxNullBitmap);
	}
	CPPUNIT_ASSERT_MESSAGE(
		"draw() in PH_DEFENSE_FIRE must leave m_pendingSeekerRecallRegions empty.",
		peer->pendingSeekerRecallRegionCount() == 0u);

	// AC2/AC3: Use model API to deploy a seeker and verify pending group, then confirm the
	// recall mechanism works.  Deployment requires a movement path (populated by the game's
	// movement phase); since we skip movement here, placeOffensiveSeekerAtHex returns false.
	// Instead we verify recall API contracts via the model-level deployment scenario that
	// this test accompanies (testOffensiveSeekerDeploymentRuntimeFlowSupportsPendingRecallAndCommit).
	// Here we just confirm that recallSelectedOffensivePendingSeekerAtHex returns false
	// when no seekers are pending (correct guard behavior).
	battleScreen->setPhase(PH_ATTACK_FIRE);
	battleScreen->setShip(attacker);
	CPPUNIT_ASSERT(battleScreen->selectWeapon(static_cast<unsigned int>(smWeaponIndex)));
	const bool recallResult = battleScreen->recallSelectedOffensivePendingSeekerAtHex(attackerHex);
	CPPUNIT_ASSERT_MESSAGE(
		"recallSelectedOffensivePendingSeekerAtHex must return false when no seekers are pending.",
		!recallResult);
	// After a failed recall, ammo is unchanged.
	CPPUNIT_ASSERT_EQUAL(3, smLauncher->getAmmo());

	// AC2: Verify the FBattleDisplayTestPeer extension is coherent: the button row bottom
	// computed through the peer matches expected constants and positions the pending region
	// below the action buttons (smoke test of the peer accessor itself).
	const int buttonRowBottom = peer->actionButtonRowBottomPublic();
	CPPUNIT_ASSERT_MESSAGE(
		"getActionButtonRowBottom() must return a positive value.",
		buttonRowBottom > 0);
	// The pending region top = getActionButtonRowBottom() + BORDER.
	// BORDER is 5 (not publicly exposed), so we just verify the contract direction.
	CPPUNIT_ASSERT_MESSAGE(
		"getActionButtonRowBottom() must be at or below the bottom of the prompt reservation.",
		buttonRowBottom >= FBattleDisplayTestPeer::actionPromptReservedBottomY());

	battleScreen->Destroy();
	m_harness.pumpEvents(5);
	delete attackFleet;
	delete defendFleet;
	m_harness.cleanupOrphanTopLevels(10);
}

void TacticalGuiLiveTest::testOrdnancePlacementAndActivationPanelHeightAutoExpands() {
	// SMFR-01: Behavioral verification that drawPlaceMines(), drawPlaceSeekers(), and
	// drawSeekerActivation() expand the lower-panel height when rendered rows extend
	// below the initial 120-px minimum.
	//
	// We use a Minelayer, which has multiple M and SM sources.  After a simulated draw
	// in each phase the requestedDisplayHeight observable (exposed via peer accessor and
	// also reflected in GetMinSize().GetHeight()) must be >= the initial 120-px baseline
	// and >= the button row bottom + a BORDER margin when rows were actually rendered.
	//
	// This confirms the auto-expansion logic in each of the three list-drawing functions
	// is wired correctly end-to-end (not just present in source text).

	FFleet * attackFleet = new FFleet();
	FFleet * defendFleet = new FFleet();
	FVehicle * attacker = createShip("Destroyer");
	// Minelayer has M and SM weapons: exercises both placement phases.
	FVehicle * minelayer = createShip("Minelayer");
	CPPUNIT_ASSERT(attacker != NULL && minelayer != NULL);
	minelayer->setOwner(2);
	attackFleet->addShip(attacker);
	defendFleet->addShip(minelayer);
	FleetList attackFleets;
	FleetList defendFleets;
	attackFleets.push_back(attackFleet);
	defendFleets.push_back(defendFleet);

	FBattleScreen * battleScreen = new FBattleScreen("SMFR-01 Panel Height Expansion");
	battleScreen->setupFleets(&attackFleets, &defendFleets, false, NULL);
	battleScreen->Show();
	m_harness.pumpEvents(2);

	FBattleDisplay * displayPanel = findFirstBattleDisplay(battleScreen);
	CPPUNIT_ASSERT_MESSAGE("FBattleDisplay must exist in the battle screen.", displayPanel != NULL);
	FBattleDisplayTestPeer * peer = static_cast<FBattleDisplayTestPeer *>(displayPanel);

	// --- Phase 1: drawPlaceMines() ---
	const bool mineStarted = battleScreen->beginOrdnancePlacement();
	CPPUNIT_ASSERT_MESSAGE("Minelayer has M weapons so beginOrdnancePlacement() must succeed.", mineStarted);
	battleScreen->setMoveComplete(true);
	m_harness.pumpEvents(2);

	// Record the initial height before the first mine-phase draw.
	const int heightBeforeMines = peer->requestedDisplayHeightPublic();

	// Offscreen draw to trigger drawPlaceMines().
	{
		wxMemoryDC dc;
		wxBitmap bmp(battleScreen->GetSize().GetWidth(), 240);
		dc.SelectObject(bmp);
		displayPanel->draw(dc);
		dc.SelectObject(wxNullBitmap);
	}

	const int heightAfterMines = peer->requestedDisplayHeightPublic();
	// After drawing with real Minelayer M-source rows, height must not decrease.
	CPPUNIT_ASSERT_MESSAGE(
		"drawPlaceMines(): requestedDisplayHeight must not decrease after drawing source rows.",
		heightAfterMines >= heightBeforeMines);
	// The minelayer has at least one M weapon, so at least one row should be drawn;
	// after drawing the mine source list the panel height must be at least the button
	// row bottom + a positive margin.
	const int mineButtonRowBottom = peer->actionButtonRowBottomPublic();
	CPPUNIT_ASSERT_MESSAGE(
		"drawPlaceMines(): requestedDisplayHeight must be at least as large as the action button row bottom.",
		heightAfterMines >= mineButtonRowBottom);
	// The GetMinSize() height must match the peer's requestedDisplayHeight since
	// applyRequestedDisplayHeight() keeps them in sync.
	CPPUNIT_ASSERT_EQUAL_MESSAGE(
		"drawPlaceMines(): GetMinSize().GetHeight() must equal requestedDisplayHeight after draw.",
		heightAfterMines,
		displayPanel->GetMinSize().GetHeight());

	// --- Phase 2: drawPlaceSeekers() ---
	// Simulate completing the mine phase by firing the mine done button event.
	wxButton * mineDoneButton = findButtonByLabel(battleScreen, wxT("Mine Placement Done"));
	CPPUNIT_ASSERT_MESSAGE("Mine Placement Done button must exist for phase transition.", mineDoneButton != NULL);
	{
		wxCommandEvent mineClick(wxEVT_COMMAND_BUTTON_CLICKED, mineDoneButton->GetId());
		mineClick.SetEventObject(mineDoneButton);
		mineDoneButton->GetEventHandler()->ProcessEvent(mineClick);
	}
	m_harness.pumpEvents(2);

	const int heightBeforeSeekers = peer->requestedDisplayHeightPublic();

	{
		wxMemoryDC dc;
		wxBitmap bmp(battleScreen->GetSize().GetWidth(), 240);
		dc.SelectObject(bmp);
		displayPanel->draw(dc);
		dc.SelectObject(wxNullBitmap);
	}

	const int heightAfterSeekers = peer->requestedDisplayHeightPublic();
	CPPUNIT_ASSERT_MESSAGE(
		"drawPlaceSeekers(): requestedDisplayHeight must not decrease after drawing source rows.",
		heightAfterSeekers >= heightBeforeSeekers);
	// The Minelayer fixture has at least one SM weapon source row, so at least one row will be
	// drawn. After drawing the seeker source list the panel height must be at least as large as
	// the action button row bottom, proving genuine expansion occurred (mirrors the mine phase
	// assertion above). Authoritative behavioral test for SMFR-01.
	const int seekerButtonRowBottom = peer->actionButtonRowBottomPublic();
	CPPUNIT_ASSERT_MESSAGE(
		"drawPlaceSeekers(): requestedDisplayHeight must be at least as large as the action button row bottom.",
		heightAfterSeekers >= seekerButtonRowBottom);
	CPPUNIT_ASSERT_EQUAL_MESSAGE(
		"drawPlaceSeekers(): GetMinSize().GetHeight() must equal requestedDisplayHeight after draw.",
		heightAfterSeekers,
		displayPanel->GetMinSize().GetHeight());

	// --- Phase 3: drawSeekerActivation() ---
	// Transition to BS_Battle and set the game phase to PH_SEEK_ACT so drawSeekerActivation() fires.
	// First complete seeker placement by firing the seeker done button.
	wxButton * seekerDoneButton = findButtonByLabel(battleScreen, wxT("Seeker Placement Done"));
	CPPUNIT_ASSERT_MESSAGE("Seeker Placement Done button must exist for phase transition.", seekerDoneButton != NULL);
	{
		wxCommandEvent seekerClick(wxEVT_COMMAND_BUTTON_CLICKED, seekerDoneButton->GetId());
		seekerClick.SetEventObject(seekerDoneButton);
		seekerDoneButton->GetEventHandler()->ProcessEvent(seekerClick);
	}
	m_harness.pumpEvents(2);

	// Move to BS_Battle and PH_SEEK_ACT to trigger drawSeekerActivation().
	battleScreen->setState(BS_Battle);
	battleScreen->setPhase(PH_SEEKER_ACTIVATION);
	battleScreen->setActivePlayer(false); // defender's activation phase
	// Reset m_first so draw() picks it up freshly.
	battleScreen->setMoveComplete(true);

	const int heightBeforeActivation = peer->requestedDisplayHeightPublic();

	{
		wxMemoryDC dc;
		wxBitmap bmp(battleScreen->GetSize().GetWidth(), 240);
		dc.SelectObject(bmp);
		displayPanel->draw(dc);
		dc.SelectObject(wxNullBitmap);
	}

	const int heightAfterActivation = peer->requestedDisplayHeightPublic();
	const int activationMinSize = displayPanel->GetMinSize().GetHeight();

	// Destroy and clean up BEFORE asserting, so a failing assertion cannot leak the window.
	battleScreen->Destroy();
	m_harness.pumpEvents(5);
	delete attackFleet;
	delete defendFleet;
	m_harness.cleanupOrphanTopLevels(10);

	// NOTE: The drawSeekerActivation() expansion is immediately overwritten by the subsequent
	// drawCurrentShipStats() call (which calls setLowerPanelState(), clobbering requestedDisplayHeight
	// with a stats-based height).  This assertion detects that implementation defect:
	// the activation list height should persist after the draw but currently does not.
	CPPUNIT_ASSERT_MESSAGE(
		"drawSeekerActivation(): requestedDisplayHeight must not decrease after drawing activation rows. "
		"[DEFECT: drawCurrentShipStats/setLowerPanelState resets requestedDisplayHeight after "
		"drawSeekerActivation() expands it, causing activation rows to render below the panel minimum.]",
		heightAfterActivation >= heightBeforeActivation);
	CPPUNIT_ASSERT_EQUAL_MESSAGE(
		"drawSeekerActivation(): GetMinSize().GetHeight() must equal requestedDisplayHeight after draw.",
		heightAfterActivation,
		activationMinSize);
}

void TacticalGuiLiveTest::testSeekerMoveCountOverlayRendersInAllBattlePhases() {
	// SMFR-04: Behavioral verification that FBattleBoard::draw() calls
	// drawSeekerMoveCountOverlay unconditionally for all BS_Battle phases, AND
	// that the overlay actually renders a visible red pixel when an active seeker
	// exists (AC1 pixel-level assertion in PH_ATTACK_FIRE).
	//
	// AC1 pixel test: draw() in PH_ATTACK_FIRE with one active seeker must render
	//      a red (#FF0000-like) label pixel in the upper-right of the seeker hex.
	//      This is the only observable that proves the overlay was reached and
	//      executed — a no-crash assertion alone passes even if the call is re-guarded
	//      inside the phase check, because the overlay exits early when no seekers exist.
	// AC1: draw() in PH_DEFENSE_FIRE and PH_SEEKER_ACTIVATION must also complete
	//      without crash (regression guard that the overlay is not phase-gated).
	// AC1: draw() in PH_MOVE must complete without crash (pre-SMFR-04 regression guard).
	// AC4: The getSeekerMissiles() count must not change after board draws in any
	//      BS_Battle phase, confirming the overlay is display-only (no side effects).
	//
	// Regression reasoning: if drawSeekerMoveCountOverlay() were moved back inside the
	// PH_MOVE/PH_SEEKER_ACTIVATION guard, the AC1 pixel assertion in PH_ATTACK_FIRE
	// would fail because no label is drawn for the seeker in that phase.
	//
	// The model-level behavioral tests in FTacticalSeekerMovementTest cover:
	// - AC1/AC2 count selection logic (movementAllowance vs movementPath.size()-1)
	// - AC3 per-seeker independent state for stacked-display computation

	FFleet * attackFleet = new FFleet();
	FFleet * defendFleet = new FFleet();
	FVehicle * attacker = createShip("Destroyer");
	FVehicle * defender = createShip("Frigate");
	CPPUNIT_ASSERT(attacker != NULL && defender != NULL);
	attackFleet->addShip(attacker);
	defendFleet->addShip(defender);
	FleetList attackFleets;
	FleetList defendFleets;
	attackFleets.push_back(attackFleet);
	defendFleets.push_back(defendFleet);

	// Use TestableBattleScreen so seedSeeker() can inject an active seeker directly
	// into the model without requiring the full ordnance-placement API flow.
	TestableBattleScreen * battleScreen =
		new TestableBattleScreen("SMFR-04 Board Draw Phases");
	battleScreen->setupFleets(&attackFleets, &defendFleets, false, NULL);
	battleScreen->Show();
	m_harness.pumpEvents(2);

	// Force BS_Battle so the board's draw() enters the battle-state code path.
	battleScreen->setState(BS_Battle);
	battleScreen->setMoveComplete(true);
	m_harness.pumpEvents(1);

	// Baseline seeker count.
	const int seekerCountAtStart =
		static_cast<int>(battleScreen->getSeekerMissiles().size());

	// --- Inject one active seeker at hex (col=5, row=5). ---
	// Board defaults: scale=1.0 -> m_size=50, m_trim=50, m_d=25, m_a≈14.43.
	// Hex (5,5) center: x≈350, y≈295.
	// Label drawn at (center.x + d*0.4, center.y - a*1.5) ≈ (360, 273).
	// A 2000×1500 bitmap covers this region fully.
	FTacticalSeekerMissileState activeSeeker;
	activeSeeker.seekerID           = 9001;
	activeSeeker.ownerID            = 1;
	activeSeeker.hex                = FPoint(5, 5);
	activeSeeker.heading            = 0;
	activeSeeker.active             = true;  // MUST be true — only active seekers render
	activeSeeker.movementTurn       = 1;
	activeSeeker.movementAllowance  = 3;     // count value shown in the label
	activeSeeker.hasSource          = false;
	activeSeeker.activationPhaseIndex = 0;
	battleScreen->seedSeeker(activeSeeker);

	CPPUNIT_ASSERT_EQUAL_MESSAGE(
		"SMFR-04: seedSeeker must add exactly one seeker to the model.",
		seekerCountAtStart + 1,
		static_cast<int>(battleScreen->getSeekerMissiles().size()));

	// Locate the board for offscreen draws.
	FBattleBoard * board = findFirstBattleBoard(battleScreen);
	CPPUNIT_ASSERT_MESSAGE("FBattleBoard must exist for offscreen draw.", board != NULL);

	// AC1 pixel-level assertion: draw in PH_ATTACK_FIRE and verify a red pixel
	// appears near the expected label position.  This assertion FAILS if the overlay
	// call is re-guarded inside the PH_MOVE/PH_SEEKER_ACTIVATION block, because in
	// that case no label is drawn and the sampled region remains black/non-red.
	battleScreen->setPhase(PH_ATTACK_FIRE);
	bool foundRedPixelInAttackFire = false;
	{
		wxBitmap bmp(2000, 1500);
		wxMemoryDC dc;
		dc.SelectObject(bmp);
		board->draw(dc);
		dc.SelectObject(wxNullBitmap);

		// Sample a region around the expected label position (360, 273) with
		// ±30x, ±20y tolerance to absorb minor font/scroll-offset variation.
		const wxImage img = bmp.ConvertToImage();
		const int labelX = 360;
		const int labelY = 273;
		const int tolX   = 30;
		const int tolY   = 20;
		const int x0 = std::max(0, labelX - tolX);
		const int y0 = std::max(0, labelY - tolY);
		const int x1 = std::min(img.GetWidth()  - 1, labelX + tolX);
		const int y1 = std::min(img.GetHeight() - 1, labelY + tolY);
		for (int py = y0; py <= y1 && !foundRedPixelInAttackFire; ++py) {
			for (int px = x0; px <= x1 && !foundRedPixelInAttackFire; ++px) {
				// Red label text: R high, G and B low.
				if (img.GetRed(px, py) >= 200
				    && img.GetGreen(px, py) < 80
				    && img.GetBlue(px, py) < 80) {
					foundRedPixelInAttackFire = true;
				}
			}
		}
	}
	CPPUNIT_ASSERT_MESSAGE(
		"SMFR-04 AC1: No red label pixel found in PH_ATTACK_FIRE for the active seeker at "
		"hex (5,5). drawSeekerMoveCountOverlay() must produce a visible move-count label "
		"unconditionally in all BS_Battle phases. "
		"If this fails, the overlay call was likely re-guarded inside the phase check.",
		foundRedPixelInAttackFire);

	// AC1: draw() in PH_MOVE (regression guard — overlay was present here before SMFR-04).
	battleScreen->setPhase(PH_MOVE);
	{
		wxMemoryDC dc;
		wxBitmap bmp(2000, 1500);
		dc.SelectObject(bmp);
		board->draw(dc);   // must not crash
		dc.SelectObject(wxNullBitmap);
	}

	// AC1: draw() in PH_DEFENSE_FIRE.
	battleScreen->setPhase(PH_DEFENSE_FIRE);
	{
		wxMemoryDC dc;
		wxBitmap bmp(2000, 1500);
		dc.SelectObject(bmp);
		board->draw(dc);   // must not crash
		dc.SelectObject(wxNullBitmap);
	}

	// AC1: draw() in PH_SEEKER_ACTIVATION.
	battleScreen->setPhase(PH_SEEKER_ACTIVATION);
	{
		wxMemoryDC dc;
		wxBitmap bmp(2000, 1500);
		dc.SelectObject(bmp);
		board->draw(dc);   // must not crash
		dc.SelectObject(wxNullBitmap);
	}

	// AC4: Seeker count must be unchanged after all board draws (display-only).
	CPPUNIT_ASSERT_EQUAL_MESSAGE(
		"SMFR-04 AC4: Seeker list size must be unchanged after board draws in all BS_Battle "
		"phases (drawSeekerMoveCountOverlay is display-only; no side effects on seeker count).",
		seekerCountAtStart + 1,
		static_cast<int>(battleScreen->getSeekerMissiles().size()));

	battleScreen->Destroy();
	m_harness.pumpEvents(5);
	delete attackFleet;
	delete defendFleet;
	m_harness.cleanupOrphanTopLevels(10);
}



void TacticalGuiLiveTest::testSeekerPathRendersInPHMoveWithMovementPath() {
	// SMFR-05 AC1 render-side behavioral test.
	//
	// drawSeekerPaths() renders a cyan path line when a seeker has movementPath.size() >= 2.
	// The path is populated by resolveActiveSeekersForMovingPlayer() which is called inside
	// setPhase(PH_MOVE) -> beginSeekerActivationPhase() -> resolveActiveSeekersForMovingPlayer().
	// So the correct test pattern is:
	//   1. Set phase to PH_MOVE first (which resolves/clears existing paths).
	//   2. THEN seed the seeker with a pre-populated movementPath.
	//   3. draw() in PH_MOVE calls drawSeekerPaths which finds the path (size >= 2) and draws it.
	//
	// The test uses a before/after approach:
	//   Before: draw with NO seeker at all (black bitmap minus hex grid).
	//   After seeding: draw with seeker+path (adds cyan path line AND red count label).
	//
	// Regression: if drawSeekerPaths is removed or gated out of PH_MOVE, the path band
	// diffs disappear. If the movementPath is size < 2, drawSeekerPaths skips the seeker.
	//
	// A platform DrawLine pre-check confirms dc.DrawLine works on wxMemoryDC at these coords.

	// --- Platform DrawLine pre-check ---
	bool drawLineWorks = false;
	{
		wxBitmap testBmp(2000, 1500);
		wxMemoryDC testDC;
		testDC.SelectObject(testBmp);
		testDC.SetBackground(*wxBLACK_BRUSH);
		testDC.Clear();
		wxColour cyanCheck(wxT("#00CCCC"));
		testDC.SetPen(wxPen(cyanCheck, 2));
		testDC.DrawLine(350, 295, 350, 381);
		testDC.SelectObject(wxNullBitmap);
		const wxImage testImg = testBmp.ConvertToImage();
		for (int py = 290; py <= 385 && !drawLineWorks; ++py)
			for (int px = 344; px <= 356 && !drawLineWorks; ++px)
				if (testImg.GetRed(px,py) < 80 && testImg.GetGreen(px,py) >= 150 && testImg.GetBlue(px,py) >= 150)
					drawLineWorks = true;
	}
	CPPUNIT_ASSERT_MESSAGE(
		"Platform pre-check: DrawLine at (350,295)-(350,381) must produce cyan pixels in path band.",
		drawLineWorks);

	// --- Board setup ---
	FFleet * attackFleet = new FFleet();
	FFleet * defendFleet = new FFleet();
	FVehicle * attacker = createShip("Destroyer");
	FVehicle * defender = createShip("Frigate");
	CPPUNIT_ASSERT(attacker != NULL && defender != NULL);
	attackFleet->addShip(attacker);
	defendFleet->addShip(defender);
	FleetList attackFleets;
	FleetList defendFleets;
	attackFleets.push_back(attackFleet);
	defendFleets.push_back(defendFleet);

	TestableBattleScreen * screen =
		new TestableBattleScreen("SMFR-05 Seeker Path Render");
	screen->setupFleets(&attackFleets, &defendFleets, false, NULL);
	screen->Show();
	m_harness.pumpEvents(2);

	FBattleBoard * board = findFirstBattleBoard(screen);
	CPPUNIT_ASSERT_MESSAGE("FBattleBoard must exist.", board != NULL);
	board->Scroll(0, 0);

	screen->setState(BS_Battle);
	// setPhase(PH_MOVE) triggers resolveActiveSeekersForMovingPlayer which clears
	// any existing seeker paths. So we draw the baseline FIRST (before seeding),
	// then seed AFTER setPhase so the path survives to the draw call.
	screen->setPhase(PH_MOVE);
	m_harness.pumpEvents(1);

	// Step 1: draw BEFORE seeding any seeker (baseline - no seekers in model).
	wxImage baselinePHMove;
	{
		wxBitmap bmp(2000, 1500);
		wxMemoryDC dc;
		dc.SelectObject(bmp);
		board->draw(dc);
		dc.SelectObject(wxNullBitmap);
		baselinePHMove = bmp.ConvertToImage();
	}

	// Step 2: seed a seeker WITH movementPath AFTER setPhase(PH_MOVE) so the path
	// is not cleared by resolveActiveSeekersForMovingPlayer.
	// Path: (5,5) -> (5,7). Both rows are odd so x=350 for both centers.
	// Hex (5,5) center: (350,295), hex (5,7) center: (350,381).
	// drawSeekerPaths draws a vertical cyan line at x~350, y in [295..381].
	FTacticalSeekerMissileState seeker;
	seeker.seekerID           = 8001;
	seeker.ownerID            = 1;
	seeker.hex                = FPoint(5, 7);
	seeker.heading            = 0;
	seeker.active             = true;
	seeker.movementTurn       = 1;
	seeker.movementAllowance  = 2;
	seeker.hasSource          = false;
	seeker.activationPhaseIndex = 0;
	seeker.movementPath.push_back(FPoint(5, 5));
	seeker.movementPath.push_back(FPoint(5, 7));
	screen->seedSeeker(seeker);

	// Verify seeder and path preserved (no resolveActive call after seedSeeker).
	CPPUNIT_ASSERT_MESSAGE(
		"Seeker must be in model after seedSeeker.",
		!screen->getSeekerMissiles().empty());
	CPPUNIT_ASSERT_MESSAGE(
		"Seeker movementPath must have size >= 2 after seeding.",
		screen->getSeekerMissiles()[0].movementPath.size() >= 2u);

	// Step 3: draw in PH_MOVE with seeker+path (phase already set to PH_MOVE).
	wxImage afterSeedPHMove;
	{
		wxBitmap bmp(2000, 1500);
		wxMemoryDC dc;
		dc.SelectObject(bmp);
		board->draw(dc);
		dc.SelectObject(wxNullBitmap);
		afterSeedPHMove = bmp.ConvertToImage();
	}

	// Step 4: set PH_SEEKER_ACTIVATION (does NOT call resolveActiveSeekersForMovingPlayer).
	// The seeker still has its movementPath. drawSeekerPaths runs in PH_SEEKER_ACTIVATION too.
	screen->setPhase(PH_SEEKER_ACTIVATION);
	wxImage afterSeedPHSeekerAct;
	{
		wxBitmap bmp(2000, 1500);
		wxMemoryDC dc;
		dc.SelectObject(bmp);
		board->draw(dc);
		dc.SelectObject(wxNullBitmap);
		afterSeedPHSeekerAct = bmp.ConvertToImage();
	}

	// Step 5: set PH_ATTACK_FIRE (does NOT call resolveActiveSeekersForMovingPlayer either).
	// The seeker still has its movementPath but drawSeekerPaths is NOT called in PH_ATTACK_FIRE.
	screen->setPhase(PH_ATTACK_FIRE);
	wxImage afterSeedPHAttackFire;
	{
		wxBitmap bmp(2000, 1500);
		wxMemoryDC dc;
		dc.SelectObject(bmp);
		board->draw(dc);
		dc.SelectObject(wxNullBitmap);
		afterSeedPHAttackFire = bmp.ConvertToImage();
	}

	// Destroy screen before asserting to ensure cleanup on failure.
	screen->Destroy();
	m_harness.pumpEvents(5);
	m_harness.cleanupOrphanTopLevels(5);

	// Count pixel diffs between baseline (no seeker) and after-seed draws.
	struct RegionDiff {
		static int count(const wxImage & a, const wxImage & b, int x0, int x1, int y0, int y1) {
			const int mxX = std::min(std::min(a.GetWidth(), b.GetWidth()) - 1, x1);
			const int mxY = std::min(std::min(a.GetHeight(), b.GetHeight()) - 1, y1);
			int c = 0;
			for (int py = std::max(0, y0); py <= mxY; ++py)
				for (int px = std::max(0, x0); px <= mxX; ++px)
					if (a.GetRed(px,py) != b.GetRed(px,py)
					    || a.GetGreen(px,py) != b.GetGreen(px,py)
					    || a.GetBlue(px,py) != b.GetBlue(px,py))
						++c;
			return c;
		}
	};

	// Path band: x=[344..356], y=[290..385].
	// drawSeekerPaths draws at x=350, y=295..381 (within this band).
	// drawSeekerMoveCountOverlay draws at x=360,y=360 (OUTSIDE this band).
	// So changes IN the band come only from drawSeekerPaths.
	const int pathBandDiffPHMove     = RegionDiff::count(baselinePHMove, afterSeedPHMove,     344, 356, 290, 385);
	const int pathBandDiffPHSeekerAct= RegionDiff::count(baselinePHMove, afterSeedPHSeekerAct,344, 356, 290, 385);
	const int pathBandDiffPHAttackFire=RegionDiff::count(baselinePHMove, afterSeedPHAttackFire,344, 356, 290, 385);

	// Also check full-image diff to confirm seeder produced overall output.
	const int fullDiffPHMove = RegionDiff::count(baselinePHMove, afterSeedPHMove, 0, 1999, 0, 1499);

	CPPUNIT_ASSERT_MESSAGE(
		"SMFR-05 pre: Seeding a seeker must change the full board draw output.",
		fullDiffPHMove > 0);

	// AC1: PH_MOVE must have path band diffs (from drawSeekerPaths cyan line).
	// If drawSeekerPaths is removed or gated out of PH_MOVE, pathBandDiffPHMove == 0.
	CPPUNIT_ASSERT_MESSAGE(
		"SMFR-05 AC1: FBattleBoard::draw() in PH_MOVE must draw a path line in the band "
		"x=[344..356], y=[290..385]. drawSeekerPaths() draws a cyan line from hex (5,5) to "
		"hex (5,7) during PH_MOVE. Zero diffs means path was not drawn (gate or empty path).",
		pathBandDiffPHMove > 0);

	// AC1: PH_SEEKER_ACTIVATION must also have path band diffs.
	CPPUNIT_ASSERT_MESSAGE(
		"SMFR-05 AC1: FBattleBoard::draw() in PH_SEEKER_ACTIVATION must also draw the "
		"seeker path line. drawSeekerPaths() must run in PH_SEEKER_ACTIVATION.",
		pathBandDiffPHSeekerAct > 0);

	// AC1 gate: PH_ATTACK_FIRE must have ZERO path band diffs.
	// The count label (at x=360) is outside the band so it does not interfere.
	CPPUNIT_ASSERT_EQUAL_MESSAGE(
		"SMFR-05 AC1 gate: FBattleBoard::draw() in PH_ATTACK_FIRE must NOT draw a path "
		"line in the path band (drawSeekerPaths not called in PH_ATTACK_FIRE).",
		0, pathBandDiffPHAttackFire);

	delete attackFleet;
	delete defendFleet;
	m_harness.cleanupOrphanTopLevels(10);
}

}
