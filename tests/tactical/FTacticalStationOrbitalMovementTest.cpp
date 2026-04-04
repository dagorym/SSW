/**
 * @file FTacticalStationOrbitalMovementTest.cpp
 * @brief Implementation file for FTacticalGame station orbital movement tests
 * @author Tom Stephens
 * @date Created: Mar 26, 2026
 */

#include "FTacticalStationOrbitalMovementTest.h"

#include "ships/FFortifiedStation.h"
#include "ships/FFrigate.h"
#include "strategic/FFleet.h"
#include "tactical/FTacticalGame.h"

namespace FrontierTests {

namespace {

using namespace Frontier;

struct FOrbitFixture {
FTacticalGame game;
FleetList attackFleets;
FleetList defendFleets;
FFleet * attackFleet;
FFleet * defendFleet;
FFrigate * attacker;
FFrigate * defender;
FFortifiedStation * station;
};

int turnHeading(int heading, int turn) {
if (turn > 0) {
heading += turn;
if (heading > 5) {
heading -= 6;
}
} else if (turn < 0) {
heading += turn;
if (heading < 0) {
heading += 6;
}
}
return heading;
}

int computePlanetTurnDirection(const FPoint & planetPos, const FPoint & currentHex, int currentHeading) {
const int portBackDir = (currentHeading - 2 < 0) ? currentHeading + 4 : currentHeading - 2;
if (FHexMap::findNextHex(currentHex, portBackDir) == planetPos) {
return -1;
}
const int starboardBackDir = (currentHeading + 2 > 5) ? currentHeading - 4 : currentHeading + 2;
if (FHexMap::findNextHex(currentHex, starboardBackDir) == planetPos) {
return 1;
}
return 0;
}

int findOrbitalHeading(const FPoint & planetPos, const FPoint & stationPos) {
for (int heading = 0; heading < 6; ++heading) {
const FPoint nextHex = FHexMap::findNextHex(stationPos, heading);
if (nextHex.getX() == planetPos.getX() && nextHex.getY() == planetPos.getY()) {
continue;
}
if (computePlanetTurnDirection(planetPos, nextHex, heading) != 0) {
return heading;
}
}
return -1;
}

void setupOrbitFixture(FOrbitFixture & fixture) {
fixture.attackFleet = new FFleet();
fixture.defendFleet = new FFleet();
fixture.attacker = new FFrigate();
fixture.defender = new FFrigate();
fixture.station = new FFortifiedStation();

fixture.attackFleet->addShip(fixture.attacker);
fixture.defendFleet->addShip(fixture.defender);
fixture.attackFleets.push_back(fixture.attackFleet);
fixture.defendFleets.push_back(fixture.defendFleet);
}

void destroyOrbitFixture(FOrbitFixture & fixture) {
for (FleetList::iterator itr = fixture.attackFleets.begin(); itr != fixture.attackFleets.end(); ++itr) {
delete *itr;
}
for (FleetList::iterator itr = fixture.defendFleets.begin(); itr != fixture.defendFleets.end(); ++itr) {
delete *itr;
}
fixture.attackFleets.clear();
fixture.defendFleets.clear();
delete fixture.station;
}

void initializeStationOrbit(FOrbitFixture & fixture, const FPoint & planetPos, const FPoint & stationPos, int heading) {
fixture.station->setHeading(heading);
fixture.game.setupFleets(&fixture.attackFleets, &fixture.defendFleets, true, fixture.station);
fixture.game.setControlState(true);
CPPUNIT_ASSERT(fixture.game.placePlanet(planetPos));
CPPUNIT_ASSERT(fixture.game.placeStation(stationPos));
fixture.game.setState(BS_Battle);
fixture.game.setMovingPlayer(false);
}

void runDefenderMove(FOrbitFixture & fixture) {
fixture.game.setMovingPlayer(false);
fixture.game.resetMovementState();
fixture.game.completeMovePhase();
}

}

CPPUNIT_TEST_SUITE_REGISTRATION( FTacticalStationOrbitalMovementTest );

void FTacticalStationOrbitalMovementTest::setUp() {
}

void FTacticalStationOrbitalMovementTest::tearDown() {
}

void FTacticalStationOrbitalMovementTest::testStationHeadingUpdatedAfterOrbit() {
FOrbitFixture fixture;
setupOrbitFixture(fixture);

const FPoint planetPos(20, 20);
const FPoint stationPos = FHexMap::findNextHex(planetPos, 0);
const int startHeading = findOrbitalHeading(planetPos, stationPos);
CPPUNIT_ASSERT(startHeading >= 0);

initializeStationOrbit(fixture, planetPos, stationPos, startHeading);
runDefenderMove(fixture);

const FPoint orbitHex = fixture.game.getStationPos();
const int turnDir = computePlanetTurnDirection(planetPos, orbitHex, startHeading);
const int expectedHeading = turnHeading(startHeading, turnDir);

CPPUNIT_ASSERT_EQUAL(1, FHexMap::computeHexDistance(planetPos, orbitHex));
CPPUNIT_ASSERT(turnDir != 0);
CPPUNIT_ASSERT_EQUAL(expectedHeading, fixture.station->getHeading());

destroyOrbitFixture(fixture);
}

void FTacticalStationOrbitalMovementTest::testStationSpeedPreservedAfterOrbit() {
FOrbitFixture fixture;
setupOrbitFixture(fixture);

const FPoint planetPos(20, 20);
const FPoint stationPos = FHexMap::findNextHex(planetPos, 0);
const int startHeading = findOrbitalHeading(planetPos, stationPos);
CPPUNIT_ASSERT(startHeading >= 0);

initializeStationOrbit(fixture, planetPos, stationPos, startHeading);
const int initialSpeed = fixture.station->getSpeed();
runDefenderMove(fixture);
runDefenderMove(fixture);

CPPUNIT_ASSERT_EQUAL(initialSpeed, fixture.station->getSpeed());
CPPUNIT_ASSERT_EQUAL(1, FHexMap::computeHexDistance(planetPos, fixture.game.getStationPos()));

destroyOrbitFixture(fixture);
}

void FTacticalStationOrbitalMovementTest::testNonStationHeadingUnchangedByFix() {
FOrbitFixture fixture;
setupOrbitFixture(fixture);

fixture.game.setupFleets(&fixture.attackFleets, &fixture.defendFleets, false, NULL);
fixture.game.setState(BS_SetupDefendFleet);
fixture.game.setControlState(true);
fixture.game.setShip(fixture.defender);
CPPUNIT_ASSERT(fixture.game.placeShip(FPoint(10, 10)));
CPPUNIT_ASSERT(fixture.game.setShipPlacementHeading(2));

fixture.game.setState(BS_SetupAttackFleet);
fixture.game.setControlState(true);
fixture.game.setShip(fixture.attacker);
CPPUNIT_ASSERT(fixture.game.placeShip(FPoint(15, 10)));
CPPUNIT_ASSERT(fixture.game.setShipPlacementHeading(1));

fixture.game.setState(BS_Battle);
fixture.game.setMovingPlayer(true);
fixture.game.setPhase(PH_MOVE);
CPPUNIT_ASSERT(fixture.game.selectShipFromHex(FPoint(15, 10)));
const std::vector<FPoint> & moves = fixture.game.getMovementHexes();
CPPUNIT_ASSERT(!moves.empty());
CPPUNIT_ASSERT(fixture.game.handleHexClick(moves.front()));

const FTacticalTurnData * turnData = fixture.game.findTurnData(fixture.attacker->getID());
CPPUNIT_ASSERT(turnData != NULL);
const int expectedHeading = turnData->curHeading;

fixture.game.completeMovePhase();

CPPUNIT_ASSERT_EQUAL(expectedHeading, fixture.attacker->getHeading());

destroyOrbitFixture(fixture);
}

}
