/**
 * @file FTacticalCombatReportTest.h
 * @brief Header file for the FTacticalCombatReportTest class
 */

#ifndef FTacticalCombatReportTest_H_
#define FTacticalCombatReportTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "tactical/FBattleScreen.h"
#include "tactical/FTacticalCombatReport.h"

namespace FrontierTests {
using namespace Frontier;

class FTacticalCombatReportTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE( FTacticalCombatReportTest );
	CPPUNIT_TEST( testReportModelPreservesRawAttackAndImmediateEventData );
	CPPUNIT_TEST( testReportModelDefinesSeparateShipSummaryRollupTypes );
	CPPUNIT_TEST( testReportModelSupportsAllPlannedReportingModes );
	CPPUNIT_TEST( testBattleScreenExposesTacticalReportLifecycleApi );
	CPPUNIT_TEST( testBattleScreenBuildsShipSummaryFromRawReportData );
	CPPUNIT_TEST( testBattleScreenOnlyTouchesPhaseFlowThroughFleetSetupReset );
	CPPUNIT_TEST( testVehicleDamageReportingApiCapturesExplicitEffectTypes );
	CPPUNIT_TEST_SUITE_END();

private:
	std::string readFile(const std::string & path);
	std::string extractFunctionBody(const std::string & source, const std::string & signature);

public:
	void setUp();
	void tearDown();

	void testReportModelPreservesRawAttackAndImmediateEventData();
	void testReportModelDefinesSeparateShipSummaryRollupTypes();
	void testReportModelSupportsAllPlannedReportingModes();
	void testBattleScreenExposesTacticalReportLifecycleApi();
	void testBattleScreenBuildsShipSummaryFromRawReportData();
	void testBattleScreenOnlyTouchesPhaseFlowThroughFleetSetupReset();
	void testVehicleDamageReportingApiCapturesExplicitEffectTypes();
};

}

#endif
