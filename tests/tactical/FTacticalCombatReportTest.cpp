/**
 * @file FTacticalCombatReportTest.cpp
 * @brief Implementation file for the FTacticalCombatReportTest class
 */

#include "FTacticalCombatReportTest.h"

#include <fstream>

namespace FrontierTests {
using namespace Frontier;

namespace {

std::string repoFile(const std::string & relativePath) {
	return std::string(TACTICAL_TEST_REPO_ROOT) + "/" + relativePath;
}

}

CPPUNIT_TEST_SUITE_REGISTRATION( FTacticalCombatReportTest );

void FTacticalCombatReportTest::setUp() {
}

void FTacticalCombatReportTest::tearDown() {
}

std::string FTacticalCombatReportTest::readFile(const std::string & path) {
	std::ifstream file(path.c_str());
	CPPUNIT_ASSERT_MESSAGE(path, file.is_open());
	return std::string((std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());
}

std::string FTacticalCombatReportTest::extractFunctionBody(const std::string & source, const std::string & signature) {
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

void FTacticalCombatReportTest::testReportModelPreservesRawAttackAndImmediateEventData() {
	// AC: preserve raw per-attack data, nested internal events, and report-level immediate events.
	FTacticalReportEvent internalEvent;
	internalEvent.eventType = TRET_InternalDamage;
	internalEvent.subject = FTacticalShipReference(7, 1, "Defender");
	internalEvent.rollValue = 8;
	internalEvent.hullDamage = 2;
	internalEvent.attackIndex = 0;
	internalEvent.label = "Engine hit";
	internalEvent.detail = "ADF reduced";

	FTacticalAttackReport attack;
	attack.attacker = FTacticalShipReference(3, 0, "Attacker");
	attack.target = FTacticalShipReference(7, 1, "Defender");
	attack.weapon = FTacticalWeaponReference(11, "Laser Battery");
	attack.hitRoll = 4;
	attack.attackRollModifier = -1;
	attack.targetRange = 3;
	attack.hullDamage = 6;
	attack.damageTableModifier = 2;
	attack.hit = true;
	attack.intercepted = false;
	attack.immediate = false;
	attack.note = "rear arc";
	attack.internalEvents.push_back(internalEvent);

	FTacticalReportEvent immediateEvent;
	immediateEvent.eventType = TRET_ElectricalFire;
	immediateEvent.subject = FTacticalShipReference(7, 1, "Defender");
	immediateEvent.rollValue = 10;
	immediateEvent.hullDamage = 4;
	immediateEvent.immediate = true;
	immediateEvent.label = "Electrical fire";
	immediateEvent.detail = "bridge section";

	FTacticalCombatReport report;
	report.context.reportType = TRT_OffensiveFire;
	report.context.phase = 42;
	report.context.actingPlayerID = 9;
	report.context.title = "Round 1";
	report.active = true;
	report.attacks.push_back(attack);
	report.events.push_back(immediateEvent);

	CPPUNIT_ASSERT(report.active);
	CPPUNIT_ASSERT(report.context.reportType == TRT_OffensiveFire);
	CPPUNIT_ASSERT(report.context.phase == 42);
	CPPUNIT_ASSERT(report.attacks.size() == 1);
	CPPUNIT_ASSERT(report.events.size() == 1);
	CPPUNIT_ASSERT(report.attacks[0].attacker.shipName == "Attacker");
	CPPUNIT_ASSERT(report.attacks[0].target.shipName == "Defender");
	CPPUNIT_ASSERT(report.attacks[0].weapon.weaponName == "Laser Battery");
	CPPUNIT_ASSERT(report.attacks[0].internalEvents.size() == 1);
	CPPUNIT_ASSERT(report.attacks[0].internalEvents[0].detail == "ADF reduced");
	CPPUNIT_ASSERT(report.events[0].immediate);
	CPPUNIT_ASSERT(report.events[0].detail == "bridge section");

	report.clear();
	CPPUNIT_ASSERT(!report.active);
	CPPUNIT_ASSERT(report.attacks.empty());
	CPPUNIT_ASSERT(report.events.empty());
	CPPUNIT_ASSERT(report.context.reportType == TRT_None);
}

void FTacticalCombatReportTest::testReportModelDefinesSeparateShipSummaryRollupTypes() {
	// AC: summary rollup is distinct from raw report storage.
	FTacticalCombatReport report;
	FTacticalCombatReportSummary summary;
	FTacticalShipReportSummary shipSummary;

	report.attacks.push_back(FTacticalAttackReport());
	report.events.push_back(FTacticalReportEvent());
	shipSummary.ship = FTacticalShipReference(13, 2, "Cruiser");
	shipSummary.attacksMade = 2;
	shipSummary.hitsScored = 1;
	shipSummary.hullDamageDealt = 5;
	shipSummary.displayLines.push_back("Laser Battery: hit for 5 damage");
	summary.ships.push_back(shipSummary);
	summary.displayLines.push_back("Cruiser: 2 attacks, 1 hits scored, 5 damage dealt, 0 damage taken");

	CPPUNIT_ASSERT(report.attacks.size() == 1);
	CPPUNIT_ASSERT(report.events.size() == 1);
	CPPUNIT_ASSERT(summary.ships.size() == 1);
	CPPUNIT_ASSERT(summary.ships[0].ship.shipName == "Cruiser");
	CPPUNIT_ASSERT(summary.ships[0].displayLines.size() == 1);
	CPPUNIT_ASSERT(summary.displayLines.size() == 1);
	CPPUNIT_ASSERT(summary.displayLines[0].find("Cruiser") != std::string::npos);
}

void FTacticalCombatReportTest::testReportModelSupportsAllPlannedReportingModes() {
	// AC: support defensive fire, offensive fire, electrical fire, and mine damage reporting.
	CPPUNIT_ASSERT(TRT_DefensiveFire != TRT_None);
	CPPUNIT_ASSERT(TRT_OffensiveFire != TRT_None);
	CPPUNIT_ASSERT(TRT_ElectricalFire != TRT_None);
	CPPUNIT_ASSERT(TRT_MineDamage != TRT_None);
	CPPUNIT_ASSERT(TRET_InternalDamage != TRET_None);
	CPPUNIT_ASSERT(TRET_ElectricalFire != TRET_None);
	CPPUNIT_ASSERT(TRET_MineDamage != TRET_None);
	CPPUNIT_ASSERT(TRET_DefenseEffect != TRET_None);
	CPPUNIT_ASSERT(TRET_Note != TRET_None);
}

void FTacticalCombatReportTest::testBattleScreenExposesTacticalReportLifecycleApi() {
	// AC: FBattleScreen exposes begin/append/build/clear APIs for current report state.
	const std::string header = readFile(repoFile("include/tactical/FBattleScreen.h"));
	CPPUNIT_ASSERT(header.find("void beginTacticalReport(const FTacticalCombatReportContext & context);") != std::string::npos);
	CPPUNIT_ASSERT(header.find("void appendTacticalAttackReport(const FTacticalAttackReport & attack);") != std::string::npos);
	CPPUNIT_ASSERT(header.find("void appendTacticalReportEvent(const FTacticalReportEvent & event);") != std::string::npos);
	CPPUNIT_ASSERT(header.find("FTacticalCombatReportSummary buildCurrentTacticalReportSummary() const;") != std::string::npos);
	CPPUNIT_ASSERT(header.find("void clearTacticalReport();") != std::string::npos);
	CPPUNIT_ASSERT(header.find("const FTacticalCombatReport & getCurrentTacticalReport() const { return m_tacticalReport; }") != std::string::npos);
	CPPUNIT_ASSERT(header.find("FTacticalCombatReport m_tacticalReport;") != std::string::npos);
}

void FTacticalCombatReportTest::testBattleScreenBuildsShipSummaryFromRawReportData() {
	// AC: preserve raw data while building a separate per-ship display rollup.
	const std::string source = readFile(repoFile("src/tactical/FBattleScreen.cpp"));
	const std::string summaryBody =
		extractFunctionBody(source, "FTacticalCombatReportSummary FBattleScreen::buildCurrentTacticalReportSummary() const");

	CPPUNIT_ASSERT(summaryBody.find("std::map<TacticalShipSummaryKey, FTacticalShipReportSummary> summaryMap;") != std::string::npos);
	CPPUNIT_ASSERT(summaryBody.find("for (unsigned int i = 0; i < m_tacticalReport.attacks.size(); i++)") != std::string::npos);
	CPPUNIT_ASSERT(summaryBody.find("const FTacticalAttackReport & attack = m_tacticalReport.attacks[i];") != std::string::npos);
	CPPUNIT_ASSERT(summaryBody.find("for (unsigned int j = 0; j < attack.internalEvents.size(); j++)") != std::string::npos);
	CPPUNIT_ASSERT(summaryBody.find("for (unsigned int i = 0; i < m_tacticalReport.events.size(); i++)") != std::string::npos);
	CPPUNIT_ASSERT(summaryBody.find("summary.ships.push_back(summaryMap[summaryOrder[i]]);") != std::string::npos);
	CPPUNIT_ASSERT(summaryBody.find("summary.displayLines.push_back(os.str());") != std::string::npos);
}

void FTacticalCombatReportTest::testBattleScreenOnlyTouchesPhaseFlowThroughFleetSetupReset() {
	// AC: no phase behavior changes beyond report-state reset on fleet setup.
	const std::string source = readFile(repoFile("src/tactical/FBattleScreen.cpp"));
	const std::string setupBody =
		extractFunctionBody(source, "int FBattleScreen::setupFleets(FleetList *aList, FleetList *dList, bool planet, FVehicle * station)");
	const std::string setPhaseBody =
		extractFunctionBody(source, "void FBattleScreen::setPhase(int p)");
	const std::string applyFireDamageBody =
		extractFunctionBody(source, "void FBattleScreen::applyFireDamage()");

	CPPUNIT_ASSERT(setupBody.find("clearTacticalReport();") != std::string::npos);
	CPPUNIT_ASSERT(setPhaseBody.find("m_tacticalReport") == std::string::npos);
	CPPUNIT_ASSERT(setPhaseBody.find("beginTacticalReport") == std::string::npos);
	CPPUNIT_ASSERT(setPhaseBody.find("appendTacticalAttackReport") == std::string::npos);
	CPPUNIT_ASSERT(setPhaseBody.find("appendTacticalReportEvent") == std::string::npos);
	CPPUNIT_ASSERT(setPhaseBody.find("buildCurrentTacticalReportSummary") == std::string::npos);
	CPPUNIT_ASSERT(setPhaseBody.find("clearTacticalReport") == std::string::npos);
	CPPUNIT_ASSERT(applyFireDamageBody.find("m_tacticalReport") == std::string::npos);
	CPPUNIT_ASSERT(applyFireDamageBody.find("appendTactical") == std::string::npos);
}

}
