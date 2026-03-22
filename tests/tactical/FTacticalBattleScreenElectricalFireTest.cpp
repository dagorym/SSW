/**
 * @file FTacticalBattleScreenElectricalFireTest.cpp
 * @brief Implementation file for the FTacticalBattleScreenElectricalFireTest class
 */

#include "FTacticalBattleScreenElectricalFireTest.h"

#include <fstream>
#include <iterator>
#include <vector>

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

CPPUNIT_TEST_SUITE_REGISTRATION( FTacticalBattleScreenElectricalFireTest );

void FTacticalBattleScreenElectricalFireTest::setUp() {
}

void FTacticalBattleScreenElectricalFireTest::tearDown() {
}

std::string FTacticalBattleScreenElectricalFireTest::readFile(const std::string & path) {
	std::ifstream file(path.c_str());
	CPPUNIT_ASSERT_MESSAGE(path, file.is_open());
	return std::string((std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());
}

std::string FTacticalBattleScreenElectricalFireTest::extractFunctionBody(const std::string & source, const std::string & signature) {
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

void FTacticalBattleScreenElectricalFireTest::assertAppearsInOrder(const std::string & haystack, const std::vector<std::string> & needles) {
	std::string::size_type searchFrom = 0;
	for (std::vector<std::string>::const_iterator itr = needles.begin(); itr != needles.end(); ++itr) {
		std::string::size_type found = haystack.find(*itr, searchFrom);
		CPPUNIT_ASSERT_MESSAGE(std::string("Expected to find '") + *itr + "' in the requested source order", found != std::string::npos);
		searchFrom = found + itr->size();
	}
}

void FTacticalBattleScreenElectricalFireTest::testApplyFireDamageStartsImmediateElectricalFireReportBeforeCleanup() {
	// AC: electrical fire damage starts an immediate tactical report before any destroyed-ship cleanup occurs.
	const std::string source = readFile(repoFile("src/tactical/FBattleScreen.cpp"));
	const std::string body = extractFunctionBody(source, "void FBattleScreen::applyFireDamage()");

	std::vector<std::string> sequence;
	sequence.push_back("clearTacticalReport();");
	sequence.push_back("context.reportType = TRT_ElectricalFire;");
	sequence.push_back("context.phase = m_phase;");
	sequence.push_back("context.actingPlayerID = getMovingPlayerID();");
	sequence.push_back("context.immediate = true;");
	sequence.push_back("beginTacticalReport(context);");
	sequence.push_back("VehicleList sList = getShipList(getMovingPlayerID());");
	sequence.push_back("appendTacticalDamageResolutionEvents(m_tacticalReport, *itr, resolution);");
	sequence.push_back("FTacticalCombatReportSummary summary = buildCurrentTacticalReportSummary();");
	sequence.push_back("showTacticalDamageSummaryDialog(summary);");
	sequence.push_back("toggleActivePlayer();");
	sequence.push_back("clearDestroyedShips();");
	sequence.push_back("toggleActivePlayer();");
	sequence.push_back("clearTacticalReport();");
	assertAppearsInOrder(body, sequence);
}

void FTacticalBattleScreenElectricalFireTest::testApplyFireDamageUsesSharedTacticalDamageSummaryDialog() {
	// AC: the electrical-fire flow uses the shared tactical damage summary dialog for the report.
	const std::string source = readFile(repoFile("src/tactical/FBattleScreen.cpp"));
	const std::string applyBody = extractFunctionBody(source, "void FBattleScreen::applyFireDamage()");
	const std::string dialogBody = extractFunctionBody(source, "int FBattleScreen::showTacticalDamageSummaryDialog(const FTacticalCombatReportSummary & summary)");

	assertContains(applyBody, "showTacticalDamageSummaryDialog(summary);");
	assertContains(applyBody, "appendTacticalDamageResolutionEvents(m_tacticalReport, *itr, resolution);");
	assertContains(dialogBody, "TacticalDamageSummaryGUI dialog(this, summary);");
	assertContains(dialogBody, "return dialog.ShowModal();");
}

void FTacticalBattleScreenElectricalFireTest::testApplyFireDamageRestoresMovingPlayerAfterDestroyedShipCleanup() {
	// AC: destroyed ships are cleaned up only after the modal summary closes, and control returns to the moving player.
	const std::string source = readFile(repoFile("src/tactical/FBattleScreen.cpp"));
	const std::string body = extractFunctionBody(source, "void FBattleScreen::applyFireDamage()");

	std::vector<std::string> sequence;
	sequence.push_back("context.reportType = TRT_ElectricalFire;");
	sequence.push_back("context.immediate = true;");
	sequence.push_back("beginTacticalReport(context);");
	sequence.push_back("VehicleList sList = getShipList(getMovingPlayerID());");
	sequence.push_back("appendTacticalDamageResolutionEvents(m_tacticalReport, *itr, resolution);");
	sequence.push_back("FTacticalCombatReportSummary summary = buildCurrentTacticalReportSummary();");
	sequence.push_back("showTacticalDamageSummaryDialog(summary);");
	sequence.push_back("toggleActivePlayer();");
	sequence.push_back("clearDestroyedShips();");
	sequence.push_back("toggleActivePlayer();");
	sequence.push_back("clearTacticalReport();");
	assertAppearsInOrder(body, sequence);

	CPPUNIT_ASSERT(body.find("setPhase(PH_MOVE)") == std::string::npos);
}

}
