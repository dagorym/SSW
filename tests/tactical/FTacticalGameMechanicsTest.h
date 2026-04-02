/**
 * @file FTacticalGameMechanicsTest.h
 * @brief Header file for FTacticalGame mechanics implementation tests
 */

#ifndef _FTACTICALGAMEMECHANICSTEST_H_
#define _FTACTICALGAMEMECHANICSTEST_H_

#include <cppunit/extensions/HelperMacros.h>
#include <string>

namespace FrontierTests {

class FTacticalGameMechanicsTest : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( FTacticalGameMechanicsTest );
CPPUNIT_TEST( testFTacticalGameImplementationCompilesStandalone );
CPPUNIT_TEST( testHeaderExposesAdditiveMechanicsApiSurface );
CPPUNIT_TEST( testResetInitializesSafeLegacyCompatibleDefaults );
CPPUNIT_TEST( testTacticalReportLifecycleUsesSharedReportTypes );
CPPUNIT_TEST( testFireAllWeaponsOwnsCombatReportLifecycleAndCleanup );
CPPUNIT_TEST( testDestroyedShipBookkeepingLifecycleContractIsExplicit );
CPPUNIT_TEST( testMovementHelpersResetAndFinalizeTurnData );
CPPUNIT_TEST( testWinnerAndCombatEndHelpersResolveBattleState );
CPPUNIT_TEST( testDestroyedShipCleanupPurgesHexAndTurnBookkeeping );
CPPUNIT_TEST( testFireICMCollectsOnlyActionableHexInterceptions );
CPPUNIT_TEST( testInteractionApisAndRendererAccessorsAreExposed );
CPPUNIT_TEST( testHexClickDispatchAndTargetSelectionRulesFlowThroughModelState );
CPPUNIT_TEST( testMinePlacementAndMoveFireProgressionUpdateModelState );
CPPUNIT_TEST( testImplementationRemainsSelfContainedWithoutLegacyWxRewire );
CPPUNIT_TEST_SUITE_END();

private:
std::string readFile(const std::string & path);
std::string extractFunctionBody(const std::string & source, const std::string & signature);
void assertContains(const std::string & haystack, const std::string & needle);

public:
void setUp();
void tearDown();

void testFTacticalGameImplementationCompilesStandalone();
void testHeaderExposesAdditiveMechanicsApiSurface();
void testResetInitializesSafeLegacyCompatibleDefaults();
void testTacticalReportLifecycleUsesSharedReportTypes();
void testFireAllWeaponsOwnsCombatReportLifecycleAndCleanup();
void testDestroyedShipBookkeepingLifecycleContractIsExplicit();
void testMovementHelpersResetAndFinalizeTurnData();
void testWinnerAndCombatEndHelpersResolveBattleState();
void testDestroyedShipCleanupPurgesHexAndTurnBookkeeping();
void testFireICMCollectsOnlyActionableHexInterceptions();
void testInteractionApisAndRendererAccessorsAreExposed();
void testHexClickDispatchAndTargetSelectionRulesFlowThroughModelState();
void testMinePlacementAndMoveFireProgressionUpdateModelState();
void testImplementationRemainsSelfContainedWithoutLegacyWxRewire();
};

}

#endif
