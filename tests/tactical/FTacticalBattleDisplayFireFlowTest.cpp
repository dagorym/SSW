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

void FTacticalBattleDisplayFireFlowTest::testFireAllWeaponsShowsSharedSummaryBeforeDestroyedShipCleanup() {
	// AC: defensive and offensive fire both build a tactical report, show the shared summary dialog, and only then clear destroyed ships.
	const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
	const std::string body = extractFunctionBody(source, "void FBattleDisplay::fireAllWeapons()");

	assertContains(body, "m_parent->fireICM();");
	assertContains(body, "beginTacticalReport(context);");
	assertContains(body, "getWeapon(i)->fire()");
	assertContains(body, "appendTacticalAttackReport");
	assertContains(body, "FTacticalCombatReportSummary summary = m_parent->buildCurrentTacticalReportSummary();");
	assertContains(body, "m_parent->showTacticalDamageSummaryDialog(summary);");
	assertContains(body, "m_parent->clearTacticalReport();");
	assertContains(body, "m_parent->clearDestroyedShips();");

	assertBefore(body, "beginTacticalReport(context);", "getWeapon(i)->fire()");
	assertBefore(body, "getWeapon(i)->fire()", "appendTacticalAttackReport");
	assertBefore(body, "appendTacticalAttackReport", "FTacticalCombatReportSummary summary = m_parent->buildCurrentTacticalReportSummary();");
	assertBefore(body, "FTacticalCombatReportSummary summary = m_parent->buildCurrentTacticalReportSummary();", "m_parent->showTacticalDamageSummaryDialog(summary);");
	assertBefore(body, "m_parent->showTacticalDamageSummaryDialog(summary);", "m_parent->clearTacticalReport();");
	assertBefore(body, "m_parent->clearTacticalReport();", "m_parent->clearDestroyedShips();");
	CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), countOccurrences(body, "m_parent->clearDestroyedShips();"));
}

void FTacticalBattleDisplayFireFlowTest::testBuildTacticalAttackReportPreservesStructuredAttackAndEffectDetailData() {
	// AC: the structured attack report preserves attacker, target, weapon, hit, damage, and per-effect detail data from FTacticalAttackResult.
	const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
	const std::string reportBody = extractFunctionBody(source, "FTacticalAttackReport buildTacticalAttackReport(const FTacticalAttackResult & result)");
	const std::string eventBody = extractFunctionBody(source, "FTacticalReportEvent buildTacticalAttackEvent(");

	assertContains(reportBody, "report.attacker = FTacticalShipReference(result.attackerID, result.attackerOwnerID, result.attackerName);");
	assertContains(reportBody, "report.target = FTacticalShipReference(result.targetID, result.targetOwnerID, result.targetName);");
	assertContains(reportBody, "report.weapon = FTacticalWeaponReference(result.weaponID, result.weaponName);");
	assertContains(reportBody, "report.hitRoll = result.hitRoll;");
	assertContains(reportBody, "report.targetRange = result.targetRange;");
	assertContains(reportBody, "report.hullDamage = result.totalHullDamageApplied;");
	assertContains(reportBody, "report.hit = result.hit();");
	assertContains(reportBody, "report.note = result.note;");
	assertContains(reportBody, "report.internalEvents.push_back(buildTacticalAttackEvent(report, *itr));");
	assertContains(eventBody, "event.subject = report.target;");
	assertContains(eventBody, "event.source = report.attacker;");
	assertContains(eventBody, "event.target = report.target;");
	assertContains(eventBody, "event.hullDamage = effect.hullDamageApplied;");
	assertContains(eventBody, "event.immediate = report.immediate;");
	assertContains(eventBody, "event.label = effect.label;");
	assertContains(eventBody, "event.detail = effect.detail;");
}

void FTacticalBattleDisplayFireFlowTest::testDefensiveFireDoneWaitsForSummaryAcknowledgementBeforeAdvancingToOffensiveFire() {
	// AC: after the defensive-fire summary dialog is acknowledged, the battle advances to offensive fire.
	const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
	const std::string body = extractFunctionBody(source, "void FBattleDisplay::onDefensiveFireDone( wxCommandEvent& event )");

	assertContains(body, "fireAllWeapons();");
	assertContains(body, "m_parent->setPhase(PH_ATTACK_FIRE);");
	assertContains(body, "m_parent->setWeapon(NULL);");
	assertBefore(body, "fireAllWeapons();", "m_parent->setPhase(PH_ATTACK_FIRE);");
	assertBefore(body, "m_parent->setPhase(PH_ATTACK_FIRE);", "m_parent->setWeapon(NULL);");
	CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(0), countOccurrences(body, "toggleMovingPlayer();"));
}

void FTacticalBattleDisplayFireFlowTest::testOffensiveFireDoneWaitsForSummaryAcknowledgementBeforeReturningToMovement() {
	// AC: after the offensive-fire summary dialog is acknowledged, the battle returns to movement and toggles the moving player exactly once.
	const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
	const std::string body = extractFunctionBody(source, "void FBattleDisplay::onOffensiveFireDone( wxCommandEvent& event )");

	assertContains(body, "fireAllWeapons();");
	assertContains(body, "m_parent->toggleMovingPlayer();");
	assertContains(body, "m_parent->setPhase(PH_MOVE);");
	assertBefore(body, "fireAllWeapons();", "m_parent->toggleMovingPlayer();");
	assertBefore(body, "m_parent->toggleMovingPlayer();", "m_parent->setPhase(PH_MOVE);");
	CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), countOccurrences(body, "m_parent->toggleMovingPlayer();"));
}

}
