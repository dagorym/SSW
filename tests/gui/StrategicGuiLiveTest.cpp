/**
 * @file StrategicGuiLiveTest.cpp
 * @brief Implementation file for live strategic GUI smoke coverage
 */

#include "StrategicGuiLiveTest.h"

#include <wx/panel.h>
#include <wx/toplevel.h>
#include <wx/window.h>

#include "FGamePanel.h"
#include "FMainFrame.h"
#include "gui/SatharRetreatGUI.h"
#include "gui/SelectJumpGUI.h"
#include "gui/SystemDialogGUI.h"
#include "gui/ViewFleetGUI.h"
#include "gui/WXStrategicUI.h"
#include "ships/FVehicle.h"
#include "strategic/FFleet.h"
#include "strategic/FMap.h"
#include "strategic/FPlayer.h"
#include "strategic/FSystem.h"
#include "wxWidgets.h"

namespace FrontierTests {
using namespace Frontier;

CPPUNIT_TEST_SUITE_REGISTRATION( StrategicGuiLiveTest );

void StrategicGuiLiveTest::setUp() {
	CPPUNIT_ASSERT(m_harness.bootstrap());
}

void StrategicGuiLiveTest::tearDown() {
}

void StrategicGuiLiveTest::testMainFrameBuildsExpectedInitialUI() {
	FMainFrame * frame = new FMainFrame("FMainFrame Test", wxDefaultPosition, wxSize(800, 600));
	frame->Show();
	m_harness.pumpEvents();

	wxWindow * mapPanel = frame->FindWindow("MapPanel");
	CPPUNIT_ASSERT(mapPanel != NULL);
	CPPUNIT_ASSERT(dynamic_cast<FGamePanel *>(mapPanel) != NULL);

	wxMenuBar * menuBar = frame->GetMenuBar();
	CPPUNIT_ASSERT(menuBar != NULL);

	wxMenuItem * saveItem = menuBar->FindItem(ID_Save);
	wxMenuItem * closeItem = menuBar->FindItem(ID_Close);
	wxMenuItem * retreatItem = menuBar->FindItem(ID_ShowRetreatCond);
	wxMenuItem * endSatharItem = menuBar->FindItem(ID_EndSatharTurn);
	wxMenuItem * endUPFItem = menuBar->FindItem(ID_EndUPFTurn);
	wxMenuItem * placeNovaItem = menuBar->FindItem(ID_PlaceNova);
	wxMenuItem * addSatharItem = menuBar->FindItem(ID_AddSatharShips);

	CPPUNIT_ASSERT(saveItem != NULL);
	CPPUNIT_ASSERT(closeItem != NULL);
	CPPUNIT_ASSERT(retreatItem != NULL);
	CPPUNIT_ASSERT(endSatharItem != NULL);
	CPPUNIT_ASSERT(endUPFItem != NULL);
	CPPUNIT_ASSERT(placeNovaItem != NULL);
	CPPUNIT_ASSERT(addSatharItem != NULL);

	CPPUNIT_ASSERT(!saveItem->IsEnabled());
	CPPUNIT_ASSERT(!closeItem->IsEnabled());
	CPPUNIT_ASSERT(!retreatItem->IsEnabled());
	CPPUNIT_ASSERT(!endSatharItem->IsEnabled());
	CPPUNIT_ASSERT(!endUPFItem->IsEnabled());
	CPPUNIT_ASSERT(!placeNovaItem->IsEnabled());
	CPPUNIT_ASSERT(!addSatharItem->IsEnabled());

	frame->Destroy();
	m_harness.pumpEvents(10);
}

void StrategicGuiLiveTest::testGamePanelPaintTracksParentSize() {
	wxFrame * frame = new wxFrame(NULL, wxID_ANY, "FGamePanel Parent", wxDefaultPosition, wxSize(640, 480));
	FGamePanel * panel = new FGamePanel(frame);
	frame->Show();
	m_harness.pumpEvents();

	frame->SetClientSize(480, 320);
	panel->Refresh();
	panel->Update();
	m_harness.pumpEvents(2);

	const wxSize parentSize = frame->GetClientSize();
	const wxSize panelSize = panel->GetClientSize();
	CPPUNIT_ASSERT_EQUAL(parentSize.GetWidth(), panelSize.GetWidth());
	CPPUNIT_ASSERT_EQUAL(parentSize.GetHeight(), panelSize.GetHeight());

	frame->Destroy();
	m_harness.pumpEvents(10);
}

void StrategicGuiLiveTest::testStrategicDialogsCloseModallyWithoutInput() {
	wxFrame * parent = new wxFrame(NULL, wxID_ANY, "Dialog Parent", wxDefaultPosition, wxSize(500, 400));
	parent->Show();
	m_harness.pumpEvents();

	FPlayer player;
	player.setName("GUI Test Player");
	FMap & map = FMap::create();
	FSystem system("Prenglar", 0.0f, 0.0f, 0.0f, player.getID());
	FSystem destination("White Light", 1.0f, 0.0f, 0.0f, player.getID());
	FFleet fleet;
	fleet.setName("Test Fleet");
	fleet.setOwner(player.getID());
	fleet.addShip(createShip("AssaultScout"));

	SatharRetreatGUI retreatDialog(parent);
	CPPUNIT_ASSERT_EQUAL(static_cast<int>(wxID_CANCEL),
	                     m_harness.showModalWithAutoDismiss(retreatDialog, wxID_CANCEL, 25));

	SystemDialogGUI systemDialog(parent, &system, &map, &player, "System Information");
	CPPUNIT_ASSERT_EQUAL(static_cast<int>(wxID_CANCEL),
	                     m_harness.showModalWithAutoDismiss(systemDialog, wxID_CANCEL, 25));

	ViewFleetGUI fleetDialog(parent, &fleet, &system, &destination);
	CPPUNIT_ASSERT_EQUAL(static_cast<int>(wxID_CANCEL),
	                     m_harness.showModalWithAutoDismiss(fleetDialog, wxID_CANCEL, 25));

	SelectJumpGUI jumpDialog(parent, &fleet, &map, system.getName(), player.getID());
	CPPUNIT_ASSERT_EQUAL(static_cast<int>(wxID_CANCEL),
	                     m_harness.showModalWithAutoDismiss(jumpDialog, wxID_CANCEL, 25));

	parent->Destroy();
	m_harness.pumpEvents(10);
}

void StrategicGuiLiveTest::testWXStrategicUIParentBackedModalAndRedrawPaths() {
	wxFrame * parent = new wxFrame(NULL, wxID_ANY, "WXStrategicUI Parent", wxDefaultPosition, wxSize(500, 400));
	wxPanel * redrawPanel = new wxPanel(parent, wxID_ANY);
	bool sawPaint = false;
	redrawPanel->Bind(wxEVT_PAINT, [&](wxPaintEvent & event) {
		wxPaintDC dc(redrawPanel);
		sawPaint = true;
		event.Skip();
	});
	parent->Show();
	redrawPanel->Show();
	m_harness.pumpEvents();

	FPlayer player;
	player.setName("GUI Test Player");
	FMap & map = FMap::create();
	FSystem system("Prenglar", 0.0f, 0.0f, 0.0f, player.getID());
	FSystem destination("White Light", 1.0f, 0.0f, 0.0f, player.getID());
	FFleet fleet;
	fleet.setName("Test Fleet");
	fleet.setOwner(player.getID());
	fleet.addShip(createShip("AssaultScout"));

	WXStrategicUI ui(redrawPanel);

	const int retreatResult = m_harness.runModalFunctionWithAutoDismiss(
		[&]() { return ui.selectRetreatCondition(); }, wxID_CANCEL, 25);
	CPPUNIT_ASSERT_EQUAL(static_cast<int>(wxID_CANCEL), retreatResult);

	m_harness.runVoidFunctionWithAutoDismiss(
		[&]() { ui.showSystemDialog(&system, &map, &player); }, wxID_CANCEL, 25);
	m_harness.runVoidFunctionWithAutoDismiss(
		[&]() { ui.showFleetDialog(&fleet, &system, &destination); }, wxID_CANCEL, 25);
	m_harness.runVoidFunctionWithAutoDismiss(
		[&]() { ui.showMessage("Smoke Message", "Parent-backed strategic message"); }, wxID_OK, 25);
	m_harness.runVoidFunctionWithAutoDismiss(
		[&]() { ui.notifyFailedJump("Smoke Fleet"); }, wxID_OK, 25);
	m_harness.runVoidFunctionWithAutoDismiss(
		[&]() { ui.notifyVictory(1); }, wxID_OK, 25);
	m_harness.runVoidFunctionWithAutoDismiss(
		[&]() { ui.showRetreatConditions("Retreat when conditions are met."); }, wxID_OK, 25);

	ui.requestRedraw();
	redrawPanel->Update();
	m_harness.pumpEvents(5);
	CPPUNIT_ASSERT(sawPaint);

	parent->Destroy();
	m_harness.pumpEvents(10);
}

}
