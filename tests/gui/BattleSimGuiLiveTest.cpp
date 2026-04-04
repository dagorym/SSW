/**
 * @file BattleSimGuiLiveTest.cpp
 * @brief Implementation file for live BattleSim GUI launch-path coverage
 */

#include "BattleSimGuiLiveTest.h"

#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/filename.h>
#include <wx/frame.h>
#include <wx/toplevel.h>

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
	button->GetEventHandler()->ProcessEvent(event);
}

int countShownTopLevels(const WXGuiTestHarness & harness) {
	const std::vector<wxTopLevelWindow *> topLevels = harness.getTopLevelWindows(false);
	int shownTopLevels = 0;
	for (std::vector<wxTopLevelWindow *>::const_iterator itr = topLevels.begin();
	     itr != topLevels.end();
	     ++itr) {
		if (*itr != NULL && (*itr)->IsShown() && !(*itr)->IsBeingDeleted()) {
			++shownTopLevels;
		}
	}
	return shownTopLevels;
}

void stabilizeTopLevels(WXGuiTestHarness & harness, int attempts = 4) {
	for (int attempt = 0; attempt < attempts; ++attempt) {
		harness.cleanupOrphanTopLevels(10);
		harness.pumpEvents(10);
	}
}

void forceCloseShownTopLevels(WXGuiTestHarness & harness, int attempts = 3) {
	for (int attempt = 0; attempt < attempts; ++attempt) {
		const std::vector<wxTopLevelWindow *> topLevels = harness.getTopLevelWindows(false);
		for (std::vector<wxTopLevelWindow *>::const_iterator itr = topLevels.begin();
		     itr != topLevels.end();
		     ++itr) {
			wxTopLevelWindow * topLevel = *itr;
			if (topLevel == NULL || topLevel->IsBeingDeleted() || !topLevel->IsShown()) {
				continue;
			}
			wxDialog * dialog = dynamic_cast<wxDialog *>(topLevel);
			if (dialog != NULL && dialog->IsModal()) {
				dialog->EndModal(wxID_CANCEL);
			}
			topLevel->Destroy();
		}
		harness.pumpEvents(10);
	}
}

bool wasTopLevelPresent(const std::vector<wxTopLevelWindow *> & existingTopLevels, wxTopLevelWindow * candidate) {
	for (std::vector<wxTopLevelWindow *>::const_iterator itr = existingTopLevels.begin();
	     itr != existingTopLevels.end();
	     ++itr) {
		if (*itr == candidate) {
			return true;
		}
	}
	return false;
}

int countShownTopLevelsNotInBaseline(const WXGuiTestHarness & harness,
                                     const std::vector<wxTopLevelWindow *> & baselineTopLevels) {
	const std::vector<wxTopLevelWindow *> topLevels = harness.getTopLevelWindows(false);
	int shownTopLevels = 0;
	for (std::vector<wxTopLevelWindow *>::const_iterator itr = topLevels.begin(); itr != topLevels.end(); ++itr) {
		if (*itr == NULL || !(*itr)->IsShown() || (*itr)->IsBeingDeleted()) {
			continue;
		}
		if (!wasTopLevelPresent(baselineTopLevels, *itr)) {
			++shownTopLevels;
		}
	}
	return shownTopLevels;
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
	m_harness.cleanupOrphanTopLevels(10);
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
	m_harness.cleanupOrphanTopLevels(10);
	m_harness.shutdown();
}

void BattleSimGuiLiveTest::testBattleSimFrameOpensLocalGameDialogAndReturns() {
	const std::vector<wxTopLevelWindow *> baselineTopLevels = m_harness.getTopLevelWindows(false);
	BattleSimFrame frame("BattleSim Test Frame", wxDefaultPosition, wxSize(360, 240));
	frame.Show();
	m_harness.pumpEvents();

	wxButton * localButton = findButtonByLabel(&frame, wxT("Play a Local Game"));
	bool localDialogWindowPresented = false;
	m_harness.runVoidFunctionWithAction([&]() {
		clickButton(&frame, localButton);
	}, [&]() {
		wxDialog * launchedDialog = m_harness.waitForModalDialog(200, 5);
		LocalGameDialog * localDialog = dynamic_cast<LocalGameDialog *>(launchedDialog);
		localDialogWindowPresented = (localDialog != NULL && localDialog->GetParent() == &frame);
	}, 0, 200);

	CPPUNIT_ASSERT(localDialogWindowPresented);
	CPPUNIT_ASSERT(frame.IsShown());
	if (frame.IsShown()) {
		frame.Hide();
	}
	frame.Destroy();
	m_harness.pumpEvents(10);
	stabilizeTopLevels(m_harness);
	forceCloseShownTopLevels(m_harness);
	CPPUNIT_ASSERT_EQUAL(0, countShownTopLevelsNotInBaseline(m_harness, baselineTopLevels));
}

void BattleSimGuiLiveTest::testLocalGameDialogLaunchesPredefinedAndCustomModalChains() {
	const std::vector<wxTopLevelWindow *> baselineTopLevels = m_harness.getTopLevelWindows(false);
	wxFrame * parent = new wxFrame(NULL, wxID_ANY, "BattleSim Dialog Parent");
	parent->Show();
	m_harness.pumpEvents();

	{
		LocalGameDialogTestPeer * predefinedDialog = new LocalGameDialogTestPeer(parent);
		bool scenarioDialogPresented = false;
		m_harness.runVoidFunctionWithAction([&]() {
			predefinedDialog->clickPlayPredefined();
		}, [&]() {
			wxDialog * launchedDialog = m_harness.waitForModalDialog(200, 5);
			ScenarioDialog * scenarioDialog = dynamic_cast<ScenarioDialog *>(launchedDialog);
			scenarioDialogPresented =
			        (scenarioDialog != NULL && scenarioDialog->GetParent() == predefinedDialog);
		}, 0, 200);
		CPPUNIT_ASSERT(scenarioDialogPresented);
		if (predefinedDialog->IsShown()) {
			predefinedDialog->Hide();
		}
		predefinedDialog->Destroy();
		m_harness.pumpEvents(5);
	}

	{
		LocalGameDialogTestPeer * customDialog = new LocalGameDialogTestPeer(parent);
		bool scenarioEditorPresented = false;
		m_harness.runVoidFunctionWithAction([&]() {
			customDialog->clickCreateNew();
		}, [&]() {
			wxDialog * launchedDialog = m_harness.waitForModalDialog(200, 5);
			ScenarioEditorGUI * scenarioEditorDialog = dynamic_cast<ScenarioEditorGUI *>(launchedDialog);
			scenarioEditorPresented =
			        (scenarioEditorDialog != NULL && scenarioEditorDialog->GetParent() == customDialog);
		}, 0, 200);
		CPPUNIT_ASSERT(scenarioEditorPresented);
		if (customDialog->IsShown()) {
			customDialog->Hide();
		}
		customDialog->Destroy();
		m_harness.pumpEvents(5);
	}

	if (parent->IsShown()) {
		parent->Hide();
	}
	parent->Destroy();
	m_harness.pumpEvents(10);
	stabilizeTopLevels(m_harness);
	forceCloseShownTopLevels(m_harness);
	CPPUNIT_ASSERT_EQUAL(0, countShownTopLevelsNotInBaseline(m_harness, baselineTopLevels));
}

void BattleSimGuiLiveTest::testScenarioDialogScenarioPathLaunchesBattleScreenWithLifecycleCoverage() {
	const std::vector<wxTopLevelWindow *> baselineTopLevels = m_harness.getTopLevelWindows(false);
	const int baselineShownTopLevels = countShownTopLevels(m_harness);
	wxFrame * parent = new wxFrame(NULL, wxID_ANY, "Scenario Dialog Parent");
	parent->Show();
	m_harness.pumpEvents();

	FBattleScreen::resetLifecycleCounters();
	{
		ScenarioDialogTestPeer * dialog = new ScenarioDialogTestPeer(parent);
		bool launchedTopLevelPresented = false;
		m_harness.runVoidFunctionWithAction([&]() {
			dialog->clickScenario1();
		}, [&]() {
			wxTopLevelWindow * launchedTopLevel = m_harness.waitForTopLevelWindow([&](wxTopLevelWindow * topLevel) {
				return topLevel != NULL && topLevel != parent && topLevel != dialog;
			}, 200, 5);
			launchedTopLevelPresented = (launchedTopLevel != NULL);
		}, 0, 200);
		CPPUNIT_ASSERT(launchedTopLevelPresented);
		if (dialog->IsShown()) {
			dialog->Hide();
		}
		dialog->Destroy();
		m_harness.pumpEvents(5);
	}

	CPPUNIT_ASSERT(FBattleScreen::getConstructedCount() >= 1);
	CPPUNIT_ASSERT_EQUAL(FBattleScreen::getConstructedCount(), FBattleScreen::getDestroyedCount());
	CPPUNIT_ASSERT_EQUAL(0, FBattleScreen::getLiveInstanceCount());
	if (parent->IsShown()) {
		parent->Hide();
	}
	parent->Destroy();
	m_harness.pumpEvents(10);
	stabilizeTopLevels(m_harness);
	forceCloseShownTopLevels(m_harness);
	CPPUNIT_ASSERT_EQUAL(baselineShownTopLevels, countShownTopLevels(m_harness));
	CPPUNIT_ASSERT_EQUAL(0, countShownTopLevelsNotInBaseline(m_harness, baselineTopLevels));
}

void BattleSimGuiLiveTest::testScenarioEditorStartBattleLaunchesBattleScreenWithLifecycleCoverage() {
	const std::vector<wxTopLevelWindow *> baselineTopLevels = m_harness.getTopLevelWindows(false);
	const int baselineShownTopLevels = countShownTopLevels(m_harness);
	wxFrame * parent = new wxFrame(NULL, wxID_ANY, "Scenario Editor Parent");
	parent->Show();
	m_harness.pumpEvents();

	FBattleScreen::resetLifecycleCounters();
	{
		ScenarioEditorGUITestPeer * dialog = new ScenarioEditorGUITestPeer(parent);
		bool launchedTopLevelPresented = false;
		m_harness.runVoidFunctionWithAction([&]() {
			dialog->chooseDefenderTeam(1);
			dialog->chooseDefenderType(0);
			dialog->addDefenderShip();
			dialog->chooseAttackerTeam(2);
			dialog->chooseAttackerType(0);
			dialog->addAttackerShip();
			dialog->clickStartBattle();
		}, [&]() {
			wxTopLevelWindow * launchedTopLevel = m_harness.waitForTopLevelWindow([&](wxTopLevelWindow * topLevel) {
				return topLevel != NULL && topLevel != parent && topLevel != dialog;
			}, 200, 5);
			launchedTopLevelPresented = (launchedTopLevel != NULL);
		}, 0, 200);
		CPPUNIT_ASSERT(launchedTopLevelPresented);
		CPPUNIT_ASSERT(dialog->finalizeCalled());
		if (dialog->IsShown()) {
			dialog->Hide();
		}
		dialog->Destroy();
		m_harness.pumpEvents(5);
	}

	CPPUNIT_ASSERT(FBattleScreen::getConstructedCount() >= 1);
	CPPUNIT_ASSERT_EQUAL(FBattleScreen::getConstructedCount(), FBattleScreen::getDestroyedCount());
	CPPUNIT_ASSERT_EQUAL(0, FBattleScreen::getLiveInstanceCount());
	if (parent->IsShown()) {
		parent->Hide();
	}
	parent->Destroy();
	m_harness.pumpEvents(10);
	stabilizeTopLevels(m_harness);
	forceCloseShownTopLevels(m_harness);
	CPPUNIT_ASSERT_EQUAL(baselineShownTopLevels, countShownTopLevels(m_harness));
	CPPUNIT_ASSERT_EQUAL(0, countShownTopLevelsNotInBaseline(m_harness, baselineTopLevels));
}

}
