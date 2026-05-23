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

/**
 * @brief Provides live BattleSim GUI launch and modal-ownership regression coverage.
 *
 * The fixture validates BattleSim launcher dialogs, startup splash/frame creation, and
 * scenario-to-battle-screen launch paths while keeping top-level window lifecycle cleanup
 * deterministic under the shared GUI harness. Modal tactical close-path coverage now requires
 * the caller to unwind and the launched `FBattleScreen` instance to stop showing and fully
 * settle its lifecycle counters.
 *
 * @author gpt-5.3-codex (medium), gpt-5.4 (high)
 * @date Created: Apr 04, 2026
 * @date Last Modified: May 23, 2026
 */
class BattleSimGuiLiveTest : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( BattleSimGuiLiveTest );
CPPUNIT_TEST( testBattleSimFrameOpensLocalGameDialogAndReturns );
CPPUNIT_TEST( testLocalGameDialogLaunchesPredefinedAndCustomModalChains );
CPPUNIT_TEST( testBattleSimLaunchDialogsRetainFirstShowSizingContracts );
CPPUNIT_TEST( testStartupLaunchCreatesCenteredSplashAndBattleSimFrame );
CPPUNIT_TEST( testScenarioDialogMenuQuitUnwindsBattleScreenModalCaller );
CPPUNIT_TEST( testScenarioDialogTitleBarCloseUnwindsBattleScreenModalCaller );
CPPUNIT_TEST( testScenarioDialogScenarioPathLaunchesBattleScreenWithLifecycleCoverage );
CPPUNIT_TEST( testScenarioEditorStartBattleLaunchesBattleScreenWithLifecycleCoverage );
CPPUNIT_TEST_SUITE_END();

private:
	WXGuiTestHarness m_harness;
	wxString m_originalCwd;

public:
	/**
	 * @brief Bootstraps wx harness state and captures the original working directory.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	void setUp();
	/**
	 * @brief Restores the original working directory and cleans residual top-level windows.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	void tearDown();

	/**
	 * @brief Verifies BattleSimFrame launches LocalGameDialog and returns cleanly.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	void testBattleSimFrameOpensLocalGameDialogAndReturns();
	/**
	 * @brief Verifies LocalGameDialog launches predefined and custom scenario modal chains.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	void testLocalGameDialogLaunchesPredefinedAndCustomModalChains();
	/**
	 * @brief Confirms BattleSim launch dialogs retain first-show sizing expectations.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 14, 2026
	 * @date Last Modified: Apr 14, 2026
	 */
	void testBattleSimLaunchDialogsRetainFirstShowSizingContracts();
	/**
	 * @brief Verifies startup launch creates centered splash and BattleSim frame surfaces.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 16, 2026
	 * @date Last Modified: Apr 16, 2026
	 */
	void testStartupLaunchCreatesCenteredSplashAndBattleSimFrame();
	/**
	 * @brief Verifies scenario-launched battle screen unwinds modal caller via File->Quit.
	 *
	 * The modal caller must return with no shown or live `FBattleScreen` left behind.
	 *
	 * @author gpt-5.4 (high)
	 * @date Created: May 22, 2026
	 * @date Last Modified: May 23, 2026
	 */
	void testScenarioDialogMenuQuitUnwindsBattleScreenModalCaller();
	/**
	 * @brief Verifies scenario-launched battle screen unwinds modal caller via title-bar close.
	 *
	 * The modal caller must return with no shown or live `FBattleScreen` left behind after a
	 * real posted close event.
	 *
	 * @author gpt-5.4 (high)
	 * @date Created: May 23, 2026
	 * @date Last Modified: May 23, 2026
	 */
	void testScenarioDialogTitleBarCloseUnwindsBattleScreenModalCaller();
	/**
	 * @brief Validates scenario-dialog flow launches battle screen with lifecycle checks.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	void testScenarioDialogScenarioPathLaunchesBattleScreenWithLifecycleCoverage();
	/**
	 * @brief Validates scenario-editor start-battle flow launches battle screen cleanly.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	void testScenarioEditorStartBattleLaunchesBattleScreenWithLifecycleCoverage();
};

}

#endif
