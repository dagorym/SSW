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

class FTacticalMoveRouteSelectionTest : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( FTacticalMoveRouteSelectionTest );
CPPUNIT_TEST( testFirstHighlightedHexExtendsRouteByOne );
CPPUNIT_TEST( testLaterHighlightedHexBecomesRouteEndpoint );
CPPUNIT_TEST( testSelectingEarlierPathHexTrimsRouteToClickedHex );
CPPUNIT_TEST( testStoppedShipCanChooseAnyAdjacentFacingWithoutDisplacement );
CPPUNIT_TEST( testStoppedShipFacingOnlyMoveCommitsHeadingWithoutChangingHexOrSpeed );
CPPUNIT_TEST( testStoppedShipForwardMoveUsesSelectedFacingForFirstHex );
CPPUNIT_TEST( testNonStoppedShipDoesNotAllowFreeAdjacentFacingSelection );
CPPUNIT_TEST( testStoppedShipWithZeroMRCannotFreelyRotate );
CPPUNIT_TEST_SUITE_END();

public:
void setUp();
void tearDown();

void testFirstHighlightedHexExtendsRouteByOne();
void testLaterHighlightedHexBecomesRouteEndpoint();
void testSelectingEarlierPathHexTrimsRouteToClickedHex();
void testStoppedShipCanChooseAnyAdjacentFacingWithoutDisplacement();
void testStoppedShipFacingOnlyMoveCommitsHeadingWithoutChangingHexOrSpeed();
void testStoppedShipForwardMoveUsesSelectedFacingForFirstHex();
void testNonStoppedShipDoesNotAllowFreeAdjacentFacingSelection();
void testStoppedShipWithZeroMRCannotFreelyRotate();
};

} // namespace FrontierTests

#endif
