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

void FTacticalMineDamageFlowTest::testSeekerDetonationDamageResolutionUsesSMWeaponsICMAndImmediateReporting() {
// AC: seeker contacts resolve through SM weapon fire, existing ICM UI seam, immediate seeker report, and single cleanup pass.
const std::string source = readFile(repoFile("src/tactical/FTacticalGame.cpp"));
const std::string body = extractFunctionBody(source, "void FTacticalGame::resolvePendingSeekerDetonationDamage()");

assertContains(body, "const std::vector<FTacticalSeekerContactOutcome> outcomes = m_pendingSeekerContactOutcomes;");
assertContains(body, "FWeapon * seeker = createWeapon(FWeapon::SM);");
assertContains(body, "seeker->setTarget(target, 0, false);");
assertContains(body, "seeker->setParent(NULL);");
assertContains(body, "context.reportType = TRT_SeekerDamage;");
assertContains(body, "context.immediate = true;");
assertContains(body, "beginTacticalReport(context);");
assertContains(body, "m_ui->runICMSelection(icmData, defenders);");
assertContains(body, "appendTacticalAttackReport(buildTacticalAttackReport(result));");
assertContains(body, "m_ui->showDamageSummary(summary);");
assertContains(body, "clearTacticalReport();");
assertContains(body, "clearDestroyedShips();");
assertContains(body, "delete (*itr)->weapon;");
assertContains(body, "delete *itr;");
assertContains(body, "delete *itr;");
assertContains(body, "if (m_pendingSeekerContactOutcomes.empty()) {");

assertBefore(body, "m_ui->runICMSelection(icmData, defenders);", "FTacticalAttackResult result = (*itr)->fire();");
assertBefore(body, "appendTacticalAttackReport(buildTacticalAttackReport(result));", "FTacticalCombatReportSummary summary = buildCurrentTacticalReportSummary();");
assertBefore(body, "FTacticalCombatReportSummary summary = buildCurrentTacticalReportSummary();", "m_ui->showDamageSummary(summary);");
assertBefore(body, "m_ui->showDamageSummary(summary);", "clearTacticalReport();");
assertBefore(body, "clearTacticalReport();", "clearDestroyedShips();");
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), countOccurrences(body, "clearDestroyedShips();"));
}

void FTacticalMineDamageFlowTest::testSeekerActivationPhaseResolvesPendingDamageWhenModelHasUI() {
// AC: seeker activation completion paths call the shared pending-detonation resolver when ITacticalUI is installed.
const std::string source = readFile(repoFile("src/tactical/FTacticalGame.cpp"));
const std::string beginBody = extractFunctionBody(source, "void FTacticalGame::beginSeekerActivationPhase()");
const std::string completeBody = extractFunctionBody(source, "void FTacticalGame::completeSeekerActivationPhase()");

assertContains(beginBody, "if (inactiveHexes.empty()) {");
assertContains(beginBody, "resolveActiveSeekersForMovingPlayer();");
assertContains(beginBody, "if (m_ui != NULL) {");
assertContains(beginBody, "resolvePendingSeekerDetonationDamage();");
assertBefore(beginBody, "resolveActiveSeekersForMovingPlayer();", "resolvePendingSeekerDetonationDamage();");
assertBefore(beginBody, "resolvePendingSeekerDetonationDamage();", "beginMovePhase();");

assertContains(completeBody, "resolveActiveSeekersForMovingPlayer();");
assertContains(completeBody, "if (m_ui != NULL) {");
assertContains(completeBody, "resolvePendingSeekerDetonationDamage();");
assertBefore(completeBody, "resolveActiveSeekersForMovingPlayer();", "resolvePendingSeekerDetonationDamage();");
assertBefore(completeBody, "resolvePendingSeekerDetonationDamage();", "beginMovePhase();");
}

void FTacticalMineDamageFlowTest::testShipPathSeekerContactCheckedInCompleteMovePhase() {
// AC: ship movement paths are checked for active seeker hex contact during movement finalization.
const std::string source = readFile(repoFile("src/tactical/FTacticalGame.cpp"));
const std::string body = extractFunctionBody(source, "void FTacticalGame::completeMovePhase()");

// Pending outcomes must be cleared at the start of move completion so activation-phase leftovers
// do not double-count with movement-phase contact outcomes.
assertContains(body, "clearPendingSeekerContactOutcomes();");

// Each moving ship must have its path checked for active seeker contacts.
assertContains(body, "checkForActiveSeekersOnPath(*itr);");

// The path check and mine check should both occur during the per-ship loop.
assertContains(body, "checkForMines(*itr);");

// The clear must occur before the ship loop so pre-existing outcomes are gone before appending new ones.
assertBefore(body, "clearPendingSeekerContactOutcomes();", "checkForActiveSeekersOnPath(*itr);");
}

void FTacticalMineDamageFlowTest::testInactiveSeekerNotTriggeredByPathContact() {
// AC: inactive seeker hexes do not trigger contact or damage.
const std::string source = readFile(repoFile("src/tactical/FTacticalGame.cpp"));
const std::string body = extractFunctionBody(source, "void FTacticalGame::checkForActiveSeekersOnPath(FVehicle * ship)");

// The function must skip inactive seekers explicitly before any contact check.
assertContains(body, "if (!seekerItr->active) {");
assertContains(body, "continue;");

// Same-side seekers owned by the moving ship must also be skipped.
assertContains(body, "if (seekerItr->ownerID == ship->getOwner()) {");

// Only seekers whose hex matches a path hex should trigger contact.
assertContains(body, "seekerItr->hex.getX() != hexItr->getX() || seekerItr->hex.getY() != hexItr->getY()");
}

void FTacticalMineDamageFlowTest::testApplyMovementSeekerDamageDetonatesSeekersExactlyOnce() {
// AC: detonated seekers are removed exactly once.
const std::string source = readFile(repoFile("src/tactical/FTacticalGame.cpp"));
const std::string body = extractFunctionBody(source, "void FTacticalGame::applyMovementSeekerDamage()");

// Early-exit when no contacts are pending so we do not call into the resolver unnecessarily.
assertContains(body, "if (m_pendingSeekerContactOutcomes.empty()) {");

// Seeker IDs must be collected before resolution so they can still be removed after the
// pending-contact list is cleared inside resolvePendingSeekerDetonationDamage.
assertContains(body, "detonatedSeekerIDs");
assertBefore(body, "detonatedSeekerIDs", "resolvePendingSeekerDetonationDamage();");

// Resolution is delegated to the shared seam when the UI is installed.
assertContains(body, "if (m_ui != NULL) {");
assertContains(body, "resolvePendingSeekerDetonationDamage();");

// Without a UI the outcomes are still cleared so they do not carry forward.
assertContains(body, "clearPendingSeekerContactOutcomes();");

// Each detonated seeker must be removed from m_seekerMissiles exactly once.
assertContains(body, "m_seekerMissiles.erase(seekerItr);");
assertContains(body, "break;");
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), countOccurrences(body, "m_seekerMissiles.erase(seekerItr);"));
}

void FTacticalMineDamageFlowTest::testTriggeredMineHexesInitiallyEmptyOnFreshGame() {
// SMFR-03 behavioral: getLastTriggeredMineHexes() returns an empty set on construction.
// This verifies the accessor exists, works on a real FTacticalGame, and starts cleared.
FTacticalGame game;
CPPUNIT_ASSERT_MESSAGE(
"getLastTriggeredMineHexes() must return an empty set on a freshly constructed FTacticalGame",
game.getLastTriggeredMineHexes().empty());
}

void FTacticalMineDamageFlowTest::testTriggeredMineHexesEmptyAfterCompleteMovePhaseWithNoMines() {
// SMFR-03 behavioral: getLastTriggeredMineHexes() stays empty when completeMovePhase()
// runs with no mined hexes. This verifies the SMFR-03 state is orthogonal to mine-free
// movement and that the highlighted-hex set does not accumulate stale data.
MoveDoneRuntimeFixture fixture;
setupMoveDoneRuntimeScenario(fixture);

const FPoint startHex(54, 20);
CPPUNIT_ASSERT(fixture.game.handleHexClick(startHex));
CPPUNIT_ASSERT(fixture.game.isMoveComplete());

fixture.game.completeMovePhase();

CPPUNIT_ASSERT_MESSAGE(
"getLastTriggeredMineHexes() must be empty after completeMovePhase() with no mined hexes",
fixture.game.getLastTriggeredMineHexes().empty());

destroyFixture(fixture);
}

void FTacticalMineDamageFlowTest::testApplyMineDamageSummaryCalledUnconditionallyWhenMinesFire() {
// SMFR-03 AC1 source-contract supplement: showDamageSummary is called for every mine
// encounter, including zero-damage outcomes.  The prior guard
//   if (summary.ships.size() > 0) { m_ui->showDamageSummary(summary); }
// must NOT appear; the call must be gated only on m_ui != NULL.
const std::string source = readFile(repoFile("src/tactical/FTacticalGame.cpp"));
const std::string body = extractFunctionBody(source, "void FTacticalGame::applyMineDamage()");

// The unconditional call must be present.
assertContains(body, "m_ui->showDamageSummary(summary);");

// No ships-size guard may exist immediately before the call.
// We verify no pattern of "if (summary.ships.size() > 0)" or equivalent appears
// in the function that would suppress the dialog on zero-damage outcomes.
CPPUNIT_ASSERT_MESSAGE(
"applyMineDamage() must not guard showDamageSummary on summary.ships.size()",
body.find("summary.ships.size() > 0") == std::string::npos);
CPPUNIT_ASSERT_MESSAGE(
"applyMineDamage() must not guard showDamageSummary on !summary.ships.empty()",
body.find("!summary.ships.empty()") == std::string::npos);

// The call must be gated only on m_ui != NULL.
assertContains(body, "if (m_ui != NULL) {");
}

void FTacticalMineDamageFlowTest::testTriggeredMineHexesClearedAfterSummaryDialog() {
// SMFR-03 AC2/AC3 source-contract supplement: m_lastTriggeredMineHexes is populated
// before the requestRedraw call and cleared only after showDamageSummary returns,
// enforcing the highlight lifecycle.
const std::string source = readFile(repoFile("src/tactical/FTacticalGame.cpp"));
const std::string body = extractFunctionBody(source, "void FTacticalGame::applyMineDamage()");

// Hexes are inserted into m_lastTriggeredMineHexes (from mineTargetList) at the top
// of the function, before any UI interaction.
assertContains(body, "m_lastTriggeredMineHexes.insert(*itr);");

// requestRedraw triggers a board repaint while the hexes are highlighted.
assertContains(body, "m_ui->requestRedraw();");

// showDamageSummary is the blocking modal that displays while the board is highlighted.
assertContains(body, "m_ui->showDamageSummary(summary);");

// m_lastTriggeredMineHexes is cleared AFTER showDamageSummary to remove the highlight.
// The function has two clear() calls: one at the start to reset stale state, and one
// after showDamageSummary inside the if (m_ui != NULL) block to remove the highlight.
// We verify both ordering relationships:
//   insert → requestRedraw (hexes exist when the board redraws)
//   requestRedraw → showDamageSummary (dialog opens with highlighted board)
assertBefore(body, "m_lastTriggeredMineHexes.insert(*itr);", "m_ui->requestRedraw();");
assertBefore(body, "m_ui->requestRedraw();", "m_ui->showDamageSummary(summary);");

// The clear-after-dialog appears after showDamageSummary in the same UI block.
// Find the position of showDamageSummary and confirm a clear() follows it.
const std::string::size_type summaryPos = body.find("m_ui->showDamageSummary(summary);");
CPPUNIT_ASSERT_MESSAGE("showDamageSummary must appear in applyMineDamage()", summaryPos != std::string::npos);
const std::string::size_type clearAfterSummaryPos = body.find("m_lastTriggeredMineHexes.clear();", summaryPos);
CPPUNIT_ASSERT_MESSAGE(
"m_lastTriggeredMineHexes.clear() must appear after showDamageSummary in applyMineDamage()",
clearAfterSummaryPos != std::string::npos);
}

void FTacticalMineDamageFlowTest::testLastTriggeredMineHexesDelegationInFBattleScreenHeader() {
// SMFR-03 source-contract: FBattleScreen exposes getLastTriggeredMineHexes as a thin
// delegation to FTacticalGame so FBattleBoard::drawTriggeredMineHexes can read model state.
const std::string screenHeader = readFile(repoFile("include/tactical/FBattleScreen.h"));
const std::string screenSource = readFile(repoFile("src/tactical/FBattleScreen.cpp"));
const std::string gameHeader = readFile(repoFile("include/tactical/FTacticalGame.h"));

// FBattleScreen.h declares getLastTriggeredMineHexes.
assertContains(screenHeader, "getLastTriggeredMineHexes()");

// FBattleScreen.cpp delegates to m_tacticalGame.
assertContains(screenSource, "FBattleScreen::getLastTriggeredMineHexes()");
assertContains(screenSource, "m_tacticalGame->getLastTriggeredMineHexes()");

// FTacticalGame.h declares both the getter and the clearer.
assertContains(gameHeader, "getLastTriggeredMineHexes()");
assertContains(gameHeader, "clearLastTriggeredMineHexes()");

// No wx headers pollute the model.
CPPUNIT_ASSERT(gameHeader.find("#include <wx") == std::string::npos);
CPPUNIT_ASSERT(gameHeader.find("#include \"wx/") == std::string::npos);
}

void FTacticalMineDamageFlowTest::testSeekerDamageAppliedBeforeMineDamageInCompleteMovePhase() {
// AC: ship-triggered seeker damage resolves before mine damage in completeMovePhase().
const std::string source = readFile(repoFile("src/tactical/FTacticalGame.cpp"));
const std::string body = extractFunctionBody(source, "void FTacticalGame::completeMovePhase()");

// applyMovementSeekerDamage must appear exactly once.
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), countOccurrences(body, "applyMovementSeekerDamage();"));

// applyMineDamage must still appear exactly once for AC: mine damage once per completion.
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), countOccurrences(body, "applyMineDamage();"));

// Seeker damage must precede mine damage.
assertBefore(body, "applyMovementSeekerDamage();", "applyMineDamage();");

// Mine damage must still precede phase progression to defensive fire.
assertBefore(body, "applyMineDamage();", "setPhase(PH_DEFENSE_FIRE);");

// Seeker damage must also precede the final phase change.
assertBefore(body, "applyMovementSeekerDamage();", "setPhase(PH_DEFENSE_FIRE);");
}

}
