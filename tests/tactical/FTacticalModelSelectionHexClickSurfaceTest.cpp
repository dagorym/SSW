/**
 * @file FTacticalModelSelectionHexClickSurfaceTest.cpp
 * @brief Implementation file for FTacticalGame selection and hex-click API surface tests
 */

#include "FTacticalModelSelectionHexClickSurfaceTest.h"

#include <fstream>

namespace FrontierTests {

namespace {

std::string repoFile(const std::string & relativePath) {
return std::string(TACTICAL_TEST_REPO_ROOT) + "/" + relativePath;
}

}

CPPUNIT_TEST_SUITE_REGISTRATION( FTacticalModelSelectionHexClickSurfaceTest );

void FTacticalModelSelectionHexClickSurfaceTest::setUp() {
}

void FTacticalModelSelectionHexClickSurfaceTest::tearDown() {
}

std::string FTacticalModelSelectionHexClickSurfaceTest::readFile(const std::string & path) {
std::ifstream file(path.c_str());
CPPUNIT_ASSERT_MESSAGE(path, file.is_open());
return std::string((std::istreambuf_iterator<char>(file)),
std::istreambuf_iterator<char>());
}

std::string FTacticalModelSelectionHexClickSurfaceTest::extractFunctionBody(const std::string & source, const std::string & signature) {
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

void FTacticalModelSelectionHexClickSurfaceTest::assertContains(const std::string & haystack, const std::string & needle) {
CPPUNIT_ASSERT_MESSAGE(
std::string("Expected to find '") + needle + "' in inspected source",
haystack.find(needle) != std::string::npos);
}

void FTacticalModelSelectionHexClickSurfaceTest::testHeaderDeclaresMilestone8SelectionAndHexClickApi() {
const std::string header = readFile(repoFile("include/tactical/FTacticalGame.h"));

assertContains(header, "bool selectWeapon(unsigned int weaponIndex);");
assertContains(header, "bool selectDefense(unsigned int defenseIndex);");
assertContains(header, "bool selectShipFromHex(const FPoint & hex);");
assertContains(header, "bool handleHexClick(const FPoint & hex);");
assertContains(header, "bool assignTargetFromHex(const FPoint & hex);");
assertContains(header, "bool placeMineAtHex(const FPoint & hex);");
assertContains(header, "bool isHexMinable(const FPoint & hex);");
assertContains(header, "const VehicleList & getHexOccupants(const FPoint & hex) const;");
assertContains(header, "const std::vector<FTacticalMovePreviewRoute> & getStoppedShipPreviewRoutes() const");
assertContains(header, "const std::vector<int> & getStoppedShipPreviewHeadingsForHex(const FPoint & hex) const;");
assertContains(header, "bool isHexInBounds(const FPoint & hex) const;");
assertContains(header, "bool isHexOccupied(const FPoint & hex) const;");
}

void FTacticalModelSelectionHexClickSurfaceTest::testImplementationDefinesSelectionAndHexClickHandlers() {
const std::string source = readFile(repoFile("src/tactical/FTacticalGame.cpp"));

assertContains(source, "bool FTacticalGame::selectWeapon(unsigned int weaponIndex)");
assertContains(source, "bool FTacticalGame::selectDefense(unsigned int defenseIndex)");
assertContains(source, "bool FTacticalGame::selectShipFromHex(const FPoint & hex)");
assertContains(source, "bool FTacticalGame::assignTargetFromHex(const FPoint & hex)");
assertContains(source, "bool FTacticalGame::placeMineAtHex(const FPoint & hex)");
assertContains(source, "bool FTacticalGame::isHexMinable(const FPoint & hex)");
assertContains(source, "bool FTacticalGame::handleHexClick(const FPoint & hex)");
assertContains(source, "const VehicleList & FTacticalGame::getHexOccupants(const FPoint & hex) const");
assertContains(source, "const std::vector<int> & FTacticalGame::getStoppedShipPreviewHeadingsForHex(const FPoint & hex) const");
assertContains(source, "bool FTacticalGame::isHexInBounds(const FPoint & hex) const");
assertContains(source, "bool FTacticalGame::isHexOccupied(const FPoint & hex) const");
}

void FTacticalModelSelectionHexClickSurfaceTest::testHexClickRoutesThroughModelOwnedSelectionAndTargetingFlow() {
const std::string source = readFile(repoFile("src/tactical/FTacticalGame.cpp"));
const std::string body = extractFunctionBody(source, "bool FTacticalGame::handleHexClick(const FPoint & hex)");

assertContains(body, "if (!isHexInBounds(hex)) {");
assertContains(body, "case BS_Battle:");
assertContains(body, "if (m_drawRoute && handleMoveHexSelection(hex)) {");
assertContains(body, "const bool selected = selectShipFromHex(hex);");
assertContains(body, "if (m_curWeapon != NULL && assignTargetFromHex(hex)) {");
assertContains(body, "if (m_curWeapon != NULL && m_curWeapon->getType() == FWeapon::M && isHexMinable(hex)) {");
assertContains(body, "return placeMineAtHex(hex);");
assertContains(body, "if (selectShipFromHex(hex)) {");
assertContains(body, "setWeapon(NULL);");
}

void FTacticalModelSelectionHexClickSurfaceTest::testMoveHexSelectionSupportsStoppedShipFacingChoice() {
const std::string source = readFile(repoFile("src/tactical/FTacticalGame.cpp"));
const std::string body = extractFunctionBody(source, "bool FTacticalGame::handleMoveHexSelection(const FPoint & hex)");

assertContains(body, "if (canUseStoppedShipFreeRotation(m_curShip, turnData)) {");
assertContains(body, "if (isAdjacentFacingSelection(m_shipPos, hex, selectedHeading)");
assertContains(body, "&& selectedHeading != turnData->curHeading) {");
assertContains(body, "turnData->startHeading = selectedHeading;");
assertContains(body, "turnData->curHeading = selectedHeading;");
assertContains(body, "turnData->finalHeading = selectedHeading;");
assertContains(body, "turnData->nMoved = 0;");
	assertContains(body, "turnData->path.clear();");
	assertContains(body, "turnData->path.addPoint(m_shipPos);");
	assertContains(body, "m_moved = 0;");
	assertContains(body, "computeRemainingMoves(m_shipPos);");
	assertContains(body, "const std::vector<int> & previewHeadings = getStoppedShipPreviewHeadingsForHex(hex);");
	assertContains(body, "const FHexPath originalPath = turnData->path;");
	assertContains(body, "for (std::vector<int>::const_iterator headingItr = previewHeadings.begin();");
	assertContains(body, "turnData->startHeading = *headingItr;");
	assertContains(body, "bool foundPreviewHex = findHexInList(m_movementHexes, hex, previewMoved);");
	assertContains(body, "foundPreviewHex = findHexInList(m_leftHexes, hex, previewMoved);");
	assertContains(body, "foundPreviewHex = findHexInList(m_rightHexes, hex, previewMoved);");
	assertContains(body, "if (!resolvedPreviewHeading) {");
	assertContains(body, "turnData->path = originalPath;");
	assertContains(body, "if (turnData->path.isPointOnPath(hex)) {");
	assertContains(body, "found = findHexInList(m_movementHexes, hex, moved);");
}

void FTacticalModelSelectionHexClickSurfaceTest::testStoppedShipPreviewApiAndLifecycleHooksAreExposed() {
const std::string source = readFile(repoFile("src/tactical/FTacticalGame.cpp"));
const std::string selectBody = extractFunctionBody(source, "bool FTacticalGame::selectShipFromHex(const FPoint & hex)");
const std::string setupBody = extractFunctionBody(source, "void FTacticalGame::setInitialRoute()");
const std::string computeBody = extractFunctionBody(source, "void FTacticalGame::computeRemainingMoves(FPoint start)");
const std::string clearBody = extractFunctionBody(source, "void FTacticalGame::clearMovementHighlights()");
const std::string headingLookupBody = extractFunctionBody(source,
	"const std::vector<int> & FTacticalGame::getStoppedShipPreviewHeadingsForHex(const FPoint & hex) const");

assertContains(source, "void FTacticalGame::clearStoppedShipPreviewRoutes()");
assertContains(source, "void FTacticalGame::rebuildStoppedShipPreviewRoutes()");
assertContains(setupBody, "clearStoppedShipPreviewRoutes();");
assertContains(setupBody, "rebuildStoppedShipPreviewRoutes();");
assertContains(computeBody, "rebuildStoppedShipPreviewRoutes();");
assertContains(clearBody, "clearStoppedShipPreviewRoutes();");
assertContains(selectBody, "clearStoppedShipPreviewRoutes();");
assertContains(headingLookupBody, "return EMPTY_PREVIEW_HEADING_LIST;");
assertContains(headingLookupBody, "return itr->second;");
}

}
