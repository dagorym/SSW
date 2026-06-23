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
 * @author gpt-5.3-codex (medium), gpt-5.4 (high), claude-sonnet-4-6 (high), claude-sonnet-4-6 (medium)
 * @date Created: Apr 04, 2026
 * @date Last Modified: Jun 22, 2026
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
CPPUNIT_TEST( testOnSetSpeedMinePlacementPreservesShipForFirstBoardClick );
CPPUNIT_TEST( testOffensiveSeekerPendingListRegionVisibilityAndRecall );
CPPUNIT_TEST( testOrdnancePlacementAndActivationPanelHeightAutoExpands );
CPPUNIT_TEST( testSeekerMoveCountOverlayRendersInAllBattlePhases );
CPPUNIT_TEST( testSeekerPathRendersInPHMoveWithMovementPath );
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
 * @brief Verifies two-phase deployment produces correct Done buttons per phase.
 *
 * Runtime checks confirm that a Minelayer (both M and SM weapons) in BS_PlaceMines
 * shows only the "Mine Placement Done" button, and after transitioning to BS_PlaceSeekers
 * via completeMinePlacement() shows only the "Seeker Placement Done" button.
 *
 * @author claude-sonnet-4-6 (high), claude-sonnet-4-6 (medium)
 * @date Created: May 30, 2026
 * @date Last Modified: Jun 02, 2026
 */
void testMinePlacementDoneButtonLabelReflectsOrdnanceTypes();
	/**
	 * @brief Behavioral regression for PGS-01: onSetSpeed mine-entry path preserves
	 *        m_curShip and m_curWeapon so the first board click can record a mine.
	 *
	 * Drives the real onSetSpeed/mine-entry path by setting up a FBattleScreen in
	 * BS_SetupDefendFleet with getDone()==true, then fires the "Set Speed" button event
	 * and asserts that:
	 * - The screen transitions to BS_PlaceMines.
	 * - getShip() is non-NULL (fixed: enteredMinePlacement flag prevents setShip(NULL)).
	 * - getWeapon() is non-NULL and of type M.
	 * - handleHexClick() in BS_PlaceMines records a mine (ammo decrement, hex in
	 *   getMinedHexes(), record in getPlacedOrdnance()).
	 *
	 * This test MUST fail against the unfixed code and pass after the fix.
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 22, 2026
	 * @date Last Modified: Jun 22, 2026
	 */
	void testOnSetSpeedMinePlacementPreservesShipForFirstBoardClick();
	/**
	 * @brief Verifies the pending-seeker list widget region during PH_ATTACK_FIRE seeker deployment.
	 *
	 * SMF-03: drawOffensiveSeekerPendingRows now runs in draw() inside a PH_ATTACK_FIRE guard,
	 * placing the pending list in a dedicated region left of the ship-status widget.
	 * This test confirms that after deploying seekers:
	 * - m_pendingSeekerRecallRegions is populated with one region per pending group.
	 * - Each recall region is positioned within the lower-panel left column (x >= leftOffset,
	 *   x < shipStatsLeftMargin) so the list stays left of ship-status.
	 * - Each recall region's top is at or below getActionButtonRowBottom()+BORDER, meaning
	 *   it does not overlap the action-button row.
	 * - Recall (via recallSelectedOffensivePendingSeekerAtHex) removes one seeker from the
	 *   model and the next draw rebuilds a shorter pending list.
	 * - The ship-status region (drawCurrentShipStats) is unaffected by the pending list.
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 02, 2026
	 * @date Last Modified: Jun 02, 2026
	 */
	void testOffensiveSeekerPendingListRegionVisibilityAndRecall();

	/**
	 * @brief Behavioral verification that drawPlaceMines(), drawPlaceSeekers(), and
	 * drawSeekerActivation() each expand the lower-panel height when rendered rows
	 * extend below the initial 120-px minimum.
	 *
	 * SMFR-01: Drives each placement/activation draw phase via offscreen wxMemoryDC
	 * and asserts that requestedDisplayHeight (via peer accessor) and GetMinSize().GetHeight()
	 * do not decrease after drawing a Minelayer's source list, confirming end-to-end wiring
	 * of the auto-expansion path. The seeker phase additionally asserts that
	 * requestedDisplayHeight is at least as large as actionButtonRowBottom(), proving genuine
	 * expansion occurred rather than merely a non-decrease from the pre-draw baseline.
	 * This is the authoritative behavioral test for the SMFR-01 height-expansion acceptance
	 * criterion; the three structural source-contract tests in FTacticalBattleDisplayFireFlowTest
	 * supplement this test by locking code shape but do not substitute for it.
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 19, 2026
	 * @date Last Modified: Jun 19, 2026
	 */
	void testOrdnancePlacementAndActivationPanelHeightAutoExpands();
	/**
	 * @brief Behavioral: drawSeekerMoveCountOverlay fires for all BS_Battle phases,
	 * confirmed by a pixel-level observable with an active seeker in PH_ATTACK_FIRE.
	 *
	 * SMFR-04 pass-2 strengthening: Uses TestableBattleScreen to inject one active
	 * seeker (hex 5,5, movementAllowance=3) before rendering.  Drives
	 * FBattleBoard::draw() in PH_ATTACK_FIRE via offscreen wxMemoryDC and asserts
	 * that at least one red (#FF0000-like) pixel appears in the upper-right label
	 * region of that hex — the only pixel-level observable that proves the overlay
	 * was reached and drew output.  A no-crash assertion alone would pass even if
	 * the overlay were re-guarded inside PH_MOVE/PH_SEEKER_ACTIVATION because the
	 * overlay exits early when the seeker list is empty.
	 *
	 * Also verifies no-crash for PH_MOVE, PH_DEFENSE_FIRE, and PH_SEEKER_ACTIVATION,
	 * and AC4 (seeker count unchanged after all draws).
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 19, 2026
	 * @date Last Modified: Jun 19, 2026
	 */
	void testSeekerMoveCountOverlayRendersInAllBattlePhases();

	/**
	 * @brief Behavioral render test: drawSeekerPaths draws a visible path line in the
	 * board during PH_MOVE and PH_SEEKER_ACTIVATION when a seeker has movementPath >= 2.
	 *
	 * SMFR-05 AC1 render-side coverage: Uses TestableBattleScreen to inject one
	 * active seeker with a pre-populated movementPath (5,5)->(5,7) AFTER calling
	 * setPhase(PH_MOVE) so the path is not cleared by resolveActiveSeekersForMovingPlayer.
	 * Drives FBattleBoard::draw() via offscreen wxMemoryDC in PH_MOVE, PH_SEEKER_ACTIVATION,
	 * and PH_ATTACK_FIRE. Asserts that:
	 * - PH_MOVE and PH_SEEKER_ACTIVATION produce pixel differences in the path band
	 *   x=[344..356], y=[290..385] vs the no-seeker baseline (drawSeekerPaths ran).
	 * - PH_ATTACK_FIRE produces zero diffs in that band (drawSeekerPaths not called).
	 * A platform pre-check confirms dc.DrawLine works on wxMemoryDC at those coords.
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 19, 2026
	 * @date Last Modified: Jun 19, 2026
	 */
	void testSeekerPathRendersInPHMoveWithMovementPath();
};

}

#endif
