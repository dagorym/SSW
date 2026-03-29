/**
 * @file TacticalTests.cpp
 * @brief Main program for tactical unit tests
 */

#include <cppunit/ui/text/TestRunner.h>

#include "FTacticalCombatReportTest.h"
#include "FTacticalAttackResultTest.h"
#include "FTacticalDamageSummaryGUITest.h"
#include "FTacticalBattleScreenReportContextTest.h"
#include "FTacticalBattleScreenElectricalFireTest.h"
#include "FTacticalMineDamageReportSummaryTest.h"
#include "FTacticalMineDamageFlowTest.h"
#include "FTacticalBattleDisplayFireFlowTest.h"
#include "FTacticalBatteryRangeClampTest.h"
#include "FTacticalGameHeaderTest.h"

using namespace FrontierTests;

int main(int argc, char **argv) {
	CppUnit::TextUi::TestRunner runner;
	runner.addTest(FTacticalCombatReportTest::suite());
	runner.addTest(FTacticalAttackResultTest::suite());
	runner.addTest(FTacticalDamageSummaryGUITest::suite());
	runner.addTest(FTacticalBattleScreenReportContextTest::suite());
	runner.addTest(FTacticalBattleScreenElectricalFireTest::suite());
	runner.addTest(FTacticalMineDamageReportSummaryTest::suite());
	runner.addTest(FTacticalMineDamageFlowTest::suite());
	runner.addTest(FTacticalBattleDisplayFireFlowTest::suite());
	runner.addTest(FTacticalBatteryRangeClampTest::suite());
	runner.addTest(FTacticalGameHeaderTest::suite());
	bool wasSuccessful = runner.run("", false);
	return wasSuccessful ? 0 : 1;
}
