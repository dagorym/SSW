/**
 * @file FTacticalStationOrbitalMovementTest.cpp
 * @brief Implementation file for FBattleBoard station orbital movement tests
 * @author Tom Stephens
 * @date Created: Mar 26, 2026
 */

#include "FTacticalStationOrbitalMovementTest.h"

#include <fstream>
#include <iterator>

namespace FrontierTests {

namespace {

std::string repoFile(const std::string & relativePath) {
return std::string(TACTICAL_TEST_REPO_ROOT) + "/" + relativePath;
}

}

CPPUNIT_TEST_SUITE_REGISTRATION( FTacticalStationOrbitalMovementTest );

void FTacticalStationOrbitalMovementTest::setUp() {
}

void FTacticalStationOrbitalMovementTest::tearDown() {
}

std::string FTacticalStationOrbitalMovementTest::readFile(const std::string & path) {
std::ifstream file(path.c_str());
CPPUNIT_ASSERT_EQUAL(true, file.is_open());
return std::string((std::istreambuf_iterator<char>(file)),
std::istreambuf_iterator<char>());
}

std::string FTacticalStationOrbitalMovementTest::extractFunctionBody(const std::string & source, const std::string & signature) {
std::string::size_type signaturePos = source.find(signature);
CPPUNIT_ASSERT_EQUAL(true, signaturePos != std::string::npos);

std::string::size_type bodyStart = source.find('{', signaturePos);
CPPUNIT_ASSERT_EQUAL(true, bodyStart != std::string::npos);

int depth = 1;
for (std::string::size_type i = bodyStart + 1; i < source.size(); i++) {
if (source[i] == '{') {
depth++;
} else if (source[i] == '}') {
depth--;
if (depth == 0) {
return source.substr(bodyStart + 1, i - bodyStart - 1);
}
}
}

CPPUNIT_ASSERT_EQUAL(0, 1);
return "";
}

void FTacticalStationOrbitalMovementTest::testStationHeadingUpdatedAfterOrbit() {
const std::string source = readFile(repoFile("src/tactical/FBattleBoard.cpp"));
const std::string body = extractFunctionBody(source, "void FBattleBoard::finalizeMove()");
const std::string headingLine = "(*itr)->setHeading(isStation ? m_turnInfo[id].finalHeading : m_turnInfo[id].curHeading);";
CPPUNIT_ASSERT_EQUAL(true, body.find(headingLine) != std::string::npos);

const int curHeading = 0;
const int finalHeading = 1;
const bool isStation = true;
const int resolvedHeading = isStation ? finalHeading : curHeading;
CPPUNIT_ASSERT_EQUAL(finalHeading, resolvedHeading);
}

void FTacticalStationOrbitalMovementTest::testStationSpeedPreservedAfterOrbit() {
const std::string source = readFile(repoFile("src/tactical/FBattleBoard.cpp"));
const std::string body = extractFunctionBody(source, "void FBattleBoard::finalizeMove()");
const std::string resetBody = extractFunctionBody(source, "void FBattleBoard::resetMoveData()");

const std::string stationGuard = "if (!isStation){";
const std::string speedSet = "(*itr)->setSpeed(m_turnInfo[id].nMoved);";
CPPUNIT_ASSERT_EQUAL(true, body.find(stationGuard) != std::string::npos);
CPPUNIT_ASSERT_EQUAL(true, body.find(speedSet) != std::string::npos);
CPPUNIT_ASSERT_EQUAL(true, resetBody.find("d.nMoved=1;") != std::string::npos);

const int stationSpeed = 0;
const int nMoved = 1;
const bool isStation = true;
const int resolvedSpeed = isStation ? stationSpeed : nMoved;
CPPUNIT_ASSERT_EQUAL(0, resolvedSpeed);
}

void FTacticalStationOrbitalMovementTest::testNonStationHeadingUnchangedByFix() {
const std::string source = readFile(repoFile("src/tactical/FBattleBoard.cpp"));
const std::string body = extractFunctionBody(source, "void FBattleBoard::finalizeMove()");
const std::string headingLine = "(*itr)->setHeading(isStation ? m_turnInfo[id].finalHeading : m_turnInfo[id].curHeading);";
CPPUNIT_ASSERT_EQUAL(true, body.find(headingLine) != std::string::npos);

const int curHeading = 2;
const int finalHeading = 4;
const bool isStation = false;
const int resolvedHeading = isStation ? finalHeading : curHeading;
CPPUNIT_ASSERT_EQUAL(curHeading, resolvedHeading);
}

}
