/**
 * @file FTacticalBattleBoardRendererDelegationTest.h
 * @brief Header file for FBattleBoard renderer delegation tests
 */

#ifndef FTacticalBattleBoardRendererDelegationTest_H_
#define FTacticalBattleBoardRendererDelegationTest_H_

#include <cppunit/extensions/HelperMacros.h>
#include <string>

namespace FrontierTests {

/**
 * @brief CppUnit fixture for FTacticalBattleBoardRendererDelegationTest tactical regression coverage.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium), gpt-5.4 (high), claude-sonnet-4-6 (standard), claude-sonnet-4-6 (standard)
 * @date Created: Apr 01, 2026
 * @date Last Modified: May 30, 2026
 */
class FTacticalBattleBoardRendererDelegationTest : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( FTacticalBattleBoardRendererDelegationTest );
CPPUNIT_TEST( testDrawUsesBattleScreenModelAccessors );
CPPUNIT_TEST( testOverlayDrawingUsesModelStateAccessors );
CPPUNIT_TEST( testDrawShipsUsesTemporaryFacingForStoppedSelectedMover );
CPPUNIT_TEST( testDrawSeekerMissilesUsesActivationAndBattleVisibilityRules );
CPPUNIT_TEST( testOnLeftUpOnlyHitTestsAndForwardsHexClick );
CPPUNIT_TEST( testOnLeftUpRoutesActivationPhaseThroughSeekerSelection );
CPPUNIT_TEST( testHeaderRemovesDuplicatedMechanicsState );
CPPUNIT_TEST( testOnMotionUsesPlacementForwardersOnly );
CPPUNIT_TEST( testBoardConstructorLoadsSeekerMissileIconThroughResolveAssetPath );
CPPUNIT_TEST( testDrawSeekerPathsIsCalledInMovePhaseWithCyanPen );
CPPUNIT_TEST( testDrawSeekerMissilesPendingOffensiveFireBranch );
CPPUNIT_TEST_SUITE_END();

private:
std::string readFile(const std::string & path);
std::string extractFunctionBody(const std::string & source, const std::string & signature);
void assertContains(const std::string & haystack, const std::string & needle);

public:
void setUp();
void tearDown();

/**
 * @brief Verifies draw Uses Battle Screen Model Accessors.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 01, 2026
 * @date Last Modified: Apr 01, 2026
 */
void testDrawUsesBattleScreenModelAccessors();
/**
 * @brief Verifies overlay Drawing Uses Model State Accessors.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 01, 2026
 * @date Last Modified: Apr 01, 2026
 */
void testOverlayDrawingUsesModelStateAccessors();
/**
 * @brief Verifies draw Ships Uses Temporary Facing For Stopped Selected Mover.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: May 09, 2026
 * @date Last Modified: May 09, 2026
 */
void testDrawShipsUsesTemporaryFacingForStoppedSelectedMover();
/**
 * @brief Verifies draw Seeker Missiles Uses Activation And Battle Visibility Rules.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 * SMC-02: updated to also assert that active seekers for the moving player
 * are rendered during PH_SEEKER_ACTIVATION so activations are visible on the board.
 *
 * @author gpt-5.4 (high), claude-sonnet-4-6 (standard)
 * @date Created: May 25, 2026
 * @date Last Modified: May 30, 2026
 */
void testDrawSeekerMissilesUsesActivationAndBattleVisibilityRules();
/**
 * @brief Verifies on Left Up Only Hit Tests And Forwards Hex Click.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 01, 2026
 * @date Last Modified: Apr 01, 2026
 */
void testOnLeftUpOnlyHitTestsAndForwardsHexClick();
/**
 * @brief Verifies activation-phase board clicks route to seeker-activation selection.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.4 (high)
 * @date Created: May 25, 2026
 * @date Last Modified: May 25, 2026
 */
void testOnLeftUpRoutesActivationPhaseThroughSeekerSelection();
/**
 * @brief Verifies header Removes Duplicated Mechanics State.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 01, 2026
 * @date Last Modified: Apr 01, 2026
 */
void testHeaderRemovesDuplicatedMechanicsState();
/**
 * @brief Verifies on Motion Uses Placement Forwarders Only.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 01, 2026
 * @date Last Modified: Apr 01, 2026
 */
void testOnMotionUsesPlacementForwardersOnly();
/**
 * @brief Verifies board constructor loads seeker icon via shared asset resolution.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.4 (high)
 * @date Created: May 25, 2026
 * @date Last Modified: May 25, 2026
 */
void testBoardConstructorLoadsSeekerMissileIconThroughResolveAssetPath();
/**
 * @brief Verifies drawSeekerPaths is gated to PH_MOVE and uses a cyan pen with width 2.
 *
 * SMC-07: seeker movement paths are drawn in cyan (pen width 2) and only during the
 * movement phase, consistent with ship-path behavior but using a distinct color.
 *
 * @author claude-sonnet-4-6 (standard)
 * @date Created: May 30, 2026
 * @date Last Modified: May 30, 2026
 */
void testDrawSeekerPathsIsCalledInMovePhaseWithCyanPen();

/**
 * @brief Verifies drawSeekerMissiles handles PH_ATTACK_FIRE by drawing pending offensive-fire seeker hexes.
 *
 * SMC-05: during PH_ATTACK_FIRE the renderer draws the seeker icon on each in-bounds
 * hex returned by getAllPendingOffensiveFireSeekerHexes() (no rotation), then also
 * draws committed active seekers with heading rotation, then returns early so the
 * normal battle-phase path is skipped.
 *
 * @author claude-sonnet-4-6 (standard)
 * @date Created: May 30, 2026
 * @date Last Modified: May 30, 2026
 */
void testDrawSeekerMissilesPendingOffensiveFireBranch();
};

}

#endif
