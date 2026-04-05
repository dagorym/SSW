/**
 * @file FTacticalShipPlacementOrientationTest.cpp
 * @brief Runtime regression tests for two-step ship placement orientation
 * @author Tom Stephens
 * @date Created: Apr 05, 2026
 */

#include "FTacticalShipPlacementOrientationTest.h"

#include "core/FHexMap.h"
#include "strategic/FFleet.h"
#include "tactical/FTacticalGame.h"

namespace FrontierTests {

namespace {

using namespace Frontier;

class FPlacementHarnessShip : public FVehicle {
public:
	FPlacementHarnessShip(const std::string & name) {
		m_name = name;
		m_type = "PlacementHarness";
		m_maxHP = 10;
		m_currentHP = 10;
		m_maxADF = 1;
		m_currentADF = 1;
		m_maxMR = 1;
		m_currentMR = 1;
		m_maxDCR = 1;
		m_currentDCR = 1;
		m_speed = 0;
		m_heading = 0;
	}
};

struct FPlacementFixture {
	FTacticalGame game;
	FleetList attackFleets;
	FleetList defendFleets;
	FFleet * attackFleet;
	FFleet * defendFleet;
	FPlacementHarnessShip * attacker;
	FPlacementHarnessShip * defender;
};

void destroyFixture(FPlacementFixture & fixture) {
	for (FleetList::iterator itr = fixture.attackFleets.begin(); itr != fixture.attackFleets.end(); ++itr) {
		delete *itr;
	}
	for (FleetList::iterator itr = fixture.defendFleets.begin(); itr != fixture.defendFleets.end(); ++itr) {
		delete *itr;
	}
	fixture.attackFleets.clear();
	fixture.defendFleets.clear();
}

void setupFixture(FPlacementFixture & fixture) {
	fixture.attackFleet = new FFleet();
	fixture.defendFleet = new FFleet();
	fixture.attacker = new FPlacementHarnessShip("PlacementAttacker");
	fixture.defender = new FPlacementHarnessShip("PlacementDefender");
	fixture.attackFleet->addShip(fixture.attacker);
	fixture.defendFleet->addShip(fixture.defender);
	fixture.attackFleets.push_back(fixture.attackFleet);
	fixture.defendFleets.push_back(fixture.defendFleet);
	fixture.game.setupFleets(&fixture.attackFleets, &fixture.defendFleets, false, NULL);
}

void beginPlacement(FPlacementFixture & fixture, FVehicle * ship, int state, const FPoint & placeHex) {
	fixture.game.setState(state);
	fixture.game.setControlState(true);
	fixture.game.setShip(ship);
	CPPUNIT_ASSERT(fixture.game.handleHexClick(placeHex));
	CPPUNIT_ASSERT(fixture.game.hasShipPlacementPendingRotation());
	CPPUNIT_ASSERT_EQUAL(placeHex.getX(), fixture.game.getSelectedShipHex().getX());
	CPPUNIT_ASSERT_EQUAL(placeHex.getY(), fixture.game.getSelectedShipHex().getY());
}

}

CPPUNIT_TEST_SUITE_REGISTRATION( FTacticalShipPlacementOrientationTest );

void FTacticalShipPlacementOrientationTest::setUp() {
}

void FTacticalShipPlacementOrientationTest::tearDown() {
}

void FTacticalShipPlacementOrientationTest::testPreviewByHexUpdatesHeadingWithoutAdvancingPlacement() {
	FPlacementFixture fixture;
	setupFixture(fixture);

	const FPoint placeHex(10, 10);
	beginPlacement(fixture, fixture.defender, BS_SetupDefendFleet, placeHex);

	const int previewHeading = 2;
	const FPoint previewHex = FHexMap::findNextHex(placeHex, previewHeading);

	CPPUNIT_ASSERT(fixture.game.setShipPlacementHeadingByHex(previewHex));
	CPPUNIT_ASSERT_EQUAL(previewHeading, fixture.defender->getHeading());
	CPPUNIT_ASSERT(fixture.game.hasShipPlacementPendingRotation());
	CPPUNIT_ASSERT(fixture.game.getControlState());
	CPPUNIT_ASSERT(fixture.game.getPhase() != PH_SET_SPEED);

	destroyFixture(fixture);
}

void FTacticalShipPlacementOrientationTest::testDefenderSetupSecondClickCommitsLastPreviewedHeading() {
	FPlacementFixture fixture;
	setupFixture(fixture);

	const FPoint placeHex(12, 12);
	beginPlacement(fixture, fixture.defender, BS_SetupDefendFleet, placeHex);

	const int firstPreviewHeading = 5;
	const int finalPreviewHeading = 1;
	CPPUNIT_ASSERT(fixture.game.setShipPlacementHeadingByHex(FHexMap::findNextHex(placeHex, firstPreviewHeading)));
	CPPUNIT_ASSERT_EQUAL(firstPreviewHeading, fixture.defender->getHeading());
	CPPUNIT_ASSERT(fixture.game.setShipPlacementHeadingByHex(FHexMap::findNextHex(placeHex, finalPreviewHeading)));
	CPPUNIT_ASSERT_EQUAL(finalPreviewHeading, fixture.defender->getHeading());

	CPPUNIT_ASSERT(fixture.game.handleHexClick(placeHex));
	CPPUNIT_ASSERT_EQUAL(finalPreviewHeading, fixture.defender->getHeading());
	CPPUNIT_ASSERT(!fixture.game.hasShipPlacementPendingRotation());
	CPPUNIT_ASSERT(!fixture.game.getControlState());
	CPPUNIT_ASSERT_EQUAL(static_cast<int>(PH_SET_SPEED), fixture.game.getPhase());

	destroyFixture(fixture);
}

void FTacticalShipPlacementOrientationTest::testAttackerSetupSecondClickCommitsLastPreviewedHeading() {
	FPlacementFixture fixture;
	setupFixture(fixture);

	const FPoint defenderHex(8, 8);
	beginPlacement(fixture, fixture.defender, BS_SetupDefendFleet, defenderHex);
	CPPUNIT_ASSERT(fixture.game.setShipPlacementHeading(3));

	const FPoint attackerHex(42, 20);
	beginPlacement(fixture, fixture.attacker, BS_SetupAttackFleet, attackerHex);

	const int previewHeading = 4;
	CPPUNIT_ASSERT(fixture.game.setShipPlacementHeadingByHex(FHexMap::findNextHex(attackerHex, previewHeading)));
	CPPUNIT_ASSERT_EQUAL(previewHeading, fixture.attacker->getHeading());
	CPPUNIT_ASSERT(fixture.game.handleHexClick(attackerHex));
	CPPUNIT_ASSERT_EQUAL(previewHeading, fixture.attacker->getHeading());
	CPPUNIT_ASSERT(!fixture.game.hasShipPlacementPendingRotation());
	CPPUNIT_ASSERT(!fixture.game.getControlState());
	CPPUNIT_ASSERT_EQUAL(static_cast<int>(PH_SET_SPEED), fixture.game.getPhase());

	destroyFixture(fixture);
}

}
