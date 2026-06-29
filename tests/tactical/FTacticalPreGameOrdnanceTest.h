/**
 * @file FTacticalPreGameOrdnanceTest.h
 * @brief Header file for PGS-04 pre-game ordnance behavioral tests.
 *
 * Covers:
 *   (a) Undeploy-one from stacked (hex, source) returns ammo and decrements count.
 *   (b) Mine removal with non-placing source selected still removes mine and
 *       restores ammo to the placing ship.
 *
 * @author claude-sonnet-4-6 (medium)
 * @date Created: Jun 22, 2026
 * @date Last Modified: Jun 22, 2026
 */

#ifndef FTACTICALPREGAMEORDNANCETEST_H_
#define FTACTICALPREGAMEORDNANCETEST_H_

#include <cppunit/extensions/HelperMacros.h>

namespace FrontierTests {

/**
 * @brief CppUnit fixture for PGS-04 pre-game ordnance behavioral regression coverage.
 *
 * Tests that:
 *  - getPlacedSeekerHexGroups() groups inactive seekers by (hex, source) and returns
 *    correct counts during BS_PlaceSeekers, and returns empty outside that state.
 *  - recallPlacedSeekerAtHexSource() removes exactly one seeker from a stacked
 *    (hex, source) group, erases the corresponding placed-ordnance record, restores
 *    one ammo round to the placing ship, and leaves remaining seekers in place.
 *  - During BS_PlaceMines, clicking a mined hex removes the mine and restores ammo
 *    to the placing ship regardless of which source is currently selected.
 *
 * @author claude-sonnet-4-6 (medium)
 * @date Created: Jun 22, 2026
 * @date Last Modified: Jun 22, 2026
 */
class FTacticalPreGameOrdnanceTest : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( FTacticalPreGameOrdnanceTest );
CPPUNIT_TEST( testGetPlacedSeekerHexGroupsReturnsEmptyOutsidePlaceSeekers );
CPPUNIT_TEST( testGetPlacedSeekerHexGroupsGroupsByHexAndSource );
CPPUNIT_TEST( testRecallPlacedSeekerDecrementsCountAndRestoresAmmo );
CPPUNIT_TEST( testRecallFromStackedSeekerLeavesRemainingSeekersInPlace );
CPPUNIT_TEST( testRecallReturnsFalseWhenNothingToRecall );
CPPUNIT_TEST( testMineRemovalWithNonPlacingSourceSelectedRestoresAmmoToPlacingShip );
CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

    /**
     * @brief getPlacedSeekerHexGroups() must return empty vector outside BS_PlaceSeekers.
     *
     * Verifies that the accessor returns an empty result during BS_PlaceMines and
     * other non-seeker states, so the centered undeploy list never appears during
     * the mine phase.
     *
     * @author claude-sonnet-4-6 (medium)
     * @date Created: Jun 22, 2026
     * @date Last Modified: Jun 22, 2026
     */
    void testGetPlacedSeekerHexGroupsReturnsEmptyOutsidePlaceSeekers();

    /**
     * @brief getPlacedSeekerHexGroups() must group stacked inactive seekers by (hex, source).
     *
     * Places three seekers at the same hex from the same source and asserts that
     * getPlacedSeekerHexGroups() returns exactly one group with count == 3.
     * Then places one seeker at a different hex and asserts two groups are returned.
     *
     * @author claude-sonnet-4-6 (medium)
     * @date Created: Jun 22, 2026
     * @date Last Modified: Jun 22, 2026
     */
    void testGetPlacedSeekerHexGroupsGroupsByHexAndSource();

    /**
     * @brief recallPlacedSeekerAtHexSource() removes one seeker and restores one ammo round.
     *
     * AC (a): Places two seekers at the same hex from the same source, then calls
     * recallPlacedSeekerAtHexSource() once and asserts that:
     *  - the call returns true
     *  - getSeekerMissiles().size() == 1 (one seeker remains)
     *  - seeker launcher ammo is restored by exactly one round
     *  - getPlacedOrdnance() SM-type count is decremented by exactly one
     *
     * @author claude-sonnet-4-6 (medium)
     * @date Created: Jun 22, 2026
     * @date Last Modified: Jun 22, 2026
     */
    void testRecallPlacedSeekerDecrementsCountAndRestoresAmmo();

    /**
     * @brief recallPlacedSeekerAtHexSource() with stacked seekers leaves remaining seekers in place.
     *
     * Places three seekers at the same hex from the same source, recalls one, and
     * asserts that the remaining two seekers are still present and that
     * getPlacedSeekerHexGroups() reports count == 2 for that group.
     *
     * @author claude-sonnet-4-6 (medium)
     * @date Created: Jun 22, 2026
     * @date Last Modified: Jun 22, 2026
     */
    void testRecallFromStackedSeekerLeavesRemainingSeekersInPlace();

    /**
     * @brief recallPlacedSeekerAtHexSource() returns false when no matching seeker exists.
     *
     * Verifies that a recall attempt on a hex with no placed seekers returns false
     * and does not change seeker count or ammo.
     *
     * @author claude-sonnet-4-6 (medium)
     * @date Created: Jun 22, 2026
     * @date Last Modified: Jun 22, 2026
     */
    void testRecallReturnsFalseWhenNothingToRecall();

    /**
     * @brief Mine removal with non-placing source selected restores ammo to placing ship.
     *
     * AC (b): Sets up two minelayer ships. Ship A places a mine while selected.
     * Then Ship B is selected as the current source. Clicking the mined hex must:
     *  - remove the mine (getMinedHexes() no longer contains that hex)
     *  - restore ammo to Ship A's mine launcher (the placing ship), not Ship B
     *  - Ship B's mine launcher ammo must be unchanged
     *  - getPlacedOrdnance() must have the mine record removed
     *
     * @author claude-sonnet-4-6 (medium)
     * @date Created: Jun 22, 2026
     * @date Last Modified: Jun 22, 2026
     */
    void testMineRemovalWithNonPlacingSourceSelectedRestoresAmmoToPlacingShip();
};

}

#endif /* FTACTICALPREGAMEORDNANCETEST_H_ */
