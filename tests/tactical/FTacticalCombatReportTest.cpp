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

const FTacticalShipReportSummary * findShipSummary(
	const FTacticalCombatReportSummary & summary,
	const std::string & shipName) {
	for (unsigned int i = 0; i < summary.ships.size(); i++) {
		if (summary.ships[i].ship.shipName == shipName) {
			return &summary.ships[i];
		}
	}
	return NULL;
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

void FTacticalCombatReportTest::testBuildTacticalCombatReportSummaryCapturesImmediateElectricalFireAndMineDamageEvents() {
	// AC: immediate electrical-fire and mine-damage reports preserve their event types and keep non-hull-damage ships visible in the summary.
	FTacticalCombatReport report;
	report.context.reportType = TRT_ElectricalFire;
	report.context.phase = 17;
	report.context.actingPlayerID = 4;
	report.context.immediate = true;
	report.context.title = "Immediate damage";

	FTacticalReportEvent electricalFire;
	electricalFire.eventType = TRET_ElectricalFire;
	electricalFire.subject = FTacticalShipReference(21, 2, "Electrical Target");
	electricalFire.rollValue = 9;
	electricalFire.hullDamage = 0;
	electricalFire.immediate = true;
	electricalFire.label = "Electrical fire";

	FTacticalReportEvent mineDamage;
	mineDamage.eventType = TRET_MineDamage;
	mineDamage.subject = FTacticalShipReference(22, 3, "Mine Target");
	mineDamage.rollValue = 5;
	mineDamage.hullDamage = 0;
	mineDamage.immediate = true;
	mineDamage.label = "Mine damage";

	report.events.push_back(electricalFire);
	report.events.push_back(mineDamage);

	const FTacticalCombatReportSummary summary = buildTacticalCombatReportSummary(report);
	const FTacticalShipReportSummary * electricalSummary = findShipSummary(summary, "Electrical Target");
	const FTacticalShipReportSummary * mineSummary = findShipSummary(summary, "Mine Target");

	CPPUNIT_ASSERT(summary.context.immediate);
	CPPUNIT_ASSERT(summary.context.reportType == TRT_ElectricalFire);
	CPPUNIT_ASSERT(summary.context.phase == 17);
	CPPUNIT_ASSERT(summary.ships.size() == 2);
	CPPUNIT_ASSERT(summary.displayLines.size() == 2);
	CPPUNIT_ASSERT(electricalSummary != NULL);
	CPPUNIT_ASSERT(mineSummary != NULL);
	CPPUNIT_ASSERT(electricalSummary->hullDamageTaken == 0);
	CPPUNIT_ASSERT(electricalSummary->nonHullEffectsTaken == 1);
	CPPUNIT_ASSERT(electricalSummary->rawEvents.size() == 1);
	CPPUNIT_ASSERT(electricalSummary->rawEvents[0].eventType == TRET_ElectricalFire);
	CPPUNIT_ASSERT(electricalSummary->displayLines[0].find("Electrical fire") != std::string::npos);
	CPPUNIT_ASSERT(mineSummary->hullDamageTaken == 0);
	CPPUNIT_ASSERT(mineSummary->nonHullEffectsTaken == 1);
	CPPUNIT_ASSERT(mineSummary->rawEvents.size() == 1);
	CPPUNIT_ASSERT(mineSummary->rawEvents[0].eventType == TRET_MineDamage);
	CPPUNIT_ASSERT(mineSummary->displayLines[0].find("Mine damage") != std::string::npos);
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

void FTacticalCombatReportTest::testBuildTacticalCombatReportSummaryAggregatesMultipleAttacksPerShip() {
	// AC: multiple attacks against one ship aggregate into one player-facing ship summary.
	FTacticalCombatReport report;

	FTacticalAttackReport firstAttack;
	firstAttack.attacker = FTacticalShipReference(1, 1, "Destroyer");
	firstAttack.target = FTacticalShipReference(2, 2, "Frigate");
	firstAttack.hit = true;
	firstAttack.hullDamage = 3;

	FTacticalAttackReport secondAttack = firstAttack;
	secondAttack.attacker = FTacticalShipReference(3, 1, "Cruiser");
	secondAttack.hullDamage = 2;

	report.attacks.push_back(firstAttack);
	report.attacks.push_back(secondAttack);

	const FTacticalCombatReportSummary summary = buildTacticalCombatReportSummary(report);
	const FTacticalShipReportSummary * shipSummary = findShipSummary(summary, "Frigate");

	CPPUNIT_ASSERT(shipSummary != NULL);
	CPPUNIT_ASSERT(summary.ships.size() == 1);
	CPPUNIT_ASSERT(summary.displayLines.size() == 1);
	CPPUNIT_ASSERT(shipSummary->attacksReceived == 2);
	CPPUNIT_ASSERT(shipSummary->hitsTaken == 2);
	CPPUNIT_ASSERT(shipSummary->damagingAttacksReceived == 2);
	CPPUNIT_ASSERT(shipSummary->hullDamageTaken == 5);
	CPPUNIT_ASSERT(shipSummary->displayLines.size() == 1);
	CPPUNIT_ASSERT(shipSummary->displayLines[0].find("Frigate: 5 hull damage from 2 attacks") != std::string::npos);
}

void FTacticalCombatReportTest::testBuildTacticalCombatReportSummarySummarizesHullDamageAndEffects() {
	// AC: summary line carries both hull damage and non-hull effects while raw details remain available.
	FTacticalCombatReport report;

	FTacticalAttackReport attack;
	attack.attacker = FTacticalShipReference(1, 1, "Destroyer");
	attack.target = FTacticalShipReference(2, 2, "Frigate");
	attack.hit = true;
	attack.hullDamage = 4;

	FTacticalReportEvent internalDamage;
	internalDamage.eventType = TRET_InternalDamage;
	internalDamage.subject = attack.target;
	internalDamage.label = "ADF reduced";
	internalDamage.hullDamage = 0;
	attack.internalEvents.push_back(internalDamage);

	report.attacks.push_back(attack);

	FTacticalReportEvent fireEvent;
	fireEvent.eventType = TRET_ElectricalFire;
	fireEvent.subject = attack.target;
	fireEvent.label = "Electrical fire";
	report.events.push_back(fireEvent);

	const FTacticalCombatReportSummary summary = buildTacticalCombatReportSummary(report);
	const FTacticalShipReportSummary * shipSummary = findShipSummary(summary, "Frigate");

	CPPUNIT_ASSERT(shipSummary != NULL);
	CPPUNIT_ASSERT(shipSummary->hullDamageTaken == 4);
	CPPUNIT_ASSERT(shipSummary->nonHullEffectsTaken == 2);
	CPPUNIT_ASSERT(shipSummary->internalEventsTriggered == 2);
	CPPUNIT_ASSERT(shipSummary->rawAttacksReceived.size() == 1);
	CPPUNIT_ASSERT(shipSummary->rawEvents.size() == 2);
	CPPUNIT_ASSERT(shipSummary->displayLines.size() == 1);
	CPPUNIT_ASSERT(shipSummary->displayLines[0].find("4 hull damage") != std::string::npos);
	CPPUNIT_ASSERT(shipSummary->displayLines[0].find("ADF reduced") != std::string::npos);
	CPPUNIT_ASSERT(shipSummary->displayLines[0].find("Electrical fire") != std::string::npos);
}

void FTacticalCombatReportTest::testBuildTacticalCombatReportSummaryOmitsUndamagedShips() {
	// AC: ships without hull damage or damaging effects stay out of the player-facing summary.
	FTacticalCombatReport report;

	FTacticalAttackReport miss;
	miss.attacker = FTacticalShipReference(1, 1, "Destroyer");
	miss.target = FTacticalShipReference(2, 2, "Frigate");
	miss.hit = false;
	miss.hullDamage = 0;
	report.attacks.push_back(miss);

	FTacticalReportEvent note;
	note.eventType = TRET_Note;
	note.subject = miss.target;
	note.label = "Target evaded";
	report.events.push_back(note);

	const FTacticalCombatReportSummary summary = buildTacticalCombatReportSummary(report);

	CPPUNIT_ASSERT(summary.ships.empty());
	CPPUNIT_ASSERT(summary.displayLines.empty());
}

void FTacticalCombatReportTest::testBuildTacticalCombatReportSummaryUsesStoredShipReferences() {
	// AC: aggregation uses stored ship references, not board state or live ship objects.
	FTacticalCombatReport report;

	FTacticalAttackReport attack;
	attack.attacker = FTacticalShipReference(10, 1, "Destroyer");
	attack.target = FTacticalShipReference(77, 2, "Destroyed Frigate");
	attack.hit = true;
	attack.hullDamage = 6;
	report.attacks.push_back(attack);

	FTacticalReportEvent mineDamage;
	mineDamage.eventType = TRET_MineDamage;
	mineDamage.subject = attack.target;
	mineDamage.label = "Mine damage";
	report.events.push_back(mineDamage);

	const FTacticalCombatReportSummary summary = buildTacticalCombatReportSummary(report);
	const FTacticalShipReportSummary * shipSummary = findShipSummary(summary, "Destroyed Frigate");

	CPPUNIT_ASSERT(shipSummary != NULL);
	CPPUNIT_ASSERT(shipSummary->ship.shipID == 77);
	CPPUNIT_ASSERT(shipSummary->ship.ownerID == 2);
	CPPUNIT_ASSERT(shipSummary->rawAttacksReceived.size() == 1);
	CPPUNIT_ASSERT(shipSummary->rawAttacksReceived[0].target.shipID == 77);
	CPPUNIT_ASSERT(shipSummary->rawEvents.size() == 1);
	CPPUNIT_ASSERT(shipSummary->rawEvents[0].subject.shipName == "Destroyed Frigate");
	CPPUNIT_ASSERT(shipSummary->displayLines[0].find("Destroyed Frigate") != std::string::npos);
}

void FTacticalCombatReportTest::testBattleScreenDelegatesSummaryGenerationToModelBuilder() {
	// AC: summary logic stays in the model; FBattleScreen delegates directly to it.
	const std::string source = readFile(repoFile("src/tactical/FBattleScreen.cpp"));
	const std::string summaryBody =
		extractFunctionBody(source, "FTacticalCombatReportSummary FBattleScreen::buildCurrentTacticalReportSummary() const");

	CPPUNIT_ASSERT(summaryBody.find("return buildTacticalCombatReportSummary(m_tacticalReport);") != std::string::npos);
	CPPUNIT_ASSERT(summaryBody.find("std::map<") == std::string::npos);
	CPPUNIT_ASSERT(summaryBody.find("rawAttacksReceived") == std::string::npos);
	CPPUNIT_ASSERT(summaryBody.find("rawEvents") == std::string::npos);
}

void FTacticalCombatReportTest::testVehicleDamageReportingApiCapturesExplicitEffectTypes() {
	// AC: FVehicle damage reporting exposes optional result output and all planned effect categories.
	const std::string vehicleHeader = readFile(repoFile("include/ships/FVehicle.h"));
	const std::string vehicleSource = readFile(repoFile("src/ships/FVehicle.cpp"));
	const std::string reportHeader = readFile(repoFile("include/tactical/FTacticalCombatReport.h"));

	CPPUNIT_ASSERT(vehicleHeader.find("virtual void takeDamage (int damage, int damageMod = 0, bool basic = false, FTacticalDamageResolution * result = NULL);") != std::string::npos);
	CPPUNIT_ASSERT(vehicleHeader.find("virtual void advancedDamage(int damage, int damageMod, FTacticalDamageResolution * result = NULL);") != std::string::npos);
	CPPUNIT_ASSERT(vehicleSource.find("initializeDamageResolution(result, damage, damageMod, !basic);") != std::string::npos);
	CPPUNIT_ASSERT(vehicleSource.find("appendHullDamageEffect(result, previousHP, getHP(), -1, damage);") != std::string::npos);
	CPPUNIT_ASSERT(vehicleSource.find("result->damageTableRoll = roll;") != std::string::npos);
	CPPUNIT_ASSERT(vehicleSource.find("effect.weaponID = (*wItr)->getID();") != std::string::npos);
	CPPUNIT_ASSERT(vehicleSource.find("effect.weaponName = (*wItr)->getLongName();") != std::string::npos);
	CPPUNIT_ASSERT(vehicleSource.find("effect.defenseName = (*dItr)->getLongName();") != std::string::npos);

	CPPUNIT_ASSERT(reportHeader.find("TDET_HullDamage") != std::string::npos);
	CPPUNIT_ASSERT(reportHeader.find("TDET_ADFLoss") != std::string::npos);
	CPPUNIT_ASSERT(reportHeader.find("TDET_MRLoss") != std::string::npos);
	CPPUNIT_ASSERT(reportHeader.find("TDET_WeaponDamaged") != std::string::npos);
	CPPUNIT_ASSERT(reportHeader.find("TDET_DefenseDamaged") != std::string::npos);
	CPPUNIT_ASSERT(reportHeader.find("TDET_PowerSystemDamaged") != std::string::npos);
	CPPUNIT_ASSERT(reportHeader.find("TDET_CombatControlDamaged") != std::string::npos);
	CPPUNIT_ASSERT(reportHeader.find("TDET_NavigationError") != std::string::npos);
	CPPUNIT_ASSERT(reportHeader.find("TDET_ElectricalFire") != std::string::npos);
	CPPUNIT_ASSERT(reportHeader.find("TDET_DCRLoss") != std::string::npos);
}

}
