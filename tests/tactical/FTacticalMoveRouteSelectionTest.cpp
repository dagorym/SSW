/**
 * @file FTacticalMoveRouteSelectionTest.cpp
 * @brief Implementation file for FTacticalGame move route selection regression tests
 * @author Tom Stephens
 * @date Created: Apr 04, 2026
 */

#include "FTacticalMoveRouteSelectionTest.h"

#include "ships/FFrigate.h"
#include "strategic/FFleet.h"
#include "tactical/FTacticalGame.h"

namespace FrontierTests {

namespace {

using namespace Frontier;

struct FMoveRouteFixture {
FTacticalGame game;
FleetList attackFleets;
FleetList defendFleets;
FFleet * attackFleet;
FFleet * defendFleet;
FFrigate * attacker;
FFrigate * defender;
FPoint attackerPos;
FPoint defenderPos;
};

void setupFixture(FMoveRouteFixture & fixture) {
fixture.attackFleet = new FFleet();
fixture.defendFleet = new FFleet();
fixture.attacker = new FFrigate();
fixture.defender = new FFrigate();
fixture.attackerPos = FPoint(15, 10);
fixture.defenderPos = FPoint(25, 25);

fixture.attackFleet->addShip(fixture.attacker);
fixture.defendFleet->addShip(fixture.defender);
fixture.attackFleets.push_back(fixture.attackFleet);
fixture.defendFleets.push_back(fixture.defendFleet);

fixture.game.setupFleets(&fixture.attackFleets, &fixture.defendFleets, false, NULL);
fixture.game.setState(BS_SetupDefendFleet);
fixture.game.setControlState(true);
fixture.game.setShip(fixture.defender);
CPPUNIT_ASSERT(fixture.game.placeShip(fixture.defenderPos));
CPPUNIT_ASSERT(fixture.game.setShipPlacementHeading(3));

fixture.game.setState(BS_SetupAttackFleet);
fixture.game.setControlState(true);
fixture.game.setShip(fixture.attacker);
CPPUNIT_ASSERT(fixture.game.placeShip(fixture.attackerPos));
CPPUNIT_ASSERT(fixture.game.setShipPlacementHeading(1));

fixture.game.setState(BS_Battle);
fixture.game.setMovingPlayer(true);
fixture.game.setPhase(PH_MOVE);
CPPUNIT_ASSERT(fixture.game.selectShipFromHex(fixture.attackerPos));
}

void destroyFixture(FMoveRouteFixture & fixture) {
for (FleetList::iterator itr = fixture.attackFleets.begin(); itr != fixture.attackFleets.end(); ++itr) {
delete *itr;
}
for (FleetList::iterator itr = fixture.defendFleets.begin(); itr != fixture.defendFleets.end(); ++itr) {
delete *itr;
}
fixture.attackFleets.clear();
fixture.defendFleets.clear();
}

FTacticalTurnData * requireTurnData(FMoveRouteFixture & fixture) {
FTacticalTurnData * turnData = fixture.game.findTurnData(fixture.attacker->getID());
CPPUNIT_ASSERT(turnData != NULL);
return turnData;
}

void assertHeadingMatchesPath(const FTacticalTurnData & turnData) {
const PointList path = turnData.path.getFullPath();
if (path.size() == 1) {
CPPUNIT_ASSERT_EQUAL(turnData.startHeading, turnData.curHeading);
} else {
CPPUNIT_ASSERT_EQUAL(FHexMap::computeHeading(path[path.size() - 2], path[path.size() - 1]), turnData.curHeading);
}
CPPUNIT_ASSERT_EQUAL(turnData.curHeading, turnData.finalHeading);
}

void assertSamePoint(const FPoint & expected, const FPoint & actual) {
CPPUNIT_ASSERT_EQUAL(expected.getX(), actual.getX());
CPPUNIT_ASSERT_EQUAL(expected.getY(), actual.getY());
}

}

CPPUNIT_TEST_SUITE_REGISTRATION( FTacticalMoveRouteSelectionTest );

void FTacticalMoveRouteSelectionTest::setUp() {
}

void FTacticalMoveRouteSelectionTest::tearDown() {
}

void FTacticalMoveRouteSelectionTest::testFirstHighlightedHexExtendsRouteByOne() {
FMoveRouteFixture fixture;
setupFixture(fixture);

const std::vector<FPoint> & moves = fixture.game.getMovementHexes();
CPPUNIT_ASSERT(!moves.empty());
const FPoint clicked = moves.front();

CPPUNIT_ASSERT(fixture.game.handleHexClick(clicked));

FTacticalTurnData * turnData = requireTurnData(fixture);
assertSamePoint(clicked, turnData->path.endPoint());
CPPUNIT_ASSERT_EQUAL(2U, turnData->path.getPathLength());
CPPUNIT_ASSERT_EQUAL(1, turnData->nMoved);
assertHeadingMatchesPath(*turnData);

destroyFixture(fixture);
}

void FTacticalMoveRouteSelectionTest::testLaterHighlightedHexBecomesRouteEndpoint() {
FMoveRouteFixture fixture;
setupFixture(fixture);

const std::vector<FPoint> & moves = fixture.game.getMovementHexes();
CPPUNIT_ASSERT(moves.size() >= 3);
const FPoint clicked = moves[2];

CPPUNIT_ASSERT(fixture.game.handleHexClick(clicked));

FTacticalTurnData * turnData = requireTurnData(fixture);
assertSamePoint(clicked, turnData->path.endPoint());
CPPUNIT_ASSERT_EQUAL(4U, turnData->path.getPathLength());
CPPUNIT_ASSERT_EQUAL(3, turnData->nMoved);
assertHeadingMatchesPath(*turnData);

destroyFixture(fixture);
}

void FTacticalMoveRouteSelectionTest::testSelectingEarlierPathHexTrimsRouteToClickedHex() {
FMoveRouteFixture fixture;
setupFixture(fixture);

const std::vector<FPoint> & moves = fixture.game.getMovementHexes();
CPPUNIT_ASSERT(moves.size() >= 3);
CPPUNIT_ASSERT(fixture.game.handleHexClick(moves[2]));

FTacticalTurnData * extendedTurnData = requireTurnData(fixture);
const FPoint earlierPathHex = extendedTurnData->path.getFullPath()[1];
CPPUNIT_ASSERT(fixture.game.handleHexClick(earlierPathHex));

FTacticalTurnData * trimmedTurnData = requireTurnData(fixture);
assertSamePoint(earlierPathHex, trimmedTurnData->path.endPoint());
CPPUNIT_ASSERT_EQUAL(2U, trimmedTurnData->path.getPathLength());
CPPUNIT_ASSERT_EQUAL(1, trimmedTurnData->nMoved);
assertHeadingMatchesPath(*trimmedTurnData);

destroyFixture(fixture);
}

}
