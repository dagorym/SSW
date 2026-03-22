/**
 * @file FTacticalMineDamageReportSummaryTest.cpp
 * @brief Implementation file for the FTacticalMineDamageReportSummaryTest class
 */

#include "FTacticalMineDamageReportSummaryTest.h"

#include "tactical/FTacticalCombatReport.h"

namespace FrontierTests {
using namespace Frontier;

namespace {

const FTacticalShipReportSummary * findShipSummary(
	const FTacticalCombatReportSummary & summary,
	const std::string & shipName) {
	for (unsigned int i = 0; i < summary.ships.size(); ++i) {
		if (summary.ships[i].ship.shipName == shipName) {
			return &summary.ships[i];
		}
	}
	return NULL;
}

}

CPPUNIT_TEST_SUITE_REGISTRATION( FTacticalMineDamageReportSummaryTest );

void FTacticalMineDamageReportSummaryTest::setUp() {
}

void FTacticalMineDamageReportSummaryTest::tearDown() {
}

void FTacticalMineDamageReportSummaryTest::testMineDamageReportSummaryPreservesImmediateEventAndVisibleShipRollup() {
	// AC: mine-damage reports preserve immediate context and render a visible ship summary for the triggered target.
	FTacticalCombatReport report;
	report.context.reportType = TRT_MineDamage;
	report.context.phase = 18;
	report.context.actingPlayerID = 4;
	report.context.immediate = true;

	FTacticalReportEvent mineDamage;
	mineDamage.eventType = TRET_MineDamage;
	mineDamage.subject = FTacticalShipReference(22, 3, "Scout");
	mineDamage.rollValue = 6;
	mineDamage.hullDamage = 0;
	mineDamage.immediate = true;
	mineDamage.label = "Mine damage";
	mineDamage.detail = "Triggered by mined hex";

	report.events.push_back(mineDamage);

	const FTacticalCombatReportSummary summary = buildTacticalCombatReportSummary(report);
	const FTacticalShipReportSummary * shipSummary = findShipSummary(summary, "Scout");

	CPPUNIT_ASSERT(summary.context.reportType == TRT_MineDamage);
	CPPUNIT_ASSERT(summary.context.immediate);
	CPPUNIT_ASSERT(summary.context.phase == 18);
	CPPUNIT_ASSERT(summary.context.actingPlayerID == 4);
	CPPUNIT_ASSERT_EQUAL(std::string("Immediate report"), tacticalCombatReportContextModeLabel(summary.context));
	CPPUNIT_ASSERT_EQUAL(std::string("Tactical Damage Summary - Mine Damage"), tacticalCombatReportDialogTitle(summary.context));
	CPPUNIT_ASSERT_EQUAL(std::string("Mine Damage"), tacticalCombatReportTypeLabel(TRT_MineDamage));
	CPPUNIT_ASSERT(summary.ships.size() == 1);
	CPPUNIT_ASSERT(summary.displayLines.size() == 1);
	CPPUNIT_ASSERT(shipSummary != NULL);
	CPPUNIT_ASSERT_EQUAL(std::string("Scout"), shipSummary->ship.shipName);
	CPPUNIT_ASSERT(shipSummary->rawEvents.size() == 1);
	CPPUNIT_ASSERT(shipSummary->rawEvents[0].eventType == TRET_MineDamage);
	CPPUNIT_ASSERT_EQUAL(1, shipSummary->nonHullEffectsTaken);
	CPPUNIT_ASSERT_EQUAL(0, shipSummary->hullDamageTaken);
	CPPUNIT_ASSERT(shipSummary->hasSustainedDamage());
	CPPUNIT_ASSERT(shipSummary->displayLines[0].find("Mine damage") != std::string::npos);
	CPPUNIT_ASSERT(summary.displayLines[0].find("Mine damage") != std::string::npos);
}

}
