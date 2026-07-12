/**
 * @file FGameTest.cpp
 * @brief Implementation file for the FGameTest class
 * @author Tom Stephens, Claude Sonnet 5 (medium)
 * @date Created: Jun 19, 2009
 * @date Last Modified: Jul 11, 2026
 */

#include "FGameTest.h"
#include <cstdio>
#include <fstream>
#include <vector>
#include "strategic/FFleet.h"
#include "strategic/FSystem.h"
#include "strategic/FMap.h"
#include "strategic/FPlanet.h"
#include "strategic/FPlayer.h"
#include "strategic/IStrategicUI.h"
#include "ships/FVehicle.h"
#include "weapons/FWeapon.h"
//#include "strategic/FGame.h"

namespace FrontierTests {
using namespace Frontier;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FGameTest );

namespace {

/**
 * @brief Read back FGame's loss counters from a real FGame::save() stream.
 *
 * FGame does not expose public accessors for m_lostHC/m_lostAC/
 * m_lostSatharShips/m_lostTendaySathar/m_lostTendayUPF/m_stationsDestroyed
 * (they are private with no getters), so this helper drives the real,
 * already-covered FGame::save() serialization path and parses the leading
 * fixed-layout fields back out in the exact order FGame::save()/load()
 * read and write them: bool m_gui, unsigned int m_round, unsigned int
 * m_currentPlayer, int m_satharRetreat, then the six int loss/destruction
 * counters. This is a behavioral read of real bytes emitted by production
 * code, not a source-text inspection.
 *
 * @param game               The FGame instance to snapshot via save().
 * @param lostHC             Out: number of Sathar Heavy Cruisers lost.
 * @param lostAC             Out: number of Sathar Assault Carriers lost.
 * @param lostSatharShips    Out: total number of Sathar ships lost.
 * @param lostTendaySathar   Out: Sathar ships lost in the current tenday.
 * @param lostTendayUPF      Out: UPF ships lost in the current tenday.
 * @param stationsDestroyed  Out: number of stations destroyed.
 *
 * @author Claude Sonnet 5 (medium)
 * @date Created: Jul 11, 2026
 * @date Last Modified: Jul 11, 2026
 */
void readLossCounters(FGame &game, int &lostHC, int &lostAC, int &lostSatharShips,
		int &lostTendaySathar, int &lostTendayUPF, int &stationsDestroyed){
	const char *filename = "FGameTestLossCounters.tmp";
	std::remove(filename);
	std::ofstream os(filename, std::ios::binary);
	game.save(os);
	os.close();

	std::ifstream is(filename, std::ios::binary);
	bool gui;
	unsigned int round;
	unsigned int currentPlayer;
	int satharRetreat;
	is.read(reinterpret_cast<char *>(&gui), sizeof(gui));
	is.read(reinterpret_cast<char *>(&round), sizeof(round));
	is.read(reinterpret_cast<char *>(&currentPlayer), sizeof(currentPlayer));
	is.read(reinterpret_cast<char *>(&satharRetreat), sizeof(satharRetreat));
	is.read(reinterpret_cast<char *>(&lostHC), sizeof(lostHC));
	is.read(reinterpret_cast<char *>(&lostAC), sizeof(lostAC));
	is.read(reinterpret_cast<char *>(&lostSatharShips), sizeof(lostSatharShips));
	is.read(reinterpret_cast<char *>(&lostTendaySathar), sizeof(lostTendaySathar));
	is.read(reinterpret_cast<char *>(&lostTendayUPF), sizeof(lostTendayUPF));
	is.read(reinterpret_cast<char *>(&stationsDestroyed), sizeof(stationsDestroyed));
	is.close();
	std::remove(filename);
}

/**
 * @brief Minimal mock IStrategicUI used to drive FGame::init() with a fixed
 * Sathar retreat condition and to observe checkForVictory()'s only externally
 * visible outcome signal: the notifyVictory(int) callback.
 *
 * FGame::checkForVictory() is private and its numeric return value is not
 * otherwise observable from outside FGame; the only public path that reaches
 * it is FGame::endUPFTurn(), and the only place a UPF win (result == 1) is
 * observable is the notifyVictory(1) call the production code itself makes
 * when it decides the UPF have won. Recording those calls is a behavioral
 * read of the real evaluated outcome, not a source-text inspection.
 *
 * @author Claude Sonnet 5 (medium)
 * @date Created: Jul 11, 2026
 * @date Last Modified: Jul 11, 2026
 */
class RetreatConditionMockUI : public IStrategicUI {
public:
	explicit RetreatConditionMockUI(int retreatCondition) : m_retreatCondition(retreatCondition) {}

	void showMessage(const std::string&, const std::string&) {}
	void notifyFailedJump(const std::string&) {}
	void notifyVictory(int result) { victoryNotifications.push_back(result); }
	int selectRetreatCondition() { return m_retreatCondition; }
	int runUPFUnattachedSetup(FPlayer*, FMap*) { return 0; }
	int runSatharFleetSetup(FPlayer*, FMap*, bool) { return 0; }
	void showSystemDialog(FSystem*, FMap*, FPlayer*) {}
	void showFleetDialog(FFleet*, FSystem*, FSystem*) {}
	void showRetreatConditions(const std::string& text) { lastRetreatConditionsText = text; }
	int selectCombat(FSystem*, FleetList, FleetList, PlayerList*) { return 0; }
	void requestRedraw() {}

	int m_retreatCondition;
	std::vector<int> victoryNotifications;
	std::string lastRetreatConditionsText;
};

/**
 * @brief Adds a Sathar-owned fleet co-located with a station-bearing system so
 * FGame::checkForCombat() detects an opposing presence and invokes the
 * private cleanUpShips() through the public endUPFTurn() turn path.
 *
 * @param sathar      The Sathar FPlayer that will own the new fleet.
 * @param sys         The system to place the fleet in; must contain a station
 *                     so combat is detected even with no other UPF fleet present.
 * @param fleetName    Name to give the new fleet.
 * @param doomedCount  Number of ships to add with HP already set to 0.
 * @param survivorCount Number of ships to add at full health.
 * @return Pointer to the newly created and system/player-registered fleet.
 *
 * @author Claude Sonnet 5 (medium)
 * @date Created: Jul 11, 2026
 * @date Last Modified: Jul 11, 2026
 */
FFleet * addCoLocatedSatharRaiders(FPlayer *sathar, FSystem *sys, const std::string &fleetName,
		int doomedCount, int survivorCount){
	FFleet *raiders = new FFleet;
	raiders->setName(fleetName);
	raiders->setOwner(sathar->getID());
	raiders->setLocation(sys, false);
	for (int i = 0; i < doomedCount; i++){
		FVehicle *doomed = createShip("Destroyer");
		doomed->setHP(0);
		raiders->addShip(doomed);
	}
	for (int i = 0; i < survivorCount; i++){
		FVehicle *survivor = createShip("Destroyer");
		raiders->addShip(survivor);
	}
	sathar->addFleet(raiders);
	sys->addFleet(raiders);
	return raiders;
}

/**
 * @brief Creates a standalone fleet for the given player at the given
 * system, optionally flagged as a militia fleet via FFleet::setMilitia(),
 * with doomedCount ships of shipType added at HP 0 (destroyed) and
 * survivorCount ships of shipType left at full health. Registers the fleet
 * with both the owning FPlayer and the FSystem, matching the registration
 * pattern used by addCoLocatedSatharRaiders() above.
 *
 * FGame::cleanUpShips() (invoked through the public endUPFTurn() turn path)
 * scans every player's every fleet once combat is detected anywhere in the
 * game, not just the fleets co-located with the detected combat, so ships
 * added by this helper are processed by cleanUpShips() regardless of which
 * system a separate combat-trigger fleet (e.g. addCoLocatedSatharRaiders())
 * is placed in.
 *
 * @param owner         The FPlayer ("UPF" or "Sathar") that will own the fleet.
 * @param sys           The system to register the fleet in.
 * @param fleetName     Name to give the new fleet.
 * @param shipType      Ship type string passed to createShip() (e.g. "Destroyer", "Fighter").
 * @param doomedCount   Number of ships to add with HP already set to 0.
 * @param survivorCount Number of ships to add at full health.
 * @param militia       When true, marks the fleet as a militia fleet via setMilitia(true, ...).
 * @return Pointer to the newly created and system/player-registered fleet.
 *
 * @author Claude Sonnet 5 (medium)
 * @date Created: Jul 11, 2026
 * @date Last Modified: Jul 11, 2026
 */
FFleet * addShipLossFleet(FPlayer *owner, FSystem *sys, const std::string &fleetName,
		const std::string &shipType, int doomedCount, int survivorCount, bool militia){
	FFleet *fleet = new FFleet;
	fleet->setName(fleetName);
	fleet->setOwner(owner->getID());
	fleet->setLocation(sys, false);
	if (militia){
		fleet->setMilitia(true, sys->getName());
	}
	for (int i = 0; i < doomedCount; i++){
		FVehicle *doomed = createShip(shipType);
		doomed->setHP(0);
		fleet->addShip(doomed);
	}
	for (int i = 0; i < survivorCount; i++){
		FVehicle *survivor = createShip(shipType);
		fleet->addShip(survivor);
	}
	owner->addFleet(fleet);
	sys->addFleet(fleet);
	return fleet;
}

/**
 * @brief Sets the HP of the station at the given system/planet to 0 so the
 * next cleanUpShips() pass counts it as destroyed.
 *
 * @author Claude Sonnet 5 (medium)
 * @date Created: Jul 11, 2026
 * @date Last Modified: Jul 11, 2026
 */
void destroyStationAt(const std::string &sysName, const std::string &planetName){
	FVehicle *station = FMap::getMap().getSystem(sysName)->getPlanet(planetName)->getStation();
	CPPUNIT_ASSERT(station != NULL);
	station->setHP(0);
}

/**
 * @brief Calls FGame::endUPFTurn() repeatedly, the only public path that
 * reaches the private checkForVictory().
 *
 * @author Claude Sonnet 5 (medium)
 * @date Created: Jul 11, 2026
 * @date Last Modified: Jul 11, 2026
 */
void advanceUPFTurns(FGame *game, int count){
	for (int i = 0; i < count; i++){
		game->endUPFTurn();
	}
}

}  // namespace

void FGameTest::setUp(){
	m_g1 = &(FGame::create());
}

void FGameTest::tearDown(){
	delete m_g1;
}

void FGameTest::testConstructor(){
}

void FGameTest::testInit(){
	CPPUNIT_ASSERT( 0 == m_g1->init(NULL));
}

void FGameTest::testShowPlayers(){
	m_g1->init(NULL);
	m_g1->showPlayers();
}

void FGameTest::testEndSatharTurn(){
	m_g1->init(NULL);
	CPPUNIT_ASSERT(m_g1->isUPFTurn() == false);
	m_g1->endSatharTurn();
	CPPUNIT_ASSERT(m_g1->isUPFTurn() == true);
}

void FGameTest::testPlaceNova(){
	bool result = false;
	m_g1->init(NULL);
	// it's initally the Sathar turn so trying to place SF Nova should fail
	result = m_g1->placeNova();
	CPPUNIT_ASSERT(result == false);
	m_g1->endSatharTurn();
	result = m_g1->placeNova();
	CPPUNIT_ASSERT(result == true);
}

void FGameTest::testEndUPFTurn(){
	m_g1->init(NULL);
	m_g1->endSatharTurn();
	CPPUNIT_ASSERT(m_g1->isUPFTurn() == true);
	m_g1->endUPFTurn();
	CPPUNIT_ASSERT(m_g1->isUPFTurn() == false);
}

void FGameTest::testShowRetreatCondition(){
	m_g1->init(NULL);
	m_g1->showRetreatConditions();
}

void FGameTest::testGetPlayer(){
	m_g1->init(NULL);
	m_g1->showPlayers();
	FPlayer *p = m_g1->getPlayer(1);
	CPPUNIT_ASSERT(p != NULL);
	CPPUNIT_ASSERT(p->getName() == "UPF");
	p = m_g1->getPlayer(2);
	CPPUNIT_ASSERT(p != NULL);
	CPPUNIT_ASSERT(p->getName() == "Sathar");
}

void FGameTest::testSerialize(){
	m_g1->init(NULL);
	const char *filename = "FGameTest.tmp";
	std::remove(filename);
	std::ofstream os(filename,std::ios::binary);
	m_g1->save(os);
	os.close();
	delete m_g1;
	m_g1 = &(FGame::create());
	std::ifstream is(filename,std::ios::binary);
	m_g1->load(is);
	is.close();
	std::remove(filename);
	CPPUNIT_ASSERT(m_g1->getPlayer(2)->getName() == "Sathar");

}

/**
 * Acceptance criterion: "A ship with HP <= 0 is absent from its owning
 * FFleet::getShipList() after cleanup ... Surviving ships remain in their
 * fleet and have been reload()ed."
 *
 * Builds a live Sathar fleet co-located with the real "Task Force Prenglar"
 * UPF fleet so FGame::checkForCombat() detects opposing fleets and invokes
 * the private cleanUpShips() through the public FGame::endUPFTurn() turn
 * path (cleanUpShips() itself is private and not called directly). One ship
 * in the Sathar fleet is destroyed (HP set to 0) and one survives with its
 * torpedo ammo depleted beforehand so reload() is independently observable.
 */
void FGameTest::testCleanUpShipsRemovesDestroyedShipAndReloadsSurvivor(){
	CPPUNIT_ASSERT(m_g1->init(NULL) == 0);
	FPlayer *upf = m_g1->getPlayer(1);
	FPlayer *sathar = m_g1->getPlayer(2);
	CPPUNIT_ASSERT(upf != NULL);
	CPPUNIT_ASSERT(sathar != NULL);

	FFleet *upfFleet = upf->getFleet("Task Force Prenglar");
	CPPUNIT_ASSERT(upfFleet != NULL);
	FSystem *sys = FMap::getMap().getSystem(upfFleet->getLocation());
	CPPUNIT_ASSERT(sys != NULL);

	FFleet *raiders = new FFleet;
	raiders->setName("Test Raiders Prenglar");
	raiders->setOwner(sathar->getID());
	raiders->setLocation(sys, false);

	FVehicle *doomed = createShip("Destroyer");
	unsigned int doomedID = doomed->getID();
	doomed->setHP(0);  // destroyed: HP <= 0

	FVehicle *survivor = createShip("Destroyer");
	unsigned int survivorID = survivor->getID();
	FWeapon *torpedo = survivor->getWeapon(2);
	CPPUNIT_ASSERT(torpedo != NULL);
	CPPUNIT_ASSERT(torpedo->getMaxAmmo() == 2);
	torpedo->setCurrentAmmo(0);  // simulate spent ordnance before cleanup

	raiders->addShip(doomed);
	raiders->addShip(survivor);
	sathar->addFleet(raiders);
	sys->addFleet(raiders);

	CPPUNIT_ASSERT(raiders->getShipList().size() == 2);

	m_g1->endSatharTurn();  // establish UPF turn like the existing turn tests
	m_g1->endUPFTurn();  // drives checkForCombat() -> cleanUpShips()

	// destroyed ship removed from the live fleet
	const VehicleList &ships = raiders->getShipList();
	CPPUNIT_ASSERT(ships.size() == 1);
	for (VehicleList::const_iterator it = ships.begin(); it != ships.end(); it++){
		CPPUNIT_ASSERT((*it)->getID() != doomedID);
	}
	CPPUNIT_ASSERT(ships[0]->getID() == survivorID);

	// surviving ship remains in the fleet and was reload()ed
	CPPUNIT_ASSERT(survivor->getWeapon(2)->getAmmo() == 2);
}

/**
 * Acceptance criterion: "A fleet emptied by cleanup is absent from both its
 * owning FPlayer's fleet list and its FSystem's fleet list afterward."
 *
 * Builds a single-ship, doomed-only Sathar fleet co-located with the real
 * "Task Force Cassidine" UPF fleet, drives cleanup through the public
 * FGame::endUPFTurn() turn path, and asserts the now-empty fleet is absent
 * from both FPlayer::getFleetList() and FSystem::getFleetList() afterward.
 * The fleet object itself is deleted by cleanUpShips() once emptied, so
 * only its ID (captured beforehand) is used for the post-cleanup checks.
 */
void FGameTest::testCleanUpShipsRemovesEmptiedFleetFromPlayerAndSystem(){
	CPPUNIT_ASSERT(m_g1->init(NULL) == 0);
	FPlayer *upf = m_g1->getPlayer(1);
	FPlayer *sathar = m_g1->getPlayer(2);
	CPPUNIT_ASSERT(upf != NULL);
	CPPUNIT_ASSERT(sathar != NULL);

	FFleet *upfFleet = upf->getFleet("Task Force Cassidine");
	CPPUNIT_ASSERT(upfFleet != NULL);
	FSystem *sys = FMap::getMap().getSystem(upfFleet->getLocation());
	CPPUNIT_ASSERT(sys != NULL);

	FFleet *raiders = new FFleet;
	raiders->setName("Test Wipeout Raiders Cassidine");
	raiders->setOwner(sathar->getID());
	raiders->setLocation(sys, false);
	unsigned int raidersID = raiders->getID();

	FVehicle *doomed = createShip("Fighter");
	doomed->setHP(0);
	raiders->addShip(doomed);

	sathar->addFleet(raiders);
	sys->addFleet(raiders);

	bool foundInPlayerBefore = false;
	FleetList &playerFleetsBefore = sathar->getFleetList();
	for (FleetList::iterator it = playerFleetsBefore.begin(); it != playerFleetsBefore.end(); it++){
		if ((*it)->getID() == raidersID){
			foundInPlayerBefore = true;
		}
	}
	CPPUNIT_ASSERT(foundInPlayerBefore);

	bool foundInSystemBefore = false;
	const FleetList &sysFleetsBefore = sys->getFleetList();
	for (FleetList::const_iterator it = sysFleetsBefore.begin(); it != sysFleetsBefore.end(); it++){
		if ((*it)->getID() == raidersID){
			foundInSystemBefore = true;
		}
	}
	CPPUNIT_ASSERT(foundInSystemBefore);

	m_g1->endSatharTurn();
	m_g1->endUPFTurn();  // triggers cleanUpShips(); raiders fleet becomes empty and is pruned

	bool foundInPlayerAfter = false;
	FleetList &playerFleetsAfter = sathar->getFleetList();
	for (FleetList::iterator it = playerFleetsAfter.begin(); it != playerFleetsAfter.end(); it++){
		if ((*it)->getID() == raidersID){
			foundInPlayerAfter = true;
		}
	}
	CPPUNIT_ASSERT(!foundInPlayerAfter);

	bool foundInSystemAfter = false;
	const FleetList &sysFleetsAfter = sys->getFleetList();
	for (FleetList::const_iterator it = sysFleetsAfter.begin(); it != sysFleetsAfter.end(); it++){
		if ((*it)->getID() == raidersID){
			foundInSystemAfter = true;
		}
	}
	CPPUNIT_ASSERT(!foundInSystemAfter);
	// note: 'raiders' is deleted by cleanUpShips() once emptied; do not
	// dereference the pointer beyond this point.
}

/**
 * Acceptance criterion: "Loss counters (m_lostTendayUPF/m_lostTendaySathar/
 * m_lostSatharShips/m_lostHC/m_lostAC) increase by exactly the count of
 * matching destroyed ships per cleanUpShips() call, with no inflation from
 * re-scanning."
 *
 * FGame exposes no public getters for these private counters, so this test
 * drives the real FGame::save() serialization path (already covered by
 * testSerialize) and parses the counters back out of the emitted bytes via
 * the readLossCounters() helper above -- a behavioral read of real state,
 * not a source-text check. A destroyed Sathar Destroyer should increment
 * both m_lostSatharShips and m_lostTendaySathar by exactly 1 (Destroyer is
 * neither a Fighter, HvCruiser, nor AssaultCarrier, so m_lostHC/m_lostAC/
 * m_lostTendayUPF are unaffected). A second turn-processing pass with no
 * newly destroyed ships must not further inflate the counters, proving the
 * live-container rewrite does not re-record the same ship on re-scan.
 */
void FGameTest::testCleanUpShipsUpdatesLossCountersWithoutInflation(){
	CPPUNIT_ASSERT(m_g1->init(NULL) == 0);
	FPlayer *upf = m_g1->getPlayer(1);
	FPlayer *sathar = m_g1->getPlayer(2);
	CPPUNIT_ASSERT(upf != NULL);
	CPPUNIT_ASSERT(sathar != NULL);

	FFleet *upfFleet = upf->getFleet("Task Force Prenglar");
	CPPUNIT_ASSERT(upfFleet != NULL);
	FSystem *sys = FMap::getMap().getSystem(upfFleet->getLocation());
	CPPUNIT_ASSERT(sys != NULL);

	FFleet *raiders = new FFleet;
	raiders->setName("Test Loss Counter Raiders Prenglar");
	raiders->setOwner(sathar->getID());
	raiders->setLocation(sys, false);

	FVehicle *doomed = createShip("Destroyer");  // type != Fighter/HvCruiser/AssaultCarrier
	doomed->setHP(0);
	raiders->addShip(doomed);

	FVehicle *survivor = createShip("Destroyer");
	raiders->addShip(survivor);

	sathar->addFleet(raiders);
	sys->addFleet(raiders);

	int lostHCBefore, lostACBefore, lostSatharBefore, lostTendaySatharBefore, lostTendayUPFBefore, stationsBefore;
	readLossCounters(*m_g1, lostHCBefore, lostACBefore, lostSatharBefore,
			lostTendaySatharBefore, lostTendayUPFBefore, stationsBefore);

	m_g1->endSatharTurn();
	m_g1->endUPFTurn();  // first cleanup pass: destroys 'doomed', reloads 'survivor'

	int lostHCAfter1, lostACAfter1, lostSatharAfter1, lostTendaySatharAfter1, lostTendayUPFAfter1, stationsAfter1;
	readLossCounters(*m_g1, lostHCAfter1, lostACAfter1, lostSatharAfter1,
			lostTendaySatharAfter1, lostTendayUPFAfter1, stationsAfter1);

	CPPUNIT_ASSERT(lostSatharAfter1 == lostSatharBefore + 1);
	CPPUNIT_ASSERT(lostTendaySatharAfter1 == lostTendaySatharBefore + 1);
	CPPUNIT_ASSERT(lostHCAfter1 == lostHCBefore);
	CPPUNIT_ASSERT(lostACAfter1 == lostACBefore);
	CPPUNIT_ASSERT(lostTendayUPFAfter1 == lostTendayUPFBefore);

	// second turn-processing pass with no new destruction must not re-count
	// the already-destroyed ship (guards against the by-value-copy re-scan bug)
	m_g1->endSatharTurn();
	m_g1->endUPFTurn();

	int lostHCAfter2, lostACAfter2, lostSatharAfter2, lostTendaySatharAfter2, lostTendayUPFAfter2, stationsAfter2;
	readLossCounters(*m_g1, lostHCAfter2, lostACAfter2, lostSatharAfter2,
			lostTendaySatharAfter2, lostTendayUPFAfter2, stationsAfter2);

	CPPUNIT_ASSERT(lostSatharAfter2 == lostSatharAfter1);
	CPPUNIT_ASSERT(lostTendaySatharAfter2 == lostTendaySatharAfter1);
	CPPUNIT_ASSERT(lostHCAfter2 == lostHCAfter1);
	CPPUNIT_ASSERT(lostACAfter2 == lostACAfter1);
	CPPUNIT_ASSERT(lostTendayUPFAfter2 == lostTendayUPFAfter1);
}

/**
 * Acceptance criterion (C2): "Condition 4 selected: UPF victory reported at a
 * tenday boundary iff fewer than two stations were destroyed that tenday;
 * station counter resets at the boundary."
 *
 * Drives the private checkForVictory() through the only public path that
 * reaches it, FGame::endUPFTurn(), with m_satharRetreat == 4 and exactly one
 * station destroyed (below the two-station threshold). Because checkForVictory()
 * is private, the outcome is observed the way the production code itself
 * surfaces it -- the notifyVictory(1) callback on the installed IStrategicUI
 * mock -- and the counter reset is observed by parsing FGame::save()'s real
 * byte stream (readLossCounters), not by inspecting source text.
 *
 * This regresses the pre-fix (swapped) defect: before the fix, case 4
 * evaluated the ship-loss condition instead of the station condition, so this
 * scenario (0 ship losses either side) would NOT have produced a UPF victory
 * under the swapped code, whereas the fixed code correctly does.
 */
void FGameTest::testCheckForVictoryCondition4WinsWithStationsBelowThresholdAndResetsCounter(){
	delete m_g1;
	RetreatConditionMockUI *ui = new RetreatConditionMockUI(4);
	m_g1 = &(FGame::create(ui));
	CPPUNIT_ASSERT(m_g1->init(NULL) == 0);

	FPlayer *sathar = m_g1->getPlayer(2);
	CPPUNIT_ASSERT(sathar != NULL);
	FSystem *sys = FMap::getMap().getSystem("Theseus");
	CPPUNIT_ASSERT(sys != NULL);

	// destroy exactly one station (< 2) and trigger combat/cleanup in the
	// same system so the destruction registers before the tenday boundary
	destroyStationAt("Theseus", "Minotaur");
	addCoLocatedSatharRaiders(sathar, sys, "Test Case4 Win Raiders", 0, 1);

	m_g1->endUPFTurn();        // call #1: registers 1 destroyed station; round 0 -> 1
	advanceUPFTurns(m_g1, 10); // calls #2-#11: call #11 checks round==10 (tenday boundary)

	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), ui->victoryNotifications.size());
	CPPUNIT_ASSERT_EQUAL(1, ui->victoryNotifications[0]);

	int lostHC, lostAC, lostSathar, lostTendaySathar, lostTendayUPF, stationsDestroyed;
	readLossCounters(*m_g1, lostHC, lostAC, lostSathar, lostTendaySathar, lostTendayUPF, stationsDestroyed);
	CPPUNIT_ASSERT_EQUAL(0, stationsDestroyed);

	delete ui;
}

/**
 * Acceptance criterion (C2): station counter resets at the tenday boundary
 * regardless of the win/lose outcome, and no UPF victory is reported once the
 * two-station threshold is met.
 *
 * m_satharRetreat == 4 with exactly two stations destroyed (at the
 * threshold, not below it): checkForVictory() must not call notifyVictory(),
 * but m_stationsDestroyed must still reset to 0 at the boundary.
 */
void FGameTest::testCheckForVictoryCondition4NoWinAtThresholdButCounterStillResets(){
	delete m_g1;
	RetreatConditionMockUI *ui = new RetreatConditionMockUI(4);
	m_g1 = &(FGame::create(ui));
	CPPUNIT_ASSERT(m_g1->init(NULL) == 0);

	FPlayer *sathar = m_g1->getPlayer(2);
	CPPUNIT_ASSERT(sathar != NULL);
	FSystem *sys = FMap::getMap().getSystem("Theseus");
	CPPUNIT_ASSERT(sys != NULL);

	destroyStationAt("Theseus", "Minotaur");
	destroyStationAt("K'aken-Kar", "Ken'zah Kit");
	addCoLocatedSatharRaiders(sathar, sys, "Test Case4 NoWin Raiders", 0, 1);

	m_g1->endUPFTurn();
	advanceUPFTurns(m_g1, 10);

	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), ui->victoryNotifications.size());

	int lostHC, lostAC, lostSathar, lostTendaySathar, lostTendayUPF, stationsDestroyed;
	readLossCounters(*m_g1, lostHC, lostAC, lostSathar, lostTendaySathar, lostTendayUPF, stationsDestroyed);
	CPPUNIT_ASSERT_EQUAL(0, stationsDestroyed);

	delete ui;
}

/**
 * C2 regression case explicitly requested by the Planner: with
 * m_satharRetreat == 4, m_stationsDestroyed == 5, m_lostTendaySathar == 5,
 * and m_lostTendayUPF == 0 at a tenday boundary, the fixed code must NOT
 * declare UPF victory (5 stations destroyed is not below the two-station
 * threshold), whereas the pre-fix (swapped) code -- which evaluated
 * m_lostTendaySathar > m_lostTendayUPF for case 4 -- would have declared a
 * UPF victory here (5 > 0). This is a behavioral test that fails against the
 * unfixed/swapped code and passes against the fix.
 */
void FGameTest::testCheckForVictoryCondition4RegressionDoesNotAwardVictoryWhenStationsAtFiveDespiteShipLossImbalance(){
	delete m_g1;
	RetreatConditionMockUI *ui = new RetreatConditionMockUI(4);
	m_g1 = &(FGame::create(ui));
	CPPUNIT_ASSERT(m_g1->init(NULL) == 0);

	FPlayer *sathar = m_g1->getPlayer(2);
	CPPUNIT_ASSERT(sathar != NULL);
	FSystem *sys = FMap::getMap().getSystem("Theseus");
	CPPUNIT_ASSERT(sys != NULL);

	// 5 stations destroyed (>= 2, so the fixed station-based case 4 condition
	// is NOT satisfied)
	destroyStationAt("Theseus", "Minotaur");
	destroyStationAt("K'aken-Kar", "Ken'zah Kit");
	destroyStationAt("Araks", "Hentz");
	destroyStationAt("Prenglar", "Gran Quivera");
	destroyStationAt("K'tsa-Kar", "Kawdl-Kit");

	// 5 Sathar ship losses vs 0 UPF ship losses this tenday: with the pre-fix
	// (swapped) case 4 body, this imbalance alone would have declared a UPF
	// victory regardless of the station count.
	addCoLocatedSatharRaiders(sathar, sys, "Test Case4 Regression Raiders", 5, 0);

	m_g1->endUPFTurn();
	advanceUPFTurns(m_g1, 10);

	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), ui->victoryNotifications.size());

	delete ui;
}

/**
 * Acceptance criterion (C2): "Condition 5 selected: UPF victory reported at a
 * tenday boundary iff Sathar tenday ship losses exceed UPF tenday ship
 * losses; the tenday loss counters (m_lostTendaySathar, m_lostTendayUPF)
 * reset at the boundary."
 *
 * m_satharRetreat == 5 with 3 Sathar ships lost and 1 UPF ship lost this
 * tenday (3 > 1): checkForVictory() must call notifyVictory(1), and both
 * tenday loss counters must reset to 0 at the boundary.
 */
void FGameTest::testCheckForVictoryCondition5WinsWhenSatharLossesExceedUPFAndResetsCounters(){
	delete m_g1;
	RetreatConditionMockUI *ui = new RetreatConditionMockUI(5);
	m_g1 = &(FGame::create(ui));
	CPPUNIT_ASSERT(m_g1->init(NULL) == 0);

	FPlayer *upf = m_g1->getPlayer(1);
	FPlayer *sathar = m_g1->getPlayer(2);
	CPPUNIT_ASSERT(upf != NULL);
	CPPUNIT_ASSERT(sathar != NULL);

	FFleet *upfFleet = upf->getFleet("Task Force Prenglar");
	CPPUNIT_ASSERT(upfFleet != NULL);
	const VehicleList &upfShips = upfFleet->getShipList();
	CPPUNIT_ASSERT(upfShips.size() >= 1);
	upfShips[0]->setHP(0); // 1 UPF ship lost this tenday

	FSystem *sys = FMap::getMap().getSystem("Theseus");
	CPPUNIT_ASSERT(sys != NULL);
	addCoLocatedSatharRaiders(sathar, sys, "Test Case5 Win Raiders", 3, 0); // 3 Sathar ships lost

	m_g1->endUPFTurn();
	advanceUPFTurns(m_g1, 10);

	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), ui->victoryNotifications.size());
	CPPUNIT_ASSERT_EQUAL(1, ui->victoryNotifications[0]);

	int lostHC, lostAC, lostSathar, lostTendaySathar, lostTendayUPF, stationsDestroyed;
	readLossCounters(*m_g1, lostHC, lostAC, lostSathar, lostTendaySathar, lostTendayUPF, stationsDestroyed);
	CPPUNIT_ASSERT_EQUAL(0, lostTendaySathar);
	CPPUNIT_ASSERT_EQUAL(0, lostTendayUPF);

	delete ui;
}

/**
 * Acceptance criterion (C2): tenday loss counters reset at the boundary
 * regardless of the win/lose outcome, and no UPF victory is reported when
 * Sathar tenday losses do not exceed UPF's.
 *
 * m_satharRetreat == 5 with 1 Sathar ship lost and 2 UPF ships lost this
 * tenday (1 !> 2): checkForVictory() must not call notifyVictory(), but both
 * tenday loss counters must still reset to 0 at the boundary.
 */
void FGameTest::testCheckForVictoryCondition5NoWinWhenSatharLossesDoNotExceedUPFButCountersStillReset(){
	delete m_g1;
	RetreatConditionMockUI *ui = new RetreatConditionMockUI(5);
	m_g1 = &(FGame::create(ui));
	CPPUNIT_ASSERT(m_g1->init(NULL) == 0);

	FPlayer *upf = m_g1->getPlayer(1);
	FPlayer *sathar = m_g1->getPlayer(2);
	CPPUNIT_ASSERT(upf != NULL);
	CPPUNIT_ASSERT(sathar != NULL);

	FFleet *upfFleet = upf->getFleet("Task Force Prenglar");
	CPPUNIT_ASSERT(upfFleet != NULL);
	const VehicleList &upfShips = upfFleet->getShipList();
	CPPUNIT_ASSERT(upfShips.size() >= 2);
	upfShips[0]->setHP(0);
	upfShips[1]->setHP(0); // 2 UPF ships lost this tenday

	FSystem *sys = FMap::getMap().getSystem("Theseus");
	CPPUNIT_ASSERT(sys != NULL);
	addCoLocatedSatharRaiders(sathar, sys, "Test Case5 NoWin Raiders", 1, 0); // 1 Sathar ship lost

	m_g1->endUPFTurn();
	advanceUPFTurns(m_g1, 10);

	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), ui->victoryNotifications.size());

	int lostHC, lostAC, lostSathar, lostTendaySathar, lostTendayUPF, stationsDestroyed;
	readLossCounters(*m_g1, lostHC, lostAC, lostSathar, lostTendaySathar, lostTendayUPF, stationsDestroyed);
	CPPUNIT_ASSERT_EQUAL(0, lostTendaySathar);
	CPPUNIT_ASSERT_EQUAL(0, lostTendayUPF);

	delete ui;
}

/**
 * C2 regression case explicitly requested by the Planner: the symmetric
 * counterpart for m_satharRetreat == 5. With m_stationsDestroyed == 5 (at/
 * above the case-4 threshold), m_lostTendaySathar == 5, and
 * m_lostTendayUPF == 0 at a tenday boundary, the fixed code correctly
 * declares a UPF victory (5 > 0), whereas the pre-fix (swapped) code -- which
 * evaluated m_stationsDestroyed < 2 for case 5 -- would have missed this real
 * win (5 is not < 2). This is a behavioral test that fails against the
 * unfixed/swapped code (no notifyVictory(1) call) and passes against the fix.
 */
void FGameTest::testCheckForVictoryCondition5RegressionAwardsVictoryWhenStationsAtFiveDespiteBeingAboveThreshold(){
	delete m_g1;
	RetreatConditionMockUI *ui = new RetreatConditionMockUI(5);
	m_g1 = &(FGame::create(ui));
	CPPUNIT_ASSERT(m_g1->init(NULL) == 0);

	FPlayer *sathar = m_g1->getPlayer(2);
	CPPUNIT_ASSERT(sathar != NULL);

	// 5 stations destroyed (>= 2): with the pre-fix (swapped) case 5 body,
	// this station count alone would have suppressed the real win below.
	destroyStationAt("Theseus", "Minotaur");
	destroyStationAt("K'aken-Kar", "Ken'zah Kit");
	destroyStationAt("Araks", "Hentz");
	destroyStationAt("Prenglar", "Gran Quivera");
	destroyStationAt("K'tsa-Kar", "Kawdl-Kit");

	FSystem *sys = FMap::getMap().getSystem("Theseus");
	CPPUNIT_ASSERT(sys != NULL);
	addCoLocatedSatharRaiders(sathar, sys, "Test Case5 Regression Raiders", 5, 0); // 5 Sathar losses, 0 UPF

	m_g1->endUPFTurn();
	advanceUPFTurns(m_g1, 10);

	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), ui->victoryNotifications.size());
	CPPUNIT_ASSERT_EQUAL(1, ui->victoryNotifications[0]);

	delete ui;
}

/**
 * Acceptance criterion (C2): "The evaluated condition matches the condition
 * text for the same case number."
 *
 * showRetreatConditions() text for case 4 must reference the station-count
 * condition (the same m_stationsDestroyed counter checkForVictory()
 * evaluates for case 4), and case 5's text must reference the ship-loss
 * condition (the same m_lostTendaySathar/m_lostTendayUPF counters
 * checkForVictory() evaluates for case 5). Combined with the counter-driven
 * behavioral tests above, this ties the displayed condition text to the
 * actual evaluated behavior for each case number.
 */
void FGameTest::testShowRetreatConditionsTextMatchesEvaluatedCaseNumber(){
	delete m_g1;
	RetreatConditionMockUI *ui4 = new RetreatConditionMockUI(4);
	m_g1 = &(FGame::create(ui4));
	CPPUNIT_ASSERT(m_g1->init(NULL) == 0);
	m_g1->showRetreatConditions();
	CPPUNIT_ASSERT(ui4->lastRetreatConditionsText.find("stations") != std::string::npos);

	delete m_g1;
	delete ui4;

	RetreatConditionMockUI *ui5 = new RetreatConditionMockUI(5);
	m_g1 = &(FGame::create(ui5));
	CPPUNIT_ASSERT(m_g1->init(NULL) == 0);
	m_g1->showRetreatConditions();
	CPPUNIT_ASSERT(ui5->lastRetreatConditionsText.find("lost more ships in\ncombat") != std::string::npos);

	delete m_g1;
	delete ui5;
	m_g1 = &(FGame::create()); // leave a benign default instance for tearDown()
}

/**
 * P2-3 (S2) acceptance criterion: "m_lostSatharShips counts EVERY destroyed
 * Sathar ship, including fighters."
 *
 * Destroys 3 Sathar Fighters in a non-militia fleet and asserts
 * m_lostSatharShips increases by exactly 3, observed via the real
 * FGame::save() byte stream (readLossCounters()). Against the pre-fix
 * `getType() != "Fighter"` guard on m_lostSatharShips, destroying only
 * fighters would leave the counter unchanged (delta 0), so this assertion
 * fails against the unfixed code and passes against the fix.
 */
void FGameTest::testCleanUpShipsCountsSatharFightersTowardLostSatharShips(){
	CPPUNIT_ASSERT(m_g1->init(NULL) == 0);
	FPlayer *upf = m_g1->getPlayer(1);
	FPlayer *sathar = m_g1->getPlayer(2);
	CPPUNIT_ASSERT(upf != NULL);
	CPPUNIT_ASSERT(sathar != NULL);

	FFleet *upfFleet = upf->getFleet("Task Force Prenglar");
	CPPUNIT_ASSERT(upfFleet != NULL);
	FSystem *sys = FMap::getMap().getSystem(upfFleet->getLocation());
	CPPUNIT_ASSERT(sys != NULL);

	// combat trigger: minimal, non-destroyed Sathar presence co-located with
	// the real UPF fleet so checkForCombat() invokes cleanUpShips()
	addCoLocatedSatharRaiders(sathar, sys, "Test S2 Trigger Raiders", 0, 1);
	// the actual loss under test: 3 Sathar Fighters, non-militia fleet
	addShipLossFleet(sathar, sys, "Test S2 Fighter Squadron", "Fighter", 3, 0, false);

	int lostHCBefore, lostACBefore, lostSatharBefore, lostTendaySatharBefore, lostTendayUPFBefore, stationsBefore;
	readLossCounters(*m_g1, lostHCBefore, lostACBefore, lostSatharBefore,
			lostTendaySatharBefore, lostTendayUPFBefore, stationsBefore);

	m_g1->endSatharTurn();
	m_g1->endUPFTurn();  // drives checkForCombat() -> cleanUpShips()

	int lostHCAfter, lostACAfter, lostSatharAfter, lostTendaySatharAfter, lostTendayUPFAfter, stationsAfter;
	readLossCounters(*m_g1, lostHCAfter, lostACAfter, lostSatharAfter,
			lostTendaySatharAfter, lostTendayUPFAfter, stationsAfter);

	CPPUNIT_ASSERT_EQUAL(lostSatharBefore + 3, lostSatharAfter);
}

/**
 * P2-3 (S3) acceptance criterion: "m_lostTendaySathar increments only when
 * the destroyed ship is neither a fighter nor a member of a militia fleet."
 * This test covers the fighter exclusion on the Sathar side.
 *
 * Destroys 2 Sathar Fighters in a non-militia fleet and asserts
 * m_lostTendaySathar does NOT change. Against the pre-fix code (which
 * incremented m_lostTendaySathar unconditionally for every destroyed Sathar
 * ship, with no fighter exclusion), this scenario would show a delta of +2,
 * so this assertion fails against the unfixed code and passes against the fix.
 */
void FGameTest::testCleanUpShipsExcludesSatharFightersFromTendaySatharCounter(){
	CPPUNIT_ASSERT(m_g1->init(NULL) == 0);
	FPlayer *upf = m_g1->getPlayer(1);
	FPlayer *sathar = m_g1->getPlayer(2);
	CPPUNIT_ASSERT(upf != NULL);
	CPPUNIT_ASSERT(sathar != NULL);

	FFleet *upfFleet = upf->getFleet("Task Force Prenglar");
	CPPUNIT_ASSERT(upfFleet != NULL);
	FSystem *sys = FMap::getMap().getSystem(upfFleet->getLocation());
	CPPUNIT_ASSERT(sys != NULL);

	addCoLocatedSatharRaiders(sathar, sys, "Test S3 Sathar Fighter Trigger Raiders", 0, 1);
	addShipLossFleet(sathar, sys, "Test S3 Sathar Fighter Squadron", "Fighter", 2, 0, false);

	int lostHCBefore, lostACBefore, lostSatharBefore, lostTendaySatharBefore, lostTendayUPFBefore, stationsBefore;
	readLossCounters(*m_g1, lostHCBefore, lostACBefore, lostSatharBefore,
			lostTendaySatharBefore, lostTendayUPFBefore, stationsBefore);

	m_g1->endSatharTurn();
	m_g1->endUPFTurn();

	int lostHCAfter, lostACAfter, lostSatharAfter, lostTendaySatharAfter, lostTendayUPFAfter, stationsAfter;
	readLossCounters(*m_g1, lostHCAfter, lostACAfter, lostSatharAfter,
			lostTendaySatharAfter, lostTendayUPFAfter, stationsAfter);

	CPPUNIT_ASSERT_EQUAL(lostTendaySatharBefore, lostTendaySatharAfter);
	// cross-check: S2 still counts these same fighters toward m_lostSatharShips
	CPPUNIT_ASSERT_EQUAL(lostSatharBefore + 2, lostSatharAfter);
}

/**
 * P2-3 (S3) acceptance criterion: fighter exclusion applies to BOTH sides.
 * This test covers the fighter exclusion on the UPF side.
 *
 * Destroys 2 UPF Fighters in a non-militia fleet and asserts
 * m_lostTendayUPF does NOT change. Against the pre-fix code (which
 * incremented m_lostTendayUPF unconditionally for every destroyed UPF ship,
 * with no fighter exclusion), this scenario would show a delta of +2, so
 * this assertion fails against the unfixed code and passes against the fix.
 */
void FGameTest::testCleanUpShipsExcludesUPFFightersFromTendayUPFCounter(){
	CPPUNIT_ASSERT(m_g1->init(NULL) == 0);
	FPlayer *upf = m_g1->getPlayer(1);
	FPlayer *sathar = m_g1->getPlayer(2);
	CPPUNIT_ASSERT(upf != NULL);
	CPPUNIT_ASSERT(sathar != NULL);

	FFleet *upfFleet = upf->getFleet("Task Force Prenglar");
	CPPUNIT_ASSERT(upfFleet != NULL);
	FSystem *sys = FMap::getMap().getSystem(upfFleet->getLocation());
	CPPUNIT_ASSERT(sys != NULL);

	addCoLocatedSatharRaiders(sathar, sys, "Test S3 UPF Fighter Trigger Raiders", 0, 1);
	addShipLossFleet(upf, sys, "Test S3 UPF Fighter Squadron", "Fighter", 2, 0, false);

	int lostHCBefore, lostACBefore, lostSatharBefore, lostTendaySatharBefore, lostTendayUPFBefore, stationsBefore;
	readLossCounters(*m_g1, lostHCBefore, lostACBefore, lostSatharBefore,
			lostTendaySatharBefore, lostTendayUPFBefore, stationsBefore);

	m_g1->endSatharTurn();
	m_g1->endUPFTurn();

	int lostHCAfter, lostACAfter, lostSatharAfter, lostTendaySatharAfter, lostTendayUPFAfter, stationsAfter;
	readLossCounters(*m_g1, lostHCAfter, lostACAfter, lostSatharAfter,
			lostTendaySatharAfter, lostTendayUPFAfter, stationsAfter);

	CPPUNIT_ASSERT_EQUAL(lostTendayUPFBefore, lostTendayUPFAfter);
}

/**
 * P2-3 (S3) acceptance criterion: militia exclusion applies to the Sathar
 * side. Destroys 2 non-fighter (Destroyer) ships belonging to a militia
 * fleet (FFleet::setMilitia(true, ...)) and asserts m_lostTendaySathar does
 * NOT change. Against the pre-fix code (which incremented
 * m_lostTendaySathar unconditionally, with no militia exclusion), this
 * scenario would show a delta of +2, so this assertion fails against the
 * unfixed code and passes against the fix.
 */
void FGameTest::testCleanUpShipsExcludesSatharMilitiaShipsFromTendaySatharCounter(){
	CPPUNIT_ASSERT(m_g1->init(NULL) == 0);
	FPlayer *upf = m_g1->getPlayer(1);
	FPlayer *sathar = m_g1->getPlayer(2);
	CPPUNIT_ASSERT(upf != NULL);
	CPPUNIT_ASSERT(sathar != NULL);

	FFleet *upfFleet = upf->getFleet("Task Force Prenglar");
	CPPUNIT_ASSERT(upfFleet != NULL);
	FSystem *sys = FMap::getMap().getSystem(upfFleet->getLocation());
	CPPUNIT_ASSERT(sys != NULL);

	addCoLocatedSatharRaiders(sathar, sys, "Test S3 Sathar Militia Trigger Raiders", 0, 1);
	addShipLossFleet(sathar, sys, "Test S3 Sathar Militia Fleet", "Destroyer", 2, 0, true);

	int lostHCBefore, lostACBefore, lostSatharBefore, lostTendaySatharBefore, lostTendayUPFBefore, stationsBefore;
	readLossCounters(*m_g1, lostHCBefore, lostACBefore, lostSatharBefore,
			lostTendaySatharBefore, lostTendayUPFBefore, stationsBefore);

	m_g1->endSatharTurn();
	m_g1->endUPFTurn();

	int lostHCAfter, lostACAfter, lostSatharAfter, lostTendaySatharAfter, lostTendayUPFAfter, stationsAfter;
	readLossCounters(*m_g1, lostHCAfter, lostACAfter, lostSatharAfter,
			lostTendaySatharAfter, lostTendayUPFAfter, stationsAfter);

	CPPUNIT_ASSERT_EQUAL(lostTendaySatharBefore, lostTendaySatharAfter);
	// cross-check: S2 is unaffected by militia status -- these ships still
	// count toward m_lostSatharShips
	CPPUNIT_ASSERT_EQUAL(lostSatharBefore + 2, lostSatharAfter);
}

/**
 * P2-3 (S3) acceptance criterion: militia exclusion applies to BOTH sides.
 * This test covers the militia exclusion on the UPF side. Destroys 2
 * non-fighter (Destroyer) ships belonging to a militia fleet and asserts
 * m_lostTendayUPF does NOT change. Against the pre-fix code (which
 * incremented m_lostTendayUPF unconditionally, with no militia exclusion),
 * this scenario would show a delta of +2, so this assertion fails against
 * the unfixed code and passes against the fix.
 */
void FGameTest::testCleanUpShipsExcludesUPFMilitiaShipsFromTendayUPFCounter(){
	CPPUNIT_ASSERT(m_g1->init(NULL) == 0);
	FPlayer *upf = m_g1->getPlayer(1);
	FPlayer *sathar = m_g1->getPlayer(2);
	CPPUNIT_ASSERT(upf != NULL);
	CPPUNIT_ASSERT(sathar != NULL);

	FFleet *upfFleet = upf->getFleet("Task Force Prenglar");
	CPPUNIT_ASSERT(upfFleet != NULL);
	FSystem *sys = FMap::getMap().getSystem(upfFleet->getLocation());
	CPPUNIT_ASSERT(sys != NULL);

	addCoLocatedSatharRaiders(sathar, sys, "Test S3 UPF Militia Trigger Raiders", 0, 1);
	addShipLossFleet(upf, sys, "Test S3 UPF Militia Fleet", "Destroyer", 2, 0, true);

	int lostHCBefore, lostACBefore, lostSatharBefore, lostTendaySatharBefore, lostTendayUPFBefore, stationsBefore;
	readLossCounters(*m_g1, lostHCBefore, lostACBefore, lostSatharBefore,
			lostTendaySatharBefore, lostTendayUPFBefore, stationsBefore);

	m_g1->endSatharTurn();
	m_g1->endUPFTurn();

	int lostHCAfter, lostACAfter, lostSatharAfter, lostTendaySatharAfter, lostTendayUPFAfter, stationsAfter;
	readLossCounters(*m_g1, lostHCAfter, lostACAfter, lostSatharAfter,
			lostTendaySatharAfter, lostTendayUPFAfter, stationsAfter);

	CPPUNIT_ASSERT_EQUAL(lostTendayUPFBefore, lostTendayUPFAfter);
}

/**
 * P2-3 (S3) acceptance criterion: "non-fighter, non-militia ships ARE
 * counted" toward the tenday counters. This test covers the Sathar side.
 *
 * Destroys 3 non-fighter Destroyers in a non-militia Sathar fleet and
 * asserts m_lostTendaySathar increases by exactly 3. This guards against an
 * overzealous fix that excludes more than fighters/militia ships.
 */
void FGameTest::testCleanUpShipsCountsNonFighterNonMilitiaSatharShipsTowardTendaySatharCounter(){
	CPPUNIT_ASSERT(m_g1->init(NULL) == 0);
	FPlayer *upf = m_g1->getPlayer(1);
	FPlayer *sathar = m_g1->getPlayer(2);
	CPPUNIT_ASSERT(upf != NULL);
	CPPUNIT_ASSERT(sathar != NULL);

	FFleet *upfFleet = upf->getFleet("Task Force Prenglar");
	CPPUNIT_ASSERT(upfFleet != NULL);
	FSystem *sys = FMap::getMap().getSystem(upfFleet->getLocation());
	CPPUNIT_ASSERT(sys != NULL);

	addShipLossFleet(sathar, sys, "Test S3 Sathar Regular Fleet", "Destroyer", 3, 0, false);

	int lostHCBefore, lostACBefore, lostSatharBefore, lostTendaySatharBefore, lostTendayUPFBefore, stationsBefore;
	readLossCounters(*m_g1, lostHCBefore, lostACBefore, lostSatharBefore,
			lostTendaySatharBefore, lostTendayUPFBefore, stationsBefore);

	m_g1->endSatharTurn();
	m_g1->endUPFTurn();

	int lostHCAfter, lostACAfter, lostSatharAfter, lostTendaySatharAfter, lostTendayUPFAfter, stationsAfter;
	readLossCounters(*m_g1, lostHCAfter, lostACAfter, lostSatharAfter,
			lostTendaySatharAfter, lostTendayUPFAfter, stationsAfter);

	CPPUNIT_ASSERT_EQUAL(lostTendaySatharBefore + 3, lostTendaySatharAfter);
}

/**
 * P2-3 (S3) acceptance criterion: "non-fighter, non-militia ships ARE
 * counted" toward the tenday counters. This test covers the UPF side.
 *
 * Destroys 3 non-fighter Destroyers in a non-militia UPF fleet and asserts
 * m_lostTendayUPF increases by exactly 3.
 */
void FGameTest::testCleanUpShipsCountsNonFighterNonMilitiaUPFShipsTowardTendayUPFCounter(){
	CPPUNIT_ASSERT(m_g1->init(NULL) == 0);
	FPlayer *upf = m_g1->getPlayer(1);
	FPlayer *sathar = m_g1->getPlayer(2);
	CPPUNIT_ASSERT(upf != NULL);
	CPPUNIT_ASSERT(sathar != NULL);

	FFleet *upfFleet = upf->getFleet("Task Force Prenglar");
	CPPUNIT_ASSERT(upfFleet != NULL);
	FSystem *sys = FMap::getMap().getSystem(upfFleet->getLocation());
	CPPUNIT_ASSERT(sys != NULL);

	addCoLocatedSatharRaiders(sathar, sys, "Test S3 UPF Regular Trigger Raiders", 0, 1);
	addShipLossFleet(upf, sys, "Test S3 UPF Regular Fleet", "Destroyer", 3, 0, false);

	int lostHCBefore, lostACBefore, lostSatharBefore, lostTendaySatharBefore, lostTendayUPFBefore, stationsBefore;
	readLossCounters(*m_g1, lostHCBefore, lostACBefore, lostSatharBefore,
			lostTendaySatharBefore, lostTendayUPFBefore, stationsBefore);

	m_g1->endSatharTurn();
	m_g1->endUPFTurn();

	int lostHCAfter, lostACAfter, lostSatharAfter, lostTendaySatharAfter, lostTendayUPFAfter, stationsAfter;
	readLossCounters(*m_g1, lostHCAfter, lostACAfter, lostSatharAfter,
			lostTendaySatharAfter, lostTendayUPFAfter, stationsAfter);

	CPPUNIT_ASSERT_EQUAL(lostTendayUPFBefore + 3, lostTendayUPFAfter);
}

/**
 * P2-3 (S3) acceptance criterion (checkForVictory condition-5 boundary,
 * positive case): at a tenday boundary, with m_satharRetreat == 5, after
 * filtering out fighters and militia ships, m_lostTendaySathar (2) exceeds
 * m_lostTendayUPF (1); checkForVictory() must surface the UPF victory via
 * notifyVictory(1).
 *
 * The Sathar side also loses 3 Fighters (non-militia fleet, excluded from
 * the tenday counter) and 2 militia Destroyers (excluded via
 * FFleet::isMilitia()) as "noise" alongside the 2 countable Destroyer
 * losses, proving the filtered comparison -- not the raw destroyed-ship
 * count -- feeds the victory decision.
 */
void FGameTest::testCheckForVictoryCondition5FilteredSatharLossesExceedUPFDespiteFighterAndMilitiaNoise(){
	delete m_g1;
	RetreatConditionMockUI *ui = new RetreatConditionMockUI(5);
	m_g1 = &(FGame::create(ui));
	CPPUNIT_ASSERT(m_g1->init(NULL) == 0);

	FPlayer *upf = m_g1->getPlayer(1);
	FPlayer *sathar = m_g1->getPlayer(2);
	CPPUNIT_ASSERT(upf != NULL);
	CPPUNIT_ASSERT(sathar != NULL);

	FSystem *sys = FMap::getMap().getSystem("Theseus");
	CPPUNIT_ASSERT(sys != NULL);

	// filtered Sathar tenday losses: 2 countable Destroyers
	addShipLossFleet(sathar, sys, "Test C5 Filtered Win Sathar Regulars", "Destroyer", 2, 0, false);
	// noise, excluded from the tenday counter: 3 Fighters
	addShipLossFleet(sathar, sys, "Test C5 Filtered Win Sathar Fighters", "Fighter", 3, 0, false);
	// noise, excluded from the tenday counter: 2 militia Destroyers
	addShipLossFleet(sathar, sys, "Test C5 Filtered Win Sathar Militia", "Destroyer", 2, 0, true);
	// filtered UPF tenday losses: 1 countable Destroyer
	addShipLossFleet(upf, sys, "Test C5 Filtered Win UPF Regulars", "Destroyer", 1, 0, false);

	m_g1->endUPFTurn();
	advanceUPFTurns(m_g1, 10);

	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), ui->victoryNotifications.size());
	CPPUNIT_ASSERT_EQUAL(1, ui->victoryNotifications[0]);

	delete ui;
}

/**
 * P2-3 (S3) acceptance criterion (checkForVictory condition-5 boundary,
 * complementary/negative case): with m_satharRetreat == 5, the Sathar
 * "extra" losses are all fighters and militia ships, so after filtering,
 * m_lostTendaySathar (3) does NOT exceed m_lostTendayUPF (3) -- they are
 * equal -- and checkForVictory() must NOT declare a UPF victory.
 *
 * Raw (unfiltered) Sathar losses this tenday total 12 (3 countable
 * Destroyers + 5 Fighters + 4 militia Destroyers) versus 3 for the UPF. The
 * pre-fix code incremented m_lostTendaySathar/m_lostTendayUPF unconditionally
 * for every destroyed ship (no fighter or militia exclusion), so it would
 * have compared the raw totals (12 > 3) and wrongly declared a UPF victory.
 * This assertion (no victory) fails against that unfixed counting and passes
 * against the fix, specifically proving the filtering feeds the victory
 * check rather than the raw destroyed-ship counts.
 */
void FGameTest::testCheckForVictoryCondition5NoVictoryWhenFilteredSatharLossesDoNotExceedUPFDespiteFighterAndMilitiaNoise(){
	delete m_g1;
	RetreatConditionMockUI *ui = new RetreatConditionMockUI(5);
	m_g1 = &(FGame::create(ui));
	CPPUNIT_ASSERT(m_g1->init(NULL) == 0);

	FPlayer *upf = m_g1->getPlayer(1);
	FPlayer *sathar = m_g1->getPlayer(2);
	CPPUNIT_ASSERT(upf != NULL);
	CPPUNIT_ASSERT(sathar != NULL);

	FSystem *sys = FMap::getMap().getSystem("Theseus");
	CPPUNIT_ASSERT(sys != NULL);

	// filtered Sathar tenday losses: 3 countable Destroyers
	addShipLossFleet(sathar, sys, "Test C5 Filtered NoWin Sathar Regulars", "Destroyer", 3, 0, false);
	// noise, excluded from the tenday counter: 5 Fighters
	addShipLossFleet(sathar, sys, "Test C5 Filtered NoWin Sathar Fighters", "Fighter", 5, 0, false);
	// noise, excluded from the tenday counter: 4 militia Destroyers
	addShipLossFleet(sathar, sys, "Test C5 Filtered NoWin Sathar Militia", "Destroyer", 4, 0, true);
	// filtered UPF tenday losses: 3 countable Destroyers -- equal to the
	// filtered Sathar count, so condition 5 ("exceeds") is NOT satisfied
	addShipLossFleet(upf, sys, "Test C5 Filtered NoWin UPF Regulars", "Destroyer", 3, 0, false);

	m_g1->endUPFTurn();
	advanceUPFTurns(m_g1, 10);

	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), ui->victoryNotifications.size());

	delete ui;
}

}
