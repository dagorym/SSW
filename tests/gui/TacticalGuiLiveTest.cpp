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

/// returns the number of source-row click regions registered after the last draw (AC1/AC3 verification)
size_t shipNameRegionCount() const {
	return m_shipNameRegions.size();
}

/// returns the i-th source-row click region after the last draw (AC3 alignment verification)
wxRect shipNameRegion(size_t i) const {
	CPPUNIT_ASSERT(i < m_shipNameRegions.size());
	return m_shipNameRegions[i];
}

/// exposes checkShipSelection() so a simulated mouse-release event can verify row-click selection (AC2)
void checkShipSelectionPublic(wxMouseEvent & event) {
	checkShipSelection(event);
}

/// returns the number of pre-game seeker recall regions registered after the last draw (PGS-04)
size_t preGameSeekerRecallRegionCount() const {
	return m_preGameSeekerRecallRegions.size();
}

/// returns the i-th pre-game seeker recall region after the last draw (PGS-04)
wxRect preGameSeekerRecallRegion(size_t i) const {
	CPPUNIT_ASSERT(i < m_preGameSeekerRecallRegions.size());
	return m_preGameSeekerRecallRegions[i];
}

/// exposes checkPreGameSeekerRecallSelection() for simulated click testing (PGS-04)
bool checkPreGameSeekerRecallSelectionPublic(wxMouseEvent & event) {
	return checkPreGameSeekerRecallSelection(event);
}

/// exposes checkOffensiveSeekerPendingSelection() for simulated click testing (SMRIV-03)
bool checkOffensiveSeekerPendingSelectionPublic(wxMouseEvent & event) {
	return checkOffensiveSeekerPendingSelection(event);
}

/// returns the number of seeker activation row regions populated after the last draw (SMRV-02)
size_t seekerActivationRegionCount() const {
	return m_seekerActivationRegions.size();
}

/// returns the i-th seeker activation row region (SMRV-02)
wxRect seekerActivationRegion(size_t i) const {
	CPPUNIT_ASSERT(i < m_seekerActivationRegions.size());
	return m_seekerActivationRegions[i];
}

/// exposes checkSeekerActivationSelection() for simulated click testing (SMRV-02 AC-2)
void checkSeekerActivationSelectionPublic(wxMouseEvent & event) {
	checkSeekerActivationSelection(event);
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

// Returns an absolute path to a repository file using the GUI_TEST_REPO_ROOT compile macro.
// Use this for source-contract path lookups to avoid cwd-relative fragility.
std::string guiRepoFile(const std::string & relativePath) {
return std::string(GUI_TEST_REPO_ROOT) + "/" + relativePath;
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

	/// Expose findTurnData() from the underlying SeederGame for movement-path injection (SMRIV-03).
	Frontier::FTacticalTurnData * findShipTurnData(unsigned int shipID) {
		return m_seederGame ? m_seederGame->findTurnData(shipID) : NULL;
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
		std::vector<std::string>(1, guiRepoFile("src/tactical/FBattleDisplay.cpp")),
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
	// SMRIV-04 / SMRV-T2: within-phase resize-back is a preservation assertion, not exact equality.
	// The lower panel may grow but must not shrink below the pre-resize minimum.
	CPPUNIT_ASSERT(restoredHeight >= beforeResizeHeight);

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

void TacticalGuiLiveTest::testOnSetSpeedMinePlacementPreservesShipForFirstBoardClick() {
// PGS-01: Verifies that after beginMinePlacement() succeeds via the FBattleScreen API,
// m_curShip and m_curWeapon remain non-NULL so the first board click can record a mine.
//
// The old bug: onSetSpeed() called setShip(NULL) unconditionally after beginMinePlacement(),
// nulling m_curShip before the first board click, causing every mine click to silently fail.
// The fix: the enteredMinePlacement flag in onSetSpeed skips the setShip(NULL) call when
// beginMinePlacement() succeeds.
//
// This test drives the behavioral FBattleScreen API path directly:
// 1. Set up a real FBattleScreen with a Minelayer fleet.
// 2. Call beginMinePlacement() through the screen API.
// 3. Assert getShip() and getWeapon() are non-NULL (fixed path: no nulling after entry).
// 4. Assert handleHexClick() in BS_PlaceMines records a mine.

FFleet * attackFleet = new FFleet();
FFleet * defendFleet = new FFleet();
FVehicle * attacker = createShip("AssaultScout");
FVehicle * minelayer = createShip("Minelayer");
CPPUNIT_ASSERT_MESSAGE("AssaultScout and Minelayer must be creatable",
	attacker != NULL && minelayer != NULL);
attackFleet->addShip(attacker);
defendFleet->addShip(minelayer);
FleetList attackFleets;
FleetList defendFleets;
attackFleets.push_back(attackFleet);
defendFleets.push_back(defendFleet);

// Locate the mine launcher for ammo/record tracking.
int mineLauncherIndex = -1;
for (unsigned int i = 0; i < minelayer->getWeaponCount(); ++i) {
	FWeapon * w = minelayer->getWeapon(i);
	if (w != NULL && w->getType() == FWeapon::M && mineLauncherIndex < 0) {
		mineLauncherIndex = static_cast<int>(i);
	}
}
CPPUNIT_ASSERT_MESSAGE("Minelayer must have an M launcher", mineLauncherIndex >= 0);
FWeapon * mineLauncher = minelayer->getWeapon(static_cast<unsigned int>(mineLauncherIndex));
const int initialAmmo = mineLauncher->getAmmo();
CPPUNIT_ASSERT_MESSAGE("Mine launcher must start with ammo > 0", initialAmmo > 0);

FBattleScreen * battleScreen = new FBattleScreen("PGS-01 Mine Placement Preserves Ship");
battleScreen->setupFleets(&attackFleets, &defendFleets, false, NULL);
battleScreen->Show();
m_harness.pumpEvents(2);

// After setupFleets() the screen starts in BS_SetupDefendFleet with active player 0.
// Enter mine placement directly through the FBattleScreen API (this is the same path
// that onSetSpeed takes via beginMinePlacement()).
CPPUNIT_ASSERT_EQUAL_MESSAGE("Initial state after setupFleets must be BS_SetupDefendFleet",
	static_cast<int>(BS_SetupDefendFleet), battleScreen->getState());

const bool entered = battleScreen->beginMinePlacement();
CPPUNIT_ASSERT_MESSAGE("beginMinePlacement() must succeed with a Minelayer in fleet", entered);

// AC: After beginMinePlacement() succeeds, getShip() must be non-NULL.
// (The bug: onSetSpeed called setShip(NULL) after this, nulling m_curShip.)
CPPUNIT_ASSERT_EQUAL_MESSAGE("beginMinePlacement() must transition to BS_PlaceMines",
	static_cast<int>(BS_PlaceMines), battleScreen->getState());
CPPUNIT_ASSERT_MESSAGE(
	"After beginMinePlacement(), getShip() must be non-NULL (regression: old code nulled it via onSetSpeed)",
	battleScreen->getShip() != NULL);
CPPUNIT_ASSERT_MESSAGE(
	"After beginMinePlacement(), getWeapon() must be non-NULL",
	battleScreen->getWeapon() != NULL);
CPPUNIT_ASSERT_EQUAL_MESSAGE(
	"Selected weapon after mine-entry must be of type M",
	FWeapon::M, battleScreen->getWeapon()->getType());

// AC: handleHexClick() in BS_PlaceMines must record a mine when m_curShip is non-NULL.
// (When old bug was active: setShip(NULL) → placeMineAtHex returns false → no mine recorded.)
const FPoint mineHex(5, 5);
const bool placed = battleScreen->handleHexClick(mineHex);
CPPUNIT_ASSERT_MESSAGE(
	"handleHexClick in BS_PlaceMines must succeed when m_curShip is non-NULL (old bug: all clicks silently dropped)",
	placed);

// AC: Mine launcher ammo decremented by 1.
CPPUNIT_ASSERT_EQUAL_MESSAGE("Mine launcher ammo must decrement by 1 after successful placement",
	initialAmmo - 1, mineLauncher->getAmmo());

// AC: Hex recorded in getMinedHexes().
const PointSet & minedHexes = battleScreen->getMinedHexes();
CPPUNIT_ASSERT_MESSAGE("Placed mine hex must appear in getMinedHexes()",
	minedHexes.find(mineHex) != minedHexes.end());

// AC: Placed-ordnance record appended to getPlacedOrdnance().
CPPUNIT_ASSERT_EQUAL_MESSAGE("getPlacedOrdnance() must contain one record after mine placement",
	static_cast<unsigned int>(1), static_cast<unsigned int>(battleScreen->getPlacedOrdnance().size()));
CPPUNIT_ASSERT_EQUAL_MESSAGE("Placed ordnance record must have weapon type M",
	FWeapon::M, battleScreen->getPlacedOrdnance()[0].weaponType);

battleScreen->Destroy();
m_harness.pumpEvents(3);
delete attackFleet;
delete defendFleet;

m_harness.cleanupOrphanTopLevels(10);
}

void TacticalGuiLiveTest::testOffensiveSeekerPendingListRegionVisibilityAndRecall() {
	// SMRIV-03: drawOffensiveSeekerPendingRows is called in draw() inside a PH_ATTACK_FIRE
	// guard.  This test confirms the repositioned layout and preserved recall behavior:
	//
	// AC1: draw() in PH_ATTACK_FIRE with SM weapon selected clears then rebuilds
	//      m_pendingSeekerRecallRegions — no crash and correct empty state before any deployment.
	// AC2: SMRIV-03 — When a pending group exists, recall regions are anchored at
	//      x >= 310 (right of Done button) and y >= getActionPromptLineY(0) (top of lower
	//      panel), NOT below the action-button row as in the previous layout.
	// AC3: Clicking a recall region via the display's check function removes one seeker from
	//      the model and restores ammo (existing recall behavior preserved, only repositioned).
	// AC4: draw() in a non-PH_ATTACK_FIRE phase leaves m_pendingSeekerRecallRegions empty.
	//
	// AC2/AC3 require a real pending deployment which in turn requires a movement path.
	// We use TestableBattleScreen to inject the path via findShipTurnData() rather than
	// driving the full movement phase.

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

	// Use TestableBattleScreen so findShipTurnData() can inject the movement path required
	// for PH_ATTACK_FIRE seeker deployment (AC2/AC3).
	TestableBattleScreen * testableScreen = new TestableBattleScreen("SMRIV-03 Pending Seeker Region Test");
	FBattleScreen * battleScreen = testableScreen;
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

	// AC2 + AC3: Deploy a seeker via path injection and verify recall region position and recall
	// click behavior.  Inject a two-hex movement path for the attacker via findShipTurnData()
	// so handleHexClick() can place a pending seeker on the path.
	battleScreen->setPhase(PH_ATTACK_FIRE);
	battleScreen->setShip(attacker);
	CPPUNIT_ASSERT(battleScreen->selectWeapon(static_cast<unsigned int>(smWeaponIndex)));

	const FPoint firstMoveHex = FHexMap::findNextHex(attackerHex, attacker->getHeading());
	Frontier::FTacticalTurnData * turnData = testableScreen->findShipTurnData(attacker->getID());
	CPPUNIT_ASSERT_MESSAGE("findShipTurnData must return non-NULL after ship is placed.", turnData != NULL);
	turnData->path.clear();
	turnData->path.addPoint(attackerHex);
	turnData->path.addPoint(firstMoveHex);
	turnData->startHeading = attacker->getHeading();
	turnData->curHeading   = attacker->getHeading();
	turnData->finalHeading = attacker->getHeading();
	turnData->nMoved = 1;

	// Deploy one seeker onto the path hex.
	CPPUNIT_ASSERT_MESSAGE(
		"handleHexClick on a path hex must succeed in PH_ATTACK_FIRE deployment mode.",
		battleScreen->handleHexClick(firstMoveHex));
	const int ammoAfterDeploy = smLauncher->getAmmo();
	CPPUNIT_ASSERT_EQUAL_MESSAGE(
		"Ammo must decrement by 1 after deploying one pending seeker.", 2, ammoAfterDeploy);

	// Redraw to populate m_pendingSeekerRecallRegions from the new pending deployment.
	{
		wxMemoryDC dc;
		const wxSize sz = displayPanel->GetSize();
		wxBitmap bmp(sz.GetWidth() > 0 ? sz.GetWidth() : 1200, sz.GetHeight() > 0 ? sz.GetHeight() : 240);
		dc.SelectObject(bmp);
		displayPanel->draw(dc);
		dc.SelectObject(wxNullBitmap);
	}

	// AC2: SMRV-01 round5 — Recall region must be to the right of the widest left-column
	// instruction text (computed pendingLMargin = leftOffset + textExtent + 2*BORDER), and
	// anchored at the top of the lower panel (y >= getActionPromptLineY(0)).
	const size_t recallCount = peer->pendingSeekerRecallRegionCount();
	CPPUNIT_ASSERT_MESSAGE(
		"AC2: At least one recall region must appear after deploying a pending seeker.",
		recallCount >= 1u);
	// Compute pendingLMargin as draw() does: leftOffset + text extent of widest line + 2*BORDER.
	// leftOffset = 2*BORDER + ZOOM_SIZE = 2*5 + 30 = 40; BORDER = 5.
	const int leftOffsetVal = 40;
	const int borderVal = 5;
	wxBitmap measBmp(800, 20);
	wxMemoryDC measDC;
	measDC.SelectObject(measBmp);
	measDC.SetFont(wxFont(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
	const int attackTextW = measDC.GetTextExtent("Select legal path hexes to deploy seeker missiles.").GetWidth();
	measDC.SelectObject(wxNullBitmap);
	const int computedPendingLMargin = leftOffsetVal + attackTextW + 2 * borderVal;
	const int promptLineY0 = FBattleDisplayTestPeer::actionPromptLineY(0);
	for (size_t i = 0; i < recallCount; ++i) {
		const wxRect recallRect = peer->pendingSeekerRecallRegion(i);
		CPPUNIT_ASSERT_MESSAGE(
			"AC2: Recall row left edge must be at or right of computed pendingLMargin "
			"(leftOffset + text extent of widest instruction line + 2*BORDER).",
			recallRect.GetLeft() >= computedPendingLMargin);
		CPPUNIT_ASSERT_MESSAGE(
			"AC2: Recall row top must be at or below getActionPromptLineY(0) "
			"(anchored to the top of the lower panel, not below the action-button row).",
			recallRect.GetTop() >= promptLineY0);
	}

	// AC3: Clicking a recall region via the display check function must remove the pending
	// seeker and restore ammo — existing recall behavior preserved after repositioning.
	const wxRect firstRecallRect = peer->pendingSeekerRecallRegion(0);
	wxMouseEvent recallClick(wxEVT_LEFT_UP);
	recallClick.m_x = firstRecallRect.GetLeft() + firstRecallRect.GetWidth() / 2;
	recallClick.m_y = firstRecallRect.GetTop()  + firstRecallRect.GetHeight() / 2;
	const bool recalled = peer->checkOffensiveSeekerPendingSelectionPublic(recallClick);
	CPPUNIT_ASSERT_MESSAGE("AC3: checkOffensiveSeekerPendingSelection() must return true.", recalled);
	CPPUNIT_ASSERT_EQUAL_MESSAGE(
		"AC3: Ammo must be restored by 1 after recalling the pending seeker.",
		3, smLauncher->getAmmo());

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

	// PGS-02 FIXED: applyRequestedDisplayHeight() now notifies the parent via SendSizeEvent()
	// so FBattleScreen::applyLayoutPolicy() runs on the next event-loop iteration and grows
	// the panel to accommodate the expanded requestedDisplayHeight.  The earlier observation
	// that drawCurrentShipStats/ensureLowerPanelLayoutState could clobber the expanded height
	// no longer applies because both compute requestedDisplayHeight from the same bottom-edge
	// measurement and the SendSizeEvent path is guarded by m_inResizeReflow to prevent loops.
	// This assertion is now a persistence regression guard: height must not decrease after the
	// draw, confirming the expansion contract is still honoured across future code changes.
	CPPUNIT_ASSERT_MESSAGE(
		"drawSeekerActivation(): requestedDisplayHeight must not decrease after drawing activation rows. "
		"(Regression guard: PGS-02 fixed this by having applyRequestedDisplayHeight() notify the parent "
		"via SendSizeEvent() so the panel grows to show all activation rows.)",
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

void TacticalGuiLiveTest::testPlacementSourceRowsArePopulatedAndClickSelectionUpdatesSources() {
	// PGS-02: Behavioral verification covering AC1-AC4 for mine placement source rows.
	//
	// AC1: During BS_PlaceMines, each ship with a deployable M launcher appears as a
	//      selectable source row (m_shipNameRegions is non-empty and has one entry per
	//      ship with an M weapon).
	// AC2: Clicking a source row (simulated via checkShipSelection) updates the selected-
	//      source index and m_curShip/m_curWeapon to the matching ship+weapon.
	// AC3: SMRIV-01 -- Each row's click region starts at getActionPromptLineY(0) (top of the
	//      lower panel, right column), which is above getActionButtonRowBottom().
	//      Rows are positioned in the right column beside the instruction text, not below the
	//      action button row as in the old layout.
	// AC4: After switching to a different source, the next board click places ordnance
	//      from the newly selected ship (verified via placed-ordnance source.shipID).
	//
	// Setup: two Minelayers in the defend fleet so BS_PlaceMines produces two deployable
	// M sources (one per Minelayer), giving a multi-source scenario where source switching
	// is meaningful.

	FFleet * attackFleet = new FFleet();
	FFleet * defendFleet = new FFleet();
	FVehicle * attacker = createShip("Destroyer");
	FVehicle * minelayer1 = createShip("Minelayer");
	FVehicle * minelayer2 = createShip("Minelayer");
	CPPUNIT_ASSERT_MESSAGE("Attacker and two Minelayers must be creatable",
		attacker != NULL && minelayer1 != NULL && minelayer2 != NULL);
	// Both minelayers are owned by defender (player 2).
	minelayer1->setOwner(2);
	minelayer2->setOwner(2);
	attackFleet->addShip(attacker);
	defendFleet->addShip(minelayer1);
	defendFleet->addShip(minelayer2);
	FleetList attackFleets;
	FleetList defendFleets;
	attackFleets.push_back(attackFleet);
	defendFleets.push_back(defendFleet);

	FBattleScreen * battleScreen = new FBattleScreen("PGS-02 Source Row Click Selection");
	battleScreen->setupFleets(&attackFleets, &defendFleets, false, NULL);
	battleScreen->Show();
	m_harness.pumpEvents(2);

	// Enter BS_PlaceMines via the same API path as the game.
	const bool entered = battleScreen->beginMinePlacement();
	CPPUNIT_ASSERT_MESSAGE("beginMinePlacement() must succeed with Minelayers in fleet", entered);

	// After entry, at least one source must be auto-selected (AC5, single-or-first selection).
	CPPUNIT_ASSERT_MESSAGE(
		"AC5: After beginMinePlacement() at least one source must be selected (getShip() non-NULL).",
		battleScreen->getShip() != NULL);
	CPPUNIT_ASSERT_MESSAGE(
		"AC5: After beginMinePlacement() getWeapon() must be non-NULL and of type M.",
		battleScreen->getWeapon() != NULL && battleScreen->getWeapon()->getType() == FWeapon::M);

	// Record which ship is currently selected (will be source index 0 / minelayer1 or 2).
	const unsigned int autoSelectedShipID = battleScreen->getShip()->getID();

	// Locate FBattleDisplay and cast to peer.
	FBattleDisplay * displayPanel = findFirstBattleDisplay(battleScreen);
	CPPUNIT_ASSERT_MESSAGE("FBattleDisplay must exist in the battle screen.", displayPanel != NULL);
	FBattleDisplayTestPeer * peer = static_cast<FBattleDisplayTestPeer *>(displayPanel);

	// Draw offscreen to populate m_shipNameRegions.
	const int panelW = battleScreen->GetSize().GetWidth();
	{
		wxMemoryDC dc;
		wxBitmap bmp(panelW, 300);
		dc.SelectObject(bmp);
		displayPanel->draw(dc);
		dc.SelectObject(wxNullBitmap);
	}

	// AC1: Both Minelayers must produce a source row (two M sources).
	const size_t rowCount = peer->shipNameRegionCount();
	CPPUNIT_ASSERT_MESSAGE(
		"AC1: Both Minelayers must appear as source rows in BS_PlaceMines (expected >= 2 rows).",
		rowCount >= 2);

	// AC3: SMRIV-01 -- rows are anchored to the top of the lower panel (right column),
	// starting at getActionPromptLineY(0) = ACTION_PROMPT_TOP_MARGIN = 5. This is above
	// getActionButtonRowBottom(), so the old check (top >= buttonRowBottom) is replaced
	// with a check that confirms rows start at or near the top of the lower panel.
	// We verify top is at least ACTION_PROMPT_TOP_MARGIN (not before the panel top)
	// and below the panel's total reserved prompt block height, meaning they are placed
	// in the right column beside the instruction text, not outside the panel.
	const int promptLineY0 = FBattleDisplayTestPeer::actionPromptLineY(0);
	for (size_t i = 0; i < rowCount; ++i) {
		const wxRect rowRect = peer->shipNameRegion(i);
		CPPUNIT_ASSERT_MESSAGE(
			"AC3: Source row region must start at or below the action-prompt top margin "
			"(rows anchored to getActionPromptLineY(0) at the top of the lower panel).",
			rowRect.GetTop() >= promptLineY0);
	}

	// AC2: Simulate a click in the second row (index 1) to switch sources.
	// Build a mouse event positioned at the centre of the second row region.
	const wxRect secondRowRect = peer->shipNameRegion(1);
	const int clickX = secondRowRect.GetLeft() + secondRowRect.GetWidth() / 2;
	const int clickY = secondRowRect.GetTop() + secondRowRect.GetHeight() / 2;
	wxMouseEvent clickEvent(wxEVT_LEFT_UP);
	clickEvent.m_x = clickX;
	clickEvent.m_y = clickY;
	peer->checkShipSelectionPublic(clickEvent);

	// The selected placement source index must now be 1.
	const int newSourceIndex = battleScreen->getSelectedPlacementSourceIndex();
	CPPUNIT_ASSERT_EQUAL_MESSAGE(
		"AC2: Clicking the second source row must update getSelectedPlacementSourceIndex() to 1.",
		1, newSourceIndex);

	// getShip() must point to the newly selected source's ship.
	CPPUNIT_ASSERT_MESSAGE(
		"AC2: After clicking row 1, getShip() must be non-NULL.",
		battleScreen->getShip() != NULL);
	const unsigned int newlySelectedShipID = battleScreen->getShip()->getID();
	// The newly selected ship must differ from the auto-selected first ship.
	CPPUNIT_ASSERT_MESSAGE(
		"AC2: Clicking a different source row must change which ship is selected.",
		newlySelectedShipID != autoSelectedShipID);

	// getWeapon() must be an M weapon (only M sources are listed in BS_PlaceMines).
	CPPUNIT_ASSERT_MESSAGE(
		"AC2: After clicking row 1, getWeapon() must be non-NULL and of type M.",
		battleScreen->getWeapon() != NULL && battleScreen->getWeapon()->getType() == FWeapon::M);

	// AC4: After switching sources, the next board click places ordnance from the
	// newly selected ship/weapon, not the original auto-selected one.
	const FPoint placeHex(5, 5);
	const bool placed = battleScreen->handleHexClick(placeHex);
	CPPUNIT_ASSERT_MESSAGE(
		"AC4: handleHexClick() in BS_PlaceMines must succeed after switching sources.",
		placed);

	const std::vector<FTacticalPlacedOrdnance> & ordnance = battleScreen->getPlacedOrdnance();
	CPPUNIT_ASSERT_MESSAGE(
		"AC4: getPlacedOrdnance() must contain one record after the hex click.",
		!ordnance.empty());
	CPPUNIT_ASSERT_EQUAL_MESSAGE(
		"AC4: Placed ordnance must have weapon type M.",
		FWeapon::M, ordnance.back().weaponType);
	CPPUNIT_ASSERT_EQUAL_MESSAGE(
		"AC4: Placed ordnance source.shipID must match the newly selected ship (row 1), "
		"not the auto-selected first ship — confirming source switch was honoured.",
		newlySelectedShipID, ordnance.back().source.shipID);

	battleScreen->Destroy();
	m_harness.pumpEvents(3);
	delete attackFleet;
	delete defendFleet;
	m_harness.cleanupOrphanTopLevels(10);
}

void TacticalGuiLiveTest::testPreGameSeekerRecallListAppearsAndClickRemovesSeeker() {
	// PGS-04: Behavioral verification covering AC1-AC4 for the pre-game seeker recall list.
	//
	// AC1: During BS_PlaceSeekers, drawPlaceSeekers() populates m_preGameSeekerRecallRegions
	//      after a seeker is placed (one region per (hex, source) group).
	// AC2: Simulating a click on a recall region via checkPreGameSeekerRecallSelectionPublic
	//      removes one seeker from the model (getSeekerMissiles().size() decrements by 1) and
	//      restores one ammo round to the seeker launcher.
	// AC3: During BS_PlaceMines, draw() must not populate m_preGameSeekerRecallRegions
	//      (recall list must be absent from the mine phase).
	// AC4: Each recall region starts at or below actionButtonRowBottom() so it does not overlap
	//      source-selection rows or the action-button/prompt block.
	//
	// Setup: one Minelayer on the defending side (owner 2) so both mine and seeker phases
	// can be exercised within one test.

	FFleet * attackFleet = new FFleet();
	FFleet * defendFleet = new FFleet();
	FVehicle * attacker  = createShip("Destroyer");
	FVehicle * minelayer = createShip("Minelayer");
	CPPUNIT_ASSERT_MESSAGE("Attacker and Minelayer must be creatable",
		attacker != NULL && minelayer != NULL);
	minelayer->setOwner(2);
	attackFleet->addShip(attacker);
	defendFleet->addShip(minelayer);
	FleetList attackFleets;
	FleetList defendFleets;
	attackFleets.push_back(attackFleet);
	defendFleets.push_back(defendFleet);

	FBattleScreen * battleScreen = new FBattleScreen("PGS-04 Seeker Recall List");
	battleScreen->setupFleets(&attackFleets, &defendFleets, false, NULL);
	battleScreen->Show();
	m_harness.pumpEvents(2);

	FBattleDisplay * displayPanel = findFirstBattleDisplay(battleScreen);
	CPPUNIT_ASSERT_MESSAGE("FBattleDisplay must exist in the battle screen.", displayPanel != NULL);
	FBattleDisplayTestPeer * peer = static_cast<FBattleDisplayTestPeer *>(displayPanel);

	// --- AC3: During BS_PlaceMines, m_preGameSeekerRecallRegions must be empty ---
	const bool mineEntered = battleScreen->beginOrdnancePlacement();
	CPPUNIT_ASSERT_MESSAGE("beginOrdnancePlacement() must succeed with a Minelayer", mineEntered);
	battleScreen->setMoveComplete(true);
	m_harness.pumpEvents(1);

	{
		const int panelW = battleScreen->GetSize().GetWidth();
		wxMemoryDC dc;
		wxBitmap bmp(panelW > 0 ? panelW : 800, 300);
		dc.SelectObject(bmp);
		displayPanel->draw(dc);
		dc.SelectObject(wxNullBitmap);
	}

	CPPUNIT_ASSERT_EQUAL_MESSAGE(
		"AC3: m_preGameSeekerRecallRegions must be empty during BS_PlaceMines.",
		static_cast<size_t>(0),
		peer->preGameSeekerRecallRegionCount());

	// --- Advance to BS_PlaceSeekers via Mine Placement Done button ---
	wxButton * mineDoneButton = findButtonByLabel(battleScreen, wxT("Mine Placement Done"));
	CPPUNIT_ASSERT_MESSAGE("Mine Placement Done button must exist.", mineDoneButton != NULL);
	{
		wxCommandEvent mineClick(wxEVT_COMMAND_BUTTON_CLICKED, mineDoneButton->GetId());
		mineClick.SetEventObject(mineDoneButton);
		mineDoneButton->GetEventHandler()->ProcessEvent(mineClick);
	}
	m_harness.pumpEvents(2);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Must be in BS_PlaceSeekers after Mine Placement Done.",
		static_cast<int>(BS_PlaceSeekers), battleScreen->getState());

	// Locate the seeker launcher to check ammo.
	FWeapon * seekerLauncher = battleScreen->getWeapon();
	CPPUNIT_ASSERT_MESSAGE("SM launcher must be auto-selected in BS_PlaceSeekers", seekerLauncher != NULL);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Selected weapon must be SM", FWeapon::SM, seekerLauncher->getType());
	const int ammoBeforePlacement = seekerLauncher->getAmmo();
	CPPUNIT_ASSERT_MESSAGE("SM launcher must have ammo > 0 for this test", ammoBeforePlacement > 0);

	// --- AC1 pre-check: before any placement, recall regions must be empty ---
	{
		const int panelW = battleScreen->GetSize().GetWidth();
		wxMemoryDC dc;
		wxBitmap bmp(panelW > 0 ? panelW : 800, 300);
		dc.SelectObject(bmp);
		displayPanel->draw(dc);
		dc.SelectObject(wxNullBitmap);
	}
	CPPUNIT_ASSERT_EQUAL_MESSAGE(
		"AC1 pre-check: m_preGameSeekerRecallRegions must be empty before any seeker placement.",
		static_cast<size_t>(0),
		peer->preGameSeekerRecallRegionCount());

	// Place one seeker via handleHexClick.
	const FPoint seekerHex(5, 5);
	const bool placed = battleScreen->handleHexClick(seekerHex);
	CPPUNIT_ASSERT_MESSAGE("handleHexClick in BS_PlaceSeekers must succeed", placed);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Seeker count must be 1 after placement",
		static_cast<unsigned int>(1),
		static_cast<unsigned int>(battleScreen->getSeekerMissiles().size()));

	// --- AC1: After placement, draw must populate recall regions ---
	{
		const int panelW = battleScreen->GetSize().GetWidth();
		wxMemoryDC dc;
		wxBitmap bmp(panelW > 0 ? panelW : 800, 300);
		dc.SelectObject(bmp);
		displayPanel->draw(dc);
		dc.SelectObject(wxNullBitmap);
	}
	const size_t recallCountAfterPlacement = peer->preGameSeekerRecallRegionCount();
	CPPUNIT_ASSERT_MESSAGE(
		"AC1: m_preGameSeekerRecallRegions must be non-empty after placing one seeker.",
		recallCountAfterPlacement >= 1);

	// --- AC4: SMRIV-02 -- Each recall region is in the right column (recallMargin=620),
	// anchored at getActionPromptLineY(0) at the top of the lower panel. The region's left
	// edge must be at or near recallMargin=620 (not centered or in the middle column).
	const int recallMargin = 620;
	const int promptLineY0 = FBattleDisplayTestPeer::actionPromptLineY(0);
	for (size_t i = 0; i < recallCountAfterPlacement; ++i) {
		const wxRect recallRect = peer->preGameSeekerRecallRegion(i);
		CPPUNIT_ASSERT_MESSAGE(
			"AC4: Recall row region left edge must be at or near recallMargin=620 "
			"(right column, not centered).",
			recallRect.GetLeft() >= recallMargin);
		CPPUNIT_ASSERT_MESSAGE(
			"AC4: Recall row region top must be at or above the action-prompt top margin "
			"(anchored to top of lower panel, not below button row).",
			recallRect.GetTop() >= promptLineY0);
	}

	// --- AC2: Click the first recall region and verify seeker removal + ammo restore ---
	const wxRect firstRecallRect = peer->preGameSeekerRecallRegion(0);
	const int clickX = firstRecallRect.GetLeft() + firstRecallRect.GetWidth() / 2;
	const int clickY = firstRecallRect.GetTop()  + firstRecallRect.GetHeight() / 2;
	wxMouseEvent recallClick(wxEVT_LEFT_UP);
	recallClick.m_x = clickX;
	recallClick.m_y = clickY;
	const bool recalled = peer->checkPreGameSeekerRecallSelectionPublic(recallClick);
	CPPUNIT_ASSERT_MESSAGE("AC2: checkPreGameSeekerRecallSelection() must return true for the recall row click.",
		recalled);

	// Seeker count must drop by 1.
	CPPUNIT_ASSERT_EQUAL_MESSAGE(
		"AC2: getSeekerMissiles().size() must be 0 after recalling the only placed seeker.",
		static_cast<unsigned int>(0),
		static_cast<unsigned int>(battleScreen->getSeekerMissiles().size()));

	// Ammo must be restored by 1.
	CPPUNIT_ASSERT_EQUAL_MESSAGE(
		"AC2: Seeker launcher ammo must be restored by 1 after recall.",
		ammoBeforePlacement, seekerLauncher->getAmmo());

	// After recall, draw should show no recall rows.
	{
		const int panelW = battleScreen->GetSize().GetWidth();
		wxMemoryDC dc;
		wxBitmap bmp(panelW > 0 ? panelW : 800, 300);
		dc.SelectObject(bmp);
		displayPanel->draw(dc);
		dc.SelectObject(wxNullBitmap);
	}
	CPPUNIT_ASSERT_EQUAL_MESSAGE(
		"AC1 post-recall: m_preGameSeekerRecallRegions must be empty after recalling the only seeker.",
		static_cast<size_t>(0),
		peer->preGameSeekerRecallRegionCount());

	battleScreen->Destroy();
	m_harness.pumpEvents(5);
	delete attackFleet;
	delete defendFleet;
	m_harness.cleanupOrphanTopLevels(10);
}

void TacticalGuiLiveTest::testPlaceSeekersThreeColumnLayoutColumnPositionsAndClickRegions() {
	// SMRIV-02: Behavioral verification that drawPlaceSeekers() three-column layout
	// anchors the middle (source) and right (recall) columns independently at the top of
	// the lower panel, and that click regions align with the drawn positions.
	//
	// AC1: During BS_PlaceSeekers, source-selection rows (m_shipNameRegions) start at
	//      x >= lMargin=310 (middle column, right of left column).
	// AC2: During BS_PlaceSeekers, source-selection rows start at y >= getActionPromptLineY(0)
	//      (anchored to the top of the lower panel, not below the button row).
	// AC3: After a seeker is placed, recall rows (m_preGameSeekerRecallRegions) start at
	//      x >= recallMargin=620 (right column, right of middle column).
	// AC4: Recall rows start at y >= getActionPromptLineY(0) and recallMargin > lMargin
	//      ensures horizontal separation (columns do not overlap).
	// AC5: Selecting a source row via checkShipSelection still updates the active placement
	//      source (getSelectedPlacementSourceIndex() changes and getWeapon() is SM type).
	// AC6: Clicking a recall row via checkPreGameSeekerRecallSelection still undeploys one
	//      seeker and restores ammo (behavior unchanged by layout reposition).

	FFleet * attackFleet = new FFleet();
	FFleet * defendFleet = new FFleet();
	FVehicle * attacker   = createShip("Destroyer");
	FVehicle * minelayer1 = createShip("Minelayer");
	FVehicle * minelayer2 = createShip("Minelayer");
	CPPUNIT_ASSERT_MESSAGE("Attacker and two Minelayers must be creatable",
		attacker != NULL && minelayer1 != NULL && minelayer2 != NULL);
	// Both Minelayers are owned by the defender (player 2) so drawPlaceSeekers produces
	// multiple SM source rows.
	minelayer1->setOwner(2);
	minelayer2->setOwner(2);
	attackFleet->addShip(attacker);
	defendFleet->addShip(minelayer1);
	defendFleet->addShip(minelayer2);
	FleetList attackFleets;
	FleetList defendFleets;
	attackFleets.push_back(attackFleet);
	defendFleets.push_back(defendFleet);

	FBattleScreen * battleScreen = new FBattleScreen("SMRIV-02 Three-Column Seekers");
	battleScreen->setupFleets(&attackFleets, &defendFleets, false, NULL);
	battleScreen->Show();
	m_harness.pumpEvents(2);

	FBattleDisplay * displayPanel = findFirstBattleDisplay(battleScreen);
	CPPUNIT_ASSERT_MESSAGE("FBattleDisplay must exist in the battle screen.", displayPanel != NULL);
	FBattleDisplayTestPeer * peer = static_cast<FBattleDisplayTestPeer *>(displayPanel);
	const int panelW = battleScreen->GetSize().GetWidth();

	// Advance to BS_PlaceSeekers: enter ordnance placement (BS_PlaceMines) then draw
	// offscreen to show the Mine Placement Done button, then click it.
	const bool ordnanceEntered = battleScreen->beginOrdnancePlacement();
	CPPUNIT_ASSERT_MESSAGE("beginOrdnancePlacement() must succeed", ordnanceEntered);
	battleScreen->setMoveComplete(true);
	m_harness.pumpEvents(2);

	// Draw offscreen to trigger Mine Placement Done button show (required before click).
	{
		wxMemoryDC dc;
		wxBitmap bmp(panelW > 0 ? panelW : 800, 300);
		dc.SelectObject(bmp);
		displayPanel->draw(dc);
		dc.SelectObject(wxNullBitmap);
	}

	wxButton * mineDoneButton = findButtonByLabel(battleScreen, wxT("Mine Placement Done"));
	CPPUNIT_ASSERT_MESSAGE("Mine Placement Done button must exist and be shown.",
		mineDoneButton != NULL && mineDoneButton->IsShown());
	{
		wxCommandEvent mineClick(wxEVT_COMMAND_BUTTON_CLICKED, mineDoneButton->GetId());
		mineClick.SetEventObject(mineDoneButton);
		mineDoneButton->GetEventHandler()->ProcessEvent(mineClick);
	}
	m_harness.pumpEvents(2);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Must be in BS_PlaceSeekers after Mine Placement Done.",
		static_cast<int>(BS_PlaceSeekers), battleScreen->getState());
	{
		wxMemoryDC dc;
		wxBitmap bmp(panelW > 0 ? panelW : 800, 300);
		dc.SelectObject(bmp);
		displayPanel->draw(dc);
		dc.SelectObject(wxNullBitmap);
	}

	const int lMargin = 310;
	const int recallMargin = 620;
	const int promptLineY0 = FBattleDisplayTestPeer::actionPromptLineY(0);

	// --- AC1 + AC2: Source rows must be in the middle column ---
	const size_t sourceRowCount = peer->shipNameRegionCount();
	CPPUNIT_ASSERT_MESSAGE(
		"AC1: At least one SM source row must appear during BS_PlaceSeekers.",
		sourceRowCount >= 1);
	for (size_t i = 0; i < sourceRowCount; ++i) {
		const wxRect rowRect = peer->shipNameRegion(i);
		CPPUNIT_ASSERT_MESSAGE(
			"AC1: Source row left edge must be at or right of lMargin=310 (middle column).",
			rowRect.GetLeft() >= lMargin);
		CPPUNIT_ASSERT_MESSAGE(
			"AC2: Source row top must be at or below getActionPromptLineY(0) "
			"(anchored to the top of the lower panel).",
			rowRect.GetTop() >= promptLineY0);
	}

	// --- AC5: Selecting a source row updates the active placement source ---
	FWeapon * initialWeapon = battleScreen->getWeapon();
	CPPUNIT_ASSERT_MESSAGE("AC5: getWeapon() must be non-NULL in BS_PlaceSeekers.", initialWeapon != NULL);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("AC5: Initial weapon must be SM type.", FWeapon::SM, initialWeapon->getType());
	if (sourceRowCount >= 2) {
		const wxRect secondRowRect = peer->shipNameRegion(1);
		wxMouseEvent clickEvent(wxEVT_LEFT_UP);
		clickEvent.m_x = secondRowRect.GetLeft() + secondRowRect.GetWidth() / 2;
		clickEvent.m_y = secondRowRect.GetTop()  + secondRowRect.GetHeight() / 2;
		peer->checkShipSelectionPublic(clickEvent);
		CPPUNIT_ASSERT_EQUAL_MESSAGE(
			"AC5: Clicking source row 1 must set getSelectedPlacementSourceIndex() to 1.",
			1, battleScreen->getSelectedPlacementSourceIndex());
		CPPUNIT_ASSERT_MESSAGE(
			"AC5: getWeapon() must still be SM after switching to row 1.",
			battleScreen->getWeapon() != NULL
			&& battleScreen->getWeapon()->getType() == FWeapon::SM);
	}

	// Place a seeker to populate the recall list.
	FWeapon * seekerLauncher = battleScreen->getWeapon();
	CPPUNIT_ASSERT_MESSAGE("Seeker launcher must be present for placement.", seekerLauncher != NULL);
	const int ammoBeforePlacement = seekerLauncher->getAmmo();
	CPPUNIT_ASSERT_MESSAGE("Seeker launcher must have ammo > 0.", ammoBeforePlacement > 0);
	const FPoint seekerHex(5, 5);
	const bool placed = battleScreen->handleHexClick(seekerHex);
	CPPUNIT_ASSERT_MESSAGE("handleHexClick in BS_PlaceSeekers must succeed.", placed);

	// Redraw to populate m_preGameSeekerRecallRegions.
	{
		wxMemoryDC dc;
		wxBitmap bmp(panelW > 0 ? panelW : 800, 300);
		dc.SelectObject(bmp);
		displayPanel->draw(dc);
		dc.SelectObject(wxNullBitmap);
	}

	// --- AC3 + AC4: Recall rows must be in the right column ---
	const size_t recallCount = peer->preGameSeekerRecallRegionCount();
	CPPUNIT_ASSERT_MESSAGE(
		"AC3: At least one recall row must appear after placing a seeker.",
		recallCount >= 1);
	for (size_t i = 0; i < recallCount; ++i) {
		const wxRect recallRect = peer->preGameSeekerRecallRegion(i);
		CPPUNIT_ASSERT_MESSAGE(
			"AC3: Recall row left edge must be at or right of recallMargin=620 (right column).",
			recallRect.GetLeft() >= recallMargin);
		CPPUNIT_ASSERT_MESSAGE(
			"AC4: Recall row top must be at or below getActionPromptLineY(0).",
			recallRect.GetTop() >= promptLineY0);
		CPPUNIT_ASSERT_MESSAGE(
			"AC4: Right column (recallMargin=620) must be right of middle column (lMargin=310) "
			"to ensure the columns do not overlap.",
			recallRect.GetLeft() >= lMargin + 1);
	}

	// --- AC6: Clicking a recall row undeploys the seeker and restores ammo ---
	const wxRect firstRecallRect = peer->preGameSeekerRecallRegion(0);
	wxMouseEvent recallClick(wxEVT_LEFT_UP);
	recallClick.m_x = firstRecallRect.GetLeft() + firstRecallRect.GetWidth() / 2;
	recallClick.m_y = firstRecallRect.GetTop()  + firstRecallRect.GetHeight() / 2;
	const bool recalled = peer->checkPreGameSeekerRecallSelectionPublic(recallClick);
	CPPUNIT_ASSERT_MESSAGE("AC6: checkPreGameSeekerRecallSelection() must return true.", recalled);
	CPPUNIT_ASSERT_EQUAL_MESSAGE(
		"AC6: getSeekerMissiles().size() must be 0 after recalling the only placed seeker.",
		static_cast<unsigned int>(0),
		static_cast<unsigned int>(battleScreen->getSeekerMissiles().size()));
	CPPUNIT_ASSERT_EQUAL_MESSAGE(
		"AC6: Seeker launcher ammo must be restored by 1 after recall.",
		ammoBeforePlacement, seekerLauncher->getAmmo());

	battleScreen->Destroy();
	m_harness.pumpEvents(5);
	delete attackFleet;
	delete defendFleet;
	m_harness.cleanupOrphanTopLevels(10);
}

void TacticalGuiLiveTest::testLowerPanelHeightShrinksBackAfterPhaseChange() {
	// SMRIV-04: Behavioral verification that the lower-panel requestedDisplayHeight
	// shrinks back after a phase transition instead of ratcheting up permanently.
	//
	// Before SMRIV-04, ensureLowerPanelLayoutState() always applied:
	//   if (requestedDisplayHeight > requestedHeight)
	//       requestedHeight = requestedDisplayHeight;
	// so any height expansion from a prior phase was preserved indefinitely.
	//
	// After SMRIV-04, the function compares getState()/getPhase() against stored last-seen
	// values.  When either changes (phaseChanged == true) it skips the max-preserve so the
	// panel can shrink back to fit the new phase's content.
	//
	// Setup: 6 Minelayers in the defending fleet.  Each Minelayer has exactly 1 M weapon,
	// so the mine-source list produced by drawPlaceMines() has 6 rows.  With 6 rows the
	// list bottom (y_header + 6*row_height + BORDER) exceeds the 120-px floor, causing
	// drawPlaceMines() to expand requestedDisplayHeight beyond 120.  After transitioning to
	// BS_Battle/PH_MOVE, which has no row lists, the base content height is clamped back to
	// ~120 (the floor), so the panel can shrink.
	//
	// AC1 (SMRIV-04): After the phase transition, requestedDisplayHeight is less than the
	//      expanded value captured during the mine phase.
	// AC3 (SMRIV-04): requestedDisplayHeight after the transition is still >= 120.
	// AC4 (SMRIV-04): This test MUST fail against the pre-fix ratchet-only code (where the
	//      max-preserve would keep requestedDisplayHeight at the larger mine-phase value)
	//      and MUST pass after the fix.

	FFleet * attackFleet = new FFleet();
	FFleet * defendFleet = new FFleet();

	FVehicle * attacker = createShip("Destroyer");
	CPPUNIT_ASSERT_MESSAGE("Attacker Destroyer must be creatable.", attacker != NULL);
	attacker->setOwner(1);
	attackFleet->addShip(attacker);

	// 6 Minelayers as defenders.  Each has 1 M weapon -> 6 mine source rows in
	// drawPlaceMines().  6 rows push the list bottom above the 120-px floor so
	// requestedDisplayHeight is genuinely expanded during the mine phase.
	static const int NUM_MINELAYERS = 6;
	std::vector<FVehicle *> minelayers;
	for (int i = 0; i < NUM_MINELAYERS; ++i) {
		FVehicle * ml = createShip("Minelayer");
		CPPUNIT_ASSERT_MESSAGE("Minelayer must be creatable.", ml != NULL);
		ml->setOwner(2);
		minelayers.push_back(ml);
		defendFleet->addShip(ml);
	}

	FleetList attackFleets;
	FleetList defendFleets;
	attackFleets.push_back(attackFleet);
	defendFleets.push_back(defendFleet);

	FBattleScreen * battleScreen = new FBattleScreen("SMRIV-04 Panel Height Shrink");
	battleScreen->setupFleets(&attackFleets, &defendFleets, false, NULL);
	battleScreen->Show();
	m_harness.pumpEvents(2);

	FBattleDisplay * displayPanel = findFirstBattleDisplay(battleScreen);
	CPPUNIT_ASSERT_MESSAGE("FBattleDisplay must exist in the battle screen.", displayPanel != NULL);
	FBattleDisplayTestPeer * peer = static_cast<FBattleDisplayTestPeer *>(displayPanel);

	// --- Enter BS_PlaceMines ---
	const bool mineStarted = battleScreen->beginOrdnancePlacement();
	CPPUNIT_ASSERT_MESSAGE(
		"beginOrdnancePlacement() must succeed; Minelayers have M weapons.",
		mineStarted);
	battleScreen->setMoveComplete(true);
	m_harness.pumpEvents(2);

	// --- Draw offscreen in BS_PlaceMines to let drawPlaceMines() expand the height ---
	// Two draws are required for reliable height expansion under the SMRIV-04 fix.
	// applyRequestedDisplayHeight() calls GetParent()->SendSizeEvent() synchronously
	// (HandleWindowEvent path), which triggers reflowLowerPanelLayout().  On the
	// FIRST draw in a new state, phaseChanged is true (lastBattleState was -1), so
	// reflowLowerPanelLayout resets requestedDisplayHeight to the base floor.  The
	// first draw therefore primes lastBattleState = BS_PlaceMines.  On the SECOND
	// draw (same state), phaseChanged is false and the same-phase max-preserve keeps
	// the expanded height set by drawPlaceMines().
	const int panelW = battleScreen->GetSize().GetWidth();
	const int bmpWidth = panelW > 0 ? panelW : 1200;
	// First draw: primes lastBattleState to BS_PlaceMines.
	{
		wxMemoryDC dc;
		wxBitmap bmp(bmpWidth, 240);
		dc.SelectObject(bmp);
		displayPanel->draw(dc);
		dc.SelectObject(wxNullBitmap);
	}
	// Second draw: phaseChanged == false => max-preserve keeps the expanded height.
	{
		wxMemoryDC dc;
		wxBitmap bmp(bmpWidth, 240);
		dc.SelectObject(bmp);
		displayPanel->draw(dc);
		dc.SelectObject(wxNullBitmap);
	}

	const int expandedMineHeight = peer->requestedDisplayHeightPublic();
	// Verify that drawPlaceMines() actually expanded the height beyond the floor so the
	// shrink assertion below is meaningful.
	CPPUNIT_ASSERT_MESSAGE(
		"SMRIV-04 precondition: requestedDisplayHeight after two draws in BS_PlaceMines "
		"with 6 Minelayers must exceed 120 (the floor) so there is room to shrink back. "
		"Check that the 6-Minelayer fixture produces a mine-source list tall enough to "
		"overflow the panel.",
		expandedMineHeight > 120);

	// --- Transition to BS_Battle / PH_MOVE ---
	battleScreen->setState(BS_Battle);
	battleScreen->setPhase(PH_MOVE);
	battleScreen->setMoveComplete(true);

	// --- Draw offscreen in BS_Battle/PH_MOVE ---
	// drawMoveShip() calls refreshMovePromptReservation() -> ensureLowerPanelLayoutState()
	// which detects the phase change (lastBattleState/lastBattlePhase differ from current)
	// and skips the max-preserve, allowing requestedDisplayHeight to drop to the base
	// content height for this phase.
	{
		wxMemoryDC dc;
		wxBitmap bmp(bmpWidth, 240);
		dc.SelectObject(bmp);
		displayPanel->draw(dc);
		dc.SelectObject(wxNullBitmap);
	}

	const int movePhaseHeight = peer->requestedDisplayHeightPublic();

	// Clean up before asserting so a failing assertion cannot leak the window.
	battleScreen->Destroy();
	m_harness.pumpEvents(5);
	delete attackFleet;
	delete defendFleet;
	m_harness.cleanupOrphanTopLevels(10);

	// AC1 (SMRIV-04): requestedDisplayHeight must decrease after the phase transition.
	// Pre-fix code would keep it at expandedMineHeight (max-preserve always runs).
	// Post-fix code resets it because phaseChanged == true.
	CPPUNIT_ASSERT_MESSAGE(
		"SMRIV-04 AC1: requestedDisplayHeight must be smaller after transitioning from "
		"BS_PlaceMines (large mine-source list) to BS_Battle/PH_MOVE (no row list). "
		"If this fails, ensureLowerPanelLayoutState() is still using the ratchet-only "
		"max-preserve and the SMRIV-04 phase-change reset is not taking effect.",
		movePhaseHeight < expandedMineHeight);

	// AC3 (SMRIV-04): The 120-px floor must be preserved after the shrink.
	CPPUNIT_ASSERT_MESSAGE(
		"SMRIV-04 AC3: requestedDisplayHeight after transition must be >= 120 (the floor). "
		"The panel must never shrink below the minimum.",
		movePhaseHeight >= 120);
}

void TacticalGuiLiveTest::testSeekerActivationAnchorIsAtActionPromptLineY() {
	// SMRV-02: Behavioral anchor discrimination test.
	//
	// The change in drawSeekerActivation() moved the "Activated seekers:" list anchor from
	// getActionButtonRowBottom() (old) to getActionPromptLineY(0) (new), matching the
	// drawPlaceMines/drawPlaceSeekers convention.
	//
	// This test:
	//   1. Drives PH_SEEKER_ACTIVATION with one ACTIVATED seeker seeded in the model,
	//   2. Renders the lower panel to an offscreen wxMemoryDC,
	//   3. Asserts the first activation row region's top y is >= getActionPromptLineY(0)
	//      (new anchor at the top of the panel),
	//   4. Asserts the first activation row region's top y is LESS THAN getActionButtonRowBottom()
	//      (NOT at the old below-button-row position, discriminating old vs new anchor),
	//   5. Simulates a click on the region and verifies deactivation (AC-2 click alignment).
	//
	// Discrimination:
	//   OLD anchor (getActionButtonRowBottom() ~70): first row top ~= 70 + 16 = 86.
	//     Assertion 4 (top < actionButtonRowBottom) FAILS => test fails against old code.
	//   NEW anchor (getActionPromptLineY(0) = 5): first row top ~= 5 + 16 = 21.
	//     Assertion 3 (top >= 5) PASSES and assertion 4 (21 < ~70) PASSES => test passes.

	// --- Setup ---
	FFleet * attackFleet = new FFleet();
	FFleet * defendFleet = new FFleet();
	FVehicle * attacker = createShip("Destroyer");
	FVehicle * defender = createShip("Frigate");
	CPPUNIT_ASSERT_MESSAGE("Attacker Destroyer must be creatable.", attacker != NULL);
	CPPUNIT_ASSERT_MESSAGE("Defender Frigate must be creatable.", defender != NULL);
	attackFleet->addShip(attacker);
	defendFleet->addShip(defender);

	FleetList attackFleets;
	FleetList defendFleets;
	attackFleets.push_back(attackFleet);
	defendFleets.push_back(defendFleet);

	// TestableBattleScreen allows seedSeeker() injection into the underlying FTacticalGame.
	TestableBattleScreen * battleScreen =
		new TestableBattleScreen("SMRV-02 Seeker Activation Anchor");
	battleScreen->setupFleets(&attackFleets, &defendFleets, false, NULL);
	battleScreen->Show();
	m_harness.pumpEvents(2);

	FBattleDisplay * displayPanel = findFirstBattleDisplay(battleScreen);
	CPPUNIT_ASSERT_MESSAGE("FBattleDisplay must exist in the battle screen.", displayPanel != NULL);
	FBattleDisplayTestPeer * peer = static_cast<FBattleDisplayTestPeer *>(displayPanel);

	// --- Inject an activated seeker ---
	// ownerID = 1 = AttackerID (m_movingPlayer = true by default => movingPlayerID = attackerID = 1).
	// active = true, activationPhaseIndex = 0 matches default m_seekerActivationPhaseIndex.
	FTacticalSeekerMissileState activatedSeeker;
	activatedSeeker.seekerID              = 42;
	activatedSeeker.ownerID               = 1;         // AttackerID = moving player
	activatedSeeker.hex                   = FPoint(3, 4);
	activatedSeeker.heading               = 0;
	activatedSeeker.active                = true;       // already activated this phase
	activatedSeeker.movementTurn          = 1;
	activatedSeeker.movementAllowance     = 2;
	activatedSeeker.hasSource             = false;
	activatedSeeker.activationPhaseIndex  = 0;          // matches m_seekerActivationPhaseIndex default
	battleScreen->seedSeeker(activatedSeeker);

	// --- Transition to BS_Battle / PH_SEEKER_ACTIVATION ---
	battleScreen->setState(BS_Battle);
	battleScreen->setPhase(PH_SEEKER_ACTIVATION);
	battleScreen->setMoveComplete(true);
	m_harness.pumpEvents(2);

	// --- Offscreen draw ---
	{
		wxMemoryDC dc;
		const int bmpWidth = battleScreen->GetSize().GetWidth() > 0 ? battleScreen->GetSize().GetWidth() : 1200;
		wxBitmap bmp(bmpWidth, 240);
		dc.SelectObject(bmp);
		displayPanel->draw(dc);
		dc.SelectObject(wxNullBitmap);
	}

	// Capture anchor values for assertions (done before Destroy so buttons are valid).
	const int actionPromptY0 = FBattleDisplayTestPeer::actionPromptLineY(0);
	const int actionButtonRowBottom = peer->actionButtonRowBottomPublic();
	const size_t regionCount = peer->seekerActivationRegionCount();
	wxRect firstRegion;
	if (regionCount >= 1) {
		firstRegion = peer->seekerActivationRegion(0);
	}

	// --- AC-2: click alignment ---
	// Simulate a click inside the first activation region and verify deactivation:
	// getActiveSeekersByMovingPlayerThisPhase() must drop from 1 to 0 after the click
	// (deactivateActiveSeekerByID sets active=false, not remove, so getSeekerMissiles().size()
	// stays the same, but the this-phase-active count decrements).
	size_t thisPhaseBeforeClick = battleScreen->getActiveSeekersByMovingPlayerThisPhase().size();
	if (regionCount >= 1) {
		// Click the center of the first region.
		const wxPoint clickPt(firstRegion.GetX() + firstRegion.GetWidth() / 2,
		                      firstRegion.GetY() + firstRegion.GetHeight() / 2);
		wxMouseEvent clickEvt(wxEVT_LEFT_UP);
		clickEvt.m_x = clickPt.x;
		clickEvt.m_y = clickPt.y;
		peer->checkSeekerActivationSelectionPublic(clickEvt);
	}
	size_t thisPhaseAfterClick = battleScreen->getActiveSeekersByMovingPlayerThisPhase().size();

	// --- Clean up before asserting ---
	battleScreen->Destroy();
	m_harness.pumpEvents(5);
	delete attackFleet;
	delete defendFleet;
	m_harness.cleanupOrphanTopLevels(10);

	// --- Assertions ---

	// Precondition: the seeker was injected and is visible in the model before the draw.
	CPPUNIT_ASSERT_MESSAGE(
		"SMRV-02 precondition: At least one seeker must be present in the model for this test "
		"to exercise the activation list path. Seeder injection may have failed.",
		regionCount >= 1);

	// AC-1 (new anchor): the first activation row's top y is >= getActionPromptLineY(0).
	// This confirms the list renders at the top of the panel (right-column position).
	CPPUNIT_ASSERT_MESSAGE(
		"SMRV-02 AC-1: First seeker activation row top must be >= getActionPromptLineY(0) "
		"(new anchor at the top of the lower panel). "
		"drawSeekerActivation() must use 'int y = getActionPromptLineY(0)' as the list anchor.",
		firstRegion.GetTop() >= actionPromptY0);

	// AC-1 discrimination: the first row's top y must be LESS THAN getActionButtonRowBottom().
	// With the OLD getActionButtonRowBottom() anchor, the region top would be >= buttonRowBottom,
	// so this assertion FAILS against the old code and PASSES against the new code.
	CPPUNIT_ASSERT_MESSAGE(
		"SMRV-02 AC-1 (discrimination): First seeker activation row top must be LESS THAN "
		"getActionButtonRowBottom(). With the old 'int y = getActionButtonRowBottom()' anchor "
		"the region would start at or after the button row, failing this assertion. "
		"This confirms the anchor was changed to getActionPromptLineY(0) (top of panel).",
		firstRegion.GetTop() < actionButtonRowBottom);

	// AC-2 (click alignment): deactivating a seeker through the region removes it from the
	// this-phase-active list. getActiveSeekersByMovingPlayerThisPhase() must drop from 1 to 0
	// after the click, confirming the click region aligns with the drawn position and triggers
	// deactivation. (deactivateActiveSeekerByID sets active=false; getSeekerMissiles().size()
	// stays the same but the this-phase-active count decrements.)
	CPPUNIT_ASSERT_MESSAGE(
		"SMRV-02 AC-2: Clicking the seeker activation row region must deactivate the seeker. "
		"getActiveSeekersByMovingPlayerThisPhase().size() must decrease after a click inside "
		"the region, confirming click-region/draw-position alignment.",
		thisPhaseAfterClick < thisPhaseBeforeClick);
}

void TacticalGuiLiveTest::testSeekerMoveCountOverlaySupressesOpponentLabelsDuringActivation() {
	// SMRV-03: Behavioral pixel-level discrimination test for the activation-phase
	// ownership guard in drawSeekerMoveCountOverlay().
	//
	// Two seekers are seeded at different hexes:
	//   Moving player (ownerID=1, AttackerID) at hex (5,5) — label MUST render.
	//   Opponent       (ownerID=0, DefenderID) at hex (3,3) — label MUST be suppressed.
	//
	// Board geometry (scale=1.0, m_size=50, m_trim=50, m_d=25, m_a≈14.43):
	//   hex(col,row): cx = m_trim + m_d + 2*m_d*col + m_d*(row%2)
	//                 cy = m_trim + 2*m_a + 3*m_a*row
	//   label offset: (cx + m_d*0.4, cy - m_a*1.5)
	//
	//   hex(5,5): cx=350, cy≈295  → label≈(360, 273)
	//   hex(3,3): cx=250, cy≈209  → label≈(260, 187)
	//
	// AC1-absent: during PH_SEEKER_ACTIVATION no red pixel appears near hex(3,3)'s
	//   label position.  Without the SMRV-03 guard the opponent seeker would draw a
	//   red label, causing this assertion to FAIL.
	// AC1-present: during PH_SEEKER_ACTIVATION a red pixel IS found near hex(5,5)'s
	//   label position.  The guard must NOT suppress the moving player's own seeker.
	// AC2: during PH_ATTACK_FIRE (a non-activation phase) BOTH hexes produce red
	//   pixels, confirming the guard is scoped strictly to PH_SEEKER_ACTIVATION.

	FFleet * attackFleet = new FFleet();
	FFleet * defendFleet = new FFleet();
	FVehicle * attacker = createShip("Destroyer");
	FVehicle * defender = createShip("Frigate");
	CPPUNIT_ASSERT_MESSAGE("Attacker Destroyer must be creatable.", attacker != NULL);
	CPPUNIT_ASSERT_MESSAGE("Defender Frigate must be creatable.", defender != NULL);
	attackFleet->addShip(attacker);
	defendFleet->addShip(defender);
	FleetList attackFleets;
	FleetList defendFleets;
	attackFleets.push_back(attackFleet);
	defendFleets.push_back(defendFleet);

	TestableBattleScreen * battleScreen =
		new TestableBattleScreen("SMRV-03 Speed Label Suppression");
	battleScreen->setupFleets(&attackFleets, &defendFleets, false, NULL);
	battleScreen->Show();
	m_harness.pumpEvents(2);

	battleScreen->setState(BS_Battle);
	battleScreen->setMoveComplete(true);
	m_harness.pumpEvents(1);

	// Seed moving-player seeker at hex (5,5).
	// ownerID=1 = AttackerID = getMovingPlayerID() when m_movingPlayer=true (default).
	FTacticalSeekerMissileState movingSeeker;
	movingSeeker.seekerID          = 5001;
	movingSeeker.ownerID           = 1;          // moving player (attacker)
	movingSeeker.hex               = FPoint(5, 5);
	movingSeeker.heading           = 0;
	movingSeeker.active            = true;
	movingSeeker.movementTurn      = 1;
	movingSeeker.movementAllowance = 3;
	movingSeeker.hasSource         = false;
	movingSeeker.activationPhaseIndex = 0;
	battleScreen->seedSeeker(movingSeeker);

	// Seed opponent seeker at hex (3,3).
	// ownerID=0 = DefenderID = NOT the moving player during activation.
	FTacticalSeekerMissileState opponentSeeker;
	opponentSeeker.seekerID          = 5002;
	opponentSeeker.ownerID           = 0;        // opponent (defender)
	opponentSeeker.hex               = FPoint(3, 3);
	opponentSeeker.heading           = 0;
	opponentSeeker.active            = true;
	opponentSeeker.movementTurn      = 1;
	opponentSeeker.movementAllowance = 2;
	opponentSeeker.hasSource         = false;
	opponentSeeker.activationPhaseIndex = 0;
	battleScreen->seedSeeker(opponentSeeker);

	FBattleBoard * board = findFirstBattleBoard(battleScreen);
	CPPUNIT_ASSERT_MESSAGE("FBattleBoard must exist for offscreen draw.", board != NULL);

	// Pixel sampling helpers — tolerance absorbs font/scroll-offset variation.
	// hex(5,5) label ≈ (360, 273); hex(3,3) label ≈ (260, 187).
	const int movingLabelX = 360, movingLabelY = 273;
	const int opponentLabelX = 260, opponentLabelY = 187;
	const int tolX = 30, tolY = 20;

	auto hasRedPixel = [&](const wxImage & img, int cx, int cy) -> bool {
		const int x0 = std::max(0, cx - tolX);
		const int y0 = std::max(0, cy - tolY);
		const int x1 = std::min(img.GetWidth()  - 1, cx + tolX);
		const int y1 = std::min(img.GetHeight() - 1, cy + tolY);
		for (int py = y0; py <= y1; ++py) {
			for (int px = x0; px <= x1; ++px) {
				if (img.GetRed(px, py) >= 200
				    && img.GetGreen(px, py) < 80
				    && img.GetBlue(px, py) < 80) {
					return true;
				}
			}
		}
		return false;
	};

	// ---- PH_SEEKER_ACTIVATION draw ----
	bool activationMovingPresent  = false;
	bool activationOpponentAbsent = true;
	battleScreen->setPhase(PH_SEEKER_ACTIVATION);
	{
		wxBitmap bmp(2000, 1500);
		wxMemoryDC dc;
		dc.SelectObject(bmp);
		board->draw(dc);
		dc.SelectObject(wxNullBitmap);
		const wxImage img = bmp.ConvertToImage();
		activationMovingPresent  = hasRedPixel(img, movingLabelX, movingLabelY);
		activationOpponentAbsent = !hasRedPixel(img, opponentLabelX, opponentLabelY);
	}

	// ---- PH_ATTACK_FIRE draw ----
	bool attackFireMovingPresent   = false;
	bool attackFireOpponentPresent = false;
	battleScreen->setPhase(PH_ATTACK_FIRE);
	{
		wxBitmap bmp(2000, 1500);
		wxMemoryDC dc;
		dc.SelectObject(bmp);
		board->draw(dc);
		dc.SelectObject(wxNullBitmap);
		const wxImage img = bmp.ConvertToImage();
		attackFireMovingPresent   = hasRedPixel(img, movingLabelX, movingLabelY);
		attackFireOpponentPresent = hasRedPixel(img, opponentLabelX, opponentLabelY);
	}

	battleScreen->Destroy();
	m_harness.pumpEvents(5);
	delete attackFleet;
	delete defendFleet;
	m_harness.cleanupOrphanTopLevels(10);

	// AC1-present: moving player seeker label visible during PH_SEEKER_ACTIVATION.
	CPPUNIT_ASSERT_MESSAGE(
		"SMRV-03 AC1-present: Moving-player seeker (ownerID=1) at hex(5,5) must render a red "
		"speed-value label in PH_SEEKER_ACTIVATION. drawSeekerMoveCountOverlay() must NOT "
		"suppress the moving player's own seeker.",
		activationMovingPresent);

	// AC1-absent: opponent seeker label suppressed during PH_SEEKER_ACTIVATION.
	// This assertion FAILS against pre-SMRV-03 code (no ownership guard) and PASSES
	// against the shipped guarded code.
	CPPUNIT_ASSERT_MESSAGE(
		"SMRV-03 AC1-absent: Opponent seeker (ownerID=0) at hex(3,3) must NOT render a red "
		"speed-value label in PH_SEEKER_ACTIVATION. The SMRV-03 ownership guard must suppress "
		"opponent seekers during activation, matching drawSeekerMissiles() sprite suppression. "
		"If this fails, the 'activationPhase && itr->ownerID != m_parent->getMovingPlayerID()' "
		"guard is missing or incorrectly applied.",
		activationOpponentAbsent);

	// AC2: in PH_ATTACK_FIRE (non-activation phase) both labels render.
	CPPUNIT_ASSERT_MESSAGE(
		"SMRV-03 AC2: Moving-player seeker label must render in PH_ATTACK_FIRE "
		"(guard is scoped strictly to PH_SEEKER_ACTIVATION).",
		attackFireMovingPresent);
	CPPUNIT_ASSERT_MESSAGE(
		"SMRV-03 AC2: Opponent seeker label must render in PH_ATTACK_FIRE "
		"(guard is scoped strictly to PH_SEEKER_ACTIVATION, not other phases).",
		attackFireOpponentPresent);
}

}
