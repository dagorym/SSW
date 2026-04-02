/**
 * @file FTacticalBattleScreenDelegationTest.h
 * @brief Header file for the FTacticalBattleScreenDelegationTest class
 */

#ifndef FTacticalBattleScreenDelegationTest_H_
#define FTacticalBattleScreenDelegationTest_H_

#include <cppunit/extensions/HelperMacros.h>
#include <string>

namespace FrontierTests {

class FTacticalBattleScreenDelegationTest : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( FTacticalBattleScreenDelegationTest );
CPPUNIT_TEST( testBattleScreenOwnsAndInstallsTacticalModelAndUI );
CPPUNIT_TEST( testBattleScreenDetachesTacticalUIBeforeTeardown );
CPPUNIT_TEST( testBattleScreenStateAndSelectionApiDelegatesToTacticalModel );
CPPUNIT_TEST( testBattleScreenForwardsInteractionAndRendererAccessorsToModel );
CPPUNIT_TEST( testBattleScreenForwardsSelectionAndHexInteractionMutatorsToModel );
CPPUNIT_TEST( testBattleScreenDamageSummaryDialogDelegatesThroughInstalledUI );
CPPUNIT_TEST( testBattleScreenPhaseWeaponAndCleanupFlowDelegatesToTacticalModel );
CPPUNIT_TEST_SUITE_END();

private:
std::string readFile(const std::string & path);
std::string extractFunctionBody(const std::string & source, const std::string & signature);
void assertContains(const std::string & haystack, const std::string & needle);

public:
void setUp();
void tearDown();

void testBattleScreenOwnsAndInstallsTacticalModelAndUI();
void testBattleScreenDetachesTacticalUIBeforeTeardown();
void testBattleScreenStateAndSelectionApiDelegatesToTacticalModel();
void testBattleScreenForwardsInteractionAndRendererAccessorsToModel();
void testBattleScreenForwardsSelectionAndHexInteractionMutatorsToModel();
void testBattleScreenDamageSummaryDialogDelegatesThroughInstalledUI();
void testBattleScreenPhaseWeaponAndCleanupFlowDelegatesToTacticalModel();
};

}

#endif
