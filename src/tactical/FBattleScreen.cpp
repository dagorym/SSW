/**
 * @file FBattleScreen.cpp
 * @brief Implementation file for BattleScreen class
 * @author Tom Stephens, Claude Sonnet 4.6 (medium), gpt-5.4 (high), claude-sonnet-4-6 (standard), claude-opus-4-8 (medium)
 * @date Created:  Jul 11, 2008
 * @date Last Modified:  Jun 22, 2026
 *
 */

#include "tactical/FBattleScreen.h"
#include "Frontier.h"
#include "wxWidgets.h"
#include "core/FGameConfig.h"
#include <wx/evtloop.h>
#include <wx/modalhook.h>
#ifdef __WXGTK__
#include <gtk/gtk.h>
#endif
#include "gui/WXTacticalUI.h"
#include "tactical/ITacticalUI.h"
#include "tactical/FTacticalGame.h"

namespace Frontier {

namespace {
int g_battleScreenConstructedCount = 0;
int g_battleScreenDestroyedCount = 0;
int g_battleScreenLiveCount = 0;
const int BATTLE_SCREEN_BASE_DISPLAY_HEIGHT = 120;
const int BATTLE_SCREEN_MAP_MIN_PERCENT = 60;
}

struct FDestroyedShipCleanupLifecycle {
	const std::vector<unsigned int> * destroyedShipIDs;
	FVehicle * selectedShip;
	void (*clearSelectedShip)(void *);
	void (*reDraw)(void *);
	void (*clearDestroyedShipBookkeeping)(void *);
	bool (*hasWinner)(void *);
	void (*declareWinner)(void *);
	void * context;
};

void runDestroyedShipCleanupLifecycle(const FDestroyedShipCleanupLifecycle & lifecycle) {
	const std::vector<unsigned int> * destroyedShipIDs = lifecycle.destroyedShipIDs;
	if (destroyedShipIDs != NULL && !destroyedShipIDs->empty()) {
		FVehicle * selectedShip = lifecycle.selectedShip;
		if (selectedShip != NULL) {
			const unsigned int selectedShipID = selectedShip->getID();
			for (std::vector<unsigned int>::const_iterator itr = destroyedShipIDs->begin();
				itr != destroyedShipIDs->end();
				++itr) {
				if ((*itr) == selectedShipID) {
					if (lifecycle.clearSelectedShip != NULL) {
						lifecycle.clearSelectedShip(lifecycle.context);
					}
					break;
				}
			}
		}
		if (lifecycle.reDraw != NULL) {
			lifecycle.reDraw(lifecycle.context);
		}
	}
	if (lifecycle.clearDestroyedShipBookkeeping != NULL) {
		lifecycle.clearDestroyedShipBookkeeping(lifecycle.context);
	}
	if (lifecycle.hasWinner != NULL
		&& lifecycle.declareWinner != NULL
		&& lifecycle.hasWinner(lifecycle.context)) {
		lifecycle.declareWinner(lifecycle.context);
	}
}

namespace {

struct FDestroyedShipCleanupContext {
	FBattleScreen * screen;
	FTacticalGame * tacticalGame;
	void (FBattleScreen::*declareWinnerMethod)();
};

void clearSelectedShipForCleanup(void * context) {
	FDestroyedShipCleanupContext * cleanupContext = static_cast<FDestroyedShipCleanupContext *>(context);
	cleanupContext->screen->setShip(NULL);
}

void redrawForCleanup(void * context) {
	FDestroyedShipCleanupContext * cleanupContext = static_cast<FDestroyedShipCleanupContext *>(context);
	cleanupContext->screen->reDraw();
}

void clearDestroyedShipBookkeepingForCleanup(void * context) {
	FDestroyedShipCleanupContext * cleanupContext = static_cast<FDestroyedShipCleanupContext *>(context);
	cleanupContext->tacticalGame->clearLastDestroyedShipIDs();
}

bool hasWinnerForCleanup(void * context) {
	FDestroyedShipCleanupContext * cleanupContext = static_cast<FDestroyedShipCleanupContext *>(context);
	return cleanupContext->tacticalGame->hasWinner();
}

void declareWinnerForCleanup(void * context) {
	FDestroyedShipCleanupContext * cleanupContext = static_cast<FDestroyedShipCleanupContext *>(context);
	(cleanupContext->screen->*(cleanupContext->declareWinnerMethod))();
}

}

FBattleScreen::FBattleScreen(const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( (wxFrame *)NULL, -1, title, pos, size, style ),
	m_modalEventLoop(NULL),
	m_modalReturnCode(wxID_CANCEL),
	m_modalActive(false),
	m_closeInProgress(false)
{
	g_battleScreenConstructedCount++;
	g_battleScreenLiveCount++;
//	m_wd = new wxWindowDisabler(this);
	SetExtraStyle(GetExtraStyle() | wxTOPLEVEL_EX_DIALOG);
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
//	this->MakeModal(true);
	wxColour black(wxT("#000000"));// black

//	CreateStatusBar();
//	SetStatusText( "Welcome to the BattleScreen!" );


	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer1->AddGrowableCol( 0 );
	fgSizer1->AddGrowableRow( 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_map = new FBattleBoard( this );
	fgSizer1->Add( m_map, 5, wxEXPAND|wxALL, 1 );

	m_display = new FBattleDisplay( this );
	m_displayRequestedMinHeight = BATTLE_SCREEN_BASE_DISPLAY_HEIGHT;

	fgSizer1->Add( m_display, 1, wxEXPAND | wxALL, 1 );

	this->SetSizer( fgSizer1 );
	applyLayoutPolicy();
	Centre();
	SetOwnBackgroundColour(black);

	m_tacticalGame = new FTacticalGame();
	m_tacticalUI = new WX\
TacticalUI(this);
	m_tacticalGame->installUI(m_tacticalUI);

	wxMenu *menuFile = new wxMenu;
	menuFile->Append(ID_TacticalLoadGame, "&Load Game");
	menuFile->Append(ID_TacticalSaveGame, "&Save Game");
	menuFile->AppendSeparator();
	menuFile->Append(ID_TacticalQuit, "&Quit");
	menuFile->Enable(ID_TacticalLoadGame, false);
	menuFile->Enable(ID_TacticalSaveGame, false);

	wxMenu *menuSettings = new wxMenu;
	menuSettings->Append(ID_TacticalDamageDetails, "&Damage Details");
	menuSettings->Enable(ID_TacticalDamageDetails, false);

	wxMenu *menuHelp = new wxMenu;
	menuHelp->Append(ID_TacticalUsersGuide, "&User's Guide");
	menuHelp->Append(ID_TacticalAbout, "&About");
	menuHelp->Enable(ID_TacticalUsersGuide, false);
	menuHelp->Enable(ID_TacticalAbout, false);

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "&File");
	menuBar->Append(menuSettings, "&Settings");
	menuBar->Append(menuHelp, "&Help");
	SetMenuBar(menuBar);

	Bind(wxEVT_MENU, &FBattleScreen::onMenuQuit, this, ID_TacticalQuit);
	Bind(wxEVT_CLOSE_WINDOW, &FBattleScreen::onClose, this);
	Bind(wxEVT_SIZE, &FBattleScreen::onSize, this);

#ifdef __WXGTK__
	// wxWidgets' IsMenuEventAllowed() (src/gtk/menu.cpp) blocks menu events
	// from wxFrame windows whenever wxModalDialogHook::GetOpenCount() > 0,
	// i.e. whenever any wxDialog is running ShowModal().  FBattleScreen is a
	// wxFrame acting as a modal window launched from within SelectCombatGUI's
	// modal loop, so wx silently drops Quit menu events.
	//
	// Fix: connect an "activate" handler AFTER wx's own handler.  wx's handler
	// returns early without dispatching the event when a modal dialog is open.
	// Our after-handler detects that condition (GetOpenCount() > 0) and
	// dispatches the wxCommandEvent directly, bypassing IsMenuEventAllowed.
	{
		GtkWidget* gtkMenuBar = GTK_WIDGET(menuBar->GetHandle());
		GList* barItems = gtk_container_get_children(GTK_CONTAINER(gtkMenuBar));
		if (barItems) {
			GtkWidget* fileMenu = gtk_menu_item_get_submenu(GTK_MENU_ITEM(barItems->data));
			if (fileMenu) {
				GList* items = gtk_container_get_children(GTK_CONTAINER(fileMenu));
				// File menu: Load(0), Save(1), Separator(2), Quit(3)
				GList* quitNode = g_list_nth(items, 3);
				if (quitNode) {
					g_signal_connect_after(quitNode->data, "activate",
						G_CALLBACK(+[](GtkMenuItem*, void* data) {
							FBattleScreen* self = static_cast<FBattleScreen*>(data);
							// Only dispatch when wx's handler was blocked.
							// When GetOpenCount()==0, wx already dispatched normally.
							if (wxModalDialogHook::GetOpenCount() > 0) {
								wxCommandEvent evt(wxEVT_MENU, ID_TacticalQuit);
								evt.SetEventObject(self);
								self->HandleWindowEvent(evt);
							}
						}), this);
				}
				g_list_free(items);
			}
			g_list_free(barItems);
		}
	}
#endif

}

FBattleScreen::~FBattleScreen(){
	g_battleScreenDestroyedCount++;
	if (g_battleScreenLiveCount > 0) {
		g_battleScreenLiveCount--;
	}
	if (m_tacticalGame) {
		m_tacticalGame->installUI(NULL);
	}
	if (m_tacticalUI) { delete m_tacticalUI; }
	if (m_tacticalGame) { delete m_tacticalGame; }
//	delete m_wd;
//	this->MakeModal(false);
}

int FBattleScreen::ShowModal() {
	if (m_modalActive) {
		return m_modalReturnCode;
	}

	m_modalActive = true;
	m_modalReturnCode = wxID_CANCEL;
	m_closeInProgress = false;

	// NOTE: wxDialog::ShowModal() on GTK does NOT use wxWindowDisabler — it
	// relies purely on gtk_window_set_modal() for input restriction.  We
	// follow the same pattern: no wxWindowDisabler here.

	// Mirror wxDialog::ShowModal(): mark the window as modal before Show() so
	// GTK calls gtk_grab_add() during the show phase.
#ifdef __WXGTK__
	gtk_window_set_modal(GTK_WINDOW(m_widget), TRUE);
#endif

	Show(true);
	Raise();

	wxEventLoop modalEventLoop;
	m_modalEventLoop = &modalEventLoop;
	m_modalEventLoop->Run();
	m_modalEventLoop = NULL;

#ifdef __WXGTK__
	gtk_window_set_modal(GTK_WINDOW(m_widget), FALSE);
#endif

	return m_modalReturnCode;
}

void FBattleScreen::EndModal(int returnCode) {
	if (!m_modalActive) {
		return;
	}

	SetReturnCode(returnCode);
	m_modalActive = false;

	// Exit the event loop first (matching wxDialog::EndModal ordering), then
	// hide the window.  Hiding before exiting can trigger GTK unmap events
	// that re-enter the event dispatch while the loop is still marked running.
	if (m_modalEventLoop != NULL && m_modalEventLoop->IsRunning()) {
		m_modalEventLoop->Exit();
	}
	Show(false);
	m_closeInProgress = false;
}

bool FBattleScreen::IsModal() const {
	return m_modalActive;
}

void FBattleScreen::SetReturnCode(int returnCode) {
	m_modalReturnCode = returnCode;
}

int FBattleScreen::GetReturnCode() const {
	return m_modalReturnCode;
}

void FBattleScreen::resetLifecycleCounters() {
	g_battleScreenConstructedCount = 0;
	g_battleScreenDestroyedCount = 0;
	g_battleScreenLiveCount = 0;
}

int FBattleScreen::getConstructedCount() {
	return g_battleScreenConstructedCount;
}

int FBattleScreen::getDestroyedCount() {
	return g_battleScreenDestroyedCount;
}

int FBattleScreen::getLiveInstanceCount() {
	return g_battleScreenLiveCount;
}

void FBattleScreen::applyLayoutPolicy() {
	if (m_map == NULL || m_display == NULL) {
		return;
	}

	const int clientHeight = GetClientSize().GetHeight();
	if (clientHeight <= 0) {
		return;
	}

	const int mapMinHeight = (clientHeight * BATTLE_SCREEN_MAP_MIN_PERCENT) / 100;
	const int currentDisplayRequest = m_display->GetMinSize().GetHeight();
	m_displayRequestedMinHeight = currentDisplayRequest;
	if (m_displayRequestedMinHeight < BATTLE_SCREEN_BASE_DISPLAY_HEIGHT) {
		m_displayRequestedMinHeight = BATTLE_SCREEN_BASE_DISPLAY_HEIGHT;
	}

	int desiredDisplayHeight = m_displayRequestedMinHeight;
	if (desiredDisplayHeight < BATTLE_SCREEN_BASE_DISPLAY_HEIGHT) {
		desiredDisplayHeight = BATTLE_SCREEN_BASE_DISPLAY_HEIGHT;
	}

	const int maxDisplayHeight = clientHeight - mapMinHeight;
	if (maxDisplayHeight >= 0 && desiredDisplayHeight > maxDisplayHeight) {
		desiredDisplayHeight = maxDisplayHeight;
	}
	if (desiredDisplayHeight < 0) {
		desiredDisplayHeight = 0;
	}

	m_map->SetMinSize(wxSize(-1, mapMinHeight));
	m_display->SetMinSize(wxSize(-1, desiredDisplayHeight));
	Layout();
}

void FBattleScreen::onSize(wxSizeEvent & event) {
	if (m_display != NULL) {
		m_display->reflowLowerPanelLayout();
	}
	applyLayoutPolicy();
	event.Skip();
}

void FBattleScreen::draw(){
}
void FBattleScreen::onPaint(wxPaintEvent & event){
}

int FBattleScreen::setupFleets(FleetList *aList, FleetList *dList, bool planet, FVehicle * station){
	const int setupResult = m_tacticalGame->setupFleets(aList, dList, planet, station);
	if(planet){
		FGameConfig &gc = FGameConfig::create();
		ImageList iList;
		wxImage p0(gc.resolveAssetPath("icons/planet_01.png"));
		iList.push_back(p0);
		wxImage p1(gc.resolveAssetPath("icons/planet_02.png"));
		iList.push_back(p1);
		wxImage p2(gc.resolveAssetPath("icons/planet_03.png"));
		iList.push_back(p2);
		m_display->setImageList(iList);
		m_map->setPlanetImages(iList);
	}
	reDraw();
	return setupResult;
}

const int & FBattleScreen::getState() const {
	return m_tacticalGame->getState();
}

void FBattleScreen::toggleControlState() {
	m_tacticalGame->toggleControlState();
}

const bool & FBattleScreen::getControlState() const {
	return m_tacticalGame->getControlState();
}

void FBattleScreen::setPlanet(int c) {
	m_tacticalGame->setPlanet(c);
}

const int & FBattleScreen::getPlanetChoice() const {
	return m_tacticalGame->getPlanetChoice();
}

void FBattleScreen::setPlanetPosition(FPoint h) {
	m_tacticalGame->setPlanetPosition(h);
}

const FPoint & FBattleScreen::getPlanetPos() const {
	return m_tacticalGame->getPlanetPos();
}

void FBattleScreen::setStationPosition(FPoint h) {
	m_tacticalGame->setStationPosition(h);
}

const FPoint & FBattleScreen::getStationPos() const {
	return m_tacticalGame->getStationPos();
}

FVehicle * FBattleScreen::getStation() const {
	return m_tacticalGame->getStation();
}

void FBattleScreen::setState(int s) {
	m_tacticalGame->setState(s);
	m_map->Refresh();
	m_display->Refresh();
}

VehicleList FBattleScreen::getShipList() const{
	return m_tacticalGame->getShipList();
}

VehicleList FBattleScreen::getShipList(unsigned int id) const{
	return m_tacticalGame->getShipList(id);
}

void FBattleScreen::setScale(double factor) {
//	std::cerr<< "calling map setScale()" << std::endl;
	m_map->setScale(factor);
}

FVehicle * FBattleScreen::getShip() const {
	return m_tacticalGame->getShip();
}

void FBattleScreen::setShip(FVehicle * s) {
	m_tacticalGame->setShip(s);
}

bool FBattleScreen::getDone() {
	return m_tacticalGame->getDone();
}

void FBattleScreen::setDone(bool f) {
	m_tacticalGame->setDone(f);
}

bool FBattleScreen::getActivePlayer() {
	return m_tacticalGame->getActivePlayer();
}

void FBattleScreen::setActivePlayer(bool f) {
	m_tacticalGame->setActivePlayer(f);
}

void FBattleScreen::toggleActivePlayer() {
	m_tacticalGame->toggleActivePlayer();
}

int FBattleScreen::getPhase() {
	return m_tacticalGame->getPhase();
}

const unsigned int & FBattleScreen::getAttackerID() const {
	return m_tacticalGame->getAttackerID();
}

const unsigned int & FBattleScreen::getDefenderID() const {
	return m_tacticalGame->getDefenderID();
}

const unsigned int & FBattleScreen::getActivePlayerID() const {
	return m_tacticalGame->getActivePlayerID();
}

void FBattleScreen::setPhase(int p){
	m_tacticalGame->setPhase(p);
	if (p==PH_MOVE) { // we just ended a turn
		if (m_tacticalGame->getPhase() == PH_MOVE) {
			m_tacticalGame->resetMovementState();
		}
	}
	m_map->Refresh();
	m_display->Refresh();
}

void FBattleScreen::beginTacticalReport(const FTacticalCombatReportContext & context) {
	m_tacticalGame->beginTacticalReport(context);
}

void FBattleScreen::appendTacticalAttackReport(const FTacticalAttackReport & attack) {
	m_tacticalGame->appendTacticalAttackReport(attack);
}

void FBattleScreen::appendTacticalReportEvent(const FTacticalReportEvent & event) {
	m_tacticalGame->appendTacticalReportEvent(event);
}

FTacticalCombatReportSummary FBattleScreen::buildCurrentTacticalReportSummary() const {
	return m_tacticalGame->buildCurrentTacticalReportSummary();
}

int FBattleScreen::showTacticalDamageSummaryDialog(const FTacticalCombatReportSummary & summary) {
	ITacticalUI * tacticalUI = m_tacticalGame->getUI();
	if (tacticalUI != NULL) {
		return tacticalUI->showDamageSummary(summary);
	}
	return 1;
}

void FBattleScreen::clearTacticalReport() {
	m_tacticalGame->clearTacticalReport();
}

void FBattleScreen::setMoveComplete(bool s) {
	bool refresh = false;
	if (m_tacticalGame->isMoveComplete()!=s){
		refresh = true;
	}
	m_tacticalGame->setMoveComplete(s);
	if (refresh){
		m_map->Refresh();
		m_display->Refresh();
	}
}

bool FBattleScreen::isMoveComplete() const {
	return m_tacticalGame->isMoveComplete();
}

const unsigned int & FBattleScreen::getMovingPlayerID() const {
	return m_tacticalGame->getMovingPlayerID();
}

void FBattleScreen::toggleMovingPlayer() {
	m_tacticalGame->toggleMovingPlayer();
}

void FBattleScreen::setWeapon(FWeapon * w) {
//	if (w!=NULL) std::cerr << "Setting current weapon to " << w->getLongName() << std::endl;
	m_tacticalGame->setWeapon(w);
//	std::cerr << "computing weapon range" << std::endl;
	m_tacticalGame->computeWeaponRange();
//	std::cerr << "range computed." << std::endl;
}

FWeapon * FBattleScreen::getWeapon() {
	return m_tacticalGame->getWeapon();
}

bool FBattleScreen::selectWeapon(unsigned int weaponIndex) {
	const bool changed = m_tacticalGame->selectWeapon(weaponIndex);
	if (changed) {
		reDraw();
	}
	return changed;
}

bool FBattleScreen::selectDefense(unsigned int defenseIndex) {
	const bool changed = m_tacticalGame->selectDefense(defenseIndex);
	if (changed) {
		reDraw();
	}
	return changed;
}

bool FBattleScreen::selectShipFromHex(const FPoint & hex) {
	const bool changed = m_tacticalGame->selectShipFromHex(hex);
	if (changed) {
		reDraw();
	}
	return changed;
}

bool FBattleScreen::handleHexClick(const FPoint & hex) {
	const bool changed = m_tacticalGame->handleHexClick(hex);
	if (changed) {
		reDraw();
	}
	return changed;
}

bool FBattleScreen::placePlanet(const FPoint & hex) {
	const bool changed = m_tacticalGame->placePlanet(hex);
	if (changed) {
		reDraw();
	}
	return changed;
}

bool FBattleScreen::placeStation(const FPoint & hex) {
	const bool changed = m_tacticalGame->placeStation(hex);
	if (changed) {
		reDraw();
	}
	return changed;
}

bool FBattleScreen::placeShip(const FPoint & hex) {
	const bool changed = m_tacticalGame->placeShip(hex);
	if (changed) {
		reDraw();
	}
	return changed;
}

bool FBattleScreen::setShipPlacementHeading(int heading) {
	const bool changed = m_tacticalGame->setShipPlacementHeading(heading);
	if (changed) {
		reDraw();
	}
	return changed;
}

bool FBattleScreen::setShipPlacementHeadingByHex(const FPoint & hex) {
	const bool changed = m_tacticalGame->setShipPlacementHeadingByHex(hex);
	if (changed) {
		reDraw();
	}
	return changed;
}

bool FBattleScreen::beginMinePlacement() {
	const bool changed = m_tacticalGame->beginMinePlacement();
	if (changed) {
		reDraw();
	}
	return changed;
}

bool FBattleScreen::beginSeekerPlacement() {
	const bool changed = m_tacticalGame->beginSeekerPlacement();
	if (changed) {
		reDraw();
	}
	return changed;
}

bool FBattleScreen::beginOrdnancePlacement() {
	const bool changed = m_tacticalGame->beginOrdnancePlacement();
	if (changed) {
		reDraw();
	}
	return changed;
}

bool FBattleScreen::selectPlacementSource(unsigned int shipID, unsigned int weaponIndex) {
	const bool changed = m_tacticalGame->selectPlacementSource(shipID, weaponIndex);
	if (changed) {
		reDraw();
	}
	return changed;
}

bool FBattleScreen::selectPlacementSourceByIndex(unsigned int sourceIndex) {
	const bool changed = m_tacticalGame->selectPlacementSourceByIndex(sourceIndex);
	if (changed) {
		reDraw();
	}
	return changed;
}

int FBattleScreen::getSelectedPlacementSourceIndex() const {
	return m_tacticalGame->getSelectedPlacementSourceIndex();
}

const std::vector<FTacticalDeploymentSource> & FBattleScreen::getDeployablePlacementSources() const {
	return m_tacticalGame->getDeployablePlacementSources();
}

void FBattleScreen::completeMinePlacement() {
	m_tacticalGame->completeMinePlacement();
	reDraw();
}

void FBattleScreen::completeSeekerPlacement() {
	m_tacticalGame->completeSeekerPlacement();
	reDraw();
}

void FBattleScreen::completeMovePhase() {
	m_tacticalGame->completeMovePhase();
	reDraw();
}

FTacticalCombatReportSummary FBattleScreen::resolveCurrentFirePhase() {
	FTacticalCombatReportSummary summary = m_tacticalGame->resolveCurrentFirePhase();
	reDraw();
	return summary;
}

void FBattleScreen::completeDefensiveFirePhase() {
	m_tacticalGame->completeDefensiveFirePhase();
	reDraw();
}

void FBattleScreen::completeOffensiveFirePhase() {
	m_tacticalGame->completeOffensiveFirePhase();
	reDraw();
}

void FBattleScreen::computeWeaponRange() {
	m_tacticalGame->computeWeaponRange();
}

bool FBattleScreen::assignTargetFromHex(const FPoint & hex) {
	const bool changed = m_tacticalGame->assignTargetFromHex(hex);
	if (changed) {
		reDraw();
	}
	return changed;
}

bool FBattleScreen::placeOrdnanceAtHex(const FPoint & hex) {
	const bool changed = m_tacticalGame->placeOrdnanceAtHex(hex);
	if (changed) {
		reDraw();
	}
	return changed;
}

bool FBattleScreen::placeMineAtHex(const FPoint & hex) {
	const bool changed = m_tacticalGame->placeMineAtHex(hex);
	if (changed) {
		reDraw();
	}
	return changed;
}

bool FBattleScreen::isHexDeployable(const FPoint & hex) {
	return m_tacticalGame->isHexDeployable(hex);
}

bool FBattleScreen::isHexMinable(const FPoint & hex) {
	return m_tacticalGame->isHexMinable(hex);
}

const VehicleList & FBattleScreen::getHexOccupants(const FPoint & hex) const {
	return m_tacticalGame->getHexOccupants(hex);
}

const std::vector<FPoint> & FBattleScreen::getMovementHexes() const {
	return m_tacticalGame->getMovementHexes();
}

const std::vector<FPoint> & FBattleScreen::getLeftTurnHexes() const {
	return m_tacticalGame->getLeftTurnHexes();
}

const std::vector<FPoint> & FBattleScreen::getRightTurnHexes() const {
	return m_tacticalGame->getRightTurnHexes();
}

const std::vector<FTacticalMovePreviewRoute> & FBattleScreen::getStoppedShipPreviewRoutes() const {
	return m_tacticalGame->getStoppedShipPreviewRoutes();
}

const std::vector<int> & FBattleScreen::getStoppedShipPreviewHeadingsForHex(const FPoint & hex) const {
	return m_tacticalGame->getStoppedShipPreviewHeadingsForHex(hex);
}

const PointSet & FBattleScreen::getTargetHexes() const {
	return m_tacticalGame->getTargetHexes();
}

const PointSet & FBattleScreen::getHeadOnHexes() const {
	return m_tacticalGame->getHeadOnHexes();
}

const PointSet & FBattleScreen::getMinedHexes() const {
	return m_tacticalGame->getMinedHexes();
}

const PointSet & FBattleScreen::getLastTriggeredMineHexes() const {
	return m_tacticalGame->getLastTriggeredMineHexes();
}

const std::vector<FTacticalPlacedOrdnance> & FBattleScreen::getPlacedOrdnance() const {
	return m_tacticalGame->getPlacedOrdnance();
}

std::vector<FTacticalPlacedOrdnance> FBattleScreen::getPlacedOrdnanceAtHex(const FPoint & hex) const {
	return m_tacticalGame->getPlacedOrdnanceAtHex(hex);
}

const std::vector<FTacticalSeekerMissileState> & FBattleScreen::getSeekerMissiles() const {
	return m_tacticalGame->getSeekerMissiles();
}

std::vector<FTacticalSeekerMissileState> FBattleScreen::getSeekerMissilesAtHex(
	const FPoint & hex,
	bool activeOnly) const
{
	return m_tacticalGame->getSeekerMissilesAtHex(hex, activeOnly);
}

std::vector<FPoint> FBattleScreen::getInactiveSeekerActivationHexes() const {
	return m_tacticalGame->getInactiveSeekerActivationHexes();
}

bool FBattleScreen::selectSeekerActivationHex(const FPoint & hex) {
	const bool changed = m_tacticalGame->selectSeekerActivationHex(hex);
	if (changed) {
		reDraw();
	}
	return changed;
}

const FPoint & FBattleScreen::getSelectedSeekerActivationHex() const {
	return m_tacticalGame->getSelectedSeekerActivationHex();
}

std::vector<FTacticalSeekerMissileState> FBattleScreen::getSelectedInactiveSeekerActivationStack() const {
	return m_tacticalGame->getSelectedInactiveSeekerActivationStack();
}

bool FBattleScreen::isOffensiveSeekerDeploymentMode() const {
	return m_tacticalGame->isOffensiveSeekerDeploymentMode();
}

std::vector<FTacticalPendingSeekerHexGroup> FBattleScreen::getSelectedOffensivePendingSeekerHexGroups() const {
	return m_tacticalGame->getSelectedOffensivePendingSeekerHexGroups();
}

bool FBattleScreen::recallSelectedOffensivePendingSeekerAtHex(const FPoint & hex) {
	const bool changed = m_tacticalGame->recallSelectedOffensivePendingSeekerAtHex(hex);
	if (changed) {
		reDraw();
	}
	return changed;
}

std::vector<FTacticalPreGameSeekerHexGroup> FBattleScreen::getPlacedSeekerHexGroups() const {
	return m_tacticalGame->getPlacedSeekerHexGroups();
}

bool FBattleScreen::recallPlacedSeekerAtHexSource(const FPoint & hex, const FTacticalOrdnanceSource & source) {
	const bool changed = m_tacticalGame->recallPlacedSeekerAtHexSource(hex, source);
	if (changed) {
		reDraw();
	}
	return changed;
}

std::vector<FPoint> FBattleScreen::getAllPendingOffensiveFireSeekerHexes() const {
	return m_tacticalGame->getAllPendingOffensiveFireSeekerHexes();
}

bool FBattleScreen::activateSelectedInactiveSeeker(unsigned int seekerID) {
	const bool changed = m_tacticalGame->activateSelectedInactiveSeeker(seekerID);
	if (changed) {
		reDraw();
	}
	return changed;
}

bool FBattleScreen::deactivateActiveSeekerByID(unsigned int seekerID) {
	const bool changed = m_tacticalGame->deactivateActiveSeekerByID(seekerID);
	if (changed) {
		reDraw();
	}
	return changed;
}

bool FBattleScreen::activateInactiveSeekerAtHex(const FPoint & hex) {
	const bool changed = m_tacticalGame->activateInactiveSeekerAtHex(hex);
	if (changed) {
		reDraw();
	}
	return changed;
}

std::vector<FTacticalSeekerMissileState> FBattleScreen::getActiveSeekersByMovingPlayer() const {
	return m_tacticalGame->getActiveSeekersByMovingPlayer();
}

std::vector<FTacticalSeekerMissileState> FBattleScreen::getActiveSeekersByMovingPlayerThisPhase() const {
	return m_tacticalGame->getActiveSeekersByMovingPlayerThisPhase();
}

void FBattleScreen::completeSeekerActivationPhase() {
	m_tacticalGame->completeSeekerActivationPhase();
	if (!m_tacticalGame->getLastDestroyedShipIDs().empty()) {
		clearDestroyedShips();
		if (m_tacticalGame->hasWinner()) {
			return;
		}
	}
	reDraw();
}

const FHexMap & FBattleScreen::getMineTargets() const {
	return m_tacticalGame->getMineTargets();
}

unsigned int FBattleScreen::getMineOwner() const {
	return m_tacticalGame->getMineOwner();
}

const std::map<unsigned int, FTacticalTurnData> & FBattleScreen::getTurnInfo() const {
	return m_tacticalGame->getTurnInfo();
}

bool FBattleScreen::hasShipPlacementPendingRotation() const {
	return m_tacticalGame->hasShipPlacementPendingRotation();
}

const FPoint & FBattleScreen::getSelectedShipHex() const {
	return m_tacticalGame->getSelectedShipHex();
}

const VehicleList & FBattleScreen::getShipsWithMines() const {
	return m_tacticalGame->getShipsWithMines();
}

bool FBattleScreen::isHexInBounds(const FPoint & hex) const {
	return m_tacticalGame->isHexInBounds(hex);
}

bool FBattleScreen::isHexOccupied(const FPoint & hex) const {
	return m_tacticalGame->isHexOccupied(hex);
}

void FBattleScreen::clearDestroyedShips(){
	// Milestone 8 remediation contract:
	// 1) FTacticalGame::resolveCurrentFirePhase()/fireAllWeapons owns capture of
	//    m_lastDestroyedShipIDs and model-state ship removal.
	//    Legacy seam reference: const int liveShips = m_tacticalGame->clearDestroyedShips();
	// 2) FBattleScreen owns wx-side cleanup orchestration (map/display refresh and
	//    runtime view state updates) using the captured IDs.
	// 3) FTacticalGame bookkeeping is cleared exactly once here after wx-side
	//    cleanup consumes this lifecycle boundary.
	FDestroyedShipCleanupContext cleanupContext;
	cleanupContext.screen = this;
	cleanupContext.tacticalGame = m_tacticalGame;
	cleanupContext.declareWinnerMethod = &FBattleScreen::declareWinner;

	FDestroyedShipCleanupLifecycle lifecycle;
	lifecycle.destroyedShipIDs = &m_tacticalGame->getLastDestroyedShipIDs();
	lifecycle.selectedShip = getShip();
	lifecycle.clearSelectedShip = clearSelectedShipForCleanup;
	lifecycle.reDraw = redrawForCleanup;
	lifecycle.clearDestroyedShipBookkeeping = clearDestroyedShipBookkeepingForCleanup;
	lifecycle.hasWinner = hasWinnerForCleanup;
	lifecycle.declareWinner = declareWinnerForCleanup;
	lifecycle.context = &cleanupContext;

	runDestroyedShipCleanupLifecycle(lifecycle);
}

void FBattleScreen::declareWinner(){
	const bool attackerWins = m_tacticalGame->hasWinner()
		? (m_tacticalGame->getWinnerID() == m_tacticalGame->getAttackerID())
		: getActivePlayer();
	ITacticalUI * tacticalUI = m_tacticalGame->getUI();
	if (tacticalUI != NULL) {
		tacticalUI->notifyWinner(attackerWins);
	}
	closeBattleScreen(0);
}

void FBattleScreen::closeBattleScreen(int returnCode) {
	if (m_closeInProgress) {
		return;
	}

	m_closeInProgress = true;
	SetReturnCode(returnCode);

	if (IsModal()) {
		EndModal(returnCode);
		return;
	}

	Hide();
	Destroy();
	if (!IsBeingDeleted()) {
		m_closeInProgress = false;
	}
}

void FBattleScreen::onClose(wxCloseEvent & event) {
	(void)event;
	if (m_closeInProgress) {
		return;
	}

	closeBattleScreen(GetReturnCode());
}

void FBattleScreen::onMenuQuit(wxCommandEvent & WXUNUSED(event)) {
	Close(true);
}

void FBattleScreen::fireICM() {
	m_tacticalGame->fireICM();
}

void FBattleScreen::applyFireDamage(){
	m_tacticalGame->applyFireDamage();
}

const FTacticalCombatReport & FBattleScreen::getCurrentTacticalReport() const {
	return m_tacticalGame->getCurrentTacticalReport();
}

}
