/**
 * @file BattleSimGuiLiveTest.cpp
 * @brief Implementation file for live BattleSim GUI launch-path coverage
 */

#include "BattleSimGuiLiveTest.h"

#include <wx/button.h>
#include <wx/filename.h>
#include <wx/frame.h>

#include "battleSim/BattleSimFrame.h"
#include "battleSim/LocalGameDialog.h"
#include "battleSim/ScenarioDialog.h"
#include "battleSim/ScenarioEditorGUI.h"
#include "core/FGameConfig.h"
#include "tactical/FBattleScreen.h"

wxBEGIN_EVENT_TABLE(BattleSimFrame, wxFrame)
wxEND_EVENT_TABLE()

namespace FrontierTests {
using namespace Frontier;

namespace {

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

void clickButton(wxWindow * target, wxButton * button) {
	CPPUNIT_ASSERT(target != NULL);
	CPPUNIT_ASSERT(button != NULL);
	wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, button->GetId());
	event.SetEventObject(button);
	target->GetEventHandler()->ProcessEvent(event);
}

class LocalGameDialogTestPeer : public LocalGameDialog {
public:
LocalGameDialogTestPeer(wxWindow * parent)
: LocalGameDialog(parent) {
}

void clickPlayPredefined() {
	wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, m_predefinedButton->GetId());
	onPlayPredefined(event);
}

void clickCreateNew() {
	wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, m_customGameButton->GetId());
	onCreateNew(event);
}

void clickBack() {
	wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, m_backButton->GetId());
	onQuit(event);
}
};

class ScenarioDialogTestPeer : public ScenarioDialog {
public:
ScenarioDialogTestPeer(wxWindow * parent)
: ScenarioDialog(parent) {
}

void clickScenario1() {
	wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, m_scenario1Button->GetId());
	onScenario1(event);
}

void clickDone() {
	wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, m_doneButton->GetId());
	onDone(event);
}
};

class ScenarioEditorGUITestPeer : public ScenarioEditorGUI {
public:
ScenarioEditorGUITestPeer(wxWindow * parent)
: ScenarioEditorGUI(parent), m_finalizeCalled(false) {
}

bool finalizeCalled() const {
	return m_finalizeCalled;
}

void chooseDefenderTeam(int index) {
	m_defenderTeam->SetSelection(index);
	wxCommandEvent event(wxEVT_COMMAND_CHOICE_SELECTED, m_defenderTeam->GetId());
	onSelectDefenderTeam(event);
}

void chooseDefenderType(int index) {
	m_availableDefenderListBox->SetSelection(index);
	wxCommandEvent event(wxEVT_COMMAND_LISTBOX_SELECTED, m_availableDefenderListBox->GetId());
	onSelectDefenderType(event);
}

void addDefenderShip() {
	wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, m_defenderAddButton->GetId());
	onAddDefenderShip(event);
}

void chooseAttackerTeam(int index) {
	m_attackerTeam->SetSelection(index);
	wxCommandEvent event(wxEVT_COMMAND_CHOICE_SELECTED, m_attackerTeam->GetId());
	onSelectAttackerTeam(event);
}

void chooseAttackerType(int index) {
	m_availableAttackerListBox->SetSelection(index);
	wxCommandEvent event(wxEVT_COMMAND_LISTBOX_SELECTED, m_availableAttackerListBox->GetId());
	onSelectAttackerType(event);
}

void addAttackerShip() {
	wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, m_attackerAddButton->GetId());
	onAddAttackerShip(event);
}

void clickStartBattle() {
	wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, m_startBattleButton->GetId());
	onStartBattle(event);
}

protected:
virtual void finalizeStartBattle() wxOVERRIDE {
	m_finalizeCalled = true;
}

private:
bool m_finalizeCalled;
};

}

CPPUNIT_TEST_SUITE_REGISTRATION( BattleSimGuiLiveTest );

void BattleSimGuiLiveTest::setUp() {
	CPPUNIT_ASSERT(m_harness.bootstrap());
	m_originalCwd = wxGetCwd();
	if (!wxFileName::DirExists(wxT("icons")) && wxFileName::DirExists(wxT("../../icons"))) {
		CPPUNIT_ASSERT(wxSetWorkingDirectory(wxT("../../")));
	}
	FGameConfig::reset();
	FGameConfig::create();
}

void BattleSimGuiLiveTest::tearDown() {
	if (!m_originalCwd.IsEmpty()) {
		wxSetWorkingDirectory(m_originalCwd);
	}
	m_harness.shutdown();
}

void BattleSimGuiLiveTest::testBattleSimFrameOpensLocalGameDialogAndReturns() {
	BattleSimFrame frame("BattleSim Test Frame", wxDefaultPosition, wxSize(360, 240));
	frame.Show();
	m_harness.pumpEvents();

	wxButton * localButton = findButtonByLabel(&frame, wxT("Play a Local Game"));
	m_harness.runVoidFunctionWithAutoDismiss([&]() {
		clickButton(&frame, localButton);
	}, 0, 200);

	CPPUNIT_ASSERT(frame.IsShown());
	frame.Destroy();
	m_harness.pumpEvents(10);
}

void BattleSimGuiLiveTest::testLocalGameDialogLaunchesPredefinedAndCustomModalChains() {
	wxFrame parent(NULL, wxID_ANY, "BattleSim Dialog Parent");
	parent.Show();
	m_harness.pumpEvents();

	LocalGameDialogTestPeer predefinedDialog(&parent);
	m_harness.runVoidFunctionWithAutoDismiss([&]() {
		predefinedDialog.clickPlayPredefined();
	}, 0, 200);

	LocalGameDialogTestPeer customDialog(&parent);
	m_harness.runVoidFunctionWithAutoDismiss([&]() {
		customDialog.clickCreateNew();
	}, 0, 200);
}

void BattleSimGuiLiveTest::testScenarioDialogScenarioPathLaunchesBattleScreenWithLifecycleCoverage() {
	wxFrame parent(NULL, wxID_ANY, "Scenario Dialog Parent");
	parent.Show();
	m_harness.pumpEvents();

	ScenarioDialogTestPeer dialog(&parent);
	FBattleScreen::resetLifecycleCounters();
	m_harness.runVoidFunctionWithAutoDismiss([&]() {
		dialog.clickScenario1();
	}, 0, 200);
	CPPUNIT_ASSERT(FBattleScreen::getConstructedCount() >= 1);
	CPPUNIT_ASSERT_EQUAL(FBattleScreen::getConstructedCount(), FBattleScreen::getDestroyedCount());
	CPPUNIT_ASSERT_EQUAL(0, FBattleScreen::getLiveInstanceCount());
}

void BattleSimGuiLiveTest::testScenarioEditorStartBattleLaunchesBattleScreenWithLifecycleCoverage() {
	wxFrame parent(NULL, wxID_ANY, "Scenario Editor Parent");
	parent.Show();
	m_harness.pumpEvents();

	ScenarioEditorGUITestPeer dialog(&parent);
	FBattleScreen::resetLifecycleCounters();
	m_harness.runVoidFunctionWithAutoDismiss([&]() {
		dialog.chooseDefenderTeam(1);
		dialog.chooseDefenderType(0);
		dialog.addDefenderShip();
		dialog.chooseAttackerTeam(2);
		dialog.chooseAttackerType(0);
		dialog.addAttackerShip();
		dialog.clickStartBattle();
	}, 0, 200);
	CPPUNIT_ASSERT(dialog.finalizeCalled());
	CPPUNIT_ASSERT(FBattleScreen::getConstructedCount() >= 1);
	CPPUNIT_ASSERT_EQUAL(FBattleScreen::getConstructedCount(), FBattleScreen::getDestroyedCount());
	CPPUNIT_ASSERT_EQUAL(0, FBattleScreen::getLiveInstanceCount());
}

}
