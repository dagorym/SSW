/**
 * @file FTacticalDamageSummaryGUITest.cpp
 * @brief Implementation file for the FTacticalDamageSummaryGUITest class
 */

#include "FTacticalDamageSummaryGUITest.h"

#include <algorithm>

#include "tactical/FTacticalCombatReport.h"

namespace FrontierTests {
using namespace Frontier;

namespace {

bool containsLine(const std::vector<std::string> & lines, const std::string & expectedLine) {
	return std::find(lines.begin(), lines.end(), expectedLine) != lines.end();
}

}

CPPUNIT_TEST_SUITE_REGISTRATION( FTacticalDamageSummaryGUITest );

void FTacticalDamageSummaryGUITest::setUp() {
}

void FTacticalDamageSummaryGUITest::tearDown() {
}

void FTacticalDamageSummaryGUITest::testReportTypeLabelsAndDialogTitleMapToReportContext() {
	// AC: the dialog title and context text clearly identify immediate and end-of-phase report types.
	CPPUNIT_ASSERT_EQUAL(std::string("Defensive Fire Results"), tacticalCombatReportTypeLabel(TRT_DefensiveFire));
	CPPUNIT_ASSERT_EQUAL(std::string("Offensive Fire Results"), tacticalCombatReportTypeLabel(TRT_OffensiveFire));
	CPPUNIT_ASSERT_EQUAL(std::string("Electrical Fire Damage"), tacticalCombatReportTypeLabel(TRT_ElectricalFire));
	CPPUNIT_ASSERT_EQUAL(std::string("Mine Damage"), tacticalCombatReportTypeLabel(TRT_MineDamage));

	FTacticalCombatReportContext defaultContext;
	CPPUNIT_ASSERT_EQUAL(std::string("Tactical Damage Summary"), tacticalCombatReportDialogTitle(defaultContext));
	CPPUNIT_ASSERT_EQUAL(std::string("End-of-phase report"), tacticalCombatReportContextModeLabel(defaultContext));

	FTacticalCombatReportContext immediateContext;
	immediateContext.reportType = TRT_OffensiveFire;
	immediateContext.immediate = true;
	CPPUNIT_ASSERT_EQUAL(std::string("Immediate report"), tacticalCombatReportContextModeLabel(immediateContext));
	CPPUNIT_ASSERT_EQUAL(
		std::string("Tactical Damage Summary - Offensive Fire Results"),
		tacticalCombatReportDialogTitle(immediateContext));

	FTacticalCombatReportContext phaseContext;
	phaseContext.reportType = TRT_DefensiveFire;
	phaseContext.immediate = false;
	phaseContext.title = "Round 3";
	CPPUNIT_ASSERT_EQUAL(
		std::string("Tactical Damage Summary - Round 3"),
		tacticalCombatReportDialogTitle(phaseContext));
}

void FTacticalDamageSummaryGUITest::testDamageSummaryDialogBuildsShipRollupAndOptionalHitDetailSections() {
	// AC: the modal summary dialog renders ship rollups and optional hit details while preserving empty-state behavior.
	FTacticalCombatReport report;
	FTacticalShipReference target(2002, 2, "Sathar Frigate");
	FTacticalShipReference attacker(1001, 1, "UPF Destroyer");
	FTacticalWeaponReference weapon(3003, "Laser Battery");

	FTacticalAttackReport attack;
	attack.attacker = attacker;
	attack.target = target;
	attack.weapon = weapon;
	attack.hit = true;
	attack.hullDamage = 3;
	attack.note = "armor bypassed";

	FTacticalReportEvent firstWeaponHit;
	firstWeaponHit.eventType = TRET_InternalDamage;
	firstWeaponHit.damageEffectType = TDET_WeaponDamaged;
	firstWeaponHit.subject = target;
	firstWeaponHit.label = "Weapon Hit";
	firstWeaponHit.damagedWeaponType = FWeapon::LB;

	FTacticalReportEvent secondWeaponHit = firstWeaponHit;

	FTacticalReportEvent thirdWeaponHit = firstWeaponHit;
	thirdWeaponHit.damagedWeaponType = FWeapon::AR;

	FTacticalReportEvent attackScopedWeaponHit = firstWeaponHit;
	attackScopedWeaponHit.detail = "weapon feed disrupted";

	FTacticalReportEvent defenseEffect;
	defenseEffect.eventType = TRET_DefenseEffect;
	defenseEffect.subject = target;
	defenseEffect.label = "Defense damaged";
	defenseEffect.damagedDefenseType = FDefense::MS;
	defenseEffect.damagedDefenseName = "Masking Screen";

	FTacticalReportEvent protonScreenEffect = defenseEffect;
	protonScreenEffect.damagedDefenseType = FDefense::PS;
	protonScreenEffect.damagedDefenseName = "Proton Screen";

	FTacticalReportEvent attackScopedDefenseEffect = defenseEffect;
	attackScopedDefenseEffect.detail = "screen emitter overloaded";

	FTacticalReportEvent adfLoss;
	adfLoss.eventType = TRET_InternalDamage;
	adfLoss.damageEffectType = TDET_ADFLoss;
	adfLoss.subject = target;
	adfLoss.label = "ADF reduced";
	adfLoss.amount = 2;

	FTacticalReportEvent mrLoss;
	mrLoss.eventType = TRET_InternalDamage;
	mrLoss.damageEffectType = TDET_MRLoss;
	mrLoss.subject = target;
	mrLoss.label = "MR reduced";
	mrLoss.amount = 1;

	attack.internalEvents.push_back(attackScopedWeaponHit);
	attack.internalEvents.push_back(attackScopedDefenseEffect);
	report.attacks.push_back(attack);

	report.events.push_back(firstWeaponHit);
	report.events.push_back(secondWeaponHit);
	report.events.push_back(thirdWeaponHit);
	report.events.push_back(defenseEffect);
	report.events.push_back(protonScreenEffect);
	report.events.push_back(adfLoss);
	report.events.push_back(mrLoss);

	const FTacticalCombatReportSummary summary = buildTacticalCombatReportSummary(report);
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), summary.ships.size());
	CPPUNIT_ASSERT(summary.showHitDetails);
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), summary.hitDetails.size());
	CPPUNIT_ASSERT(containsLine(summary.ships[0].displayLines, "Sathar Frigate:"));
	CPPUNIT_ASSERT(containsLine(summary.ships[0].displayLines, " - 3 hull damage from 1 attack"));
	CPPUNIT_ASSERT(containsLine(summary.ships[0].displayLines, " - ADF (-2)"));
	CPPUNIT_ASSERT(containsLine(summary.ships[0].displayLines, " - MR (-1)"));
	CPPUNIT_ASSERT(summary.ships[0].displayLines[4].find(" - Weapon Hit: ") != std::string::npos);
	CPPUNIT_ASSERT(summary.ships[0].displayLines[4].find("AR") != std::string::npos);
	bool foundDefenseHitRollup = false;
	for (unsigned int i = 0; i < summary.ships[0].displayLines.size(); ++i) {
		const std::string & line = summary.ships[0].displayLines[i];
		if (line.find(" - Defense Hit: ") != std::string::npos
			&& line.find("MS") != std::string::npos
			&& line.find("PS") != std::string::npos) {
			foundDefenseHitRollup = true;
			break;
		}
	}
	CPPUNIT_ASSERT(foundDefenseHitRollup);
	CPPUNIT_ASSERT(!containsLine(summary.ships[0].displayLines, "Defense damaged"));
	CPPUNIT_ASSERT(summary.hitDetails[0].displayLine.find("3 hull damage") != std::string::npos);
	CPPUNIT_ASSERT(summary.hitDetails[0].displayLine.find("Weapon Hit (weapon feed disrupted)") != std::string::npos);
	CPPUNIT_ASSERT(summary.hitDetails[0].displayLine.find("Defense damaged (screen emitter overloaded)") != std::string::npos);
	CPPUNIT_ASSERT(summary.hitDetails[0].displayLine.find("armor bypassed") != std::string::npos);
	CPPUNIT_ASSERT(summary.hitDetails[0].displayLine.find("Attack hit target") == std::string::npos);
	CPPUNIT_ASSERT(summary.ships[0].displayLines.size() >= 6);
	CPPUNIT_ASSERT_EQUAL(std::string("Sathar Frigate:"), summary.ships[0].displayLines[0]);
	CPPUNIT_ASSERT_EQUAL(std::string(" - 3 hull damage from 1 attack"), summary.ships[0].displayLines[1]);
	CPPUNIT_ASSERT_EQUAL(std::string(" - ADF (-2)"), summary.ships[0].displayLines[2]);
	CPPUNIT_ASSERT_EQUAL(std::string(" - MR (-1)"), summary.ships[0].displayLines[3]);
	CPPUNIT_ASSERT(summary.ships[0].displayLines == summary.displayLines);
}

}
