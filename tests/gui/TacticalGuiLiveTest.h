/**
 * @file TacticalGuiLiveTest.h
 * @brief Header file for live tactical GUI regression coverage
 */

#ifndef _TACTICALGUILIVETEST_H_
#define _TACTICALGUILIVETEST_H_

#include <cppunit/extensions/HelperMacros.h>

#include "WXGuiTestHarness.h"

namespace FrontierTests {

/**
 * @brief Validates tactical live-GUI adapter flows, geometry, and modal behavior.
 *
 * The fixture drives parent-backed tactical dialogs and button-layout assertions to keep
 * tactical runtime interactions deterministic across redraw, prompt-spacing, selection, and
 * battle-screen close-path scenarios. Close-path coverage now requires tactical windows to stop
 * showing and lifecycle counters to settle instead of accepting pending-delete state alone.
 *
 * @author gpt-5.3-codex (medium), gpt-5.4 (high), claude-sonnet-4-6 (high)
 * @date Created: Apr 04, 2026
 * @date Last Modified: May 30, 2026
 */
class TacticalGuiLiveTest : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( TacticalGuiLiveTest );
CPPUNIT_TEST( testWXTacticalUIParentBackedModalAndRedrawPaths );
CPPUNIT_TEST( testBattleScreenMenuBarLabelsAndDisabledItems );
CPPUNIT_TEST( testBattleScreenMenuQuitClosesViaSharedClosePath );
CPPUNIT_TEST( testBattleScreenTitleBarCloseClosesViaSharedClosePath );
CPPUNIT_TEST( testTacticalActionButtonsRemainSizerPositionedWhenShown );
CPPUNIT_TEST( testTacticalActionButtonsStayBelowPromptReservationAcrossPhases );
CPPUNIT_TEST( testSeekerActivationPanelSourceContracts );
CPPUNIT_TEST( testSetupPlacementSourceRowsAndOrdnanceColorContracts );
CPPUNIT_TEST( testBattleScreenDefaultSizeAndLayoutPolicyRuntime );
CPPUNIT_TEST( testBattleDisplayLowerPanelLayoutStatePersistsAcrossPhaseAndGeometryChanges );
CPPUNIT_TEST( testBattleDisplayNarrowWidthStacksShipStatsBelowButtons );
CPPUNIT_TEST( testTacticalDamageSummaryDialogDisplaysContextAndCloseBehavior );
CPPUNIT_TEST( testICMSelectionDialogInteractionFinalizesAssignedCountsAndAmmo );
CPPUNIT_TEST( testMinePlacementDoneButtonLabelReflectsOrdnanceTypes );
CPPUNIT_TEST_SUITE_END();

private:
WXGuiTestHarness m_harness;

public:
/**
 * @brief Bootstraps the shared GUI harness before each tactical live test.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 04, 2026
 * @date Last Modified: Apr 04, 2026
 */
void setUp();
/**
 * @brief Tears down harness state and destroys residual top-level windows.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 04, 2026
 * @date Last Modified: Apr 04, 2026
 */
void tearDown();

/**
 * @brief Verifies parent-backed WXTacticalUI redraw, modal, and winner flows.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 04, 2026
 * @date Last Modified: Apr 04, 2026
 */
void testWXTacticalUIParentBackedModalAndRedrawPaths();
/**
 * @brief Verifies tactical battle-screen menu labels, order, and enabled states.
 *
 * @author gpt-5.4 (high)
 * @date Created: May 22, 2026
 * @date Last Modified: May 22, 2026
 */
void testBattleScreenMenuBarLabelsAndDisabledItems();
/**
 * @brief Verifies tactical Quit menu command closes through the shared close path.
 *
 * The launched `FBattleScreen` must become hidden or absent, and lifecycle counters must
 * settle so the test fails if a shown pending-delete frame is left behind.
 *
 * @author gpt-5.4 (high)
 * @date Created: May 22, 2026
 * @date Last Modified: May 23, 2026
 */
void testBattleScreenMenuQuitClosesViaSharedClosePath();
/**
 * @brief Verifies tactical title-bar close uses shared close lifecycle handling.
 *
 * The launched `FBattleScreen` must become hidden or absent, and lifecycle counters must
 * settle so the test exercises the real close event path instead of a pending-delete shortcut.
 *
 * @author gpt-5.4 (high)
 * @date Created: May 22, 2026
 * @date Last Modified: May 23, 2026
 */
void testBattleScreenTitleBarCloseClosesViaSharedClosePath();
/**
 * @brief Confirms tactical action controls stay sizer-managed when toggled visible.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 08, 2026
 * @date Last Modified: Apr 08, 2026
 */
void testTacticalActionButtonsRemainSizerPositionedWhenShown();
/**
 * @brief Verifies tactical action buttons stay below reserved multi-line prompt regions.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 08, 2026
 * @date Last Modified: Apr 08, 2026
 */
void testTacticalActionButtonsStayBelowPromptReservationAcrossPhases();
/**
 * @brief Verifies seeker activation panel and board source contracts for activation UI and rendering.
 *
 * @author gpt-5.4 (high)
 * @date Created: May 25, 2026
 * @date Last Modified: May 25, 2026
 */
void testSeekerActivationPanelSourceContracts();
/**
 * @brief Verifies setup placement source-row and source-color source-contract tokens.
 *
 * @author gpt-5.4 (high)
 * @date Created: May 24, 2026
 * @date Last Modified: May 24, 2026
 */
void testSetupPlacementSourceRowsAndOrdnanceColorContracts();
/**
 * @brief Verifies FBattleScreen default sizing and parent layout policy behavior at runtime.
 *
 * @author gpt-5.4 (high)
 * @date Created: May 16, 2026
 * @date Last Modified: May 16, 2026
 */
void testBattleScreenDefaultSizeAndLayoutPolicyRuntime();
/**
 * @brief Verifies FBattleDisplay lower-panel layout state persists across phases until geometry forces stacked mode.
 *
 * @author gpt-5.3-codex (medium), GPT-5 (high), gpt-5.4 (high)
 * @date Created: May 16, 2026
 * @date Last Modified: May 16, 2026
 */
void testBattleDisplayLowerPanelLayoutStatePersistsAcrossPhaseAndGeometryChanges();
/**
 * @brief Verifies narrow tactical widths force stacked ship stats below action buttons and increase panel height.
 *
 * @author gpt-5.4 (high)
 * @date Created: May 17, 2026
 * @date Last Modified: May 17, 2026
 */
void testBattleDisplayNarrowWidthStacksShipStatsBelowButtons();
/**
 * @brief Validates tactical damage summary modal content and close behavior.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 04, 2026
 * @date Last Modified: Apr 04, 2026
 */
void testTacticalDamageSummaryDialogDisplaysContextAndCloseBehavior();
/**
 * @brief Verifies live ICM allocation updates assignments and ammo through completion.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 04, 2026
 * @date Last Modified: Apr 04, 2026
 */
void testICMSelectionDialogInteractionFinalizesAssignedCountsAndAmmo();
/**
 * @brief Verifies mine-placement Done button label reflects deployed ordnance types.
 *
 * Source-contract assertions confirm all four label variants exist in
 * drawPlaceMines(), and runtime checks confirm seekers-only (Battleship)
 * produces "Seeker Placement Done" and mixed SM+M (Minelayer) produces
 * "Weapon Placement Done".  The empty/fallback "Mine Placement Done" case
 * is validated by the existing action-button layout and prompt-reservation
 * tests that use ships with no deployable ordnance.
 *
 * @author claude-sonnet-4-6 (high)
 * @date Created: May 30, 2026
 * @date Last Modified: May 30, 2026
 */
void testMinePlacementDoneButtonLabelReflectsOrdnanceTypes();
};

}

#endif
