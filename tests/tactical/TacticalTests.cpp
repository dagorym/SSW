/**
 * @file TacticalTests.cpp
 * @brief Main program for tactical unit tests
 */

#include <cppunit/ui/text/TestRunner.h>

#include "FTacticalAttackResultTest.h"
#include "FTacticalBattleScreenDelegationTest.h"
#include "FTacticalBattleScreenReportContextTest.h"
#include "FTacticalBattleScreenElectricalFireTest.h"
#include "FTacticalMineDamageReportSummaryTest.h"
#include "FTacticalMineDamageFlowTest.h"
#include "FTacticalBattleDisplayFireFlowTest.h"
#include "FTacticalDestroyedShipCleanupLifecycleTest.h"
#include "FTacticalBatteryRangeClampTest.h"
#include "FTacticalBattleBoardRendererDelegationTest.h"
#include "FTacticalStationOrbitalMovementTest.h"
#include "FTacticalMoveRouteSelectionTest.h"
#include "FTacticalForwardFireFinalOrientationTest.h"
#include "FTacticalGameHeaderTest.h"
#include "FTacticalGameMechanicsTest.h"
#include "FTacticalGameMockUITest.h"
#include "FTacticalModelSelectionHexClickSurfaceTest.h"
#include "ITacticalUIBoundaryTest.h"

using namespace FrontierTests;

int main(int argc, char **argv) {
CppUnit::TextUi::TestRunner runner;
runner.addTest(FTacticalAttackResultTest::suite());
runner.addTest(FTacticalBattleScreenDelegationTest::suite());
runner.addTest(FTacticalBattleScreenReportContextTest::suite());
runner.addTest(FTacticalBattleScreenElectricalFireTest::suite());
runner.addTest(FTacticalMineDamageReportSummaryTest::suite());
runner.addTest(FTacticalMineDamageFlowTest::suite());
runner.addTest(FTacticalBattleDisplayFireFlowTest::suite());
runner.addTest(FTacticalDestroyedShipCleanupLifecycleTest::suite());
runner.addTest(FTacticalBatteryRangeClampTest::suite());
runner.addTest(FTacticalBattleBoardRendererDelegationTest::suite());
runner.addTest(FTacticalStationOrbitalMovementTest::suite());
runner.addTest(FTacticalMoveRouteSelectionTest::suite());
runner.addTest(FTacticalForwardFireFinalOrientationTest::suite());
runner.addTest(FTacticalGameHeaderTest::suite());
runner.addTest(FTacticalGameMechanicsTest::suite());
runner.addTest(FTacticalGameMockUITest::suite());
runner.addTest(FTacticalModelSelectionHexClickSurfaceTest::suite());
runner.addTest(ITacticalUIBoundaryTest::suite());
bool wasSuccessful = runner.run("", false);
return wasSuccessful ? 0 : 1;
}
