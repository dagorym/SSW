/**
 * @file FGameTest.cpp
 * @brief Implementation file for the FGameTest class
 * @author Tom Stephens
 * @date Created: Jun 19, 2009
 */

#include "FGameTest.h"
#include <cstdio>
#include <fstream>
#include "strategic/FFleet.h"
#include "strategic/FSystem.h"
#include "strategic/FMap.h"
#include "strategic/FPlayer.h"
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

}
