/**
 * @file FTacticalBattleDisplayFireFlowTest.cpp
 * @brief Implementation file for the FTacticalBattleDisplayFireFlowTest class
 */

#include "FTacticalBattleDisplayFireFlowTest.h"

#include <fstream>
#include <iterator>
#include <vector>

#include "strategic/FFleet.h"
#include "tactical/FTacticalGame.h"

namespace FrontierTests {

namespace {

using namespace Frontier;

std::string repoFile(const std::string & relativePath) {
return std::string(TACTICAL_TEST_REPO_ROOT) + "/" + relativePath;
}

class FFireFlowWeaponHarness : public FWeapon {
public:
FFireFlowWeaponHarness() {
m_name = "TST";
m_fullName = "Fire Flow Harness";
m_type = FWeapon::LC;
m_range = 9;
m_nDice = 1;
m_dMod = 0;
m_baseToHitProb = 100;
}

void aimAt(FVehicle * target) {
m_target = target;
m_targetRange = 1;
m_isHeadOn = false;
}
};

class FFireFlowShipHarness : public FVehicle {
public:
FFireFlowShipHarness() {
m_name = "FireFlowShip";
m_type = "FireFlowHarness";
}

void configureForOneShotKill() {
m_maxHP = 1;
m_currentHP = 1;
m_maxADF = 1;
m_currentADF = 1;
m_maxMR = 1;
m_currentMR = 1;
m_maxDCR = 1;
m_currentDCR = 1;
}

void addWeapon(FWeapon * weapon) {
m_weapons.push_back(weapon);
weapon->setParent(this);
}
};

struct FireFlowRuntimeFixture {
FTacticalGame game;
FleetList attackFleets;
FleetList defendFleets;
FFleet * attackFleet;
FFleet * defendFleet;
FFireFlowShipHarness * attacker;
FFireFlowShipHarness * defender;
FFireFlowWeaponHarness * weapon;
};

void destroyFixture(FireFlowRuntimeFixture & fixture) {
for (FleetList::iterator itr = fixture.attackFleets.begin(); itr != fixture.attackFleets.end(); ++itr) {
delete *itr;
}
for (FleetList::iterator itr = fixture.defendFleets.begin(); itr != fixture.defendFleets.end(); ++itr) {
delete *itr;
}
fixture.attackFleets.clear();
fixture.defendFleets.clear();
}

void setupOneKillScenario(FireFlowRuntimeFixture & fixture, int firePhase) {
fixture.attackFleet = new FFleet();
fixture.defendFleet = new FFleet();
fixture.attackFleets.push_back(fixture.attackFleet);
fixture.defendFleets.push_back(fixture.defendFleet);

fixture.attacker = new FFireFlowShipHarness();
fixture.defender = new FFireFlowShipHarness();
fixture.attacker->setName("Attacker");
fixture.defender->setName("Defender");
fixture.attacker->configureForOneShotKill();
fixture.defender->configureForOneShotKill();

fixture.weapon = new FFireFlowWeaponHarness();
fixture.weapon->aimAt(fixture.defender);
fixture.attacker->addWeapon(fixture.weapon);

fixture.attackFleet->addShip(fixture.attacker);
fixture.defendFleet->addShip(fixture.defender);

fixture.game.setupFleets(&fixture.attackFleets, &fixture.defendFleets, false, NULL);
fixture.game.setPhase(firePhase);
fixture.game.setActivePlayer(true);
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
FireFlowRuntimeFixture fixture;
setupOneKillScenario(fixture, PH_DEFENSE_FIRE);

fixture.game.resolveCurrentFirePhase();

const std::vector<unsigned int> & destroyedShipIDs = fixture.game.getLastDestroyedShipIDs();
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), static_cast<unsigned int>(destroyedShipIDs.size()));
CPPUNIT_ASSERT_EQUAL(fixture.defender->getID(), destroyedShipIDs[0]);
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(0), static_cast<unsigned int>(fixture.game.getShipList(fixture.game.getDefenderID()).size()));

fixture.game.clearLastDestroyedShipIDs();
CPPUNIT_ASSERT(fixture.game.getLastDestroyedShipIDs().empty());

fixture.game.completeDefensiveFirePhase();
CPPUNIT_ASSERT(fixture.game.getLastDestroyedShipIDs().empty());
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(0), static_cast<unsigned int>(fixture.game.getShipList(fixture.game.getDefenderID()).size()));

destroyFixture(fixture);
}

void FTacticalBattleDisplayFireFlowTest::testOffensiveFireDoneDelegatesToModelFirePhaseResolution() {
FireFlowRuntimeFixture fixture;
setupOneKillScenario(fixture, PH_ATTACK_FIRE);

fixture.game.resolveCurrentFirePhase();

const std::vector<unsigned int> & destroyedShipIDs = fixture.game.getLastDestroyedShipIDs();
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), static_cast<unsigned int>(destroyedShipIDs.size()));
CPPUNIT_ASSERT_EQUAL(fixture.defender->getID(), destroyedShipIDs[0]);
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(0), static_cast<unsigned int>(fixture.game.getShipList(fixture.game.getDefenderID()).size()));

fixture.game.clearLastDestroyedShipIDs();
CPPUNIT_ASSERT(fixture.game.getLastDestroyedShipIDs().empty());

fixture.game.completeOffensiveFirePhase();
CPPUNIT_ASSERT(fixture.game.getLastDestroyedShipIDs().empty());
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(0), static_cast<unsigned int>(fixture.game.getShipList(fixture.game.getDefenderID()).size()));

destroyFixture(fixture);
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

void FTacticalBattleDisplayFireFlowTest::testMoveDoneDelegatesToBattleScreenCompleteMovePhase() {
// AC: live wx move-done callback must route through the canonical completeMovePhase seam.
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string body = extractFunctionBody(source, "void FBattleDisplay::onMoveDone( wxCommandEvent& event )");

assertContains(body, "m_parent->completeMovePhase();");
assertNotContains(body, "m_parent->setPhase(PH_FINALIZE_MOVE);");
}

void FTacticalBattleDisplayFireFlowTest::testMoveDoneDisconnectsAndHidesMoveButtonAroundDelegation() {
// AC: move-done callback still handles UI button teardown while progressing phase flow.
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string body = extractFunctionBody(source, "void FBattleDisplay::onMoveDone( wxCommandEvent& event )");

assertContains(body, "m_buttonMoveDone->Disconnect(");
assertContains(body, "m_buttonMoveDone->Hide();");
assertContains(body, "m_first=true;");
assertBefore(body, "m_parent->completeMovePhase();", "m_buttonMoveDone->Hide();");
}

}
