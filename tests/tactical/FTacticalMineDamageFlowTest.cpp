/**
 * @file FTacticalMineDamageFlowTest.cpp
 * @brief Implementation file for the FTacticalMineDamageFlowTest class
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
	// AC: mine damage builds an immediate tactical summary, shows it before cleanup, and leaves destroyed ships visible until after the dialog.
	const std::string source = readFile(repoFile("src/tactical/FBattleBoard.cpp"));
	const std::string body = extractFunctionBody(source, "void FBattleBoard::applyMineDamage()");

	assertContains(body, "context.reportType = TRT_MineDamage;");
	assertContains(body, "context.immediate = true;");
	assertContains(body, "m_parent->beginTacticalReport(context);");
	assertContains(body, "m_parent->appendTacticalAttackReport(buildMineDamageAttackReport(*itr, result));");
	assertContains(body, "FTacticalCombatReportSummary summary = m_parent->buildCurrentTacticalReportSummary();");
	assertContains(body, "m_parent->showTacticalDamageSummaryDialog(summary);");
	assertContains(body, "m_parent->clearTacticalReport();");
	assertContains(body, "m_parent->clearDestroyedShips();");

	assertBefore(body, "m_parent->appendTacticalAttackReport(buildMineDamageAttackReport(*itr, result));", "FTacticalCombatReportSummary summary = m_parent->buildCurrentTacticalReportSummary();");
	assertBefore(body, "FTacticalCombatReportSummary summary = m_parent->buildCurrentTacticalReportSummary();", "m_parent->showTacticalDamageSummaryDialog(summary);");
	assertBefore(body, "m_parent->showTacticalDamageSummaryDialog(summary);", "m_parent->clearTacticalReport();");
	assertBefore(body, "m_parent->clearTacticalReport();", "m_parent->clearDestroyedShips();");
}

void FTacticalMineDamageFlowTest::testFinalizeMoveAppliesMineDamageExactlyOncePerMovementResolution() {
	// AC: mine damage is triggered once per movement resolution and only after all move processing is complete.
	const std::string source = readFile(repoFile("src/tactical/FBattleBoard.cpp"));
	const std::string body = extractFunctionBody(source, "void FBattleBoard::finalizeMove()");

	CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), countOccurrences(body, "applyMineDamage();"));
	assertContains(body, "// Apply mine damage if any.");
	assertBefore(body, "// Apply mine damage if any.", "applyMineDamage();");
	assertBefore(body, "applyMineDamage();", "m_drawRoute = false;");
}

void FTacticalMineDamageFlowTest::testMineDamageAttackReportBuilderLeavesNestedEventsUnattachedUntilBattleScreenStoresThem() {
	// AC: mine-damage report construction preserves raw nested detail as unattached until FBattleScreen normalizes the copied attack.
	const std::string source = readFile(repoFile("src/tactical/FBattleBoard.cpp"));
	const std::string eventBody = extractFunctionBody(source, "FTacticalReportEvent buildMineDamageEvent(");
	const std::string reportBody = extractFunctionBody(source, "FTacticalAttackReport buildMineDamageAttackReport(");

	assertContains(eventBody, "reportEvent.eventType = TRET_MineDamage;");
	assertContains(eventBody, "reportEvent.subject = target;");
	assertContains(eventBody, "reportEvent.target = target;");
	assertContains(eventBody, "reportEvent.hullDamage = effect.hullDamageApplied;");
	assertContains(eventBody, "reportEvent.attackIndex = -1;");
	assertContains(eventBody, "reportEvent.immediate = true;");
	assertContains(reportBody, "report.target = FTacticalShipReference(result.targetID, result.targetOwnerID, result.targetName);");
	assertContains(reportBody, "report.internalEvents.push_back(buildMineDamageEvent(report.target, *itr));");
}

}
