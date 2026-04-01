/**
 * @file FTacticalBattleBoardRendererDelegationTest.cpp
 * @brief Implementation file for FBattleBoard renderer delegation tests
 */

#include "FTacticalBattleBoardRendererDelegationTest.h"

#include <fstream>

namespace FrontierTests {

namespace {

std::string repoFile(const std::string & relativePath) {
return std::string(TACTICAL_TEST_REPO_ROOT) + "/" + relativePath;
}

}

CPPUNIT_TEST_SUITE_REGISTRATION( FTacticalBattleBoardRendererDelegationTest );

void FTacticalBattleBoardRendererDelegationTest::setUp() {
}

void FTacticalBattleBoardRendererDelegationTest::tearDown() {
}

std::string FTacticalBattleBoardRendererDelegationTest::readFile(const std::string & path) {
std::ifstream file(path.c_str());
CPPUNIT_ASSERT_MESSAGE(path, file.is_open());
return std::string((std::istreambuf_iterator<char>(file)),
std::istreambuf_iterator<char>());
}

std::string FTacticalBattleBoardRendererDelegationTest::extractFunctionBody(const std::string & source, const std::string & signature) {
std::string::size_type signaturePos = source.find(signature);
CPPUNIT_ASSERT_MESSAGE(signature, signaturePos != std::string::npos);

std::string::size_type bodyStart = source.find('{', signaturePos);
CPPUNIT_ASSERT(bodyStart != std::string::npos);

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

CPPUNIT_FAIL("Function body not terminated");
return "";
}

void FTacticalBattleBoardRendererDelegationTest::assertContains(const std::string & haystack, const std::string & needle) {
CPPUNIT_ASSERT_MESSAGE(
std::string("Expected to find '") + needle + "' in inspected source",
haystack.find(needle) != std::string::npos);
}

void FTacticalBattleBoardRendererDelegationTest::testDrawUsesBattleScreenModelAccessors() {
// AC: draw() renders from FTacticalGame-backed state exposed through FBattleScreen accessors.
const std::string source = readFile(repoFile("src/tactical/FBattleBoard.cpp"));
const std::string body = extractFunctionBody(source, "void FBattleBoard::draw(wxDC &dc)");

assertContains(body, "drawGrid(dc);");
assertContains(body, "const FPoint & planetPos = m_parent->getPlanetPos();");
assertContains(body, "const int planetChoice = m_parent->getPlanetChoice();");
assertContains(body, "drawShips();");
assertContains(body, "if (m_parent->getState() == BS_Battle) {");
assertContains(body, "drawRoute(dc);");
assertContains(body, "if (m_parent->getState() == BS_PlaceMines) {");
assertContains(body, "drawMinedHexes(dc);");
assertContains(body, "if (m_parent->getWeapon() != NULL) {");
assertContains(body, "drawWeaponRange(dc);");
assertContains(body, "drawTarget(dc);");
}

void FTacticalBattleBoardRendererDelegationTest::testOverlayDrawingUsesModelStateAccessors() {
// AC: route/range/mine overlays are sourced from model state accessors, not board-owned mechanics data.
const std::string source = readFile(repoFile("src/tactical/FBattleBoard.cpp"));

const std::string shipsBody = extractFunctionBody(source, "void FBattleBoard::drawShips()");
assertContains(shipsBody, "const VehicleList & ships = m_parent->getHexOccupants(hex);");

const std::string routeBody = extractFunctionBody(source, "void FBattleBoard::drawRoute(wxDC &dc)");
assertContains(routeBody, "const std::map<unsigned int, FTacticalTurnData> & turnInfo = m_parent->getTurnInfo();");
assertContains(routeBody, "drawRouteHexes(dc, m_parent->getMovementHexes(), movedCount+1);");
assertContains(routeBody, "drawRouteHexes(dc, m_parent->getLeftTurnHexes(), movedCount+1);");
assertContains(routeBody, "drawRouteHexes(dc, m_parent->getRightTurnHexes(), movedCount+1);");

const std::string rangeBody = extractFunctionBody(source, "void FBattleBoard::drawWeaponRange(wxDC &dc)");
assertContains(rangeBody, "for (PointSet::const_iterator itr = m_parent->getTargetHexes().begin();");
assertContains(rangeBody, "for (PointSet::const_iterator itr = m_parent->getHeadOnHexes().begin();");

const std::string mineBody = extractFunctionBody(source, "void FBattleBoard::drawMinedHexes(wxDC &dc)");
assertContains(mineBody, "for (PointSet::const_iterator itr = m_parent->getMinedHexes().begin();");
}

void FTacticalBattleBoardRendererDelegationTest::testOnLeftUpOnlyHitTestsAndForwardsHexClick() {
// AC: onLeftUp computes clicked hex and forwards to model click handler through FBattleScreen.
const std::string source = readFile(repoFile("src/tactical/FBattleBoard.cpp"));
const std::string body = extractFunctionBody(source, "void FBattleBoard::onLeftUp(wxMouseEvent & event)");

assertContains(body, "event.GetPosition(&x,&y);");
assertContains(body, "if (getHex(x,y,a,b)){");
assertContains(body, "m_parent->handleHexClick(FPoint(a,b));");
assertContains(body, "event.Skip();");
CPPUNIT_ASSERT(body.find("placeShip(") == std::string::npos);
CPPUNIT_ASSERT(body.find("placePlanet(") == std::string::npos);
CPPUNIT_ASSERT(body.find("placeMineAtHex(") == std::string::npos);
}

void FTacticalBattleBoardRendererDelegationTest::testHeaderRemovesDuplicatedMechanicsState() {
// AC: FBattleBoard retains renderer geometry/hit-test state and no duplicate tactical mechanics containers.
const std::string header = readFile(repoFile("include/tactical/FBattleBoard.h"));

assertContains(header, "FPoint m_hexCenters[100][100];");
assertContains(header, "ImageList m_planetImages;");
assertContains(header, "wxImage * m_maskingScreenIcon;");
CPPUNIT_ASSERT(header.find("m_hexData") == std::string::npos);
CPPUNIT_ASSERT(header.find("m_turnInfo") == std::string::npos);
CPPUNIT_ASSERT(header.find("hexData") == std::string::npos);
CPPUNIT_ASSERT(header.find("turnData") == std::string::npos);
CPPUNIT_ASSERT(header.find("m_targetHexes") == std::string::npos);
CPPUNIT_ASSERT(header.find("m_minedHexList") == std::string::npos);
}

void FTacticalBattleBoardRendererDelegationTest::testOnMotionUsesPlacementForwardersOnly() {
// AC: setup movement hover handling stays as UI hit-test/rotation forwarding.
const std::string source = readFile(repoFile("src/tactical/FBattleBoard.cpp"));
const std::string body = extractFunctionBody(source, "void FBattleBoard::onMotion(wxMouseEvent & event)");

assertContains(body, "m_parent->hasShipPlacementPendingRotation()");
assertContains(body, "m_parent->setShipPlacementHeadingByHex(FPoint(a,b))");
CPPUNIT_ASSERT(body.find("computeWeaponRange(") == std::string::npos);
CPPUNIT_ASSERT(body.find("resetMovementState(") == std::string::npos);
}

}
