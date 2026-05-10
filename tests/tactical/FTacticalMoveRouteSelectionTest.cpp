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

void setupFixture(FMoveRouteFixture & fixture, int attackerSpeed = -1, int attackerMR = -1) {
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
if (attackerSpeed >= 0) {
fixture.attacker->setSpeed(attackerSpeed);
}
if (attackerMR >= 0) {
fixture.attacker->setMR(static_cast<unsigned int>(attackerMR));
}

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

bool listContainsPoint(const std::vector<FPoint> & points, const FPoint & point) {
for (std::vector<FPoint>::const_iterator itr = points.begin(); itr != points.end(); ++itr) {
if (itr->getX() == point.getX() && itr->getY() == point.getY()) {
return true;
}
}
return false;
}

bool pointsEqual(const FPoint & left, const FPoint & right) {
return left.getX() == right.getX() && left.getY() == right.getY();
}

bool hexContainsShipWithID(const VehicleList & ships, unsigned int shipID) {
for (VehicleList::const_iterator itr = ships.begin(); itr != ships.end(); ++itr) {
if ((*itr) != NULL && (*itr)->getID() == shipID) {
return true;
}
}
return false;
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

void FTacticalMoveRouteSelectionTest::testStoppedShipCanChooseAnyAdjacentFacingWithoutDisplacement() {
FMoveRouteFixture fixture;
setupFixture(fixture, 0);

FTacticalTurnData * turnData = requireTurnData(fixture);
const int originalHeading = turnData->curHeading;
for (int heading = 0; heading < 6; ++heading) {
if (heading == originalHeading) {
continue;
}
FMoveRouteFixture headingFixture;
setupFixture(headingFixture, 0);
const FPoint adjacent = FHexMap::findNextHex(headingFixture.attackerPos, heading);
CPPUNIT_ASSERT(headingFixture.game.handleHexClick(adjacent));
FTacticalTurnData * headingTurnData = requireTurnData(headingFixture);
CPPUNIT_ASSERT_EQUAL(heading, headingTurnData->startHeading);
CPPUNIT_ASSERT_EQUAL(heading, headingTurnData->curHeading);
CPPUNIT_ASSERT_EQUAL(heading, headingTurnData->finalHeading);
CPPUNIT_ASSERT_EQUAL(0, headingTurnData->nMoved);
CPPUNIT_ASSERT_EQUAL(1U, headingTurnData->path.getPathLength());
assertSamePoint(headingFixture.attackerPos, headingTurnData->path.startPoint());
assertSamePoint(headingFixture.attackerPos, headingTurnData->path.endPoint());
destroyFixture(headingFixture);
}

const int selectedHeading = (originalHeading + 1) % 6;
const FPoint selectedHex = FHexMap::findNextHex(fixture.attackerPos, selectedHeading);
CPPUNIT_ASSERT(fixture.game.handleHexClick(selectedHex));

turnData = requireTurnData(fixture);
CPPUNIT_ASSERT_EQUAL(selectedHeading, turnData->startHeading);
CPPUNIT_ASSERT_EQUAL(selectedHeading, turnData->curHeading);
CPPUNIT_ASSERT_EQUAL(selectedHeading, turnData->finalHeading);
CPPUNIT_ASSERT_EQUAL(0, turnData->nMoved);
CPPUNIT_ASSERT_EQUAL(1U, turnData->path.getPathLength());
assertSamePoint(fixture.attackerPos, turnData->path.startPoint());
assertSamePoint(fixture.attackerPos, turnData->path.endPoint());

destroyFixture(fixture);
}

void FTacticalMoveRouteSelectionTest::testStoppedShipFacingOnlyMoveCommitsHeadingWithoutChangingHexOrSpeed() {
FMoveRouteFixture fixture;
setupFixture(fixture, 0);

FTacticalTurnData * turnData = requireTurnData(fixture);
const int selectedHeading = (turnData->curHeading + 1) % 6;
const FPoint selectedHex = FHexMap::findNextHex(fixture.attackerPos, selectedHeading);
CPPUNIT_ASSERT(fixture.game.handleHexClick(selectedHex));

fixture.game.completeMovePhase();

CPPUNIT_ASSERT_EQUAL(0, fixture.attacker->getSpeed());
CPPUNIT_ASSERT_EQUAL(selectedHeading, fixture.attacker->getHeading());
CPPUNIT_ASSERT(hexContainsShipWithID(fixture.game.getHexOccupants(fixture.attackerPos), fixture.attacker->getID()));
CPPUNIT_ASSERT(!hexContainsShipWithID(fixture.game.getHexOccupants(selectedHex), fixture.attacker->getID()));

destroyFixture(fixture);
}

void FTacticalMoveRouteSelectionTest::testStoppedShipForwardMoveUsesSelectedFacingForFirstHex() {
FMoveRouteFixture fixture;
setupFixture(fixture, 0);

FTacticalTurnData * turnData = requireTurnData(fixture);
const int originalHeading = turnData->curHeading;
const int selectedHeading = (originalHeading + 1) % 6;
const FPoint selectedHex = FHexMap::findNextHex(fixture.attackerPos, selectedHeading);
CPPUNIT_ASSERT(selectedHeading != originalHeading);
CPPUNIT_ASSERT(fixture.game.handleHexClick(selectedHex));

const FPoint expectedFirstMove = FHexMap::findNextHex(fixture.attackerPos, selectedHeading);
const FPoint oldFirstMove = FHexMap::findNextHex(fixture.attackerPos, originalHeading);
CPPUNIT_ASSERT(!pointsEqual(expectedFirstMove, oldFirstMove));
CPPUNIT_ASSERT(listContainsPoint(fixture.game.getMovementHexes(), expectedFirstMove));
CPPUNIT_ASSERT(fixture.game.handleHexClick(expectedFirstMove));

turnData = requireTurnData(fixture);
const PointList path = turnData->path.getFullPath();
CPPUNIT_ASSERT(path.size() >= 2);
assertSamePoint(expectedFirstMove, path[1]);
CPPUNIT_ASSERT_EQUAL(1, turnData->nMoved);

destroyFixture(fixture);
}

void FTacticalMoveRouteSelectionTest::testNonStoppedShipDoesNotAllowFreeAdjacentFacingSelection() {
FMoveRouteFixture fixture;
setupFixture(fixture, 1);

FTacticalTurnData * turnData = requireTurnData(fixture);
const int originalHeading = turnData->curHeading;
FPoint nonForwardAdjacent(-1, -1);
for (int heading = 0; heading < 6; ++heading) {
if (heading == originalHeading) {
continue;
}
const FPoint adjacent = FHexMap::findNextHex(fixture.attackerPos, heading);
if (!listContainsPoint(fixture.game.getMovementHexes(), adjacent)) {
nonForwardAdjacent = adjacent;
break;
}
}
CPPUNIT_ASSERT(nonForwardAdjacent.getX() >= 0);
const bool selected = fixture.game.handleHexClick(nonForwardAdjacent);
turnData = requireTurnData(fixture);
if (selected) {
CPPUNIT_ASSERT(turnData->nMoved > 0);
CPPUNIT_ASSERT(turnData->path.getPathLength() > 1);
CPPUNIT_ASSERT_EQUAL(originalHeading, turnData->startHeading);
} else {
CPPUNIT_ASSERT_EQUAL(originalHeading, turnData->curHeading);
CPPUNIT_ASSERT_EQUAL(1U, turnData->path.getPathLength());
}

turnData = requireTurnData(fixture);
CPPUNIT_ASSERT_EQUAL(originalHeading, turnData->startHeading);
destroyFixture(fixture);

FMoveRouteFixture moveFixture;
setupFixture(moveFixture, 1);
CPPUNIT_ASSERT(!moveFixture.game.getMovementHexes().empty());
const FPoint forwardHex = moveFixture.game.getMovementHexes().front();
CPPUNIT_ASSERT(moveFixture.game.handleHexClick(forwardHex));
FTacticalTurnData * moveTurnData = requireTurnData(moveFixture);
CPPUNIT_ASSERT(moveTurnData->path.getPathLength() > 1);
CPPUNIT_ASSERT(moveTurnData->nMoved > 0);

destroyFixture(moveFixture);
}

void FTacticalMoveRouteSelectionTest::testStoppedShipWithZeroMRCannotFreelyRotate() {
FMoveRouteFixture fixture;
setupFixture(fixture, 0, 0);

FTacticalTurnData * turnData = requireTurnData(fixture);
const int originalHeading = turnData->curHeading;
const FPoint adjacentLeft = FHexMap::findNextHex(fixture.attackerPos, (originalHeading + 1) % 6);
CPPUNIT_ASSERT(!fixture.game.handleHexClick(adjacentLeft));
CPPUNIT_ASSERT(fixture.game.getLeftTurnHexes().empty());
CPPUNIT_ASSERT(fixture.game.getRightTurnHexes().empty());

turnData = requireTurnData(fixture);
CPPUNIT_ASSERT_EQUAL(originalHeading, turnData->curHeading);
CPPUNIT_ASSERT_EQUAL(1U, turnData->path.getPathLength());
CPPUNIT_ASSERT_EQUAL(0, turnData->nMoved);

destroyFixture(fixture);
}

}
