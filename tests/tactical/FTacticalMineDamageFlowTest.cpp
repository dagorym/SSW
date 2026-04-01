/**
 * @file FTacticalMineDamageFlowTest.cpp
 * @brief Implementation file for FTacticalGame mine-damage flow tests
 */

#include "FTacticalMineDamageFlowTest.h"

#include <fstream>

namespace FrontierTests {

namespace {

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

void FTacticalMineDamageFlowTest::testMineDamageAttackReportBuilderLeavesNestedEventsUnattachedUntilBattleScreenStoresThem() {
// AC: mine damage uses common tactical attack-report builder path and not board-local custom builders.
const std::string source = readFile(repoFile("src/tactical/FTacticalGame.cpp"));
const std::string body = extractFunctionBody(source, "void FTacticalGame::applyMineDamage()");

assertContains(body, "FTacticalAttackResult result = (*itr)->fire();");
assertContains(body, "appendTacticalAttackReport(buildTacticalAttackReport(result));");
CPPUNIT_ASSERT(body.find("buildMineDamageAttackReport(") == std::string::npos);
CPPUNIT_ASSERT(body.find("buildMineDamageEvent(") == std::string::npos);
}

}
