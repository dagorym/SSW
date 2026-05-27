/**
 * @file FTacticalSeekerMovementTest.cpp
 * @brief Source-inspection tests for tactical seeker movement helper logic
 */

#include "FTacticalSeekerMovementTest.h"

#include "core/FHexMap.h"
#include "tactical/FTacticalGame.h"

#include <algorithm>
#include <fstream>
#include <iterator>
#include <set>

namespace FrontierTests {

namespace {

std::string repoFile(const std::string & relativePath) {
return std::string(TACTICAL_TEST_REPO_ROOT) + "/" + relativePath;
}

class TestableTacticalGame : public Frontier::FTacticalGame {
public:
	using Frontier::FTacticalGame::adjustSeekerHeadingTowardTarget;
	using Frontier::FTacticalGame::moveSeekerTowardTarget;
	using Frontier::FTacticalGame::computeSeekerMovementAllowance;
	using Frontier::FTacticalGame::collectClosestSeekerTargetIDs;
	using Frontier::FTacticalGame::computeSeekerGreedyNextStep;
};

}

CPPUNIT_TEST_SUITE_REGISTRATION( FTacticalSeekerMovementTest );

void FTacticalSeekerMovementTest::setUp() {
}

void FTacticalSeekerMovementTest::tearDown() {
}

std::string FTacticalSeekerMovementTest::readFile(const std::string & path) {
std::ifstream file(path.c_str());
CPPUNIT_ASSERT_MESSAGE(path, file.is_open());
return std::string((std::istreambuf_iterator<char>(file)),
std::istreambuf_iterator<char>());
}

std::string FTacticalSeekerMovementTest::extractFunctionBody(const std::string & source, const std::string & signature) {
std::string::size_type signaturePos = source.find(signature);
CPPUNIT_ASSERT_MESSAGE(signature, signaturePos != std::string::npos);

std::string::size_type bodyStart = source.find('{', signaturePos);
CPPUNIT_ASSERT(bodyStart != std::string::npos);

int depth = 1;
for (std::string::size_type i = bodyStart + 1; i < source.size(); ++i) {
if (source[i] == '{') {
depth++;
} else if (source[i] == '}') {
depth--;
if (depth == 0) {
return source.substr(bodyStart + 1, i - bodyStart - 1);
}
}
}

CPPUNIT_FAIL("Function body not terminated");
return "";
}

void FTacticalSeekerMovementTest::assertContains(const std::string & haystack, const std::string & needle) {
CPPUNIT_ASSERT_MESSAGE(
std::string("Expected to find '") + needle + "' in inspected source",
haystack.find(needle) != std::string::npos);
}

void FTacticalSeekerMovementTest::testSeekerTargetSelectionFiltersAndTieBreaksAcrossSides() {
const std::string source = readFile(repoFile("src/tactical/FTacticalGame.cpp"));
const std::string resolveBody = extractFunctionBody(source, "void FTacticalGame::resolveActiveSeekersForMovingPlayer()");
const std::string validTargetBody = extractFunctionBody(source, "bool FTacticalGame::isValidSeekerTarget(const FVehicle * ship) const");
const std::string selectTargetBody = extractFunctionBody(source,
"FVehicle * FTacticalGame::selectClosestSeekerTarget(");
const std::string tieBreakBody = extractFunctionBody(source,
"unsigned int FTacticalGame::chooseRandomSeekerIndex(unsigned int candidateCount) const");

assertContains(resolveBody, "candidateTargets.insert(candidateTargets.end(), m_attackShips->begin(), m_attackShips->end());");
assertContains(resolveBody, "candidateTargets.insert(candidateTargets.end(), m_defendShips->begin(), m_defendShips->end());");
assertContains(resolveBody, "if (!itr->active || itr->ownerID != getMovingPlayerID()) {");
assertContains(resolveBody, "FVehicle * target = selectClosestSeekerTarget(*itr, candidateTargets);");

assertContains(validTargetBody, "ship != NULL");
assertContains(validTargetBody, "ship->getHP() > 0");
assertContains(validTargetBody, "!isStationType(const_cast<FVehicle *>(ship));");

assertContains(selectTargetBody, "if (!isValidSeekerTarget(*itr)) {");
assertContains(selectTargetBody, "const int distance = FHexMap::computeHexDistance(seeker.hex, shipHex);");
assertContains(selectTargetBody, "} else if (distance == closestDistance) {");
assertContains(selectTargetBody, "return closestShips[chooseRandomSeekerIndex(static_cast<unsigned int>(closestShips.size()))];");

assertContains(tieBreakBody, "if (candidateCount <= 1) {");
assertContains(tieBreakBody, "return static_cast<unsigned int>(irand(candidateCount) - 1);");
}

void FTacticalSeekerMovementTest::testSeekerHeadingAndMovementGreedyTurnLimits() {
const std::string source = readFile(repoFile("src/tactical/FTacticalGame.cpp"));
const std::string adjustBody = extractFunctionBody(source,
"void FTacticalGame::adjustSeekerHeadingTowardTarget(");
const std::string moveBody = extractFunctionBody(source,
"void FTacticalGame::moveSeekerTowardTarget(FTacticalSeekerMissileState & seeker, const FPoint & targetHex) const");

assertContains(adjustBody, "int delta = shortestHeadingDelta(normalizeHeading(seeker.heading), targetHeading);");
assertContains(adjustBody, "if (delta > 3) {");
assertContains(adjustBody, "delta = 3;");
assertContains(adjustBody, "} else if (delta < -3) {");
assertContains(adjustBody, "delta = -3;");
assertContains(adjustBody, "seeker.heading = normalizeHeading(seeker.heading + delta);");

assertContains(moveBody, "for (int step = 0; step < seeker.movementAllowance; ++step) {");
assertContains(moveBody, "for (int turn = -1; turn <= 1; ++turn) {");
assertContains(moveBody, "const int candidateHeading = normalizeHeading(seeker.heading + turn);");
assertContains(moveBody, "const FPoint candidateHex = FHexMap::findNextHex(seeker.hex, candidateHeading);");
assertContains(moveBody, "const int candidateDistance = FHexMap::computeHexDistance(candidateHex, targetHex);");
assertContains(moveBody, "|| candidateDistance < bestDistance");
assertContains(moveBody, "|| (candidateDistance == bestDistance && turnMagnitude < bestTurnMagnitude)) {");
assertContains(moveBody, "seeker.heading = bestHeading;");
assertContains(moveBody, "seeker.hex = bestHex;");

TestableTacticalGame game;
Frontier::FTacticalSeekerMissileState seeker;
seeker.hex = Frontier::FPoint(20, 20);
seeker.heading = 0;
seeker.movementAllowance = 1;

Frontier::FPoint targetHex = seeker.hex;
for (int i = 0; i < 5; ++i) {
	targetHex = Frontier::FHexMap::findNextHex(targetHex, 3);
}
game.adjustSeekerHeadingTowardTarget(seeker, targetHex);
CPPUNIT_ASSERT_EQUAL(3, seeker.heading);

const Frontier::FPoint startHex = seeker.hex;
game.moveSeekerTowardTarget(seeker, targetHex);
const int headingDelta = (seeker.heading - 3 + 6) % 6;
CPPUNIT_ASSERT(headingDelta == 0 || headingDelta == 1 || headingDelta == 5);
CPPUNIT_ASSERT(seeker.hex == Frontier::FHexMap::findNextHex(startHex, seeker.heading));
}

void FTacticalSeekerMovementTest::testSeekerMovementAllowanceProgression() {
const std::string source = readFile(repoFile("src/tactical/FTacticalGame.cpp"));
const std::string resolveBody = extractFunctionBody(source, "void FTacticalGame::resolveActiveSeekersForMovingPlayer()");
const std::string allowanceBody = extractFunctionBody(source,
"unsigned int FTacticalGame::computeSeekerMovementAllowance(int movementTurn) const");

assertContains(resolveBody, "itr->movementTurn += 1;");
assertContains(resolveBody, "itr->movementAllowance = static_cast<int>(computeSeekerMovementAllowance(itr->movementTurn));");
assertContains(allowanceBody, "if (movementTurn <= 0) {");
assertContains(allowanceBody, "return 0;");
assertContains(allowanceBody, "const unsigned int allowance = static_cast<unsigned int>(movementTurn * 2);");
assertContains(allowanceBody, "return (allowance > 12) ? 12 : allowance;");

TestableTacticalGame game;
CPPUNIT_ASSERT_EQUAL(0u, game.computeSeekerMovementAllowance(0));
CPPUNIT_ASSERT_EQUAL(2u, game.computeSeekerMovementAllowance(1));
CPPUNIT_ASSERT_EQUAL(4u, game.computeSeekerMovementAllowance(2));
CPPUNIT_ASSERT_EQUAL(6u, game.computeSeekerMovementAllowance(3));
CPPUNIT_ASSERT_EQUAL(8u, game.computeSeekerMovementAllowance(4));
CPPUNIT_ASSERT_EQUAL(10u, game.computeSeekerMovementAllowance(5));
CPPUNIT_ASSERT_EQUAL(12u, game.computeSeekerMovementAllowance(6));
CPPUNIT_ASSERT_EQUAL(12u, game.computeSeekerMovementAllowance(9));
}

void FTacticalSeekerMovementTest::testDeterministicTargetSelectionHelperFiltersAndReturnsTies() {
TestableTacticalGame game;
Frontier::FTacticalSeekerMissileState seeker;
seeker.hex = Frontier::FPoint(10, 10);
seeker.heading = 0;

std::vector<Frontier::FTacticalSeekerTargetSnapshot> candidates;
Frontier::FTacticalSeekerTargetSnapshot targetA = {101u, Frontier::FPoint(12, 10), 8, false};
Frontier::FTacticalSeekerTargetSnapshot targetB = {202u, Frontier::FPoint(9, 12), 6, false};
Frontier::FTacticalSeekerTargetSnapshot station = {303u, Frontier::FPoint(10, 9), 15, true};
Frontier::FTacticalSeekerTargetSnapshot destroyed = {404u, Frontier::FPoint(11, 10), 0, false};
Frontier::FTacticalSeekerTargetSnapshot offBoard = {505u, Frontier::FPoint(-1, -1), 9, false};
Frontier::FTacticalSeekerTargetSnapshot farther = {606u, Frontier::FPoint(15, 10), 5, false};
candidates.push_back(targetA);
candidates.push_back(targetB);
candidates.push_back(station);
candidates.push_back(destroyed);
candidates.push_back(offBoard);
candidates.push_back(farther);

const std::vector<unsigned int> closest = game.collectClosestSeekerTargetIDs(seeker, candidates);
CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), closest.size());
const std::set<unsigned int> closestIDs(closest.begin(), closest.end());
CPPUNIT_ASSERT(closestIDs.find(101u) != closestIDs.end());
CPPUNIT_ASSERT(closestIDs.find(202u) != closestIDs.end());
CPPUNIT_ASSERT(closestIDs.find(303u) == closestIDs.end());
CPPUNIT_ASSERT(closestIDs.find(404u) == closestIDs.end());
CPPUNIT_ASSERT(closestIDs.find(505u) == closestIDs.end());
CPPUNIT_ASSERT(closestIDs.find(606u) == closestIDs.end());
}

void FTacticalSeekerMovementTest::testDeterministicGreedyNextStepHelperReturnsOneStepMove() {
TestableTacticalGame game;
Frontier::FTacticalSeekerMissileState seeker;
seeker.hex = Frontier::FPoint(20, 20);
seeker.heading = 0;
seeker.movementAllowance = 6;

const Frontier::FPoint targetHex(24, 17);
Frontier::FTacticalSeekerMissileState nextState;
const bool advanced = game.computeSeekerGreedyNextStep(seeker, targetHex, nextState);
CPPUNIT_ASSERT(advanced);

const int previousDistance = Frontier::FHexMap::computeHexDistance(seeker.hex, targetHex);
const int nextDistance = Frontier::FHexMap::computeHexDistance(nextState.hex, targetHex);
int expectedBestDistance = -1;
for (int turn = -1; turn <= 1; ++turn) {
	const int candidateHeading = (seeker.heading + turn + 6) % 6;
	const Frontier::FPoint candidateHex = Frontier::FHexMap::findNextHex(seeker.hex, candidateHeading);
	const int candidateDistance = Frontier::FHexMap::computeHexDistance(candidateHex, targetHex);
	if (expectedBestDistance < 0 || candidateDistance < expectedBestDistance) {
		expectedBestDistance = candidateDistance;
	}
}
CPPUNIT_ASSERT_EQUAL(expectedBestDistance, nextDistance);
if (expectedBestDistance < previousDistance) {
	CPPUNIT_ASSERT(nextDistance < previousDistance);
}

const std::vector<int> legalHeadings = {
	((seeker.heading + 5) % 6),
	(seeker.heading % 6),
	((seeker.heading + 1) % 6)
};
CPPUNIT_ASSERT(std::find(legalHeadings.begin(), legalHeadings.end(), nextState.heading) != legalHeadings.end());
CPPUNIT_ASSERT(nextState.hex == Frontier::FHexMap::findNextHex(seeker.hex, nextState.heading));
}

void FTacticalSeekerMovementTest::testSeekerHelpersRemainModelOnlyAndFHexMapGeneric() {
const std::string header = readFile(repoFile("include/tactical/FTacticalGame.h"));
const std::string source = readFile(repoFile("src/tactical/FTacticalGame.cpp"));
const std::string mapHeader = readFile(repoFile("include/core/FHexMap.h"));
const std::string mapSource = readFile(repoFile("src/core/FHexMap.cpp"));
const std::string resolveBody = extractFunctionBody(source, "void FTacticalGame::resolveActiveSeekersForMovingPlayer()");
const std::string selectTargetBody = extractFunctionBody(source,
"FVehicle * FTacticalGame::selectClosestSeekerTarget(");
const std::string moveBody = extractFunctionBody(source,
"void FTacticalGame::moveSeekerTowardTarget(FTacticalSeekerMissileState & seeker, const FPoint & targetHex) const");

assertContains(header, "void resolveActiveSeekersForMovingPlayer();");
assertContains(header, "unsigned int computeSeekerMovementAllowance(int movementTurn) const;");
assertContains(header, "unsigned int chooseRandomSeekerIndex(unsigned int candidateCount) const;");
assertContains(resolveBody, "FPoint targetHex;");
assertContains(selectTargetBody, "const int distance = FHexMap::computeHexDistance(seeker.hex, shipHex);");
assertContains(moveBody, "const FPoint candidateHex = FHexMap::findNextHex(seeker.hex, candidateHeading);");

CPPUNIT_ASSERT(resolveBody.find("wx") == std::string::npos);
CPPUNIT_ASSERT(selectTargetBody.find("wx") == std::string::npos);
CPPUNIT_ASSERT(moveBody.find("wx") == std::string::npos);
CPPUNIT_ASSERT(mapHeader.find("wx") == std::string::npos);
CPPUNIT_ASSERT(mapSource.find("wx") == std::string::npos);
}

}
