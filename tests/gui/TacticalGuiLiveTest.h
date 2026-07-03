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
 * @author gpt-5.3-codex (medium), gpt-5.4 (high), claude-sonnet-4-6 (high), claude-sonnet-4-6 (medium), claude-sonnet-4-6 (medium), claude-sonnet-4-6 (medium), claude-sonnet-4-6 (medium), claude-sonnet-4-6 (medium), claude-sonnet-4-6 (medium), claude-sonnet-4-6 (medium), claude-sonnet-5 (medium)
 * @date Created: Apr 04, 2026
 * @date Last Modified: Jul 03, 2026 (TMFR-03: add Turn-panel default-size/placement/caption/height-expansion coverage)
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
CPPUNIT_TEST( testPlacementSourceRowsArePopulatedAndClickSelectionUpdatesSources );
CPPUNIT_TEST( testPreGameSeekerRecallListAppearsAndClickRemovesSeeker );
CPPUNIT_TEST( testPlaceSeekersThreeColumnLayoutColumnPositionsAndClickRegions );
CPPUNIT_TEST( testLowerPanelHeightShrinksBackAfterPhaseChange );
CPPUNIT_TEST( testSeekerActivationAnchorIsAtActionPromptLineY );
CPPUNIT_TEST( testSeekerMoveCountOverlaySupressesOpponentLabelsDuringActivation );
CPPUNIT_TEST( testSeekerActivationRowTextShowsPositionAndMarginIsDynamic );
CPPUNIT_TEST( testBattleScreenExtraStyleExcludesTopLevelExDialog );
CPPUNIT_TEST( testBattleScreenDefaultStyleIncludesMinimizeBox );
CPPUNIT_TEST( testWxWindowDisablerDisablesOtherTopLevelsAndRestoresOnDelete );
CPPUNIT_TEST( testBattleScreenCloseIsIdempotentAcrossDuplicateCloseEvents );
CPPUNIT_TEST( testWXTacticalUIDismissActiveDialogDismissesNestedStackInnermostFirst );
CPPUNIT_TEST( testBattleScreenXCloseDismissesActiveChildDialog );
CPPUNIT_TEST( testTurnButtonPanelHiddenInNonMovePhase );
CPPUNIT_TEST( testTurnButtonPanelShownAndEnableStateReflectsModelInMovePhase );
CPPUNIT_TEST( testTurnButtonClickAppliesEndOfMoveTurnToModel );
CPPUNIT_TEST( testTurnPanelShownAtDefaultWindowSize );
CPPUNIT_TEST( testTurnPanelPlacedRightOfMovementDoneAndLeftOfShipInfoColumn );
CPPUNIT_TEST( testTurnPanelCaptionRendersAboveButtons );
CPPUNIT_TEST( testTurnPanelHeightExpandsToContainCaptionAndButtonsWithoutClipping );
CPPUNIT_TEST( testDefensiveFireDoneSkipsDialogWhenNoWeaponsFired );
CPPUNIT_TEST( testOffensiveFireDoneSkipsDialogWhenNoWeaponsFired );
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
	 * @brief Verifies the pending-seeker recall list position and click behavior during
	 * PH_ATTACK_FIRE seeker deployment.
	 *
	 * SMRIV-03: drawOffensiveSeekerPendingRows() is called in draw() inside a PH_ATTACK_FIRE
	 * guard at lMargin=310 / startY=getActionPromptLineY(0), anchoring the list to the top of
	 * the lower panel to the right of the Done button (matching the pre-game placement pattern).
	 * This test confirms:
	 * - m_pendingSeekerRecallRegions is empty before any deployment (AC1, AC4).
	 * - After injecting a movement path via TestableBattleScreen::findShipTurnData() and
	 *   deploying one pending seeker, recall regions are populated with x >= 310 and
	 *   y >= getActionPromptLineY(0) — anchored at the top of the lower panel, not below
	 *   the action-button row (AC2).
	 * - Clicking a recall region via checkOffensiveSeekerPendingSelection() removes the
	 *   pending seeker and restores ammo (AC3; existing recall behavior preserved).
	 *
	 * @author claude-sonnet-4-6 (medium), claude-sonnet-4-6 (medium)
	 * @date Created: Jun 02, 2026
	 * @date Last Modified: Jun 29, 2026
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

	/**
	 * @brief Behavioral verification for PGS-02: source rows are populated after draw, all row
	 * regions start below the action-button row, clicking a row updates the selected source and
	 * m_curShip/m_curWeapon, and the next board click places ordnance from the newly selected ship.
	 *
	 * AC1: Both Minelayers produce a row in m_shipNameRegions during BS_PlaceMines.
	 * AC2: Clicking row 1 updates getSelectedPlacementSourceIndex() to 1 and changes getShip().
	 * AC3: Every row region starts at or below getActionButtonRowBottom() (not clipped).
	 * AC4: handleHexClick() after source switch records ordnance with the newly selected shipID.
	 * AC5: Auto-selection on entry (getShip() non-NULL after beginMinePlacement()).
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 22, 2026
	 * @date Last Modified: Jun 22, 2026
	 */
	void testPlacementSourceRowsArePopulatedAndClickSelectionUpdatesSources();

	/**
	 * @brief Behavioral verification for PGS-04: the pre-game seeker recall list appears
	 * during BS_PlaceSeekers (right column at recallMargin=620), is populated with one row per
	 * placed inactive seeker group, and clicking a recall row removes exactly one seeker and
	 * restores ammo. The recall list must NOT appear during BS_PlaceMines.
	 * SMRIV-02: AC4 updated to verify recall regions are in the right column (x >= 620),
	 * not below the button row.
	 *
	 * AC1: During BS_PlaceSeekers, drawPlaceSeekers() populates m_preGameSeekerRecallRegions
	 *      after a seeker is placed (one region per (hex, source) group).
	 * AC2: Simulating a click on a recall region calls recallPlacedSeekerAtHexSource(),
	 *      decrements getSeekerMissiles().size() by 1, and restores one ammo round to the
	 *      seeker launcher.
	 * AC3: During BS_PlaceMines, draw() must not populate m_preGameSeekerRecallRegions
	 *      (recall list absent from mine phase).
	 * AC4: Each recall region left edge is at or right of recallMargin=620 (right column).
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 22, 2026
	 * @date Last Modified: Jun 23, 2026
	 */
	void testPreGameSeekerRecallListAppearsAndClickRemovesSeeker();

	/**
	 * @brief Behavioral verification for SMRIV-02: the BS_PlaceSeekers three-column layout
	 * anchors source rows in the middle column (lMargin=310, top of panel) and recall rows
	 * in the right column (recallMargin=620, top of panel). Click regions align with drawn
	 * positions for both columns. Selecting a source row and recalling a seeker still work.
	 *
	 * AC1: Source-selection rows have left edge >= lMargin=310 (middle column).
	 * AC2: Source-selection rows have top >= getActionPromptLineY(0) (top-of-panel anchor).
	 * AC3: Recall rows have left edge >= recallMargin=620 (right column).
	 * AC4: Recall rows have top >= getActionPromptLineY(0); recallMargin > lMargin
	 *      ensures horizontal column separation.
	 * AC5: Selecting a source row via checkShipSelection updates getSelectedPlacementSourceIndex()
	 *      and getWeapon() returns an SM weapon (behavior unchanged by layout change).
	 * AC6: Clicking a recall row via checkPreGameSeekerRecallSelection undeploys one seeker
	 *      and restores ammo (behavior unchanged by layout change).
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 23, 2026
	 * @date Last Modified: Jun 23, 2026
	 */
	void testPlaceSeekersThreeColumnLayoutColumnPositionsAndClickRegions();

	/**
	 * @brief Behavioral verification for SMRIV-04: lower-panel requestedDisplayHeight shrinks
	 * back after a phase transition, rather than ratcheting up permanently.
	 *
	 * SMRIV-04: ensureLowerPanelLayoutState() stores the last-seen battle state and phase.
	 * When either changes (phaseChanged == true) it skips the max-preserve of
	 * requestedDisplayHeight so the panel can shrink back to fit the new phase's content.
	 * When the phase is unchanged the max-preserve still applies so overflowing rows remain
	 * visible and clickable within the phase.
	 *
	 * AC1: After a phase that expanded the panel (BS_PlaceMines with many source rows)
	 *      transitions to BS_Battle/PH_MOVE, requestedDisplayHeight decreases rather than
	 *      remaining at the previously expanded value.
	 * AC3: The 120-px floor is preserved; the panel never shrinks below it.
	 * AC4: This test must fail against the pre-SMRIV-04 ratchet-only code and pass after
	 *      the fix.
	 *
	 * The test drives the behavior offscreen via wxMemoryDC draws in an FBattleScreen rig
	 * with 6 Minelayers so the BS_PlaceMines mine-source list is tall enough to expand the
	 * panel beyond the 120-px floor.  Two draws are performed in BS_PlaceMines because
	 * applyRequestedDisplayHeight() calls GetParent()->SendSizeEvent() synchronously
	 * (HandleWindowEvent path), which triggers reflowLowerPanelLayout().  On the first draw
	 * phaseChanged is true (lastBattleState was -1) and the height is reset to the floor;
	 * the first draw therefore primes lastBattleState.  The second draw runs with
	 * phaseChanged == false so the same-phase max-preserve keeps the expanded height.
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 29, 2026
	 * @date Last Modified: Jun 29, 2026
	 */
	void testLowerPanelHeightShrinksBackAfterPhaseChange();

	/**
	 * @brief Behavioral anchor discrimination test for SMRV-02: drawSeekerActivation()
	 * anchors the "Activated seekers:" list at getActionPromptLineY(0), not getActionButtonRowBottom().
	 *
	 * SMRV-02: Seeds one activated seeker (active=true, activationPhaseIndex=0, ownerID=1) via
	 * TestableBattleScreen, renders the lower panel via offscreen wxMemoryDC in
	 * PH_SEEKER_ACTIVATION, and asserts that:
	 * - m_seekerActivationRegions is populated with at least one region (the seeder was injected).
	 * - The first region's top y is >= getActionPromptLineY(0) (new top-of-panel anchor, AC-1).
	 * - The first region's top y is < getActionButtonRowBottom() — discriminates against the old
	 *   getActionButtonRowBottom() anchor: with old code the region starts at or after the button
	 *   row, failing this assertion; with new code it starts at the top of the panel, passing (AC-1).
	 * - Clicking the region via checkSeekerActivationSelection() reduces
	 *   getActiveSeekersByMovingPlayerThisPhase().size() by 1, confirming click-region/
	 *   draw-position alignment (AC-2).
	 *
	 * This test MUST fail against the pre-change (getActionButtonRowBottom) code and PASS against
	 * the shipped (getActionPromptLineY(0)) code.
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 29, 2026
	 * @date Last Modified: Jun 29, 2026
	 */
	void testSeekerActivationAnchorIsAtActionPromptLineY();

	/**
	 * @brief Behavioral pixel-level test: during PH_SEEKER_ACTIVATION the speed-value
	 * overlay suppresses opponent labels and shows moving-player labels; in other
	 * BS_Battle phases both players' labels render normally.
	 *
	 * SMRV-03: drawSeekerMoveCountOverlay() now guards on the activation phase and
	 * skips seekers not owned by the moving player, matching the sprite suppression in
	 * drawSeekerMissiles().
	 *
	 * Two seekers are seeded at distinct hexes:
	 * - Moving-player seeker (ownerID=1, AttackerID) at hex (5,5) — label MUST appear.
	 * - Opponent seeker (ownerID=0, DefenderID) at hex (3,3) — label MUST be absent.
	 *
	 * Asserts:
	 * AC1-absent: no red pixel at hex(3,3) label region during PH_SEEKER_ACTIVATION
	 *   (opponent label suppressed). This assertion FAILS against unguarded pre-change
	 *   code and PASSES against the shipped guarded code.
	 * AC1-present: a red pixel IS found at hex(5,5) label region during PH_SEEKER_ACTIVATION
	 *   (moving player's own seeker still renders).
	 * AC2: both hexes produce red pixels in PH_ATTACK_FIRE (labels unchanged outside
	 *   activation phase).
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 29, 2026
	 * @date Last Modified: Jun 29, 2026
	 */
	void testSeekerMoveCountOverlaySupressesOpponentLabelsDuringActivation();

	/**
	 * @brief Behavioral discrimination tests for SMRVI-02: drawSeekerActivation() shows
	 * seeker board position as (X,Y) instead of heading/allowance, and uses a dynamic
	 * lMargin computed from instruction text width and Done-button right edge.
	 *
	 * Three acceptance criteria are covered in one render-and-click rig:
	 *
	 * AC-a (text content): Seeds one activated seeker at hex (3,4), renders
	 * PH_SEEKER_ACTIVATION to an offscreen wxMemoryDC, and asserts the activation
	 * row region width matches the text extent of "Deactivate seeker #42 (3,4)"
	 * (new code) and does NOT match the extent of the old heading/allowance text
	 * "Deactivate seeker #42 (heading 0, allowance 2)".  The widths differ at runtime
	 * so this assertion fails against the pre-change code and passes after.
	 *
	 * AC-b (dynamic margin): Asserts the activation row region's left x-coordinate
	 * is strictly greater than 310 (the old fixed lMargin), confirming the margin is
	 * computed dynamically from instruction text and button extents.
	 *
	 * AC-c (click deactivates): Simulates a left-up click inside the activation row
	 * region via checkSeekerActivationSelection() and asserts
	 * getActiveSeekersByMovingPlayerThisPhase().size() decreases, confirming the
	 * click region aligns with the drawn position and triggers deactivation.
	 *
	 * All three assertions must fail against the pre-SMRVI-02 code and pass after
	 * the change.
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 30, 2026
	 * @date Last Modified: Jun 30, 2026
	 */
	void testSeekerActivationRowTextShowsPositionAndMarginIsDynamic();

	/**
	 * @brief Behavioral: constructing FBattleScreen must not set wxTOPLEVEL_EX_DIALOG in GetExtraStyle().
	 *
	 * TMF-02 AC1/AC2: Verifies that the constructor no longer calls
	 * SetExtraStyle(GetExtraStyle() | wxTOPLEVEL_EX_DIALOG).  On GTK,
	 * wxTOPLEVEL_EX_DIALOG triggers gtk_window_set_type_hint(DIALOG) which
	 * suppresses the minimize button in most window managers.  After the removal
	 * the bit must be absent from the extra style so the frame renders as
	 * GTK_WINDOW_TYPE_HINT_NORMAL with standard decorations.
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 30, 2026
	 * @date Last Modified: Jun 30, 2026
	 */
	void testBattleScreenExtraStyleExcludesTopLevelExDialog();

	/**
	 * @brief Behavioral: constructing FBattleScreen must include wxMINIMIZE_BOX in GetWindowStyleFlag().
	 *
	 * TMF-02 AC1: Confirms the default constructor style flag contains wxMINIMIZE_BOX
	 * so the platform window manager presents a functional minimize button.
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 30, 2026
	 * @date Last Modified: Jun 30, 2026
	 */
	void testBattleScreenDefaultStyleIncludesMinimizeBox();

	/**
	 * @brief Behavioral: wxWindowDisabler-based other-window enable/disable bookkeeping.
	 *
	 * TMFR-01: `FBattleScreen::ShowModal()` now installs a `wxWindowDisabler(this)` instead of
	 * the removed `gtk_window_set_modal()` grab, disabling every other top-level window for the
	 * battle's duration and restoring them on delete. `FBattleScreen::ShowModal()` itself cannot
	 * be driven live under the headless harness (its custom modal event loop blocks the runner;
	 * see the retired TMF-02 AC2 source-contract test this replaces), so this test exercises the
	 * `wxWindowDisabler` mechanism directly and behaviorally: it constructs two ordinary top-level
	 * windows standing in for the strategic main frame and a launching dialog, constructs a
	 * `wxWindowDisabler` skipping one of them (mirroring `new wxWindowDisabler(this)` in
	 * `ShowModal()`), and asserts the other window becomes disabled while the skipped window stays
	 * enabled, then asserts the disabled window is restored to enabled once the disabler is
	 * deleted (mirroring `EndModal()`/`~FBattleScreen()` deleting `m_windowDisabler`). This proves
	 * the enable/disable bookkeeping FBattleScreen relies on functions as required without
	 * asserting anything about `FBattleScreen::ShowModal()` from source structure alone.
	 *
	 * @author Claude Sonnet 5 (medium)
	 * @date Created: Jul 03, 2026
	 * @date Last Modified: Jul 03, 2026
	 */
	void testWxWindowDisablerDisablesOtherTopLevelsAndRestoresOnDelete();

	/**
	 * @brief Behavioral: closeBattleScreen()'s m_closeInProgress guard is safe against reentrancy.
	 *
	 * TMFR-01: the title-bar X now reaches onClose()/closeBattleScreen() via a GTK
	 * "delete-event" bypass in addition to wx's normal close routing and the File->Quit
	 * "activate" bypass, so more than one close vector can reach closeBattleScreen() for the
	 * same close request in close succession. Investigation during test authoring found that
	 * a second closeBattleScreen() call made strictly AFTER the first one has fully returned
	 * does not exercise this guard meaningfully (both the non-modal branch's
	 * `if (!IsBeingDeleted())` reset and the modal branch's EndModal() reset
	 * m_closeInProgress to false as part of a normal, successful close, so a later independent
	 * call is expected to safely re-run Hide()/Destroy(), which are themselves idempotent in
	 * wx). The guard's real job is preventing a call that arrives WHILE the first call is still
	 * executing (before it resets the flag) from re-running dialog dismissal/Hide()/Destroy()
	 * teardown concurrently. This test reproduces that reentrant scenario directly via
	 * AccessibleBattleScreen::Show(), which synthesizes a nested closeBattleScreen() call from
	 * inside the outer call's own Hide() (mirroring two close vectors firing for the same close
	 * in immediate succession), and asserts the reentrant call observes m_closeInProgress
	 * already set (took the guarded no-op path) and that the frame still ends up hidden.
	 *
	 * @author Claude Sonnet 5 (medium)
	 * @date Created: Jul 03, 2026
	 * @date Last Modified: Jul 03, 2026
	 */
	void testBattleScreenCloseIsIdempotentAcrossDuplicateCloseEvents();

	/**
	 * @brief Behavioral: WXTacticalUI::dismissActiveDialog() API surface, plus a source-contract
	 *        supplement locking the multi-dialog stack shape and iteration order.
	 *
	 * TMFR-01: WXTacticalUI's single `m_activeDialog` pointer was replaced with an
	 * innermost-first `m_dialogStack` (`std::vector<wxDialog*>`) so that more than one live
	 * child dialog can all be dismissed by one dismissActiveDialog() call. A genuinely nested
	 * two-dialog runtime scenario (opening a second showMessage() dialog from an action running
	 * inside the first dialog's live ShowModal() event loop) was attempted here and confirmed
	 * infeasible in this harness: it reliably hung the test runner indefinitely with no crash
	 * and no further progress. Per the TMFR-01 Verification Policy, this test therefore
	 * behaviorally exercises the single-dialog case (hasPendingDialog() becomes true while a
	 * real dialog is modal, then false immediately after dismissActiveDialog() -- the same API
	 * surface the multi-dialog stack path shares) and locks the multi-dialog STACK shape and
	 * iteration behavior (an innermost-first `std::vector<wxDialog*>`, walked from `rbegin()`)
	 * with a source-contract supplement, since the two-and-more-dialog runtime sequence cannot
	 * be safely reproduced live in this harness.
	 *
	 * @author Claude Sonnet 5 (medium)
	 * @date Created: Jul 03, 2026
	 * @date Last Modified: Jul 03, 2026
	 */
	void testWXTacticalUIDismissActiveDialogDismissesNestedStackInnermostFirst();

	/**
	 * @brief Behavioral: WXTacticalUI dismiss API and FBattleScreen X-close lifecycle
	 * when a child modal dialog is active.
	 *
	 * TMF-03: The fix adds hasPendingDialog()/dismissActiveDialog() to WXTacticalUI and
	 * has FBattleScreen::closeBattleScreen() call dismissActiveDialog() before Hide()/
	 * Show(false), clearing IsModal() so wxGTK never fires a second EndModal on the child.
	 *
	 * Behavioral assertions:
	 *   AC1: hasPendingDialog() returns true while ShowModal is live (dialog is tracked).
	 *   AC1: hasPendingDialog() returns false immediately after dismissActiveDialog().
	 *   AC2: FBattleScreen becomes hidden/destroyed after the subsequent close event.
	 *   AC3: No SIGABRT/crash during the full lifecycle (implicit — crash kills the test).
	 *   AC4: FBattleScreen lifecycle counters settle (constructed>=+1, destroyed>=+1, live==0).
	 *
	 * Source-contract supplement:
	 *   Verifies closeBattleScreen contains "dismissActiveDialog" — the integration point
	 *   that links the new WXTacticalUI API to the close path. This complements the
	 *   behavioral assertions because exercising closeBattleScreen while ShowModal is on
	 *   the test's call stack triggers a double-destroy race in the GTK widget tree.
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 30, 2026
	 * @date Last Modified: Jun 30, 2026
	 */
	void testBattleScreenXCloseDismissesActiveChildDialog();

	/**
	 * @brief Behavioral: Turn button panel is hidden in non-PH_MOVE phases and shown in PH_MOVE.
	 *
	 * TMF-05: draw() hides m_turnButtonPanel when the phase is not PH_MOVE.
	 * Verifies the panel is hidden after draw() in PH_DEFENSE_FIRE, PH_ATTACK_FIRE, and
	 * PH_SEEKER_ACTIVATION, and shown in PH_MOVE when a ship with MR > 0 is selected.
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 30, 2026
	 * @date Last Modified: Jun 30, 2026
	 */
	void testTurnButtonPanelHiddenInNonMovePhase();

	/**
	 * @brief Behavioral: Turn button enable state in PH_MOVE reflects model canApply* return values.
	 *
	 * TMF-05: drawMoveShip() calls canApplyEndOfMoveTurnLeft() / canApplyEndOfMoveTurnRight()
	 * and uses the results to set button enable states via wxButton::Enable().
	 * Verifies:
	 * - Both buttons enabled initially when MR > 0 and min-move satisfied.
	 * - After applying a left turn, only Turn Right is enabled (reverse only).
	 * - After reversing, both buttons re-enabled.
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 30, 2026
	 * @date Last Modified: Jun 30, 2026
	 */
	void testTurnButtonPanelShownAndEnableStateReflectsModelInMovePhase();

	/**
	 * @brief Behavioral: clicking Turn Left/Right button routes through to applyEndOfMoveTurn().
	 *
	 * TMF-05: onTurnLeft() / onTurnRight() call applyEndOfMoveTurn(+1/-1) on the parent
	 * battle screen. Verifies that after clicking Turn Left the model's pending facing
	 * is set (observable via findTurnData->pendingEndOfMoveFacing != -1) and the ship
	 * heading has changed.
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 30, 2026
	 * @date Last Modified: Jun 30, 2026
	 */
	void testTurnButtonClickAppliesEndOfMoveTurnToModel();

	/**
	 * @brief Behavioral: Turn Left/Turn Right panel is shown at the default window size.
	 *
	 * TMFR-03: before the fix, the panel's left margin was computed from the unwrapped
	 * text extent of the long move-phase instruction line, which overshot the ship-stats
	 * column and hid the panel on every paint at the default window size (panelWidth=1200,
	 * matching FBattleScreen's default wxSize(1200,900)). Verifies the panel is shown once
	 * placement is computed from wrapped line widths instead.
	 *
	 * @author claude-sonnet-5 (medium)
	 * @date Created: Jul 03, 2026
	 * @date Last Modified: Jul 03, 2026
	 */
	void testTurnPanelShownAtDefaultWindowSize();

	/**
	 * @brief Behavioral: Turn panel is placed right of Movement Done and left of ship-info column.
	 *
	 * TMFR-03: verifies panelRect.GetX() >= Movement Done button's right edge, and
	 * panelRect.GetRight() <= the ship-stats left margin, at the default window size.
	 *
	 * @author claude-sonnet-5 (medium)
	 * @date Created: Jul 03, 2026
	 * @date Last Modified: Jul 03, 2026
	 */
	void testTurnPanelPlacedRightOfMovementDoneAndLeftOfShipInfoColumn();

	/**
	 * @brief Behavioral: a caption is drawn above the Turn Left/Turn Right buttons.
	 *
	 * TMFR-03: verifies white caption text pixels are rendered in the region directly
	 * above the button panel after draw(), and locks the exact specified caption wording
	 * as a source-contract supplement to the pixel-level behavioral assertion.
	 *
	 * @author claude-sonnet-5 (medium)
	 * @date Created: Jul 03, 2026
	 * @date Last Modified: Jul 03, 2026
	 */
	void testTurnPanelCaptionRendersAboveButtons();

	/**
	 * @brief Behavioral: the lower panel grows to contain the caption plus button row.
	 *
	 * TMFR-03: verifies requestedDisplayHeight covers the caption+button block's bottom
	 * edge (no clipping) and that the block does not overlap the ship-info column or sit
	 * above the reserved action-prompt column top.
	 *
	 * @author claude-sonnet-5 (medium)
	 * @date Created: Jul 03, 2026
	 * @date Last Modified: Jul 03, 2026
	 */
	void testTurnPanelHeightExpandsToContainCaptionAndButtonsWithoutClipping();

	/**
	 * @brief Behavioral: onDefensiveFireDone does NOT invoke showDamageSummary when no weapons fired.
	 *
	 * TMF-06 AC2: installs a CountingMockTacticalUI on a FireDoneObserverBattleScreen, sets the
	 * game to BS_Battle / PH_DEFENSE_FIRE, triggers a draw to connect the button, then fires
	 * the button click event programmatically.  With an empty game (no ships loaded) the model
	 * returns weaponsFired == 0, and the guard in onDefensiveFireDone must suppress the call
	 * to showTacticalDamageSummaryDialog.  The mock's invocation counter must not increase.
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 30, 2026
	 * @date Last Modified: Jun 30, 2026
	 */
	void testDefensiveFireDoneSkipsDialogWhenNoWeaponsFired();

	/**
	 * @brief Behavioral: onOffensiveFireDone does NOT invoke showDamageSummary when no weapons fired.
	 *
	 * TMF-06 AC2: installs a CountingMockTacticalUI on a FireDoneObserverBattleScreen, sets the
	 * game to BS_Battle / PH_ATTACK_FIRE, triggers a draw to connect the button, then fires
	 * the button click event programmatically.  With an empty game (no ships loaded) the model
	 * returns weaponsFired == 0, and the guard in onOffensiveFireDone must suppress the call
	 * to showTacticalDamageSummaryDialog.  The mock's invocation counter must not increase.
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 30, 2026
	 * @date Last Modified: Jun 30, 2026
	 */
	void testOffensiveFireDoneSkipsDialogWhenNoWeaponsFired();
};

}

#endif
