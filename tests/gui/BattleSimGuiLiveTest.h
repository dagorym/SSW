/**
 * @file BattleSimGuiLiveTest.h
 * @brief Header file for live BattleSim GUI launch-path coverage
 */

#ifndef _BATTLESIMGUILIVETEST_H_
#define _BATTLESIMGUILIVETEST_H_

#include <cppunit/extensions/HelperMacros.h>
#include <wx/string.h>

#include "WXGuiTestHarness.h"

namespace FrontierTests {

class BattleSimGuiLiveTest : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( BattleSimGuiLiveTest );
CPPUNIT_TEST( testBattleSimFrameOpensLocalGameDialogAndReturns );
CPPUNIT_TEST( testLocalGameDialogLaunchesPredefinedAndCustomModalChains );
CPPUNIT_TEST( testBattleSimLaunchDialogsRetainFirstShowSizingContracts );
CPPUNIT_TEST( testStartupLaunchCreatesCenteredSplashAndBattleSimFrame );
CPPUNIT_TEST( testScenarioDialogScenarioPathLaunchesBattleScreenWithLifecycleCoverage );
CPPUNIT_TEST( testScenarioEditorStartBattleLaunchesBattleScreenWithLifecycleCoverage );
CPPUNIT_TEST_SUITE_END();

private:
	WXGuiTestHarness m_harness;
	wxString m_originalCwd;

public:
	void setUp();
	void tearDown();

	void testBattleSimFrameOpensLocalGameDialogAndReturns();
	void testLocalGameDialogLaunchesPredefinedAndCustomModalChains();
	void testBattleSimLaunchDialogsRetainFirstShowSizingContracts();
	void testStartupLaunchCreatesCenteredSplashAndBattleSimFrame();
	void testScenarioDialogScenarioPathLaunchesBattleScreenWithLifecycleCoverage();
	void testScenarioEditorStartBattleLaunchesBattleScreenWithLifecycleCoverage();
};

}

#endif
