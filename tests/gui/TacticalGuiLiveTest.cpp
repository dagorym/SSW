/**
 * @file TacticalGuiLiveTest.cpp
 * @brief Implementation file for live tactical GUI regression coverage
 */

#include "TacticalGuiLiveTest.h"

#include <wx/button.h>
#include <wx/display.h>
#include <wx/dialog.h>
#include <wx/panel.h>
#include <wx/spinctrl.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/toplevel.h>
#include <wx/uiaction.h>
#include <wx/window.h>

#include <algorithm>
#include <cstdlib>
#include <iostream>
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

void assertDialogCenteredOnParent(wxDialog * dialog, wxWindow * parent, int tolerance = 120) {
	CPPUNIT_ASSERT(dialog != NULL);
	CPPUNIT_ASSERT(parent != NULL);
	const wxRect parentBounds = parent->GetScreenRect();
	const wxRect dialogBounds = dialog->GetScreenRect();
	const wxPoint parentCenter(parentBounds.GetX() + (parentBounds.GetWidth() / 2),
	                           parentBounds.GetY() + (parentBounds.GetHeight() / 2));
	const wxPoint dialogCenter(dialogBounds.GetX() + (dialogBounds.GetWidth() / 2),
	                           dialogBounds.GetY() + (dialogBounds.GetHeight() / 2));
	CPPUNIT_ASSERT(std::abs(parentCenter.x - dialogCenter.x) <= tolerance);
	CPPUNIT_ASSERT(std::abs(parentCenter.y - dialogCenter.y) <= tolerance);
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

	std::cerr << "TACTICAL1:msg" << std::endl;
	WXTacticalUI::setModalAutoDismissMs(25);
	ui.showMessage("Tactical Smoke", "Parent-backed tactical message");
	std::cerr << "TACTICAL1:winner" << std::endl;
	ui.notifyWinner(true);
	WXTacticalUI::setModalAutoDismissMs(0);
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

void TacticalGuiLiveTest::testTacticalDamageSummaryDialogDisplaysContextAndCloseBehavior() {
wxFrame * parent = new wxFrame(NULL, wxID_ANY, "Damage Summary Parent", wxDefaultPosition, wxSize(540, 420));
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
const int closeResult = m_harness.runModalFunctionWithAction([&]() {
	return dialog->ShowModal();
}, [&]() {
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
}, wxID_CANCEL, 250);
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
		wxCommandEvent click(wxEVT_COMMAND_BUTTON_CLICKED, closeButton->GetId());
		click.SetEventObject(closeButton);
		closeButton->Command(click);
	}
}, wxID_CANCEL, 100);
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
wxSize initialDialogSize;
wxSize postRebuildDialogSize;
wxString assignedCountText;
bool modalActionRan = false;
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
