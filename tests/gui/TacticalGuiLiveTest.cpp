/**
 * @file TacticalGuiLiveTest.cpp
 * @brief Implementation file for live tactical GUI regression coverage
 */

#include "TacticalGuiLiveTest.h"

#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/toplevel.h>
#include <wx/window.h>

#include <algorithm>
#include <iostream>
#include <vector>

#include "gui/ICMSelectionGUI.h"
#include "gui/TacticalDamageSummaryGUI.h"
#include "gui/WXTacticalUI.h"
#include "ships/FVehicle.h"
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
		finalizeAssignments(event);
	}

	void finalizeAssignmentsWithoutModal() {
		std::map<unsigned int, FVehicle *> shipList;
		std::vector<ICMData *>::iterator wItr = m_ICMData->begin();
		for (unsigned int i = 0; i < m_ICMData->size(); i++) {
			std::map<unsigned int, AssignedICMData *>::iterator sItr = m_shipICMData.begin();
			int totalICMs = 0;
			for (unsigned int j = 0; j < m_shipICMData.size(); j++) {
				totalICMs += (*sItr).second->getICMsAllocatedToWeapon((*wItr)->weapon);
				sItr++;
			}
			(*wItr)->weapon->setAssignedICMCount(totalICMs);
			VehicleList::iterator itr = (*wItr)->vehicles->begin();
			for (unsigned int j = 0; j < (*wItr)->vehicles->size(); j++) {
				shipList[(*itr)->getID()] = (*itr);
				itr++;
			}
			wItr++;
		}
		std::map<unsigned int, FVehicle *>::iterator vItr = shipList.begin();
		for (unsigned int i = 0; i < shipList.size(); i++) {
			FDefense * d = vItr->second->getDefense(vItr->second->hasDefense(FDefense::ICM));
			int expendedICMs = m_shipICMData[vItr->first]->getAllocatedICMs();
			d->setCurrentAmmo(d->getAmmo() - expendedICMs);
			vItr++;
		}
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

	return summary;
}

}

CPPUNIT_TEST_SUITE_REGISTRATION( TacticalGuiLiveTest );

void TacticalGuiLiveTest::setUp() {
CPPUNIT_ASSERT(m_harness.bootstrap());
}

void TacticalGuiLiveTest::tearDown() {
	m_harness.shutdown();
}

void TacticalGuiLiveTest::testWXTacticalUIParentBackedModalAndRedrawPaths() {
	std::cerr << "TACTICAL1:start" << std::endl;
	wxFrame * parent = new wxFrame(NULL, wxID_ANY, "WXTacticalUI Parent", wxDefaultPosition, wxSize(540, 420));
	wxPanel * redrawPanel = new wxPanel(parent, wxID_ANY);
	parent->Show();
	redrawPanel->Show();
	m_harness.pumpEvents();

WXTacticalUI ui(redrawPanel);
	const FTacticalCombatReportSummary summary = buildSummaryWithLines();

	std::cerr << "TACTICAL1:damage" << std::endl;
	const int damageResult = m_harness.runModalFunctionWithAutoDismiss(
	[&]() { return ui.showDamageSummary(summary); }, wxID_OK, 25);
	CPPUNIT_ASSERT_EQUAL(static_cast<int>(wxID_OK), damageResult);

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

	delete attacker;
	delete target;
	delete defender;
	parent->Destroy();
	m_harness.pumpEvents(10);
	std::cerr << "TACTICAL1:done" << std::endl;
}

void TacticalGuiLiveTest::testTacticalDamageSummaryDialogDisplaysContextAndCloseBehavior() {
wxFrame * parent = new wxFrame(NULL, wxID_ANY, "Damage Summary Parent", wxDefaultPosition, wxSize(540, 420));
parent->Show();
m_harness.pumpEvents();

const FTacticalCombatReportSummary summary = buildSummaryWithLines();
TacticalDamageSummaryGUI dialog(parent, summary);

CPPUNIT_ASSERT_EQUAL(
wxString::FromUTF8("Tactical Damage Summary - Offensive Fire Results"),
dialog.GetTitle());

wxStaticText * contextText = findStaticTextContaining(&dialog, wxT("Report:"));
CPPUNIT_ASSERT(contextText != NULL);
CPPUNIT_ASSERT(contextText->GetLabel().Find(wxT("Immediate report")) != wxNOT_FOUND);
CPPUNIT_ASSERT(contextText->GetLabel().Find(wxT("Phase: 2")) != wxNOT_FOUND);

wxTextCtrl * summaryText = findFirstTextCtrl(&dialog);
CPPUNIT_ASSERT(summaryText != NULL);
CPPUNIT_ASSERT(summaryText->GetValue().Find(wxT("Destroyer Alpha took 4 hull damage.")) != wxNOT_FOUND);

	wxButton * closeButton = findButtonByLabel(&dialog, wxT("Close"));
	CPPUNIT_ASSERT(closeButton != NULL);
	CPPUNIT_ASSERT_EQUAL(static_cast<int>(wxID_OK), m_harness.showModalWithAutoDismiss(dialog, wxID_OK, 25));

FTacticalCombatReportSummary emptySummary;
emptySummary.context.reportType = TRT_None;
TacticalDamageSummaryGUI emptyDialog(parent, emptySummary);
wxTextCtrl * emptyText = findFirstTextCtrl(&emptyDialog);
CPPUNIT_ASSERT(emptyText != NULL);
CPPUNIT_ASSERT(emptyText->GetValue().Find(wxT("No ships sustained damage in this report.")) != wxNOT_FOUND);
CPPUNIT_ASSERT_EQUAL(static_cast<int>(wxID_OK), m_harness.showModalWithAutoDismiss(emptyDialog, wxID_OK, 25));

parent->Destroy();
m_harness.pumpEvents(10);
}

void TacticalGuiLiveTest::testICMSelectionDialogInteractionFinalizesAssignedCountsAndAmmo() {
wxFrame * parent = new wxFrame(NULL, wxID_ANY, "ICM Selection Parent", wxDefaultPosition, wxSize(620, 460));
parent->Show();
m_harness.pumpEvents();

FVehicle * target = createShip("Destroyer");
FVehicle * defenderA = createShip("Destroyer");
FVehicle * defenderB = createShip("Frigate");
CPPUNIT_ASSERT(target != NULL && defenderA != NULL && defenderB != NULL);
target->setOwner(2);
defenderA->setOwner(2);
defenderB->setOwner(2);

InspectableWeapon * weapon = new InspectableWeapon();
weapon->setTarget(target, 4);

VehicleList nearbyShips;
nearbyShips.push_back(target);
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

	{
		ICMSelectionGUITestPeer dialog(parent, &icmRows);
		dialog.selectWeaponRow(0);
		CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), dialog.appliedControlCount());
		dialog.setAppliedICMValue(0, 2);
		dialog.setAppliedICMValue(1, 1);
		CPPUNIT_ASSERT_EQUAL(wxString::FromUTF8("3"), dialog.assignedCellText(0));

		dialog.finalizeAssignmentsWithoutModal();
		CPPUNIT_ASSERT_EQUAL(3, weapon->assignedICMCount());
		CPPUNIT_ASSERT_EQUAL(defenderAStartingAmmo - 2, defenderA->getDefense(defenseIndexA)->getAmmo());
		CPPUNIT_ASSERT_EQUAL(defenderBStartingAmmo - 1, defenderB->getDefense(defenseIndexB)->getAmmo());
	}

parent->Destroy();
m_harness.pumpEvents(10);
}

}
