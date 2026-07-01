/**
 * @file FTacticalGameMechanicsTest.h
 * @brief Header file for FTacticalGame mechanics implementation tests
 */

#ifndef _FTACTICALGAMEMECHANICSTEST_H_
#define _FTACTICALGAMEMECHANICSTEST_H_

#include <cppunit/extensions/HelperMacros.h>
#include <string>

namespace FrontierTests {

/**
 * @brief CppUnit fixture for FTacticalGameMechanicsTest tactical regression coverage.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium), gpt-5.4 (high), claude-sonnet-4-6 (medium), claude-sonnet-4-6 (medium)
 * @date Created: Mar 29, 2026
 * @date Last Modified: Jun 30, 2026
 */
class FTacticalGameMechanicsTest : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( FTacticalGameMechanicsTest );
CPPUNIT_TEST( testFTacticalGameImplementationCompilesStandalone );
CPPUNIT_TEST( testHeaderExposesAdditiveMechanicsApiSurface );
CPPUNIT_TEST( testResetInitializesSafeLegacyCompatibleDefaults );
CPPUNIT_TEST( testSeekerActivationPhaseEntryAndAutoSkipFlow );
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
CPPUNIT_TEST( testOrdnancePlacementSourceTrackingAndCompatibilityFlows );
CPPUNIT_TEST( testSeekerActivationApisExposeSelectionAndOneWayActivation );
CPPUNIT_TEST( testStoppedShipFreeRotationGuardsAndFacingSelectionFlow );
CPPUNIT_TEST( testImplementationRemainsSelfContainedWithoutLegacyWxRewire );
CPPUNIT_TEST( testSeekerDeploymentPhaseStateMachineTransitions );
CPPUNIT_TEST( testSeekerActivationPhaseIndexStampingAndFiltering );
CPPUNIT_TEST( testFBattleScreenGetActiveSeekersByMovingPlayerThisPhaseDelegate );
CPPUNIT_TEST( testPreGameOrdnancePlacementRecordingBehavior );
CPPUNIT_TEST( testPreGameMinePlacementPreservesShipAfterBeginMinePlacement );
CPPUNIT_TEST( testPreGameSeekerPlacementIsAdditive );
CPPUNIT_TEST( testDefenderMovePhaseGatingRejectsCompletionBeforeMinimumMove );
CPPUNIT_TEST_SUITE_END();

private:
std::string readFile(const std::string & path);
std::string extractFunctionBody(const std::string & source, const std::string & signature);
void assertContains(const std::string & haystack, const std::string & needle);

public:
void setUp();
void tearDown();

/**
 * @brief Verifies fTactical Game Implementation Compiles Standalone.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Mar 29, 2026
 * @date Last Modified: Mar 29, 2026
 */
void testFTacticalGameImplementationCompilesStandalone();
/**
 * @brief Verifies header Exposes Additive Mechanics Api Surface.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Mar 29, 2026
 * @date Last Modified: Mar 29, 2026
 */
void testHeaderExposesAdditiveMechanicsApiSurface();
/**
 * @brief Verifies reset Initializes Safe Legacy Compatible Defaults.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Mar 29, 2026
 * @date Last Modified: Mar 29, 2026
 */
void testResetInitializesSafeLegacyCompatibleDefaults();
/**
 * @brief Verifies seeker Activation Phase Entry And Auto Skip Flow.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.4 (high)
 * @date Created: May 25, 2026
 * @date Last Modified: May 25, 2026
 */
void testSeekerActivationPhaseEntryAndAutoSkipFlow();
/**
 * @brief Verifies tactical Report Lifecycle Uses Shared Report Types.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Mar 29, 2026
 * @date Last Modified: Mar 29, 2026
 */
void testTacticalReportLifecycleUsesSharedReportTypes();
/**
 * @brief Verifies fire All Weapons Owns Combat Report Lifecycle And Cleanup.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Mar 29, 2026
 * @date Last Modified: Mar 29, 2026
 */
void testFireAllWeaponsOwnsCombatReportLifecycleAndCleanup();
/**
 * @brief Verifies destroyed Ship Bookkeeping Lifecycle Contract Is Explicit.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 01, 2026
 * @date Last Modified: Apr 01, 2026
 */
void testDestroyedShipBookkeepingLifecycleContractIsExplicit();
/**
 * @brief Verifies movement Helpers Reset And Finalize Turn Data.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Mar 29, 2026
 * @date Last Modified: Mar 29, 2026
 */
void testMovementHelpersResetAndFinalizeTurnData();
/**
 * @brief Verifies winner And Combat End Helpers Resolve Battle State.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Mar 29, 2026
 * @date Last Modified: Mar 29, 2026
 */
void testWinnerAndCombatEndHelpersResolveBattleState();
/**
 * @brief Verifies destroyed Ship Cleanup Purges Hex And Turn Bookkeeping.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Mar 29, 2026
 * @date Last Modified: Mar 29, 2026
 */
void testDestroyedShipCleanupPurgesHexAndTurnBookkeeping();
/**
 * @brief Verifies fire ICMCollects Only Actionable Hex Interceptions.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Mar 29, 2026
 * @date Last Modified: Mar 29, 2026
 */
void testFireICMCollectsOnlyActionableHexInterceptions();
/**
 * @brief Verifies interaction Apis And Renderer Accessors Are Exposed.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 01, 2026
 * @date Last Modified: Apr 01, 2026
 */
void testInteractionApisAndRendererAccessorsAreExposed();
/**
 * @brief Verifies hex Click Dispatch And Target Selection Rules Flow Through Model State.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 01, 2026
 * @date Last Modified: Apr 01, 2026
 */
void testHexClickDispatchAndTargetSelectionRulesFlowThroughModelState();
/**
 * @brief Verifies mine Placement And Move Fire Progression Update Model State.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 01, 2026
 * @date Last Modified: Apr 01, 2026
 */
void testMinePlacementAndMoveFireProgressionUpdateModelState();
/**
 * @brief Verifies ordnance Placement Source Tracking And Compatibility Flows.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.4 (high)
 * @date Created: May 24, 2026
 * @date Last Modified: May 24, 2026
 */
void testOrdnancePlacementSourceTrackingAndCompatibilityFlows();
/**
 * @brief Verifies seeker Activation Apis Expose Selection And One Way Activation.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.4 (high)
 * @date Created: May 25, 2026
 * @date Last Modified: May 25, 2026
 */
void testSeekerActivationApisExposeSelectionAndOneWayActivation();
/**
 * @brief Verifies stopped Ship Free Rotation Guards And Facing Selection Flow.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: May 09, 2026
 * @date Last Modified: May 09, 2026
 */
void testStoppedShipFreeRotationGuardsAndFacingSelectionFlow();
/**
 * @brief Verifies implementation Remains Self Contained Without Legacy Wx Rewire.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Mar 29, 2026
 * @date Last Modified: Mar 29, 2026
 */
void testImplementationRemainsSelfContainedWithoutLegacyWxRewire();
/**
 * @brief Validates BS_PlaceSeekers state, ordnance-phase filter contracts, and
 *        completeMinePlacement/completeSeekerPlacement transition rules.
 *
 * Covers acceptance criteria for SMF-01:
 * - BS_PlaceSeekers constant declared after BS_PlaceMines in Frontier.h
 * - beginOrdnancePlacement() erases SM sources before the ammo-check loop
 * - beginSeekerPlacement() filters to SM sources and sets BS_PlaceSeekers
 * - completeMinePlacement() chains to BS_PlaceSeekers when SM sources exist
 * - completeMinePlacement() skips to BS_SetupAttackFleet when no SM sources
 * - completeSeekerPlacement() transitions to BS_SetupAttackFleet with toggleActivePlayer
 * - FBattleScreen::beginSeekerPlacement() and completeSeekerPlacement() delegate to model
 * - FTacticalGame model files remain free of wx headers/types
 *
 * @author claude-sonnet-4-6 (medium)
 * @date Created: Jun 02, 2026
 * @date Last Modified: Jun 02, 2026
 */
void testSeekerDeploymentPhaseStateMachineTransitions();
	/**
	 * @brief Validates SMF-04 per-activation-phase seeker tracking: activationPhaseIndex
	 *        field exists, counter resets to 0, increments in beginSeekerActivationPhase(),
	 *        both activate entry points stamp the index, and getActiveSeekersByMovingPlayerThisPhase()
	 *        filters by index == m_seekerActivationPhaseIndex.
	 *
	 * Covers acceptance criteria for SMF-04:
	 * - activationPhaseIndex field declared in FTacticalSeekerMissileState (non-persisted)
	 * - m_seekerActivationPhaseIndex counter initialized to 0 in reset()
	 * - Counter incremented in beginSeekerActivationPhase() before phase begins
	 * - activateSelectedInactiveSeeker() stamps activationPhaseIndex = m_seekerActivationPhaseIndex
	 * - activateInactiveSeekerAtHex() stamps activationPhaseIndex = m_seekerActivationPhaseIndex
	 * - getActiveSeekersByMovingPlayerThisPhase() filters by activationPhaseIndex == m_seekerActivationPhaseIndex
	 * - getActiveSeekersByMovingPlayer() remains unchanged (no phase filter)
	 * - activationPhaseIndex not written to save/load (persistence compatible)
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 02, 2026
	 * @date Last Modified: Jun 02, 2026
	 */
	void testSeekerActivationPhaseIndexStampingAndFiltering();
	/**
	 * @brief Validates SMF-04 FBattleScreen delegation: getActiveSeekersByMovingPlayerThisPhase()
	 *        is a const method returning by value and delegates straight to the model method.
	 *        Also confirms no wx headers are present in FBattleScreen model files.
	 *
	 * Covers acceptance criteria for SMF-04:
	 * - Accessor is read-only through FBattleScreen (const method returning by value)
	 * - FBattleScreen::getActiveSeekersByMovingPlayerThisPhase() delegates to model
	 * - wx-free: no wx headers in FTacticalGame for this delegation
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 02, 2026
	 * @date Last Modified: Jun 02, 2026
	 */
	void testFBattleScreenGetActiveSeekersByMovingPlayerThisPhaseDelegate();
	/**
	 * @brief Behavioral regression test for pre-game BS_PlaceMines and BS_PlaceSeekers
	 *        placement recording.
	 *
	 * Constructs a real FTacticalGame, loads a Minelayer fleet (SM x4 + M x20),
	 * enters BS_PlaceMines via beginOrdnancePlacement(), clicks a valid hex, and
	 * asserts that:
	 * - mine launcher ammo is decremented by one
	 * - the hex is recorded in getMinedHexes()
	 * - a placed-ordnance record is appended to getPlacedOrdnance()
	 * Then advances to BS_PlaceSeekers via completeMinePlacement(), clicks a
	 * different valid hex, and asserts that:
	 * - seeker launcher ammo is decremented by one
	 * - an inactive seeker record exists in getSeekerMissiles()
	 * - a placed-ordnance record for that hex is in getPlacedOrdnance()
	 *
	 * This test MUST fail against the pre-fix regression (BS_PlaceSeekers missing
	 * from handleHexClick) and pass after the fix.
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 19, 2026
	 * @date Last Modified: Jun 19, 2026
	 */
	void testPreGameOrdnancePlacementRecordingBehavior();
	/**
	 * @brief Behavioral regression for PGS-01: beginMinePlacement() preserves m_curShip
	 *        and m_curWeapon so the first board click can place a mine.
	 *
	 * Simulates the pre-fix bug (setShip(NULL) after beginMinePlacement) and asserts
	 * placeMineAtHex fails; then confirms the fixed path (no nulling) records the mine,
	 * decrements ammo, and updates getMinedHexes() and getPlacedOrdnance().
	 *
	 * This test MUST fail against the unfixed code (where onSetSpeed always called
	 * setShip(NULL)) and pass after the fix.
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 22, 2026
	 * @date Last Modified: Jun 22, 2026
	 */
	void testPreGameMinePlacementPreservesShipAfterBeginMinePlacement();

	/**
	 * @brief Behavioral test for PGS-03: pre-game seeker placement via BS_PlaceSeekers
	 *        is additive — repeated clicks on the same hex stack multiple inactive seekers
	 *        rather than toggling/removing the previously placed one.
	 *
	 * Constructs a real FTacticalGame with a Minelayer fleet, advances to BS_PlaceSeekers,
	 * then clicks the same hex 3 times. Asserts that:
	 * - getSeekerMissiles().size() == 3 (one inactive seeker per click, none removed)
	 * - Seeker launcher ammo is decremented by 3
	 * - getPlacedOrdnance() has the mine-phase record plus 3 SM-type records (total 4)
	 * - A 4th click when ammo runs out returns false and does not add another seeker
	 *   (only exercised when initial ammo >= 4; otherwise this check is skipped)
	 *
	 * This test MUST fail against the pre-fix toggle behavior (where the second click on
	 * the same hex would remove the first seeker and restore ammo) and pass after the fix.
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 22, 2026
	 * @date Last Modified: Jun 22, 2026
	 */
	void testPreGameSeekerPlacementIsAdditive();

	/**
	 * @brief Behavioral regression for TMF-04: the defender move-phase entry gating
	 *        bug in checkMoveStatus() that allowed Movement Done to be enabled
	 *        immediately without any ship having moved.
	 *
	 * The bug: checkMoveStatus() used m_shipPos as the position fallback when a
	 * ship's path was empty (phase entry).  m_shipPos holds the last-placed or
	 * last-selected ship and is stale at phase entry — especially for the defending
	 * player, whose move phase inherits m_shipPos from the attacker's last placement.
	 * When that stale position sits at the board edge (x=54 heading 3), the next hex
	 * in the forward direction is (55, y), which fails the in-bounds check, leaving
	 * finished=true and setting isMoveComplete()=true spuriously.
	 *
	 * The fix: use findShipHex() to look up each ship's actual board position instead
	 * of the stale m_shipPos.
	 *
	 * Test scenario:
	 * - Minelayer (defender, speed=10, ADF=1, minMove=9) placed at (10,10) heading 3.
	 * - AssaultScout (attacker, speed=0) placed LAST at (54,10) so m_shipPos=(54,10).
	 * - Set movingPlayer=false (defender), state=BS_Battle, then enter PH_MOVE.
	 * - Pre-fix: checkMoveStatus() uses m_shipPos=(54,10); findNextHex((54,10),3)=(55,10)
	 *   which fails x<55; finished stays true; isMoveComplete()=true WRONG.
	 * - Post-fix: checkMoveStatus() calls findShipHex() → (10,10); findNextHex((10,10),3)
	 *   =(11,10) in bounds; finished=false; isMoveComplete()=false CORRECT.
	 * - After moving 9 hexes, isMoveComplete() must become true.
	 *
	 * This test MUST fail against the pre-fix code and pass after the fix.
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 30, 2026
	 * @date Last Modified: Jun 30, 2026
	 */
	void testDefenderMovePhaseGatingRejectsCompletionBeforeMinimumMove();
};

}

#endif
