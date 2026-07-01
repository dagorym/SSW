/**
 * @file FTacticalEndOfMoveTurnTest.h
 * @brief Header file for end-of-move single facing change behavioral tests (TMF-05).
 *
 * Covers canApplyEndOfMoveTurnLeft(), canApplyEndOfMoveTurnRight(), applyEndOfMoveTurn(),
 * finalizeMovementState() pending-turn commit, handleHexClick() pending-turn clear, and
 * speed-0 deceleration extension of the free-rotation gate.
 *
 * @author claude-sonnet-4-6 (medium)
 * @date Created: Jun 30, 2026
 * @date Last Modified: Jun 30, 2026
 */

#ifndef FTACTICALENDOFMOVETURNTEST_H_
#define FTACTICALENDOFMOVETURNTEST_H_

#include <cppunit/extensions/HelperMacros.h>

namespace FrontierTests {

/**
 * @brief CppUnit fixture for FTacticalGame end-of-move single facing change (TMF-05).
 *
 * All tests construct real FTacticalGame instances with real ship/fleet objects and
 * observe runtime results — no source-text inspection is used.
 *
 * @author claude-sonnet-4-6 (medium)
 * @date Created: Jun 30, 2026
 * @date Last Modified: Jun 30, 2026
 */
class FTacticalEndOfMoveTurnTest : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( FTacticalEndOfMoveTurnTest );
CPPUNIT_TEST( testCanApplyTurnBothDirectionsWhenMRAvailableAndMinMoveSatisfied );
CPPUNIT_TEST( testCannotApplyTurnWhenMRIsZero );
CPPUNIT_TEST( testCannotApplyTurnBeforeMinimumMoveIsSatisfied );
CPPUNIT_TEST( testCannotApplyTurnWhenPathEndHexHasMRTurnFlag );
CPPUNIT_TEST( testApplyLeftTurnSetsHeadingImmediatelyForVisualFeedback );
CPPUNIT_TEST( testApplyRightTurnSetsHeadingImmediatelyForVisualFeedback );
CPPUNIT_TEST( testPendingTurnRecordsOriginAndNewFacing );
CPPUNIT_TEST( testOnlyReverseDirectionEnabledAfterLeftTurnApplied );
CPPUNIT_TEST( testOnlyReverseDirectionEnabledAfterRightTurnApplied );
CPPUNIT_TEST( testApplyingReverseWhenLeftPendingRestoresOriginHeading );
CPPUNIT_TEST( testApplyingReverseWhenRightPendingRestoresOriginHeading );
CPPUNIT_TEST( testReverseClearsPendingFacingFields );
CPPUNIT_TEST( testSecondSameDirectionTurnIsBlockedWhenPendingExists );
CPPUNIT_TEST( testFinalizeMovementStateCommitsPendingFacing );
CPPUNIT_TEST( testFinalizeWithNoPendingUsesCurrentHeading );
CPPUNIT_TEST( testHexClickWhilePendingTurnClearsAndRestoresOriginHeading );
CPPUNIT_TEST( testSpeedZeroDecelerationShipCanUseEndOfMoveTurn );
CPPUNIT_TEST_SUITE_END();

public:
void setUp();
void tearDown();

/**
 * @brief Both turn directions are available when MR > 0, min move satisfied, no pending.
 * @author claude-sonnet-4-6 (medium)
 * @date Created: Jun 30, 2026
 * @date Last Modified: Jun 30, 2026
 */
void testCanApplyTurnBothDirectionsWhenMRAvailableAndMinMoveSatisfied();

/**
 * @brief Turn is blocked when ship's current MR is 0.
 * @author claude-sonnet-4-6 (medium)
 * @date Created: Jun 30, 2026
 * @date Last Modified: Jun 30, 2026
 */
void testCannotApplyTurnWhenMRIsZero();

/**
 * @brief Turn is blocked when ship has not satisfied its minimum move.
 * @author claude-sonnet-4-6 (medium)
 * @date Created: Jun 30, 2026
 * @date Last Modified: Jun 30, 2026
 */
void testCannotApplyTurnBeforeMinimumMoveIsSatisfied();

/**
 * @brief Turn is blocked when the path end hex already has an MR_TURN flag.
 * @author claude-sonnet-4-6 (medium)
 * @date Created: Jun 30, 2026
 * @date Last Modified: Jun 30, 2026
 */
void testCannotApplyTurnWhenPathEndHexHasMRTurnFlag();

/**
 * @brief applyEndOfMoveTurn(+1) updates ship heading immediately for visual feedback.
 * @author claude-sonnet-4-6 (medium)
 * @date Created: Jun 30, 2026
 * @date Last Modified: Jun 30, 2026
 */
void testApplyLeftTurnSetsHeadingImmediatelyForVisualFeedback();

/**
 * @brief applyEndOfMoveTurn(-1) updates ship heading immediately for visual feedback.
 * @author claude-sonnet-4-6 (medium)
 * @date Created: Jun 30, 2026
 * @date Last Modified: Jun 30, 2026
 */
void testApplyRightTurnSetsHeadingImmediatelyForVisualFeedback();

/**
 * @brief pendingEndOfMoveFacing and endOfMoveOriginFacing are correctly set after a turn.
 * @author claude-sonnet-4-6 (medium)
 * @date Created: Jun 30, 2026
 * @date Last Modified: Jun 30, 2026
 */
void testPendingTurnRecordsOriginAndNewFacing();

/**
 * @brief After a left turn is pending, only the reverse direction (right) is enabled.
 * @author claude-sonnet-4-6 (medium)
 * @date Created: Jun 30, 2026
 * @date Last Modified: Jun 30, 2026
 */
void testOnlyReverseDirectionEnabledAfterLeftTurnApplied();

/**
 * @brief After a right turn is pending, only the reverse direction (left) is enabled.
 * @author claude-sonnet-4-6 (medium)
 * @date Created: Jun 30, 2026
 * @date Last Modified: Jun 30, 2026
 */
void testOnlyReverseDirectionEnabledAfterRightTurnApplied();

/**
 * @brief Applying the reverse direction after a left turn restores the origin heading.
 * @author claude-sonnet-4-6 (medium)
 * @date Created: Jun 30, 2026
 * @date Last Modified: Jun 30, 2026
 */
void testApplyingReverseWhenLeftPendingRestoresOriginHeading();

/**
 * @brief Applying the reverse direction after a right turn restores the origin heading.
 * @author claude-sonnet-4-6 (medium)
 * @date Created: Jun 30, 2026
 * @date Last Modified: Jun 30, 2026
 */
void testApplyingReverseWhenRightPendingRestoresOriginHeading();

/**
 * @brief Reversing a pending turn clears both pendingEndOfMoveFacing and endOfMoveOriginFacing.
 * @author claude-sonnet-4-6 (medium)
 * @date Created: Jun 30, 2026
 * @date Last Modified: Jun 30, 2026
 */
void testReverseClearsPendingFacingFields();

/**
 * @brief A second turn in the same direction is blocked while a pending turn exists.
 * @author claude-sonnet-4-6 (medium)
 * @date Created: Jun 30, 2026
 * @date Last Modified: Jun 30, 2026
 */
void testSecondSameDirectionTurnIsBlockedWhenPendingExists();

/**
 * @brief finalizeMovementState() commits pendingEndOfMoveFacing to curHeading and finalHeading.
 * @author claude-sonnet-4-6 (medium)
 * @date Created: Jun 30, 2026
 * @date Last Modified: Jun 30, 2026
 */
void testFinalizeMovementStateCommitsPendingFacing();

/**
 * @brief finalizeMovementState() without pending uses curHeading unchanged.
 * @author claude-sonnet-4-6 (medium)
 * @date Created: Jun 30, 2026
 * @date Last Modified: Jun 30, 2026
 */
void testFinalizeWithNoPendingUsesCurrentHeading();

/**
 * @brief Clicking a movement hex while a pending turn is active clears the pending turn and
 *        restores the original heading before processing the hex.
 * @author claude-sonnet-4-6 (medium)
 * @date Created: Jun 30, 2026
 * @date Last Modified: Jun 30, 2026
 */
void testHexClickWhilePendingTurnClearsAndRestoresOriginHeading();

/**
 * @brief A ship that decelerated to speed 0 this turn can still use end-of-move turn.
 * @author claude-sonnet-4-6 (medium)
 * @date Created: Jun 30, 2026
 * @date Last Modified: Jun 30, 2026
 */
void testSpeedZeroDecelerationShipCanUseEndOfMoveTurn();
};

} // namespace FrontierTests

#endif // FTACTICALENDOFMOVETURNTEST_H_
