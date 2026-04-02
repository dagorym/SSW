/**
 * @file FTacticalBattleDisplayFireFlowTest.cpp
 * @brief Implementation file for the FTacticalBattleDisplayFireFlowTest class
 */

#include "FTacticalBattleDisplayFireFlowTest.h"

#include <fstream>
#include <iterator>

namespace FrontierTests {

namespace {

std::string repoFile(const std::string & relativePath) {
return std::string(TACTICAL_TEST_REPO_ROOT) + "/" + relativePath;
}

}

CPPUNIT_TEST_SUITE_REGISTRATION( FTacticalBattleDisplayFireFlowTest );

void FTacticalBattleDisplayFireFlowTest::setUp() {
}

void FTacticalBattleDisplayFireFlowTest::tearDown() {
}

std::string FTacticalBattleDisplayFireFlowTest::readFile(const std::string & path) {
std::ifstream file(path.c_str());
CPPUNIT_ASSERT_MESSAGE(path, file.is_open());
return std::string((std::istreambuf_iterator<char>(file)),
std::istreambuf_iterator<char>());
}

std::string FTacticalBattleDisplayFireFlowTest::extractFunctionBody(const std::string & source, const std::string & signature) {
std::string::size_type signaturePos = source.find(signature);
CPPUNIT_ASSERT(signaturePos != std::string::npos);

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

void FTacticalBattleDisplayFireFlowTest::assertContains(const std::string & haystack, const std::string & needle) {
CPPUNIT_ASSERT_MESSAGE(
std::string("Expected to find '") + needle + "' in inspected source",
haystack.find(needle) != std::string::npos);
}

void FTacticalBattleDisplayFireFlowTest::assertNotContains(const std::string & haystack, const std::string & needle) {
CPPUNIT_ASSERT_MESSAGE(
std::string("Expected not to find '") + needle + "' in inspected source",
haystack.find(needle) == std::string::npos);
}

void FTacticalBattleDisplayFireFlowTest::assertBefore(const std::string & haystack, const std::string & first, const std::string & second) {
const std::string::size_type firstPos = haystack.find(first);
const std::string::size_type secondPos = haystack.find(second);
CPPUNIT_ASSERT_MESSAGE(
std::string("Expected to find both source fragments in order: '") + first + "' before '" + second + "'",
firstPos != std::string::npos && secondPos != std::string::npos && firstPos < secondPos);
}

unsigned int FTacticalBattleDisplayFireFlowTest::countOccurrences(const std::string & haystack, const std::string & needle) {
unsigned int count = 0;
std::string::size_type pos = 0;
while ((pos = haystack.find(needle, pos)) != std::string::npos) {
++count;
pos += needle.size();
}
return count;
}

void FTacticalBattleDisplayFireFlowTest::testDrawAndOnPaintUseBattleScreenStateAccessors() {
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string drawBody = extractFunctionBody(source, "void FBattleDisplay::draw(wxDC &dc)");
const std::string onPaintBody = extractFunctionBody(source, "void FBattleDisplay::onPaint(wxPaintEvent & event)");

assertContains(drawBody, "switch (m_parent->getState())");
assertContains(drawBody, "if (m_parent->getControlState())");
assertContains(drawBody, "if (m_parent->getPhase()==PH_NONE)");
assertContains(drawBody, "drawPlaceMines(dc);");
assertContains(onPaintBody, "draw(dc);");
}

void FTacticalBattleDisplayFireFlowTest::testLegacyFireAllWeaponsHelperRemoved() {
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string header = readFile(repoFile("include/tactical/FBattleDisplay.h"));

assertNotContains(source, "void FBattleDisplay::fireAllWeapons(");
assertNotContains(header, "void fireAllWeapons(");
}

void FTacticalBattleDisplayFireFlowTest::testDefensiveFireDoneDelegatesToModelFirePhaseResolution() {
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string body = extractFunctionBody(source, "void FBattleDisplay::onDefensiveFireDone( wxCommandEvent& event )");

assertContains(body, "m_parent->resolveCurrentFirePhase()");
assertContains(body, "m_parent->showTacticalDamageSummaryDialog(summary);");
assertContains(body, "m_parent->clearDestroyedShips();");
assertContains(body, "m_parent->completeDefensiveFirePhase();");
assertBefore(body, "m_parent->resolveCurrentFirePhase()", "m_parent->showTacticalDamageSummaryDialog(summary);");
assertBefore(body, "m_parent->showTacticalDamageSummaryDialog(summary);", "m_parent->clearDestroyedShips();");
assertBefore(body, "m_parent->clearDestroyedShips();", "m_parent->completeDefensiveFirePhase();");
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(0), countOccurrences(body, "fireAllWeapons();"));
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(0), countOccurrences(body, "m_parent->setPhase("));
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(0), countOccurrences(body, "m_parent->toggleMovingPlayer();"));
}

void FTacticalBattleDisplayFireFlowTest::testOffensiveFireDoneDelegatesToModelFirePhaseResolution() {
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string body = extractFunctionBody(source, "void FBattleDisplay::onOffensiveFireDone( wxCommandEvent& event )");

assertContains(body, "m_parent->resolveCurrentFirePhase()");
assertContains(body, "m_parent->showTacticalDamageSummaryDialog(summary);");
assertContains(body, "m_parent->clearDestroyedShips();");
assertContains(body, "m_parent->completeOffensiveFirePhase();");
assertBefore(body, "m_parent->resolveCurrentFirePhase()", "m_parent->showTacticalDamageSummaryDialog(summary);");
assertBefore(body, "m_parent->showTacticalDamageSummaryDialog(summary);", "m_parent->clearDestroyedShips();");
assertBefore(body, "m_parent->clearDestroyedShips();", "m_parent->completeOffensiveFirePhase();");
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(0), countOccurrences(body, "fireAllWeapons();"));
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(0), countOccurrences(body, "m_parent->setPhase("));
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(0), countOccurrences(body, "m_parent->toggleMovingPlayer();"));
}

void FTacticalBattleDisplayFireFlowTest::testDefensiveFireDoneDisablesAndHidesButtonBeforeResolution() {
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string body = extractFunctionBody(source, "void FBattleDisplay::onDefensiveFireDone( wxCommandEvent& event )");

assertContains(body, "m_buttonDefensiveFireDone->Enable(false);");
assertContains(body, "m_buttonDefensiveFireDone->Hide();");
assertBefore(body, "m_buttonDefensiveFireDone->Enable(false);", "m_parent->resolveCurrentFirePhase()");
assertBefore(body, "m_buttonDefensiveFireDone->Hide();", "m_parent->resolveCurrentFirePhase()");
}

void FTacticalBattleDisplayFireFlowTest::testOffensiveFireDoneDisablesAndHidesButtonBeforeResolution() {
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string body = extractFunctionBody(source, "void FBattleDisplay::onOffensiveFireDone( wxCommandEvent& event )");

assertContains(body, "m_buttonOffensiveFireDone->Enable(false);");
assertContains(body, "m_buttonOffensiveFireDone->Hide();");
assertBefore(body, "m_buttonOffensiveFireDone->Enable(false);", "m_parent->resolveCurrentFirePhase()");
assertBefore(body, "m_buttonOffensiveFireDone->Hide();", "m_parent->resolveCurrentFirePhase()");
}

void FTacticalBattleDisplayFireFlowTest::testWeaponSelectionDelegatesToBattleScreenModelApi() {
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string body = extractFunctionBody(source, "void FBattleDisplay::checkWeaponSelection(wxMouseEvent &event)");

assertContains(body, "m_parent->selectWeapon(i);");
assertContains(body, "m_parent->reDraw();");
assertNotContains(body, "m_parent->setWeapon(");
assertNotContains(body, "->setTarget(");
assertNotContains(body, "->fire(");
}

void FTacticalBattleDisplayFireFlowTest::testDefenseSelectionDelegatesToBattleScreenModelApi() {
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string body = extractFunctionBody(source, "void FBattleDisplay::checkDefenseSelection(wxMouseEvent &event)");

assertContains(body, "m_parent->selectDefense(i);");
assertContains(body, "m_parent->reDraw();");
assertNotContains(body, "m_parent->setDefense(");
assertNotContains(body, "->setCurrentDefense(");
}

void FTacticalBattleDisplayFireFlowTest::testMinePlacementFlowUsesModelMinePlacementApis() {
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string setSpeedBody = extractFunctionBody(source, "void FBattleDisplay::onSetSpeed( wxCommandEvent& event )");
const std::string doneBody = extractFunctionBody(source, "void FBattleDisplay::onMinePlacementDone( wxCommandEvent& event )");

assertContains(setSpeedBody, "if(!m_parent->beginMinePlacement())");
assertContains(doneBody, "m_parent->completeMinePlacement();");
}

void FTacticalBattleDisplayFireFlowTest::testMinePlacementDisplayUsesModelShipList() {
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string placeMinesBody = extractFunctionBody(source, "void FBattleDisplay::drawPlaceMines(wxDC &dc)");
const std::string shipSelectionBody = extractFunctionBody(source, "void FBattleDisplay::checkShipSelection(wxMouseEvent &event)");

assertContains(placeMinesBody, "const VehicleList & shipsWithMines = m_parent->getShipsWithMines();");
assertContains(shipSelectionBody, "const VehicleList & shipsWithMines = m_parent->getShipsWithMines();");
assertContains(shipSelectionBody, "m_parent->setShip(shipsWithMines[i]);");
}

void FTacticalBattleDisplayFireFlowTest::testDisplayClickFlowUsesModelForwardingApis() {
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string onLeftUpBody = extractFunctionBody(source, "void FBattleDisplay::onLeftUp(wxMouseEvent & event)");

assertContains(onLeftUpBody, "checkWeaponSelection(event);");
assertContains(onLeftUpBody, "checkDefenseSelection(event);");
assertContains(onLeftUpBody, "checkShipSelection(event);");
assertNotContains(onLeftUpBody, "m_parent->setWeapon(");
assertNotContains(onLeftUpBody, "m_parent->setDefense(");
assertNotContains(onLeftUpBody, "m_parent->assignTargetFromHex(");
assertNotContains(onLeftUpBody, "m_parent->placeMineAtHex(");
}

}
