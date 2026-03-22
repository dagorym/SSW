/**
 * @file FTacticalBattleScreenReportContextTest.cpp
 * @brief Implementation file for the FTacticalBattleScreenReportContextTest class
 */

#include "FTacticalBattleScreenReportContextTest.h"

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

}

CPPUNIT_TEST_SUITE_REGISTRATION( FTacticalBattleScreenReportContextTest );

void FTacticalBattleScreenReportContextTest::setUp() {
}

void FTacticalBattleScreenReportContextTest::tearDown() {
}

std::string FTacticalBattleScreenReportContextTest::readFile(const std::string & path) {
	std::ifstream file(path.c_str());
	CPPUNIT_ASSERT_MESSAGE(path, file.is_open());
	return std::string((std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());
}

std::string FTacticalBattleScreenReportContextTest::extractFunctionBody(const std::string & source, const std::string & signature) {
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

void FTacticalBattleScreenReportContextTest::testAppendTacticalAttackReportPreservesImmediateFlagWhenLazySeedingContext() {
	// AC: a lazily seeded attack report keeps the attack's immediate/end-of-phase mode in the report context.
	const std::string source = readFile(repoFile("src/tactical/FBattleScreen.cpp"));
	const std::string body = extractFunctionBody(source, "void FBattleScreen::appendTacticalAttackReport(const FTacticalAttackReport & attack)");

	assertContains(body, "if (!m_tacticalReport.active)");
	assertContains(body, "context.phase = m_phase;");
	assertContains(body, "context.actingPlayerID = getActivePlayerID();");
	assertContains(body, "context.immediate = attack.immediate;");
	assertContains(body, "beginTacticalReport(context);");
	assertContains(body, "m_tacticalReport.attacks.push_back(attack);");
}

void FTacticalBattleScreenReportContextTest::testAppendTacticalReportEventStillPreservesImmediateFlagWhenLazySeedingContext() {
	// AC: report events continue to preserve their immediate flag when they seed a new report context.
	const std::string source = readFile(repoFile("src/tactical/FBattleScreen.cpp"));
	const std::string body = extractFunctionBody(source, "void FBattleScreen::appendTacticalReportEvent(const FTacticalReportEvent & event)");

	assertContains(body, "if (!m_tacticalReport.active)");
	assertContains(body, "context.phase = m_phase;");
	assertContains(body, "context.actingPlayerID = getActivePlayerID();");
	assertContains(body, "context.immediate = event.immediate;");
	assertContains(body, "beginTacticalReport(context);");
	assertContains(body, "m_tacticalReport.events.push_back(event);");
}

}
