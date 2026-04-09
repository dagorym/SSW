/**
 * @file FTacticalDamageSummaryGUITest.cpp
 * @brief Implementation file for the FTacticalDamageSummaryGUITest class
 */

#include "FTacticalDamageSummaryGUITest.h"

#include <fstream>
#include <iterator>

#include "tactical/FTacticalCombatReport.h"

namespace FrontierTests {
using namespace Frontier;

namespace {

std::string repoFile(const std::string & relativePath) {
	return std::string(TACTICAL_TEST_REPO_ROOT) + "/" + relativePath;
}

void assertContains(const std::string & haystack, const std::string & needle) {
	CPPUNIT_ASSERT_MESSAGE(
		std::string("Expected to find '") + needle + "' in inspected source",
		haystack.find(needle) != std::string::npos);
}

void assertNotContains(const std::string & haystack, const std::string & needle) {
	CPPUNIT_ASSERT_MESSAGE(
		std::string("Expected to not find '") + needle + "' in inspected source",
		haystack.find(needle) == std::string::npos);
}

}

CPPUNIT_TEST_SUITE_REGISTRATION( FTacticalDamageSummaryGUITest );

void FTacticalDamageSummaryGUITest::setUp() {
}

void FTacticalDamageSummaryGUITest::tearDown() {
}

std::string FTacticalDamageSummaryGUITest::readFile(const std::string & path) {
	std::ifstream file(path.c_str());
	CPPUNIT_ASSERT_MESSAGE(path, file.is_open());
	return std::string((std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());
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
	report.attacks.push_back(attack);

	FTacticalReportEvent firstWeaponHit;
	firstWeaponHit.eventType = TRET_InternalDamage;
	firstWeaponHit.damageEffectType = TDET_WeaponDamaged;
	firstWeaponHit.subject = target;
	firstWeaponHit.label = "Weapon Hit";
	firstWeaponHit.damagedWeaponType = FWeapon::LB;

	FTacticalReportEvent secondWeaponHit = firstWeaponHit;

	FTacticalReportEvent thirdWeaponHit = firstWeaponHit;
	thirdWeaponHit.damagedWeaponType = FWeapon::AR;

	FTacticalReportEvent defenseEffect;
	defenseEffect.eventType = TRET_DefenseEffect;
	defenseEffect.subject = target;
	defenseEffect.label = "Defense damaged";
	defenseEffect.damagedDefenseType = FDefense::MS;
	defenseEffect.damagedDefenseName = "Masking Screen";

	report.events.push_back(firstWeaponHit);
	report.events.push_back(secondWeaponHit);
	report.events.push_back(thirdWeaponHit);
	report.events.push_back(defenseEffect);

	const FTacticalCombatReportSummary summary = buildTacticalCombatReportSummary(report);
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), summary.ships.size());
	CPPUNIT_ASSERT(summary.showHitDetails);
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), summary.hitDetails.size());
	CPPUNIT_ASSERT(summary.ships[0].displayLines[0].find("Weapon Hit: LB, LB, AR") != std::string::npos);
	CPPUNIT_ASSERT(summary.ships[0].displayLines[0].find("Defense Hit: Masking Screen") != std::string::npos);
	CPPUNIT_ASSERT(summary.ships[0].displayLines[0].find("Defense damaged") == std::string::npos);

	const std::string source = readFile(repoFile("src/gui/TacticalDamageSummaryGUI.cpp"));

	assertContains(source, "toWxString(tacticalCombatReportDialogTitle(summary.context))");
	assertContains(source, "buildShipRollupText()");
	assertContains(source, "buildHitDetailText()");
	assertContains(source, "Hit Details");
	assertContains(source, "Ship Damage Summary");
	assertContains(source, "m_summary.ships.empty()");
	assertContains(source, "No ships sustained damage in this report.");
	assertContains(source, "m_summary.showHitDetails");
	assertContains(source, "m_summary.hitDetails.empty()");
	assertContains(source, "m_summary.hitDetails[i].displayLine");
	assertContains(source, "shipSummary.displayLines.empty()");
	assertContains(source, "shipSummary.displayLines[j]");
	assertContains(source, "shipSummary.ship.shipName");
	assertContains(source, "m_summary.ships[i]");
	assertContains(source, "SetAffirmativeId(wxID_OK);");
	assertContains(source, "SetDefaultItem(m_closeButton);");
	assertNotContains(source, "m_closeButton->Bind(wxEVT_BUTTON, &TacticalDamageSummaryGUI::onClose, this);");
	assertNotContains(source, "void TacticalDamageSummaryGUI::onClose(");
	assertNotContains(source, "EndModal(wxID_OK);");
}

void FTacticalDamageSummaryGUITest::testBattleScreenEntryPointAndGuiBuildWiringArePresent() {
	// AC: FBattleScreen exposes a modal entry point and the GUI build links the dialog implementation.
	const std::string header = readFile(repoFile("include/tactical/FBattleScreen.h"));
	const std::string source = readFile(repoFile("src/tactical/FBattleScreen.cpp"));
	const std::string guiMakefile = readFile(repoFile("src/gui/Makefile"));

	assertContains(header, "int showTacticalDamageSummaryDialog(const FTacticalCombatReportSummary & summary);");
	assertContains(source, "TacticalDamageSummaryGUI dialog(this, summary);");
	assertContains(source, "return dialog.ShowModal();");
	assertContains(guiMakefile, "TacticalDamageSummaryGUI.o \\");
	assertContains(guiMakefile, "TacticalDamageSummaryGUI.o: ../../include/gui/TacticalDamageSummaryGUI.h");
	assertContains(guiMakefile, "TacticalDamageSummaryGUI.o: ../../include/tactical/FTacticalCombatReport.h");
}

}
