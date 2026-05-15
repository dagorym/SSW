/**
 * @file FTacticalStationOrbitalMovementTest.h
 * @brief Header file for FBattleBoard station orbital movement tests
 * @author Tom Stephens
 * @date Created: Mar 26, 2026
 */

#ifndef _FTACTICALSTATIONORBITALMOVEMENTTEST_H_
#define _FTACTICALSTATIONORBITALMOVEMENTTEST_H_

#include <cppunit/extensions/HelperMacros.h>

namespace FrontierTests {

/**
 * @brief CppUnit fixture for FTacticalStationOrbitalMovementTest tactical regression coverage.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Mar 26, 2026
 * @date Last Modified: May 08, 2026
 */
class FTacticalStationOrbitalMovementTest : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( FTacticalStationOrbitalMovementTest );
CPPUNIT_TEST( testStationMoveAlreadyCompleteAtPhaseEntry );
CPPUNIT_TEST( testStationHeadingUpdatedAfterOrbit );
CPPUNIT_TEST( testStationSpeedPreservedAfterOrbit );
CPPUNIT_TEST( testStationPlacementRecordsSinglePendingOccupant );
CPPUNIT_TEST( testStationPlacementRelocatesPendingOccupant );
CPPUNIT_TEST( testInvalidStationPlacementDoesNotMutatePendingStation );
CPPUNIT_TEST( testStationOrbitInitializesFromRelocatedPendingHex );
CPPUNIT_TEST( testNonStationHeadingUnchangedByFix );
CPPUNIT_TEST_SUITE_END();

public:
void setUp();
void tearDown();

/**
 * @brief Verifies station Move Already Complete At Phase Entry.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 05, 2026
 * @date Last Modified: Apr 05, 2026
 */
void testStationMoveAlreadyCompleteAtPhaseEntry();
/**
 * @brief Verifies station Heading Updated After Orbit.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Mar 26, 2026
 * @date Last Modified: Mar 26, 2026
 */
void testStationHeadingUpdatedAfterOrbit();
/**
 * @brief Verifies station Speed Preserved After Orbit.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Mar 26, 2026
 * @date Last Modified: Mar 26, 2026
 */
void testStationSpeedPreservedAfterOrbit();
/**
 * @brief Verifies station Placement Records Single Pending Occupant.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: May 08, 2026
 * @date Last Modified: May 08, 2026
 */
void testStationPlacementRecordsSinglePendingOccupant();
/**
 * @brief Verifies station Placement Relocates Pending Occupant.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: May 08, 2026
 * @date Last Modified: May 08, 2026
 */
void testStationPlacementRelocatesPendingOccupant();
/**
 * @brief Verifies invalid Station Placement Does Not Mutate Pending Station.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: May 08, 2026
 * @date Last Modified: May 08, 2026
 */
void testInvalidStationPlacementDoesNotMutatePendingStation();
/**
 * @brief Verifies station Orbit Initializes From Relocated Pending Hex.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: May 08, 2026
 * @date Last Modified: May 08, 2026
 */
void testStationOrbitInitializesFromRelocatedPendingHex();
/**
 * @brief Verifies non Station Heading Unchanged By Fix.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Mar 26, 2026
 * @date Last Modified: Mar 26, 2026
 */
void testNonStationHeadingUnchangedByFix();
};

} // namespace FrontierTests

#endif
