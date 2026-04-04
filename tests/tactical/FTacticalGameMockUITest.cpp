/**
 * @file FTacticalGameMockUITest.cpp
 * @brief Implementation file for FTacticalGame mock UI runtime tests
 */

#include "FTacticalGameMockUITest.h"

#include <vector>

#include "strategic/FFleet.h"
#include "tactical/FTacticalGame.h"
#include "tactical/ITacticalUI.h"

namespace FrontierTests {

namespace {

using namespace Frontier;

class FDeterministicKillWeaponHarness : public FWeapon {
public:
FDeterministicKillWeaponHarness() {
m_name = "TST";
m_fullName = "Deterministic Kill Harness";
m_type = FWeapon::LC;
m_range = 9;
m_nDice = 1;
m_dMod = 0;
m_baseToHitProb = 100;
m_damageTableMod = 0;
}

void aimAt(FVehicle * target) {
m_target = target;
m_targetRange = 1;
m_isHeadOn = false;
}
};

class FDeterministicShipHarness : public FVehicle {
public:
FDeterministicShipHarness() {
m_name = "DeterministicShip";
m_type = "DeterministicHarness";
}

void configureOneHullPoint() {
m_maxHP = 1;
m_currentHP = 1;
m_maxADF = 1;
m_currentADF = 1;
m_maxMR = 1;
m_currentMR = 1;
m_maxDCR = 1;
m_currentDCR = 1;
}

void addWeapon(FWeapon * weapon) {
m_weapons.push_back(weapon);
weapon->setParent(this);
}
};

class FMockTacticalUI : public ITacticalUI {
public:
FMockTacticalUI()
: redrawCount(0),
  showMessageCount(0),
  showDamageSummaryCount(0),
  runICMSelectionCount(0),
  notifyWinnerCount(0),
  lastAttackerWins(false) {
}

void requestRedraw() {
redrawCount++;
}

void showMessage(const std::string&, const std::string&) {
showMessageCount++;
}

int showDamageSummary(const FTacticalCombatReportSummary&) {
showDamageSummaryCount++;
return 0;
}

int runICMSelection(std::vector<ICMData*>&, VehicleList*) {
runICMSelectionCount++;
return 0;
}

void notifyWinner(bool attackerWins) {
notifyWinnerCount++;
lastAttackerWins = attackerWins;
}

int redrawCount;
int showMessageCount;
int showDamageSummaryCount;
int runICMSelectionCount;
int notifyWinnerCount;
bool lastAttackerWins;
};

struct RuntimeFixture {
FTacticalGame game;
FleetList attackFleets;
FleetList defendFleets;
FFleet * attackFleet;
FFleet * defendFleet;
FDeterministicShipHarness * attacker;
FDeterministicShipHarness * defender;
FDeterministicKillWeaponHarness * weapon;
FMockTacticalUI ui;
};

void setupDeterministicBattle(RuntimeFixture & fixture) {
fixture.attackFleet = new FFleet();
fixture.defendFleet = new FFleet();
fixture.attackFleets.push_back(fixture.attackFleet);
fixture.defendFleets.push_back(fixture.defendFleet);

fixture.attacker = new FDeterministicShipHarness();
fixture.defender = new FDeterministicShipHarness();
fixture.attacker->setName("Attacker");
fixture.defender->setName("Defender");
fixture.attacker->configureOneHullPoint();
fixture.defender->configureOneHullPoint();

fixture.weapon = new FDeterministicKillWeaponHarness();
fixture.weapon->aimAt(fixture.defender);
fixture.attacker->addWeapon(fixture.weapon);

fixture.attackFleet->addShip(fixture.attacker);
fixture.defendFleet->addShip(fixture.defender);

fixture.game.installUI(&fixture.ui);
fixture.game.setupFleets(&fixture.attackFleets, &fixture.defendFleets, false, NULL);
fixture.game.setState(BS_Battle);
fixture.game.setPhase(PH_ATTACK_FIRE);
fixture.game.setActivePlayer(true);
}

void destroyFixture(RuntimeFixture & fixture) {
fixture.game.installUI(NULL);
for (FleetList::iterator itr = fixture.attackFleets.begin(); itr != fixture.attackFleets.end(); ++itr) {
delete *itr;
}
for (FleetList::iterator itr = fixture.defendFleets.begin(); itr != fixture.defendFleets.end(); ++itr) {
delete *itr;
}
fixture.attackFleets.clear();
fixture.defendFleets.clear();
}

}

CPPUNIT_TEST_SUITE_REGISTRATION( FTacticalGameMockUITest );

void FTacticalGameMockUITest::setUp() {
}

void FTacticalGameMockUITest::tearDown() {
}

void FTacticalGameMockUITest::testDeterministicTwoShipCombatDeclaresAttackerWinnerViaModelState() {
RuntimeFixture fixture;
setupDeterministicBattle(fixture);

fixture.game.fireAllWeapons();

CPPUNIT_ASSERT(fixture.game.isCombatOver());
CPPUNIT_ASSERT(fixture.game.hasWinner());
CPPUNIT_ASSERT_EQUAL(fixture.game.getAttackerID(), fixture.game.getWinnerID());
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(0), static_cast<unsigned int>(fixture.game.getShipList(fixture.game.getDefenderID()).size()));
CPPUNIT_ASSERT_EQUAL(0, fixture.ui.notifyWinnerCount);
CPPUNIT_ASSERT_EQUAL(0, fixture.ui.runICMSelectionCount);
CPPUNIT_ASSERT_EQUAL(0, fixture.ui.showDamageSummaryCount);
CPPUNIT_ASSERT_EQUAL(0, fixture.ui.showMessageCount);
CPPUNIT_ASSERT_EQUAL(0, fixture.ui.redrawCount);

fixture.ui.notifyWinner(fixture.game.getWinnerID() == fixture.game.getAttackerID());
CPPUNIT_ASSERT_EQUAL(1, fixture.ui.notifyWinnerCount);
CPPUNIT_ASSERT_EQUAL(true, fixture.ui.lastAttackerWins);

destroyFixture(fixture);
}

}
