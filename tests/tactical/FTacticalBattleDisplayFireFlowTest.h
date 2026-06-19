/**
 * @file FTacticalBattleDisplayFireFlowTest.h
 * @brief Header file for the FTacticalBattleDisplayFireFlowTest class
 */

#ifndef FTacticalBattleDisplayFireFlowTest_H_
#define FTacticalBattleDisplayFireFlowTest_H_

#include <cppunit/extensions/HelperMacros.h>
#include <string>

namespace FrontierTests {

/**
 * @brief CppUnit fixture for FTacticalBattleDisplayFireFlowTest tactical regression coverage.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium), gpt-5.4 (high), claude-sonnet-4-6 (medium)
 * @date Created: Mar 22, 2026
 * @date Last Modified: Jun 19, 2026
 */
class FTacticalBattleDisplayFireFlowTest : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( FTacticalBattleDisplayFireFlowTest );
CPPUNIT_TEST( testDrawAndOnPaintUseBattleScreenStateAccessors );
CPPUNIT_TEST( testZoomImageLoadUsesResolveAssetPath );
CPPUNIT_TEST( testBattleBoardAndBattleScreenUseResolveAssetPathForTacticalImages );
CPPUNIT_TEST( testLegacyFireAllWeaponsHelperRemoved );
CPPUNIT_TEST( testDefensiveFireDoneDelegatesToModelFirePhaseResolution );
CPPUNIT_TEST( testOffensiveFireDoneDelegatesToModelFirePhaseResolution );
CPPUNIT_TEST( testDefensiveFireDoneDisablesAndHidesButtonBeforeResolution );
CPPUNIT_TEST( testOffensiveFireDoneDisablesAndHidesButtonBeforeResolution );
CPPUNIT_TEST( testWeaponSelectionDelegatesToBattleScreenModelApi );
CPPUNIT_TEST( testDefenseSelectionDelegatesToBattleScreenModelApi );
CPPUNIT_TEST( testMinePlacementFlowUsesModelMinePlacementApis );
CPPUNIT_TEST( testMinePlacementDisplayUsesModelShipList );
CPPUNIT_TEST( testSetupPlacementBoardUsesSourceSpecificOrdnanceRendering );
CPPUNIT_TEST( testDisplayClickFlowUsesModelForwardingApis );
CPPUNIT_TEST( testMoveDoneDelegatesToBattleScreenCompleteMovePhase );
CPPUNIT_TEST( testMoveDoneDisconnectsAndHidesMoveButtonAroundDelegation );
CPPUNIT_TEST( testMovePromptDifferentiatesStoppedFacingNormalAndNoShipCases );
CPPUNIT_TEST( testActionPromptSpacingContractConstantsAndHelpersDefined );
CPPUNIT_TEST( testActionPromptSpacingContractAppliedAcrossActionPhases );
CPPUNIT_TEST( testActionButtonShowPathsRelayoutAfterVisibilityChange );
CPPUNIT_TEST( testActionButtonHidePathsRelayoutAfterVisibilityChange );
CPPUNIT_TEST( testSeekerActivationDrawAndClickFlowUseActivationPhaseRouting );
CPPUNIT_TEST( testSeekerActivationPanelListsInstructionAndOneRowPerInactiveSeeker );
CPPUNIT_TEST( testSeekerActivationButtonUsesShowHideDisconnectAndRelayoutPattern );
CPPUNIT_TEST( testOffensiveSeekerDeploymentRuntimeFlowSupportsPendingRecallAndCommit );
CPPUNIT_TEST( testLowerPanelLayoutStateDefinesSharedPromptStatsAndHeightFields );
CPPUNIT_TEST( testLowerPanelLayoutStatePersistsSplitUntilGeometryInvalidatesIt );
CPPUNIT_TEST( testRequestedDisplayHeightFlowsFromLayoutStateIntoMinSize );
CPPUNIT_TEST( testBattleScreenDefaultsTo1200x900InConstructorSignature );
CPPUNIT_TEST( testBattleScreenLayoutPolicyUsesBaselineAndMapFloorConstants );
CPPUNIT_TEST( testBattleScreenLayoutPolicyAppliesDisplayRequestAndMapFloorClamp );
CPPUNIT_TEST( testBattleScreenResizeHandlerReflowsLowerPanelBeforeLayoutPolicy );
CPPUNIT_TEST( testLowerPanelReflowPathRecomputesMovePromptReservationFromCurrentGeometry );
CPPUNIT_TEST( testMovePromptConstrainedWidthSelectionPathUsesDeterministicHelpers );
CPPUNIT_TEST( testShipStatsMeasurementAndSplitEligibilityUseContentBasedSizing );
CPPUNIT_TEST( testPlaceMinesSourceListStartsAtActionButtonRowBottom );
CPPUNIT_TEST( testTwoPhaseSeekerDeploymentDrawAndClickDispatching );
CPPUNIT_TEST( testDrawPlaceSeekersUsesSeekerSpecificPromptsAndSMFilter );
CPPUNIT_TEST( testMinePhaseUsesExactPromptTextAndMFilter );
CPPUNIT_TEST( testGetActionButtonRowBottomIncludesSeekerPlacementDoneButton );
CPPUNIT_TEST( testDrawPlaceMinesExpandsPanelHeightWhenRowsExceedMinimum );
CPPUNIT_TEST( testDrawPlaceSeekersExpandsPanelHeightWhenRowsExceedMinimum );
CPPUNIT_TEST( testDrawSeekerActivationExpandsPanelHeightWhenRowsExceedMinimum );
CPPUNIT_TEST_SUITE_END();

private:
std::string readFile(const std::string & path);
std::string extractFunctionBody(const std::string & source, const std::string & signature);
void assertContains(const std::string & haystack, const std::string & needle);
void assertNotContains(const std::string & haystack, const std::string & needle);
void assertBefore(const std::string & haystack, const std::string & first, const std::string & second);
unsigned int countOccurrences(const std::string & haystack, const std::string & needle);

public:
void setUp();
void tearDown();

/**
 * @brief Verifies draw And On Paint Use Battle Screen State Accessors.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 01, 2026
 * @date Last Modified: Apr 01, 2026
 */
void testDrawAndOnPaintUseBattleScreenStateAccessors();
/**
 * @brief Verifies zoom Image Load Uses Resolve Asset Path.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 17, 2026
 * @date Last Modified: Apr 17, 2026
 */
void testZoomImageLoadUsesResolveAssetPath();
/**
 * @brief Verifies battle Board And Battle Screen Use Resolve Asset Path For Tactical Images.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 17, 2026
 * @date Last Modified: Apr 17, 2026
 */
void testBattleBoardAndBattleScreenUseResolveAssetPathForTacticalImages();
/**
 * @brief Verifies legacy Fire All Weapons Helper Removed.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 01, 2026
 * @date Last Modified: Apr 01, 2026
 */
void testLegacyFireAllWeaponsHelperRemoved();
/**
 * @brief Verifies defensive Fire Done Delegates To Model Fire Phase Resolution.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 01, 2026
 * @date Last Modified: Apr 01, 2026
 */
void testDefensiveFireDoneDelegatesToModelFirePhaseResolution();
/**
 * @brief Verifies offensive Fire Done Delegates To Model Fire Phase Resolution.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 01, 2026
 * @date Last Modified: Apr 01, 2026
 */
void testOffensiveFireDoneDelegatesToModelFirePhaseResolution();
/**
 * @brief Verifies defensive Fire Done Disables And Hides Button Before Resolution.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 02, 2026
 * @date Last Modified: Apr 02, 2026
 */
void testDefensiveFireDoneDisablesAndHidesButtonBeforeResolution();
/**
 * @brief Verifies offensive Fire Done Disables And Hides Button Before Resolution.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 02, 2026
 * @date Last Modified: Apr 02, 2026
 */
void testOffensiveFireDoneDisablesAndHidesButtonBeforeResolution();
/**
 * @brief Verifies weapon Selection Delegates To Battle Screen Model Api.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 01, 2026
 * @date Last Modified: Apr 01, 2026
 */
void testWeaponSelectionDelegatesToBattleScreenModelApi();
/**
 * @brief Verifies defense Selection Delegates To Battle Screen Model Api.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 01, 2026
 * @date Last Modified: Apr 01, 2026
 */
void testDefenseSelectionDelegatesToBattleScreenModelApi();
/**
 * @brief Verifies mine Placement Flow Uses Model Mine Placement Apis.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 01, 2026
 * @date Last Modified: Apr 01, 2026
 */
void testMinePlacementFlowUsesModelMinePlacementApis();
/**
 * @brief Verifies mine Placement Display Uses Model Ship List.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 01, 2026
 * @date Last Modified: Apr 01, 2026
 */
void testMinePlacementDisplayUsesModelShipList();
/**
 * @brief Verifies setup-board rendering uses source-indexed placed ordnance colors.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.4 (high)
 * @date Created: May 24, 2026
 * @date Last Modified: May 24, 2026
 */
void testSetupPlacementBoardUsesSourceSpecificOrdnanceRendering();
/**
 * @brief Verifies display Click Flow Uses Model Forwarding Apis.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 01, 2026
 * @date Last Modified: Apr 01, 2026
 */
void testDisplayClickFlowUsesModelForwardingApis();
/**
 * @brief Verifies move Done Delegates To Battle Screen Complete Move Phase.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 02, 2026
 * @date Last Modified: Apr 02, 2026
 */
void testMoveDoneDelegatesToBattleScreenCompleteMovePhase();
/**
 * @brief Verifies move Done Disconnects And Hides Move Button Around Delegation.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 02, 2026
 * @date Last Modified: Apr 02, 2026
 */
void testMoveDoneDisconnectsAndHidesMoveButtonAroundDelegation();
/**
 * @brief Verifies move Prompt Differentiates Stopped Facing Normal And No Ship Cases.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: May 09, 2026
 * @date Last Modified: May 09, 2026
 */
void testMovePromptDifferentiatesStoppedFacingNormalAndNoShipCases();
/**
 * @brief Verifies action Prompt Spacing Contract Constants And Helpers Defined.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 08, 2026
 * @date Last Modified: Apr 08, 2026
 */
void testActionPromptSpacingContractConstantsAndHelpersDefined();
/**
 * @brief Verifies action Prompt Spacing Contract Applied Across Action Phases.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 08, 2026
 * @date Last Modified: Apr 08, 2026
 */
void testActionPromptSpacingContractAppliedAcrossActionPhases();
/**
 * @brief Verifies action Button Show Paths Relayout After Visibility Change.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 08, 2026
 * @date Last Modified: Apr 08, 2026
 */
void testActionButtonShowPathsRelayoutAfterVisibilityChange();
/**
 * @brief Verifies action Button Hide Paths Relayout After Visibility Change.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 08, 2026
 * @date Last Modified: Apr 08, 2026
 */
void testActionButtonHidePathsRelayoutAfterVisibilityChange();
/**
 * @brief Verifies seeker activation lower-panel draw/click flow routes through activation-specific handlers.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.4 (high)
 * @date Created: May 25, 2026
 * @date Last Modified: May 25, 2026
 */
void testSeekerActivationDrawAndClickFlowUseActivationPhaseRouting();
/**
 * @brief Verifies seeker activation panel instructions and one-row-per-inactive-seeker stack rendering.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.4 (high)
 * @date Created: May 25, 2026
 * @date Last Modified: May 25, 2026
 */
void testSeekerActivationPanelListsInstructionAndOneRowPerInactiveSeeker();
/**
 * @brief Verifies seeker activation done button follows the standard connect/show/hide/disconnect/relayout contract.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.4 (high)
 * @date Created: May 25, 2026
 * @date Last Modified: May 25, 2026
 */
void testSeekerActivationButtonUsesShowHideDisconnectAndRelayoutPattern();
/**
 * @brief Verifies offensive-fire seeker deployment supports pending placement, recall, and commit.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.4 (high)
 * @date Created: May 25, 2026
 * @date Last Modified: May 25, 2026
 */
void testOffensiveSeekerDeploymentRuntimeFlowSupportsPendingRecallAndCommit();
/**
 * @brief Verifies FBattleDisplay declares shared lower-panel prompt/stats/height state.
 *
 * @author gpt-5.3-codex (medium), GPT-5 (high)
 * @date Created: May 16, 2026
 * @date Last Modified: May 16, 2026
 */
void testLowerPanelLayoutStateDefinesSharedPromptStatsAndHeightFields();
/**
 * @brief Verifies lower-panel split state is preserved unless geometry invalidates it.
 *
 * @author gpt-5.3-codex (medium), GPT-5 (high)
 * @date Created: May 16, 2026
 * @date Last Modified: May 16, 2026
 */
void testLowerPanelLayoutStatePersistsSplitUntilGeometryInvalidatesIt();
/**
 * @brief Verifies requested display height propagates from lower-panel layout state.
 *
 * @author gpt-5.3-codex (medium), GPT-5 (high)
 * @date Created: May 16, 2026
 * @date Last Modified: May 16, 2026
 */
void testRequestedDisplayHeightFlowsFromLayoutStateIntoMinSize();
/**
 * @brief Verifies FBattleScreen constructor default size is explicitly 1200x900.
 *
 * @author gpt-5.4 (high)
 * @date Created: May 16, 2026
 * @date Last Modified: May 16, 2026
 */
void testBattleScreenDefaultsTo1200x900InConstructorSignature();
/**
 * @brief Verifies FBattleScreen layout policy constants define 120 baseline and 60 percent map floor.
 *
 * @author gpt-5.4 (high)
 * @date Created: May 16, 2026
 * @date Last Modified: May 16, 2026
 */
void testBattleScreenLayoutPolicyUsesBaselineAndMapFloorConstants();
/**
 * @brief Verifies FBattleScreen layout policy honors larger display requests and clamps to preserve map floor.
 *
 * @author gpt-5.4 (high)
 * @date Created: May 16, 2026
 * @date Last Modified: May 16, 2026
 */
void testBattleScreenLayoutPolicyAppliesDisplayRequestAndMapFloorClamp();
/**
 * @brief Verifies FBattleScreen resize handler reflows lower-panel layout before screen-level policy.
 *
 * @author gpt-5.4 (high)
 * @date Created: May 16, 2026
 * @date Last Modified: May 16, 2026
 */
void testBattleScreenResizeHandlerReflowsLowerPanelBeforeLayoutPolicy();
/**
 * @brief Verifies FBattleDisplay reflow path recalculates move prompt reservations using current geometry.
 *
 * @author gpt-5.4 (high)
 * @date Created: May 16, 2026
 * @date Last Modified: May 16, 2026
 */
void testLowerPanelReflowPathRecomputesMovePromptReservationFromCurrentGeometry();
/**
 * @brief Verifies constrained-width move prompt paths route through shared deterministic helper seams.
 *
 * @author gpt-5.4 (high)
 * @date Created: May 16, 2026
 * @date Last Modified: May 16, 2026
 */
void testMovePromptConstrainedWidthSelectionPathUsesDeterministicHelpers();
/**
 * @brief Verifies ship-stats split eligibility and requested height derive from measured text content.
 *
 * @author gpt-5.4 (high)
 * @date Created: May 17, 2026
 * @date Last Modified: May 17, 2026
 */
void testShipStatsMeasurementAndSplitEligibilityUseContentBasedSizing();

/**
 * @brief Verifies that drawPlaceMines() starts the source list at getActionButtonRowBottom()
 * rather than BORDER, ensuring no vertical overlap between the instruction/button region and
 * the placement source list region. Also verifies hit regions and other draw phases are unaffected.
 *
 * AC: SMC-04 -- pre-game placement panel layout overlap fix.
 *
 * @author claude-sonnet-4-6 (standard)
 * @date Created: May 30, 2026
 * @date Last Modified: May 30, 2026
 */
void testPlaceMinesSourceListStartsAtActionButtonRowBottom();

/**
 * @brief Verifies that draw() dispatches BS_PlaceSeekers to drawPlaceSeekers(), onLeftUp()
 * dispatches BS_PlaceSeekers to checkShipSelection(), and the constructor creates,
 * adds to actionSizer, and hides m_buttonSeekerPlacementDone at startup.
 *
 * AC: SMF-02 -- two-phase deployment state machine rendering.
 *
 * @author claude-sonnet-4-6 (medium)
 * @date Created: Jun 02, 2026
 * @date Last Modified: Jun 02, 2026
 */
void testTwoPhaseSeekerDeploymentDrawAndClickDispatching();

/**
 * @brief Verifies that drawPlaceSeekers() uses the seeker-specific prompt text,
 * "Select a source row to place seeker missiles." selection line, SM-only filter,
 * completeSeekerPlacement() route, and the standard show/hide/disconnect button pattern.
 *
 * AC: SMF-02 -- seeker phase content and filter correctness.
 *
 * @author claude-sonnet-4-6 (medium)
 * @date Created: Jun 02, 2026
 * @date Last Modified: Jun 02, 2026
 */
void testDrawPlaceSeekersUsesSeekerSpecificPromptsAndSMFilter();

/**
 * @brief Verifies that drawPlaceMines() uses the exact mine-specific prompt text,
 * "Select a source row to place mines." selection line, FWeapon::M-only filter,
 * and that no combined "Weapon Placement Done" label path remains.
 *
 * AC: SMF-02 -- mine phase content and filter correctness.
 *
 * @author claude-sonnet-4-6 (medium)
 * @date Created: Jun 02, 2026
 * @date Last Modified: Jun 02, 2026
 */
void testMinePhaseUsesExactPromptTextAndMFilter();

/**
 * @brief Verifies that getActionButtonRowBottom() scans m_buttonSeekerPlacementDone
 * along with the other action buttons when computing the shown-button bottom edge.
 *
 * AC: SMF-02 -- seeker placement button included in action-button row geometry.
 *
 * @author claude-sonnet-4-6 (medium)
 * @date Created: Jun 02, 2026
 * @date Last Modified: Jun 02, 2026
 */
void testGetActionButtonRowBottomIncludesSeekerPlacementDoneButton();

/**
 * @brief Source-contract: drawPlaceMines() computes mineListBottom = y + BORDER and expands
 * requestedDisplayHeight when that value exceeds the current minimum.
 *
 * AC: SMFR-01 -- mine placement row list panel-height auto-expansion mirrors
 * the pattern used by drawOffensiveSeekerPendingRows(). This test verifies code
 * structure only; the authoritative behavioral coverage is
 * testOrdnancePlacementAndActivationPanelHeightAutoExpands in TacticalGuiLiveTest.
 *
 * @author claude-sonnet-4-6 (medium)
 * @date Created: Jun 19, 2026
 * @date Last Modified: Jun 19, 2026
 */
void testDrawPlaceMinesExpandsPanelHeightWhenRowsExceedMinimum();

/**
 * @brief Source-contract: drawPlaceSeekers() computes seekerListBottom = y + BORDER and
 * expands requestedDisplayHeight when that value exceeds the current minimum.
 *
 * AC: SMFR-01 -- seeker placement row list panel-height auto-expansion. This test
 * verifies code structure only; the authoritative behavioral coverage is
 * testOrdnancePlacementAndActivationPanelHeightAutoExpands in TacticalGuiLiveTest.
 *
 * @author claude-sonnet-4-6 (medium)
 * @date Created: Jun 19, 2026
 * @date Last Modified: Jun 19, 2026
 */
void testDrawPlaceSeekersExpandsPanelHeightWhenRowsExceedMinimum();

/**
 * @brief Source-contract: drawSeekerActivation() computes activationListBottom = y + BORDER and
 * expands requestedDisplayHeight when that value exceeds the current minimum.
 *
 * AC: SMFR-01 -- seeker activation row list panel-height auto-expansion. This test
 * verifies code structure only; the authoritative behavioral coverage is
 * testOrdnancePlacementAndActivationPanelHeightAutoExpands in TacticalGuiLiveTest.
 *
 * @author claude-sonnet-4-6 (medium)
 * @date Created: Jun 19, 2026
 * @date Last Modified: Jun 19, 2026
 */
void testDrawSeekerActivationExpandsPanelHeightWhenRowsExceedMinimum();
};

}

#endif
