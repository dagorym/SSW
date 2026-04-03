/**
 * @file FBattleScreen.cpp
 * @brief Implementation file for BattleScreen class
 * @author Tom Stephens
 * @date Created:  Jul 11, 2008
 *
 */

#include "tactical/FBattleScreen.h"
#include "Frontier.h"
#include "core/FGameConfig.h"
#include "gui/WX\
TacticalUI.h"
#include "tactical/ITacticalUI.h"
#include "tactical/FTacticalGame.h"
#include <wx/wx.h>

namespace Frontier {

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

FBattleScreen::FBattleScreen(const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( (wxDialog *)NULL, -1, title, pos, size, style )
//FBattleScreen::FBattleScreen(const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( (wxFrame *)NULL, -1, title, pos, size, style )
{
//	m_wd = new wxWindowDisabler(this);
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

	fgSizer1->Add( m_display, 1, wxEXPAND | wxALL, 1 );

	this->SetSizer( fgSizer1 );
	this->Layout();
	Centre();
	SetOwnBackgroundColour(black);

	m_tacticalGame = new FTacticalGame();
	m_tacticalUI = new WX\
TacticalUI(this);
	m_tacticalGame->installUI(m_tacticalUI);

	Bind(wxEVT_CLOSE_WINDOW, &FBattleScreen::onClose, this);

}

FBattleScreen::~FBattleScreen(){
	if (m_tacticalGame) {
		m_tacticalGame->installUI(NULL);
	}
	if (m_tacticalUI) { delete m_tacticalUI; }
	if (m_tacticalGame) { delete m_tacticalGame; }
//	delete m_wd;
//	this->MakeModal(false);
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
		wxImage p0(gc.getBasePath()+"icons/planet_01.png");
		iList.push_back(p0);
		wxImage p1(gc.getBasePath()+"icons/planet_02.png");
		iList.push_back(p1);
		wxImage p2(gc.getBasePath()+"icons/planet_03.png");
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
		m_tacticalGame->resetMovementState();
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

void FBattleScreen::completeMinePlacement() {
	m_tacticalGame->completeMinePlacement();
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

bool FBattleScreen::placeMineAtHex(const FPoint & hex) {
	const bool changed = m_tacticalGame->placeMineAtHex(hex);
	if (changed) {
		reDraw();
	}
	return changed;
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

const PointSet & FBattleScreen::getTargetHexes() const {
	return m_tacticalGame->getTargetHexes();
}

const PointSet & FBattleScreen::getHeadOnHexes() const {
	return m_tacticalGame->getHeadOnHexes();
}

const PointSet & FBattleScreen::getMinedHexes() const {
	return m_tacticalGame->getMinedHexes();
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
	if (m_tacticalGame->isCloseInProgress()) {
		return;
	}

	m_tacticalGame->setCloseInProgress(true);

	if (IsModal()) {
		EndModal(returnCode);
		return;
	}

	SetReturnCode(returnCode);
	Destroy();
}

void FBattleScreen::onClose(wxCloseEvent & event) {
	if (m_tacticalGame->isCloseInProgress()) {
		event.Skip();
		return;
	}

	closeBattleScreen(GetReturnCode());
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
