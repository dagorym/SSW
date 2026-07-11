/**
 * @file StrategicGuiLiveTest.h
 * @brief Header file for live strategic GUI smoke coverage
 */

#ifndef _STRATEGICGUILIVETEST_H_
#define _STRATEGICGUILIVETEST_H_

#include <cppunit/extensions/HelperMacros.h>

#include "WXGuiTestHarness.h"

namespace FrontierTests {

/**
 * @brief Provides live strategic GUI smoke and regression coverage for modal flows.
 *
 * The fixture drives strategic launch surfaces, dialog ownership, rendering, and adapter
 * runtime guards to keep wx-backed strategic behavior deterministic under test harness
 * control.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 04, 2026
 * @date Last Modified: Apr 17, 2026
 */
class StrategicGuiLiveTest : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( StrategicGuiLiveTest );
CPPUNIT_TEST( testMainFrameBuildsExpectedInitialUI );
CPPUNIT_TEST( testMainFrameOnCloseYesInvokesSaveBeforeReset );
CPPUNIT_TEST( testMainFrameOnCloseNoResetsWithoutSaving );
CPPUNIT_TEST( testMainFrameOnCloseCancelAbortsWithoutSaveOrReset );
CPPUNIT_TEST( testGamePanelPaintTracksParentSize );
CPPUNIT_TEST( testStrategicDialogsCloseModallyWithoutInput );
CPPUNIT_TEST( testStrategicDialogsUseStaticBoxChildParents );
CPPUNIT_TEST( testWXStrategicUIParentBackedModalAndRedrawPaths );
CPPUNIT_TEST( testWXStrategicUIParentlessRetreatModalPathWithRuntime );
CPPUNIT_TEST( testWXStrategicUISourceGuardsRuntimeAndPreservesParentlessFlow );
CPPUNIT_TEST( testUPFAndSatharFleetDialogsMutateModelState );
CPPUNIT_TEST( testTransferShipsDialogMovesShipsAndUpdatesFleets );
CPPUNIT_TEST( testCombatSelectionDialogsReturnCodesAndState );
CPPUNIT_TEST( testRemediatedStrategicDialogsUseFirstShowSizingContract );
CPPUNIT_TEST( testStartupLaunchCreatesCenteredSplashAndMainFrame );
CPPUNIT_TEST( testSelectCombatLaunchesBattleScreenAndCleansUpLifetime );
CPPUNIT_TEST( testSelectCombatAttackTranslatesTwoPlanetsSelectionToStationIndex );
CPPUNIT_TEST( testBattleResultsDialogUpdatesShipStatistics );
CPPUNIT_TEST( testWXMapDisplayOffscreenRendering );
CPPUNIT_TEST( testWXPlayerDisplayOffscreenRendersSystemAndTransitFleets );
CPPUNIT_TEST( testWXGameDisplayOffscreenRendersTurnCounterAndIcons );
CPPUNIT_TEST_SUITE_END();

private:
	WXGuiTestHarness m_harness;

public:
	/**
	 * @brief Bootstraps the shared GUI harness before each strategic live test.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	void setUp();
	/**
	 * @brief Cleans shown top-level windows and shuts down harness wx state.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	void tearDown();

	/**
	 * @brief Verifies the strategic main frame constructs its expected startup controls.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	void testMainFrameBuildsExpectedInitialUI();
	/**
	 * @brief Verifies FMainFrame::onClose() invokes onSave() before resetGame() when the
	 * Yes/No/Cancel save-confirmation dialog returns wxID_YES (CRIT-5 regression coverage).
	 *
	 * @author Sonnet 5 (medium)
	 * @date Created: Jul 11, 2026
	 * @date Last Modified: Jul 11, 2026
	 */
	void testMainFrameOnCloseYesInvokesSaveBeforeReset();
	/**
	 * @brief Verifies FMainFrame::onClose() resets without saving when the save-confirmation
	 * dialog returns wxID_NO (CRIT-5 regression coverage).
	 *
	 * @author Sonnet 5 (medium)
	 * @date Created: Jul 11, 2026
	 * @date Last Modified: Jul 11, 2026
	 */
	void testMainFrameOnCloseNoResetsWithoutSaving();
	/**
	 * @brief Verifies FMainFrame::onClose() aborts the close entirely (no save, no reset) when
	 * the save-confirmation dialog returns wxID_CANCEL (CRIT-5 regression coverage).
	 *
	 * @author Sonnet 5 (medium)
	 * @date Created: Jul 11, 2026
	 * @date Last Modified: Jul 11, 2026
	 */
	void testMainFrameOnCloseCancelAbortsWithoutSaveOrReset();
	/**
	 * @brief Validates offscreen WXMapDisplay rendering for key strategic map elements.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	void testWXMapDisplayOffscreenRendering();
	/**
	 * @brief Verifies WXPlayerDisplay draws in-system and in-transit fleet icon regions.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	void testWXPlayerDisplayOffscreenRendersSystemAndTransitFleets();
	/**
	 * @brief Verifies WXGameDisplay offscreen output includes turn counter and icon draws.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	void testWXGameDisplayOffscreenRendersTurnCounterAndIcons();
	/**
	 * @brief Ensures FGamePanel paint handling tracks parent sizing in a live frame.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	void testGamePanelPaintTracksParentSize();
	/**
	 * @brief Confirms representative strategic dialogs open and close modally without input.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	void testStrategicDialogsCloseModallyWithoutInput();
	/**
	 * @brief Verifies strategic controls under static-box sizers use static-box parenting.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 05, 2026
	 * @date Last Modified: Apr 05, 2026
	 */
	void testStrategicDialogsUseStaticBoxChildParents();
	/**
	 * @brief Exercises parent-backed WXStrategicUI modal and redraw adapter behaviors.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	void testWXStrategicUIParentBackedModalAndRedrawPaths();
	/**
	 * @brief Validates parentless WXStrategicUI retreat flow when a wx runtime exists.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 17, 2026
	 * @date Last Modified: Apr 17, 2026
	 */
	void testWXStrategicUIParentlessRetreatModalPathWithRuntime();
	/**
	 * @brief Verifies runtime guards preserve parentless strategic flow and cancel fallbacks.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 17, 2026
	 * @date Last Modified: Apr 17, 2026
	 */
	void testWXStrategicUISourceGuardsRuntimeAndPreservesParentlessFlow();
	/**
	 * @brief Confirms strategic fleet-selection dialogs mutate UPF and Sathar state.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	void testUPFAndSatharFleetDialogsMutateModelState();
	/**
	 * @brief Verifies transfer dialogs move ships and update fleet ownership results.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	void testTransferShipsDialogMovesShipsAndUpdatesFleets();
	/**
	 * @brief Validates combat selection dialogs return expected modal codes and model state.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	void testCombatSelectionDialogsReturnCodesAndState();
	/**
	 * @brief Confirms remediated strategic dialogs retain first-show sizing contracts.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 14, 2026
	 * @date Last Modified: Apr 14, 2026
	 */
	void testRemediatedStrategicDialogsUseFirstShowSizingContract();
	/**
	 * @brief Verifies startup launch creates centered splash and strategic startup frame.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 16, 2026
	 * @date Last Modified: Apr 16, 2026
	 */
	void testStartupLaunchCreatesCenteredSplashAndMainFrame();
	/**
	 * @brief Verifies SelectCombat launch reaches battle screen and cleans lifecycle state.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	void testSelectCombatLaunchesBattleScreenAndCleansUpLifetime();
	/**
	 * @brief Verifies CRIT-4: TwoPlanetsGUI button selection maps to the correct
	 * 0-based getPlanetList() index (button 1 -> 0, button 2 -> 1, cancel -> 0)
	 * by driving the real onAttack() modal chain and checking which planet's
	 * station FBattleScreen received.
	 *
	 * @author Claude Sonnet 5 (medium)
	 * @date Created: Jul 11, 2026
	 * @date Last Modified: Jul 11, 2026
	 */
	void testSelectCombatAttackTranslatesTwoPlanetsSelectionToStationIndex();
	/**
	 * @brief Confirms battle-results dialog paths update editable ship statistics fields.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	void testBattleResultsDialogUpdatesShipStatistics();
};

}

#endif
