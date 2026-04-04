/**
 * @file StrategicGuiLiveTest.cpp
 * @brief Implementation file for live strategic GUI smoke coverage
 */

#include "StrategicGuiLiveTest.h"

#include <algorithm>
#include <functional>

#include <wx/dcmemory.h>
#include <wx/filename.h>
#include <wx/panel.h>
#include <wx/toplevel.h>
#include <wx/window.h>

#include "FGamePanel.h"
#include "FMainFrame.h"
#include "core/FGameConfig.h"
#include "gui/BattleResultsGUI.h"
#include "gui/CombatFleetsGUI.h"
#include "gui/CombatLocationGUI.h"
#include "gui/SelectCombatGUI.h"
#include "gui/SatharFleetsGUI.h"
#include "gui/SatharRetreatGUI.h"
#include "gui/SelectJumpGUI.h"
#include "gui/SelectResolutionGUI.h"
#include "gui/SystemDialogGUI.h"
#include "gui/TransferShipsGUI.h"
#include "gui/TwoPlanetsGUI.h"
#include "gui/WXGameDisplay.h"
#include "gui/WXIconCache.h"
#include "gui/WXMapDisplay.h"
#include "gui/WXPlayerDisplay.h"
#include "gui/UPFUnattachedGUI.h"
#include "gui/ViewFleetGUI.h"
#include "gui/WXStrategicUI.h"
#include "ships/FVehicle.h"
#include "strategic/FFleet.h"
#include "strategic/FGame.h"
#include "strategic/FJumpRoute.h"
#include "strategic/FMap.h"
#include "strategic/FPlanet.h"
#include "strategic/FPlayer.h"
#include "strategic/FSystem.h"
#include "tactical/FBattleScreen.h"
#include "wxWidgets.h"

namespace FrontierTests {
using namespace Frontier;

namespace {

wxImage renderOffscreen(int width, int height, const std::function<void(wxMemoryDC &)> &drawFn) {
	wxBitmap bitmap(width, height);
	wxMemoryDC dc(bitmap);
	dc.SetBackground(wxBrush(*wxBLACK));
	dc.Clear();
	drawFn(dc);
	dc.SelectObject(wxNullBitmap);
	return bitmap.ConvertToImage();
}

int countNonBackgroundPixels(const wxImage &image, const wxColour &background) {
	if (!image.IsOk()) {
		return 0;
	}
	int changed = 0;
	for (int y = 0; y < image.GetHeight(); ++y) {
		for (int x = 0; x < image.GetWidth(); ++x) {
			if (image.GetRed(x, y) != background.Red()
			    || image.GetGreen(x, y) != background.Green()
			    || image.GetBlue(x, y) != background.Blue()) {
				changed++;
			}
		}
	}
	return changed;
}

bool hasNonBackgroundPixelInRegion(const wxImage &image,
                                   const wxColour &background,
                                   int centerX,
                                   int centerY,
                                   int halfWidth,
                                   int halfHeight) {
	if (!image.IsOk()) {
		return false;
	}
	const int minX = std::max(0, centerX - halfWidth);
	const int maxX = std::min(image.GetWidth() - 1, centerX + halfWidth);
	const int minY = std::max(0, centerY - halfHeight);
	const int maxY = std::min(image.GetHeight() - 1, centerY + halfHeight);
	for (int y = minY; y <= maxY; ++y) {
		for (int x = minX; x <= maxX; ++x) {
			if (image.GetRed(x, y) != background.Red()
			    || image.GetGreen(x, y) != background.Green()
			    || image.GetBlue(x, y) != background.Blue()) {
				return true;
			}
		}
	}
	return false;
}

FMap & ensureFrontierMap(unsigned int upfPlayerID, unsigned int satharPlayerID) {
std::vector<unsigned int> playerIDs;
playerIDs.push_back(upfPlayerID);
playerIDs.push_back(satharPlayerID);
return FMap::create(false, playerIDs);
}

bool systemContainsFleet(const FSystem * system, const FFleet * fleet) {
const FleetList & fleets = system->getFleetList();
for (FleetList::const_iterator itr = fleets.begin(); itr != fleets.end(); ++itr) {
if ((*itr)->getID() == fleet->getID()) {
return true;
}
}
return false;
}

class UPFUnattachedGUITestPeer : public UPFUnattachedGUI {
public:
UPFUnattachedGUITestPeer(wxWindow * parent, FPlayer * player, FMap * map)
: UPFUnattachedGUI(parent, player, map) {
}

void pickFleet(const wxString & fleetName) {
m_choice1->SetStringSelection(fleetName);
wxCommandEvent event(wxEVT_COMMAND_CHOICE_SELECTED, m_choice1->GetId());
onPickFleet(event);
}

void setFleetName(const std::string & fleetName) {
m_textCtrl1->SetValue(fleetName);
wxFocusEvent event(wxEVT_KILL_FOCUS, m_textCtrl1->GetId());
onStoreName(event);
}

void chooseSystem(const wxString & systemName) {
m_choice2->SetStringSelection(systemName);
wxCommandEvent event(wxEVT_COMMAND_CHOICE_SELECTED, m_choice2->GetId());
onSelectSystem(event);
}

void selectUnattachedShip(int index) {
m_listBox1->SetSelection(index);
wxMouseEvent event(wxEVT_LEFT_UP);
onSelectShip(event);
}

void addSelectedShips() {
wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, m_button1->GetId());
onAddShip(event);
}

void selectFleetShip(int index) {
m_listBox2->SetSelection(index);
wxMouseEvent event(wxEVT_LEFT_UP);
onCheckShip(event);
}

void removeSelectedShips() {
wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, m_button2->GetId());
onRemoveShip(event);
}

void updateFleet() {
wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, m_button3->GetId());
onUpdateFleet(event);
}

void setFleetThreshold(unsigned int threshold) {
m_setFleetCount = threshold;
}

bool doneEnabled() const {
return m_button4->IsEnabled();
}
};

class SatharFleetsGUITestPeer : public SatharFleetsGUI {
public:
SatharFleetsGUITestPeer(wxWindow * parent, FPlayer * player, FMap * map, bool setup)
: SatharFleetsGUI(parent, player, map, setup) {
}

void pickFleet(const wxString & fleetName) {
m_choice1->SetStringSelection(fleetName);
wxCommandEvent event(wxEVT_COMMAND_CHOICE_SELECTED, m_choice1->GetId());
onPickFleet(event);
}

void setFleetName(const std::string & fleetName) {
m_textCtrl1->SetValue(fleetName);
wxFocusEvent event(wxEVT_KILL_FOCUS, m_textCtrl1->GetId());
onStoreName(event);
}

void chooseSystem(const wxString & systemName) {
m_choice2->SetStringSelection(systemName);
wxCommandEvent event(wxEVT_COMMAND_CHOICE_SELECTED, m_choice2->GetId());
onSelectSystem(event);
}

void selectUnassignedShip(int index) {
m_listBox1->SetSelection(index);
wxMouseEvent event(wxEVT_LEFT_UP);
onSelectShip(event);
}

void addSelectedShips() {
wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, m_button1->GetId());
onAddShip(event);
}

void selectFleetShip(int index) {
m_listBox2->SetSelection(index);
wxMouseEvent event(wxEVT_LEFT_UP);
onCheckShip(event);
}

void removeSelectedShips() {
wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, m_button2->GetId());
onRemoveShip(event);
}

void updateFleet() {
wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, m_button3->GetId());
onUpdateFleet(event);
}

void setFleetThreshold(unsigned int threshold) {
m_setFleetCount = threshold;
}
};

class TransferShipsGUITestPeer : public TransferShipsGUI {
public:
TransferShipsGUITestPeer(FPlayer * player, FFleet * sourceFleet, FSystem * system, wxWindow * parent)
: TransferShipsGUI(player, sourceFleet, system, parent) {
}

void selectFleetByName(const wxString & fleetName) {
m_choice1->SetStringSelection(fleetName);
wxCommandEvent event(wxEVT_COMMAND_CHOICE_SELECTED, m_choice1->GetId());
onSelectFleet(event);
}

void selectFleetByIndex(int index) {
m_choice1->SetSelection(index);
wxCommandEvent event(wxEVT_COMMAND_CHOICE_SELECTED, m_choice1->GetId());
onSelectFleet(event);
}

void selectSourceShip(int index) {
m_listBox1->SetSelection(index);
wxMouseEvent event(wxEVT_LEFT_UP);
onSelectFleetOneShip(event);
}

void moveToSelectedFleet() {
wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, m_button1->GetId());
onAdd(event);
}

void selectSelectedFleetShip(int index) {
m_listBox2->SetSelection(index);
wxMouseEvent event(wxEVT_LEFT_UP);
onSelectFleetTwoShip(event);
}

void moveToOriginalFleet() {
wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, m_button2->GetId());
onRemove(event);
}

void clickDone() {
wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, m_button5->GetId());
onDone(event);
}
};

class CombatFleetsGUITestPeer : public CombatFleetsGUI {
public:
CombatFleetsGUITestPeer(wxWindow * parent, FSystem * system, FleetList fleets, wxArrayInt * selections, bool allowNoDefense)
: CombatFleetsGUI(parent, system, fleets, selections, allowNoDefense) {
}

void selectFleet(int index, bool selected = true) {
m_listBox1->SetSelection(index, selected);
}

void updateSelectionState() {
wxCommandEvent event(wxEVT_COMMAND_LISTBOX_SELECTED, m_listBox1->GetId());
onListSelect(event);
}

void clickOK() {
wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, m_button2->GetId());
onOK(event);
}
};

class CombatLocationGUITestPeer : public CombatLocationGUI {
public:
CombatLocationGUITestPeer(wxWindow * parent, FPlanet * planet)
: CombatLocationGUI(parent, planet) {
}

void clickAroundPlanet() {
wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, m_button1->GetId());
OnButton1(event);
}

void clickDeepSpace() {
wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, m_button2->GetId());
OnButton2(event);
}
};

class TwoPlanetsGUITestPeer : public TwoPlanetsGUI {
public:
TwoPlanetsGUITestPeer(wxWindow * parent, FSystem * system)
: TwoPlanetsGUI(parent, system) {
}

void clickPlanetOne() {
wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, m_button1->GetId());
OnButton1(event);
}

void clickPlanetTwo() {
wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, m_button2->GetId());
OnButton2(event);
}
};

class SelectResolutionGUITestPeer : public SelectResolutionGUI {
public:
SelectResolutionGUITestPeer(wxWindow * parent, FleetList fleets, const std::string & systemName, FVehicle * station)
: SelectResolutionGUI(parent, fleets, systemName, station) {
}

void clickBattleBoard() {
wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, m_button1->GetId());
onBattleBoard(event);
}

void clickManualResults() {
wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, m_button2->GetId());
onManualResults(event);
}
};

class BattleResultsGUITestPeer : public BattleResultsGUI {
public:
BattleResultsGUITestPeer(wxWindow * parent,
                        FleetList fleets,
                        FVehicle * station,
                        bool * stationStatus,
                        PlayerList * players)
: BattleResultsGUI(parent, fleets, station, stationStatus, players) {
}

void selectFleet(int index) {
m_choice1->SetSelection(index);
wxCommandEvent event(wxEVT_COMMAND_CHOICE_SELECTED, m_choice1->GetId());
onSelectFleet(event);
}

void selectShip(int index) {
m_listBox1->SetSelection(index);
wxCommandEvent event(wxEVT_COMMAND_LISTBOX_SELECTED, m_listBox1->GetId());
onSelectShip(event);
}

void setShipName(const wxString & value) {
m_textCtrl2->SetValue(value);
}

void setShipHP(const wxString & value) {
m_textCtrl8->SetValue(value);
}

void triggerFieldEdit() {
wxKeyEvent event(wxEVT_KEY_UP);
onKeyUp(event);
}

void applyShipUpdate() {
wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, m_button4->GetId());
onUpdateShip(event);
}

bool updateButtonEnabled() const {
return m_button4->IsEnabled();
}

void clickDone() {
wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, m_button1->GetId());
onDone(event);
}
};

class SelectCombatGUITestPeer : public SelectCombatGUI {
public:
SelectCombatGUITestPeer(wxWindow * parent,
                       FSystem * system,
                       FleetList defenders,
                       FleetList attackers,
                       PlayerList * players,
                       bool satharAttacking)
: SelectCombatGUI(parent, system, defenders, attackers, players, satharAttacking), m_finishCode(-1) {
}

int finishCode() const {
	return m_finishCode;
}

void selectAttackerFleet(int index) {
	m_listBox1->SetSelection(index, true);
	wxCommandEvent event(wxEVT_COMMAND_LISTBOX_SELECTED, m_listBox1->GetId());
	OnSelectLeftFleet(event);
}

void clickAttack() {
	wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, m_button3->GetId());
	onAttack(event);
}

protected:
virtual void finishDialog(int returnCode) wxOVERRIDE {
	m_finishCode = returnCode;
}

private:
int m_finishCode;
};

}

CPPUNIT_TEST_SUITE_REGISTRATION( StrategicGuiLiveTest );

void StrategicGuiLiveTest::setUp() {
	CPPUNIT_ASSERT(m_harness.bootstrap());
	FGameConfig::reset();
	FGameConfig::create();
	static bool handlersInitialized = false;
	if (!handlersInitialized) {
		wxInitAllImageHandlers();
		handlersInitialized = true;
	}
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

void StrategicGuiLiveTest::testWXMapDisplayOffscreenRendering() {
	FPlayer upfPlayer;
	FPlayer satharPlayer;
	FMap & map = ensureFrontierMap(upfPlayer.getID(), satharPlayer.getID());
	WXMapDisplay mapDisplay;

	const wxImage rendered = renderOffscreen(800, 800, [&](wxMemoryDC &dc) {
		mapDisplay.draw(dc);
	});

	const wxColour black(0, 0, 0);
	CPPUNIT_ASSERT(rendered.IsOk());
	CPPUNIT_ASSERT(countNonBackgroundPixels(rendered, black) > 500);

	const double scale = 800.0 / map.getMaxSize();
	FSystem * prenglar = map.getSystem("Prenglar");
	CPPUNIT_ASSERT(prenglar != NULL);
	const int prenglarX = static_cast<int>(prenglar->getCoord(0) * scale);
	const int prenglarY = static_cast<int>(prenglar->getCoord(1) * scale);
	CPPUNIT_ASSERT(hasNonBackgroundPixelInRegion(rendered, black, prenglarX, prenglarY, 15, 15));
	delete &map;
}

void StrategicGuiLiveTest::testWXPlayerDisplayOffscreenRendersSystemAndTransitFleets() {
	FPlayer player;
	FPlayer satharPlayer;
	FMap & map = ensureFrontierMap(player.getID(), satharPlayer.getID());

	player.setFleetIcon("icons/UPFFighter.png");

	FSystem * prenglar = map.getSystem("Prenglar");
	FSystem * whiteLight = map.getSystem("White Light");
	CPPUNIT_ASSERT(prenglar != NULL);
	CPPUNIT_ASSERT(whiteLight != NULL);

	FFleet * systemFleet = new FFleet;
	systemFleet->setName("System Fleet");
	systemFleet->setOwner(player.getID());
	systemFleet->setIcon(player.getFleetIconName());
	systemFleet->addShip(createShip("AssaultScout"));
	systemFleet->setLocation(prenglar, false);
	player.addFleet(systemFleet);

	FFleet * transitFleet = new FFleet;
	transitFleet->setName("Transit Fleet");
	transitFleet->setOwner(player.getID());
	transitFleet->setIcon(player.getFleetIconName());
	transitFleet->addShip(createShip("AssaultScout"));
	transitFleet->setLocation(whiteLight, true, 4, FFleet::NO_DESTINATION, 1, FFleet::NO_ROUTE);
	player.addFleet(transitFleet);

	WXPlayerDisplay playerDisplay;
	WXMapDisplay mapDisplay;
	const wxImage rendered = renderOffscreen(800, 800, [&](wxMemoryDC &dc) {
		playerDisplay.drawFleets(dc, &player);
	});

	const wxColour black(0, 0, 0);
	CPPUNIT_ASSERT(rendered.IsOk());
	CPPUNIT_ASSERT(countNonBackgroundPixels(rendered, black) > 50);

	const double scale = 800.0 / map.getMaxSize();
	double xoffset = -0.2 * scale;
	double yoffset = -0.2 * scale;
	if (player.getID() != 1) {
		xoffset = 0.2 * scale;
		yoffset = -0.2 * scale;
	}

	const int systemX = static_cast<int>(prenglar->getCoord(0) * scale - 0.5 * scale + xoffset);
	const int systemY = static_cast<int>(prenglar->getCoord(1) * scale - 0.5 * scale + yoffset);
	CPPUNIT_ASSERT(hasNonBackgroundPixelInRegion(rendered, black, systemX, systemY, 30, 30));

	const int transitX = static_cast<int>((whiteLight->getCoord(0) - 0.5) * scale);
	const int transitY = static_cast<int>((whiteLight->getCoord(1) - 0.5) * scale);
	CPPUNIT_ASSERT(hasNonBackgroundPixelInRegion(rendered, black, transitX, transitY, 30, 30));
	delete &map;
}

void StrategicGuiLiveTest::testWXGameDisplayOffscreenRendersTurnCounterAndIcons() {
	FPlayer upfPlayer;
	FPlayer satharPlayer;
	FMap & map = ensureFrontierMap(upfPlayer.getID(), satharPlayer.getID());
	FGame * game = &(FGame::create());

	WXGameDisplay gameDisplay;
	const wxImage rendered = renderOffscreen(800, 800, [&](wxMemoryDC &dc) {
		gameDisplay.draw(dc, *game);
	});
	delete game;

	const wxColour black(0, 0, 0);
	CPPUNIT_ASSERT(rendered.IsOk());
	CPPUNIT_ASSERT(countNonBackgroundPixels(rendered, black) > 500);
	CPPUNIT_ASSERT(WXIconCache::instance().get("icons/day.png").IsOk());
	CPPUNIT_ASSERT(WXIconCache::instance().get("icons/tenday.png").IsOk());

	const int s = std::min(800, 800) / 20;
	CPPUNIT_ASSERT(hasNonBackgroundPixelInRegion(rendered, black, static_cast<int>(0.5 * s), static_cast<int>(0.5 * s), 30, 30));
	CPPUNIT_ASSERT(hasNonBackgroundPixelInRegion(rendered, black, static_cast<int>(2.0 * s), static_cast<int>(2.5 * s), 40, 20));
	delete &map;
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
FMap & map = ensureFrontierMap(player.getID(), player.getID() + 1);
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
		sawPaint = true;
		event.Skip();
	});
parent->Show();
redrawPanel->Show();
m_harness.pumpEvents();

FPlayer player;
player.setName("GUI Test Player");
FMap & map = ensureFrontierMap(player.getID(), player.getID() + 1);
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

void StrategicGuiLiveTest::testUPFAndSatharFleetDialogsMutateModelState() {
wxFrame * parent = new wxFrame(NULL, wxID_ANY, "Fleet Dialog Parent", wxDefaultPosition, wxSize(640, 480));
parent->Show();
m_harness.pumpEvents();

	FPlayer * upfPlayer = new FPlayer;
	upfPlayer->setName("UPF Test Player");
	upfPlayer->setFleetIcon("icons/UPF_fleet.png");
	FPlayer * satharPlayer = new FPlayer;
	satharPlayer->setName("Sathar Test Player");
	satharPlayer->setFleetIcon("icons/Sathar_fleet.png");
	FMap & map = ensureFrontierMap(upfPlayer->getID(), satharPlayer->getID());
FSystem * upfSystem = map.getSystem("Prenglar");
FSystem * satharSystem = map.getSystem("Sathar Start Circle 1");
CPPUNIT_ASSERT(upfSystem != NULL);
CPPUNIT_ASSERT(satharSystem != NULL);

	upfPlayer->addShip(createShip("AssaultScout"));
	UPFUnattachedGUITestPeer upfDialog(parent, upfPlayer, &map);
upfDialog.pickFleet("New Fleet");
upfDialog.setFleetThreshold(0);
upfDialog.setFleetName("UPF Test Fleet");
upfDialog.chooseSystem(upfSystem->getName());
upfDialog.selectUnattachedShip(0);
upfDialog.addSelectedShips();
upfDialog.selectFleetShip(0);
upfDialog.removeSelectedShips();
upfDialog.selectUnattachedShip(0);
upfDialog.addSelectedShips();
upfDialog.updateFleet();

	FFleet * upfFleet = upfPlayer->getFleet("UPF Test Fleet");
	CPPUNIT_ASSERT(upfFleet != NULL);
	CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), upfFleet->getShipCount());
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), upfPlayer->getShipList().size());
CPPUNIT_ASSERT(upfDialog.doneEnabled());
CPPUNIT_ASSERT(systemContainsFleet(upfSystem, upfFleet));

	satharPlayer->addShip(createShip("Frigate"));
	SatharFleetsGUITestPeer satharDialog(parent, satharPlayer, &map, false);
satharDialog.pickFleet("New Fleet");
satharDialog.setFleetThreshold(0);
satharDialog.setFleetName("Sathar Test Fleet");
satharDialog.chooseSystem(satharSystem->getName());
satharDialog.selectUnassignedShip(0);
satharDialog.addSelectedShips();
satharDialog.selectFleetShip(0);
satharDialog.removeSelectedShips();
satharDialog.selectUnassignedShip(0);
satharDialog.addSelectedShips();
satharDialog.updateFleet();

	FFleet * satharFleet = satharPlayer->getFleet("Sathar Test Fleet");
	CPPUNIT_ASSERT(satharFleet != NULL);
	CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), satharFleet->getShipCount());
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), satharPlayer->getShipList().size());
CPPUNIT_ASSERT(systemContainsFleet(satharSystem, satharFleet));

parent->Destroy();
m_harness.pumpEvents(10);
}

void StrategicGuiLiveTest::testTransferShipsDialogMovesShipsAndUpdatesFleets() {
wxFrame * parent = new wxFrame(NULL, wxID_ANY, "Transfer Dialog Parent", wxDefaultPosition, wxSize(640, 480));
parent->Show();
m_harness.pumpEvents();

	FPlayer * player = new FPlayer;
	player->setName("Transfer Test Player");
	player->setFleetIcon("icons/UPF_fleet.png");
	FMap & map = ensureFrontierMap(player->getID(), player->getID() + 1);
FSystem * system = map.getSystem("Prenglar");
CPPUNIT_ASSERT(system != NULL);

FFleet * sourceFleet = new FFleet;
sourceFleet->setName("Source Fleet");
	sourceFleet->setOwner(player->getID());
sourceFleet->setLocation(system, false);
FVehicle * movedShip = createShip("AssaultScout");
sourceFleet->addShip(movedShip);
sourceFleet->addShip(createShip("Frigate"));

FFleet * targetFleet = new FFleet;
targetFleet->setName("Target Fleet");
	targetFleet->setOwner(player->getID());
targetFleet->setLocation(system, false);
targetFleet->addShip(createShip("Destroyer"));

	player->addFleet(sourceFleet);
	player->addFleet(targetFleet);
	system->addFleet(sourceFleet);
	system->addFleet(targetFleet);

	TransferShipsGUITestPeer * transferDialog = new TransferShipsGUITestPeer(player, sourceFleet, system, parent);
	transferDialog->selectFleetByIndex(2);
	transferDialog->selectSourceShip(0);
	transferDialog->moveToSelectedFleet();
	transferDialog->selectSelectedFleetShip(1);
	transferDialog->moveToOriginalFleet();
	transferDialog->selectSourceShip(1);
	transferDialog->moveToSelectedFleet();
	transferDialog->clickDone();

CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), sourceFleet->getShipCount());
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(2), targetFleet->getShipCount());
CPPUNIT_ASSERT(targetFleet->getShip(movedShip->getID()) != NULL);

parent->Destroy();
m_harness.pumpEvents(10);
}

void StrategicGuiLiveTest::testCombatSelectionDialogsReturnCodesAndState() {
wxFrame * parent = new wxFrame(NULL, wxID_ANY, "Combat Dialog Parent", wxDefaultPosition, wxSize(640, 480));
parent->Show();
m_harness.pumpEvents();

FPlayer player;
FMap & map = ensureFrontierMap(player.getID(), player.getID() + 1);
FSystem * system = map.getSystem("Prenglar");
CPPUNIT_ASSERT(system != NULL);

FFleet attacker;
attacker.setName("Attacker Fleet");
FFleet defender;
defender.setName("Defender Fleet");
FleetList combatFleets;
combatFleets.push_back(&attacker);
combatFleets.push_back(&defender);
wxArrayInt selections;

CombatFleetsGUITestPeer combatDialog(parent, system, combatFleets, &selections, false);
const int combatResult = m_harness.showModalWithAction(combatDialog, [&]() {
combatDialog.selectFleet(0, true);
combatDialog.selectFleet(1, true);
combatDialog.updateSelectionState();
combatDialog.clickOK();
}, wxID_CANCEL, 200);
CPPUNIT_ASSERT_EQUAL(0, combatResult);
CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), static_cast<size_t>(selections.GetCount()));

FPlanet planet("Test Planet");
CombatLocationGUITestPeer locationDialogOne(parent, &planet);
CPPUNIT_ASSERT_EQUAL(0, m_harness.showModalWithAction(locationDialogOne, [&]() {
locationDialogOne.clickAroundPlanet();
}, wxID_CANCEL, 200));

CombatLocationGUITestPeer locationDialogTwo(parent, &planet);
CPPUNIT_ASSERT_EQUAL(1, m_harness.showModalWithAction(locationDialogTwo, [&]() {
locationDialogTwo.clickDeepSpace();
}, wxID_CANCEL, 200));

FSystem dualPlanetSystem("Dual Planet System", 0.0f, 0.0f, 0.0f, player.getID());
dualPlanetSystem.addPlanet(new FPlanet("Alpha"));
dualPlanetSystem.addPlanet(new FPlanet("Beta"));
TwoPlanetsGUITestPeer twoPlanetsDialogOne(parent, &dualPlanetSystem);
CPPUNIT_ASSERT_EQUAL(1, m_harness.showModalWithAction(twoPlanetsDialogOne, [&]() {
twoPlanetsDialogOne.clickPlanetOne();
}, wxID_CANCEL, 200));

TwoPlanetsGUITestPeer twoPlanetsDialogTwo(parent, &dualPlanetSystem);
CPPUNIT_ASSERT_EQUAL(2, m_harness.showModalWithAction(twoPlanetsDialogTwo, [&]() {
twoPlanetsDialogTwo.clickPlanetTwo();
}, wxID_CANCEL, 200));

FleetList resolutionFleets;
resolutionFleets.push_back(&attacker);
SelectResolutionGUITestPeer resolutionDialogOne(parent, resolutionFleets, "Prenglar", NULL);
CPPUNIT_ASSERT_EQUAL(0, m_harness.showModalWithAction(resolutionDialogOne, [&]() {
resolutionDialogOne.clickBattleBoard();
}, wxID_CANCEL, 200));

SelectResolutionGUITestPeer resolutionDialogTwo(parent, resolutionFleets, "Prenglar", NULL);
CPPUNIT_ASSERT_EQUAL(1, m_harness.showModalWithAction(resolutionDialogTwo, [&]() {
resolutionDialogTwo.clickManualResults();
}, wxID_CANCEL, 200));

parent->Destroy();
m_harness.pumpEvents(10);
}

void StrategicGuiLiveTest::testBattleResultsDialogUpdatesShipStatistics() {
wxFrame * parent = new wxFrame(NULL, wxID_ANY, "Battle Results Parent", wxDefaultPosition, wxSize(640, 480));
parent->Show();
m_harness.pumpEvents();

FFleet fleet;
fleet.setName("Battle Fleet");
FVehicle * ship = createShip("AssaultScout");
ship->setHP(10);
fleet.addShip(ship);
FleetList fleets;
fleets.push_back(&fleet);
bool stationAlive = true;
PlayerList players;

BattleResultsGUITestPeer battleDialog(parent, fleets, NULL, &stationAlive, &players);
battleDialog.selectFleet(1);
battleDialog.selectShip(0);
battleDialog.setShipName("Updated Scout");
battleDialog.setShipHP("5");
battleDialog.triggerFieldEdit();
CPPUNIT_ASSERT(battleDialog.updateButtonEnabled());
battleDialog.applyShipUpdate();
CPPUNIT_ASSERT_EQUAL(std::string("Updated Scout"), ship->getName());
CPPUNIT_ASSERT_EQUAL(5, ship->getHP());
CPPUNIT_ASSERT(!battleDialog.updateButtonEnabled());

BattleResultsGUITestPeer doneDialog(parent, fleets, NULL, &stationAlive, &players);
CPPUNIT_ASSERT_EQUAL(0, m_harness.showModalWithAction(doneDialog, [&]() {
doneDialog.clickDone();
}, wxID_CANCEL, 200));

	parent->Destroy();
	m_harness.pumpEvents(10);
}

void StrategicGuiLiveTest::testSelectCombatLaunchesBattleScreenAndCleansUpLifetime() {
	const wxString originalCwd = wxGetCwd();
	if (!wxFileName::DirExists(wxT("icons")) && wxFileName::DirExists(wxT("../../icons"))) {
		CPPUNIT_ASSERT(wxSetWorkingDirectory(wxT("../../")));
	}
	FGameConfig::reset();
	FGameConfig::create();

	wxFrame * parent = new wxFrame(NULL, wxID_ANY, "SelectCombat Parent", wxDefaultPosition, wxSize(720, 520));
	parent->Show();
	m_harness.pumpEvents();

	FPlayer * attackerPlayer = new FPlayer;
	FPlayer * defenderPlayer = new FPlayer;
	attackerPlayer->setName("UPF");
	defenderPlayer->setName("Sathar");
	FMap & map = ensureFrontierMap(attackerPlayer->getID(), defenderPlayer->getID());
	FSystem * system = map.getSystem("Prenglar");
	CPPUNIT_ASSERT(system != NULL);

	FFleet * attackerFleet = new FFleet;
	attackerFleet->setName("Attacker Fleet");
	attackerFleet->setIcon("icons/UPF.png");
	attackerFleet->setOwner(attackerPlayer->getID());
	attackerFleet->setLocation(system, false);
	attackerFleet->addShip(createShip("Destroyer"));
	attackerPlayer->addFleet(attackerFleet);
	system->addFleet(attackerFleet);

	FFleet * defenderFleet = new FFleet;
	defenderFleet->setName("Defender Fleet");
	defenderFleet->setIcon("icons/Sathar.png");
	defenderFleet->setOwner(defenderPlayer->getID());
	defenderFleet->setLocation(system, false);
	defenderFleet->addShip(createShip("Frigate"));
	defenderPlayer->addFleet(defenderFleet);
	system->addFleet(defenderFleet);

	FleetList attackers;
	attackers.push_back(attackerFleet);
	FleetList defenders;
	defenders.push_back(defenderFleet);
	PlayerList players;
	players.push_back(attackerPlayer);
	players.push_back(defenderPlayer);

	SelectCombatGUITestPeer dialog(parent, system, defenders, attackers, &players, false);
	FBattleScreen::resetLifecycleCounters();
	m_harness.runVoidFunctionWithAutoDismiss([&]() {
		dialog.selectAttackerFleet(0);
		dialog.clickAttack();
	}, 0, 200);
	CPPUNIT_ASSERT_EQUAL(1, dialog.finishCode());
	CPPUNIT_ASSERT(FBattleScreen::getConstructedCount() >= 1);
	CPPUNIT_ASSERT_EQUAL(FBattleScreen::getConstructedCount(), FBattleScreen::getDestroyedCount());
	CPPUNIT_ASSERT_EQUAL(0, FBattleScreen::getLiveInstanceCount());

	parent->Destroy();
	m_harness.pumpEvents(10);
	if (!originalCwd.IsEmpty()) {
		wxSetWorkingDirectory(originalCwd);
	}
	FGameConfig::reset();
	FGameConfig::create();
}

}
