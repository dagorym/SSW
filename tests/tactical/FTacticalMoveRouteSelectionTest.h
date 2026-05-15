/**
 * @file FTacticalMoveRouteSelectionTest.h
 * @brief Header file for FTacticalGame move route selection regression tests
 * @author Tom Stephens
 * @date Created: Apr 04, 2026
 */

#ifndef _FTACTICALMOVEROUTESELECTIONTEST_H_
#define _FTACTICALMOVEROUTESELECTIONTEST_H_

#include <cppunit/extensions/HelperMacros.h>

namespace FrontierTests {

/**
 * @brief CppUnit fixture for FTacticalMoveRouteSelectionTest tactical regression coverage.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 04, 2026
 * @date Last Modified: May 10, 2026
 */
class FTacticalMoveRouteSelectionTest : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( FTacticalMoveRouteSelectionTest );
CPPUNIT_TEST( testFirstHighlightedHexExtendsRouteByOne );
CPPUNIT_TEST( testLaterHighlightedHexBecomesRouteEndpoint );
CPPUNIT_TEST( testSelectingEarlierPathHexTrimsRouteToClickedHex );
CPPUNIT_TEST( testStoppedShipCanChooseAnyAdjacentFacingWithoutDisplacement );
CPPUNIT_TEST( testStoppedShipFacingOnlyMoveCommitsHeadingWithoutChangingHexOrSpeed );
CPPUNIT_TEST( testStoppedShipForwardMoveUsesSelectedFacingForFirstHex );
CPPUNIT_TEST( testStoppedShipPreviewHexClickInfersFacingAndMovesAlongPreviewDirection );
CPPUNIT_TEST( testStoppedShipPreviewFirstMoveStillSupportsTrimAndFollowOnExtension );
CPPUNIT_TEST( testStoppedShipPreviewRoutesExposeLegalStartingFacings );
CPPUNIT_TEST( testStoppedShipPreviewHeadingLookupTracksPreviewHexMembership );
CPPUNIT_TEST( testNonStoppedShipDoesNotAllowFreeAdjacentFacingSelection );
CPPUNIT_TEST( testStoppedShipWithZeroMRCannotFreelyRotate );
CPPUNIT_TEST_SUITE_END();

public:
void setUp();
void tearDown();

/**
 * @brief Verifies first Highlighted Hex Extends Route By One.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 04, 2026
 * @date Last Modified: Apr 04, 2026
 */
void testFirstHighlightedHexExtendsRouteByOne();
/**
 * @brief Verifies later Highlighted Hex Becomes Route Endpoint.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 04, 2026
 * @date Last Modified: Apr 04, 2026
 */
void testLaterHighlightedHexBecomesRouteEndpoint();
/**
 * @brief Verifies selecting Earlier Path Hex Trims Route To Clicked Hex.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 04, 2026
 * @date Last Modified: Apr 04, 2026
 */
void testSelectingEarlierPathHexTrimsRouteToClickedHex();
/**
 * @brief Verifies stopped Ship Can Choose Any Adjacent Facing Without Displacement.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: May 09, 2026
 * @date Last Modified: May 09, 2026
 */
void testStoppedShipCanChooseAnyAdjacentFacingWithoutDisplacement();
/**
 * @brief Verifies stopped Ship Facing Only Move Commits Heading Without Changing Hex Or Speed.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: May 09, 2026
 * @date Last Modified: May 09, 2026
 */
void testStoppedShipFacingOnlyMoveCommitsHeadingWithoutChangingHexOrSpeed();
/**
 * @brief Verifies stopped Ship Forward Move Uses Selected Facing For First Hex.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: May 09, 2026
 * @date Last Modified: May 09, 2026
 */
void testStoppedShipForwardMoveUsesSelectedFacingForFirstHex();
/**
 * @brief Verifies stopped Ship Preview Hex Click Infers Facing And Moves Along Preview Direction.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: May 10, 2026
 * @date Last Modified: May 10, 2026
 */
void testStoppedShipPreviewHexClickInfersFacingAndMovesAlongPreviewDirection();
/**
 * @brief Verifies stopped Ship Preview First Move Still Supports Trim And Follow On Extension.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: May 10, 2026
 * @date Last Modified: May 10, 2026
 */
void testStoppedShipPreviewFirstMoveStillSupportsTrimAndFollowOnExtension();
/**
 * @brief Verifies stopped Ship Preview Routes Expose Legal Starting Facings.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: May 10, 2026
 * @date Last Modified: May 10, 2026
 */
void testStoppedShipPreviewRoutesExposeLegalStartingFacings();
/**
 * @brief Verifies stopped Ship Preview Heading Lookup Tracks Preview Hex Membership.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: May 10, 2026
 * @date Last Modified: May 10, 2026
 */
void testStoppedShipPreviewHeadingLookupTracksPreviewHexMembership();
/**
 * @brief Verifies non Stopped Ship Does Not Allow Free Adjacent Facing Selection.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: May 09, 2026
 * @date Last Modified: May 09, 2026
 */
void testNonStoppedShipDoesNotAllowFreeAdjacentFacingSelection();
/**
 * @brief Verifies stopped Ship With Zero MRCannot Freely Rotate.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: May 09, 2026
 * @date Last Modified: May 09, 2026
 */
void testStoppedShipWithZeroMRCannotFreelyRotate();
};

} // namespace FrontierTests

#endif
