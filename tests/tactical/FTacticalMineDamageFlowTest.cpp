/**
 * @file FTacticalMineDamageFlowTest.cpp
 * @brief Implementation file for FTacticalGame mine-damage flow tests
 */

#include "FTacticalMineDamageFlowTest.h"

#include <fstream>
#include <vector>

#include "ships/FVehicle.h"
#include "strategic/FFleet.h"
#include "tactical/FTacticalGame.h"

namespace FrontierTests {

namespace {

using namespace Frontier;

std::string repoFile(const std::string & relativePath) {
return std::string(TACTICAL_TEST_REPO_ROOT) + "/" + relativePath;
}

void assertContains(const std::string & haystack, const std::string & needle) {
CPPUNIT_ASSERT_MESSAGE(
std::string("Expected to find '") + needle + "' in inspected source",
haystack.find(needle) != std::string::npos);
}

void assertBefore(const std::string & haystack, const std::string & first, const std::string & second) {
const std::string::size_type firstPos = haystack.find(first);
const std::string::size_type secondPos = haystack.find(second);
CPPUNIT_ASSERT_MESSAGE(
std::string("Expected to find both source fragments in order: '") + first + "' before '" + second + "'",
firstPos != std::string::npos && secondPos != std::string::npos && firstPos < secondPos);
}

class FMoveDoneShipHarness : public FVehicle {
public:
FMoveDoneShipHarness() {
m_name = "MoveDoneHarnessShip";
m_type = "MoveDoneHarness";
}

void configureMove(int speed, int adf, int heading) {
m_maxHP = 5;
m_currentHP = 5;
m_maxADF = adf;
m_currentADF = adf;
m_maxMR = 1;
m_currentMR = 1;
m_maxDCR = 1;
m_currentDCR = 1;
m_speed = speed;
m_heading = heading;
}
};

struct MoveDoneRuntimeFixture {
FTacticalGame game;
FleetList attackFleets;
FleetList defendFleets;
FFleet * attackFleet;
FFleet * defendFleet;
FMoveDoneShipHarness * attacker;
FMoveDoneShipHarness * defender;
};

void destroyFixture(MoveDoneRuntimeFixture & fixture) {
for (FleetList::iterator itr = fixture.attackFleets.begin(); itr != fixture.attackFleets.end(); ++itr) {
delete *itr;
}
for (FleetList::iterator itr = fixture.defendFleets.begin(); itr != fixture.defendFleets.end(); ++itr) {
delete *itr;
}
fixture.attackFleets.clear();
fixture.defendFleets.clear();
}

void setupMoveDoneRuntimeScenario(MoveDoneRuntimeFixture & fixture) {
fixture.attackFleet = new FFleet();
fixture.defendFleet = new FFleet();
fixture.attackFleets.push_back(fixture.attackFleet);
fixture.defendFleets.push_back(fixture.defendFleet);

fixture.attacker = new FMoveDoneShipHarness();
fixture.defender = new FMoveDoneShipHarness();
fixture.attacker->setName("AttackerMover");
fixture.defender->setName("DefenderAnchor");
fixture.attacker->configureMove(6, 0, 3);
fixture.defender->configureMove(0, 0, 3);

fixture.attackFleet->addShip(fixture.attacker);
fixture.defendFleet->addShip(fixture.defender);

fixture.game.setupFleets(&fixture.attackFleets, &fixture.defendFleets, false, NULL);
fixture.game.setState(BS_SetupDefendFleet);
fixture.game.setControlState(true);
fixture.game.setShip(fixture.defender);
fixture.game.placeShip(FPoint(10, 10));
fixture.game.setShipPlacementHeading(3);

fixture.game.setState(BS_SetupAttackFleet);
fixture.game.setControlState(true);
fixture.game.setShip(fixture.attacker);
fixture.game.placeShip(FPoint(54, 20));
fixture.game.setShipPlacementHeading(3);

fixture.game.setState(BS_Battle);
fixture.game.setPhase(PH_MOVE);
}

}

CPPUNIT_TEST_SUITE_REGISTRATION( FTacticalMineDamageFlowTest );

void FTacticalMineDamageFlowTest::setUp() {
}

void FTacticalMineDamageFlowTest::tearDown() {
}

std::string FTacticalMineDamageFlowTest::readFile(const std::string & path) {
std::ifstream file(path.c_str());
CPPUNIT_ASSERT_MESSAGE(path, file.is_open());
return std::string((std::istreambuf_iterator<char>(file)),
std::istreambuf_iterator<char>());
}

std::string FTacticalMineDamageFlowTest::extractFunctionBody(const std::string & source, const std::string & signature) {
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

unsigned int FTacticalMineDamageFlowTest::countOccurrences(const std::string & haystack, const std::string & needle) {
unsigned int count = 0;
std::string::size_type pos = 0;
while ((pos = haystack.find(needle, pos)) != std::string::npos) {
++count;
pos += needle.size();
}
return count;
}

void FTacticalMineDamageFlowTest::testApplyMineDamageDefersDestroyedShipCleanupUntilAfterImmediateSummaryDialog() {
// AC: mine damage creates immediate report summary and performs destroyed-ship cleanup only after summary handling.
const std::string source = readFile(repoFile("src/tactical/FTacticalGame.cpp"));
const std::string body = extractFunctionBody(source, "void FTacticalGame::applyMineDamage()");

assertContains(body, "context.reportType = TRT_MineDamage;");
assertContains(body, "context.immediate = true;");
assertContains(body, "beginTacticalReport(context);");
assertContains(body, "appendTacticalAttackReport(buildTacticalAttackReport(result));");
assertContains(body, "FTacticalCombatReportSummary summary = buildCurrentTacticalReportSummary();");
assertContains(body, "m_ui->showDamageSummary(summary);");
assertContains(body, "clearTacticalReport();");
assertContains(body, "clearDestroyedShips();");

assertBefore(body, "appendTacticalAttackReport(buildTacticalAttackReport(result));", "FTacticalCombatReportSummary summary = buildCurrentTacticalReportSummary();");
assertBefore(body, "FTacticalCombatReportSummary summary = buildCurrentTacticalReportSummary();", "m_ui->showDamageSummary(summary);");
assertBefore(body, "m_ui->showDamageSummary(summary);", "clearTacticalReport();");
assertBefore(body, "clearTacticalReport();", "clearDestroyedShips();");
}

void FTacticalMineDamageFlowTest::testFinalizeMoveAppliesMineDamageExactlyOncePerMovementResolution() {
// AC: mine damage is applied once after move finalization during movement phase progression.
const std::string source = readFile(repoFile("src/tactical/FTacticalGame.cpp"));
const std::string body = extractFunctionBody(source, "void FTacticalGame::completeMovePhase()");

CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), countOccurrences(body, "applyMineDamage();"));
assertBefore(body, "finalizeMovementState();", "applyMineDamage();");
assertBefore(body, "applyMineDamage();", "setPhase(PH_DEFENSE_FIRE);");
}

void FTacticalMineDamageFlowTest::testFinalizeMovePhaseStateDelegatesToCanonicalCompletionPath() {
// AC: PH_FINALIZE_MOVE routing must use completeMovePhase() as the single post-move resolution path.
const std::string source = readFile(repoFile("src/tactical/FTacticalGame.cpp"));
const std::string body = extractFunctionBody(source, "void FTacticalGame::setPhase(int p)");

assertContains(body, "if (p == PH_FINALIZE_MOVE) {");
assertContains(body, "completeMovePhase();");
assertContains(body, "return;");
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(0), countOccurrences(body, "finalizeMovementState();"));
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(0), countOccurrences(body, "applyMineDamage();"));
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(0), countOccurrences(body, "clearDestroyedShips();"));
}

void FTacticalMineDamageFlowTest::testMineDamageAttackReportBuilderLeavesNestedEventsUnattachedUntilBattleScreenStoresThem() {
// AC: mine damage uses common tactical attack-report builder path and not board-local custom builders.
const std::string source = readFile(repoFile("src/tactical/FTacticalGame.cpp"));
const std::string body = extractFunctionBody(source, "void FTacticalGame::applyMineDamage()");

assertContains(body, "FTacticalAttackResult result = (*itr)->fire();");
assertContains(body, "appendTacticalAttackReport(buildTacticalAttackReport(result));");
CPPUNIT_ASSERT(body.find("buildMineDamageAttackReport(") == std::string::npos);
CPPUNIT_ASSERT(body.find("buildMineDamageEvent(") == std::string::npos);
}

void FTacticalMineDamageFlowTest::testRuntimeMoveDonePathUsesCanonicalCompletionAndResolvesOffBoardOutcome() {
// AC: live runtime move-done path (FBattleDisplay->FBattleScreen->FTacticalGame::completeMovePhase) reaches canonical model seam.
MoveDoneRuntimeFixture fixture;
setupMoveDoneRuntimeScenario(fixture);

const FPoint startHex(54, 20);
CPPUNIT_ASSERT(fixture.game.handleHexClick(startHex));
CPPUNIT_ASSERT(fixture.game.isMoveComplete());
CPPUNIT_ASSERT(fixture.game.getShip() != NULL);

fixture.game.completeMovePhase();

CPPUNIT_ASSERT_EQUAL(static_cast<int>(PH_DEFENSE_FIRE), fixture.game.getPhase());
CPPUNIT_ASSERT(fixture.game.getShip() == NULL);
CPPUNIT_ASSERT(!fixture.game.isHexOccupied(startHex));
CPPUNIT_ASSERT_EQUAL(0, fixture.attacker->getHP());
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), static_cast<unsigned int>(fixture.game.getShipList(fixture.game.getDefenderID()).size()));
CPPUNIT_ASSERT(fixture.game.isHexOccupied(FPoint(10, 10)));
CPPUNIT_ASSERT(fixture.game.getLastDestroyedShipIDs().empty());

destroyFixture(fixture);
}

}
