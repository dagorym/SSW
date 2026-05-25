/**
 * @file FTacticalSeekerMovementTest.cpp
 * @brief Source-inspection tests for tactical seeker movement helper logic
 */

#include "FTacticalSeekerMovementTest.h"

#include <fstream>
#include <iterator>

namespace FrontierTests {

namespace {

std::string repoFile(const std::string & relativePath) {
return std::string(TACTICAL_TEST_REPO_ROOT) + "/" + relativePath;
}

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
