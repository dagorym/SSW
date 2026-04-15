/**
 * @file BattleSimGuiLiveTest.cpp
 * @brief Implementation file for live BattleSim GUI launch-path coverage
 */

#include "BattleSimGuiLiveTest.h"

#include <wx/button.h>
#include <wx/display.h>
#include <wx/dialog.h>
#include <wx/filename.h>
#include <wx/frame.h>
#include <wx/statbox.h>
#include <wx/toplevel.h>
#include <cstdlib>

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

void assertFrameCenteredOnDisplay(wxFrame * frame) {
	CPPUNIT_ASSERT(frame != NULL);
	const int displayIndex = wxDisplay::GetFromWindow(frame);
	CPPUNIT_ASSERT(displayIndex != wxNOT_FOUND);
	const wxRect displayBounds = wxDisplay(static_cast<unsigned int>(displayIndex)).GetClientArea();
	const wxRect frameBounds = frame->GetScreenRect();
	const wxPoint displayCenter(displayBounds.GetX() + (displayBounds.GetWidth() / 2),
	                            displayBounds.GetY() + (displayBounds.GetHeight() / 2));
	const wxPoint frameCenter(frameBounds.GetX() + (frameBounds.GetWidth() / 2),
	                          frameBounds.GetY() + (frameBounds.GetHeight() / 2));
	CPPUNIT_ASSERT(std::abs(displayCenter.x - frameCenter.x) <= 80);
	CPPUNIT_ASSERT(std::abs(displayCenter.y - frameCenter.y) <= 80);
}

bool isChildFullyInClientArea(wxWindow * parent, wxWindow * child) {
	if (parent == NULL || child == NULL) {
		return false;
	}
	const wxRect clientRect(wxPoint(0, 0), parent->GetClientSize());
	return clientRect.Contains(child->GetRect().GetTopLeft())
	    && clientRect.Contains(child->GetRect().GetBottomRight());
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

bool hasStaticBoxParent(wxWindow * child, const wxString & expectedLabel) const {
	if (child == NULL) {
		return false;
	}
	wxStaticBox * parentBox = dynamic_cast<wxStaticBox *>(child->GetParent());
	return parentBox != NULL && parentBox->GetLabel() == expectedLabel;
}

bool usesStaticBoxParenting() const {
	return hasStaticBoxParent(m_defenderTeam, wxT("Defenders"))
	    && hasStaticBoxParent(m_availableDefenderListBox, wxT("Available Ship Types"))
	    && hasStaticBoxParent(m_assignedDefenderListBox, wxT("Assigned Ships"))
	    && hasStaticBoxParent(m_attackerTeam, wxT("Attackers"))
	    && hasStaticBoxParent(m_availableAttackerListBox, wxT("Available Ship Types"))
	    && hasStaticBoxParent(m_assignedAttackerListBox, wxT("Assigned Ships"));
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
	BattleSimFrame frame("BattleSim Test Frame");
	frame.Show();
	m_harness.pumpEvents();

	wxButton * localButton = findButtonByLabel(&frame, wxT("Play a Local Game"));
	wxButton * networkButton = findButtonByLabel(&frame, wxT("Play a Network Game"));
	wxButton * quitButton = findButtonByLabel(&frame, wxT("Quit"));
	CPPUNIT_ASSERT(localButton != NULL);
	CPPUNIT_ASSERT(networkButton != NULL);
	CPPUNIT_ASSERT(quitButton != NULL);
	CPPUNIT_ASSERT(quitButton->IsShownOnScreen());
	const wxRect clientRect(wxPoint(0, 0), frame.GetClientSize());
	CPPUNIT_ASSERT(clientRect.Contains(quitButton->GetRect().GetTopLeft()));
	CPPUNIT_ASSERT(clientRect.Contains(quitButton->GetRect().GetBottomRight()));
	assertFrameCenteredOnDisplay(&frame);

	bool localDialogWindowPresented = false;
	m_harness.runVoidFunctionWithAction([&]() {
		clickButton(&frame, localButton);
	}, [&]() {
		wxDialog * launchedDialog = m_harness.waitForModalDialog(200, 5);
		LocalGameDialog * localDialog = dynamic_cast<LocalGameDialog *>(launchedDialog);
		localDialogWindowPresented = (localDialog != NULL && localDialog->GetParent() == &frame);
		if (localDialog != NULL) {
			wxButton * loadButton = findButtonByLabel(localDialog, wxT("Load an Existing Game"));
			wxButton * customButton = findButtonByLabel(localDialog, wxT("Create a New Custom Game"));
			wxButton * predefinedButton = findButtonByLabel(localDialog, wxT("Play a Predefined Scenario"));
			wxButton * backButton = findButtonByLabel(localDialog, wxT("Back"));
			CPPUNIT_ASSERT(loadButton != NULL);
			CPPUNIT_ASSERT(customButton != NULL);
			CPPUNIT_ASSERT(predefinedButton != NULL);
			CPPUNIT_ASSERT(backButton != NULL);
			CPPUNIT_ASSERT(isChildFullyInClientArea(localDialog, loadButton));
			CPPUNIT_ASSERT(isChildFullyInClientArea(localDialog, customButton));
			CPPUNIT_ASSERT(isChildFullyInClientArea(localDialog, predefinedButton));
			CPPUNIT_ASSERT(isChildFullyInClientArea(localDialog, backButton));
		}
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
			if (scenarioDialog != NULL) {
				wxButton * playButton = findButtonByLabel(scenarioDialog, wxT("Play"));
				wxButton * doneButton = findButtonByLabel(scenarioDialog, wxT("Done"));
				CPPUNIT_ASSERT(playButton != NULL);
				CPPUNIT_ASSERT(doneButton != NULL);
				CPPUNIT_ASSERT(isChildFullyInClientArea(scenarioDialog, playButton));
				CPPUNIT_ASSERT(isChildFullyInClientArea(scenarioDialog, doneButton));
			}
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
			if (scenarioEditorDialog != NULL) {
				wxButton * cancelButton = findButtonByLabel(scenarioEditorDialog, wxT("Cancel"));
				wxButton * startButton = findButtonByLabel(scenarioEditorDialog, wxT("Start Battle!"));
				CPPUNIT_ASSERT(cancelButton != NULL);
				CPPUNIT_ASSERT(startButton != NULL);
				CPPUNIT_ASSERT(isChildFullyInClientArea(scenarioEditorDialog, cancelButton));
				CPPUNIT_ASSERT(isChildFullyInClientArea(scenarioEditorDialog, startButton));
			}
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
		CPPUNIT_ASSERT(dialog->usesStaticBoxParenting());
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
