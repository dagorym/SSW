/**
 * @file FPlayerTest.h
 * @brief Header file for the FPlayerTest class
 * @author Tom Stephens
 * @date Created: Jul 18, 2009
 */

#ifndef FPlayerTest_H_
#define FPlayerTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "strategic/FPlayer.h"

namespace FrontierTests {
using namespace Frontier;

/**
 * @brief Validates FPlayer construction, fleet/ship-list management, and the
 * P5-3 hardened save/load wire layer.
 *
 * The fixture additionally inherits from Frontier::FPObject (with trivial
 * save()/load() stubs of its own) solely so its test bodies can call the
 * protected fixed-width wire helpers (writeU32/readU32/writeString/write)
 * to hand-craft raw streams for the H3 next-ID and unknown-ship-type
 * regression cases, mirroring the established pattern in FVehicleTest.
 *
 * @author Tom Stephens, Claude Sonnet 5 (medium)
 * @date Created: Jul 18, 2009
 * @date Last Modified: Jul 17, 2026
 */
class FPlayerTest : public CppUnit::TestFixture, public Frontier::FPObject{
	CPPUNIT_TEST_SUITE( FPlayerTest );
	CPPUNIT_TEST( testConstructor );
	CPPUNIT_TEST( testSetName );
	CPPUNIT_TEST( testAddFleet );
	CPPUNIT_TEST( testGetFleetByName );
	CPPUNIT_TEST( testGetFleetByLocation );
	CPPUNIT_TEST( testGetFleetList );
	CPPUNIT_TEST( testRemoveFleet );
	CPPUNIT_TEST( testSetIcon );
	CPPUNIT_TEST( testAddUnattachedShips );
	CPPUNIT_TEST( testAddDestroyedShips );
	CPPUNIT_TEST( testSerialize );
	CPPUNIT_TEST( testDestroyedShipFreedExactlyOnceOnPlayerDestruction );
	CPPUNIT_TEST( testSurvivingFleetShipNotDoubleDeletedByDestroyedList );
	CPPUNIT_TEST( testFullRoundTripPreservesUnattachedFleetsAndDestroyedShips );
	CPPUNIT_TEST( testLoadAdvancesNextIDPastLoadedID );
	CPPUNIT_TEST( testLoadReturnsNonzeroOnUnknownUnattachedShipType );
	CPPUNIT_TEST( testLoadReturnsNonzeroOnUnknownDestroyedShipType );
	CPPUNIT_TEST( testLoadReturnsNonzeroWhenTruncatedInsideOwnScalarRegion );
	CPPUNIT_TEST( testLoadReturnsNonzeroWhenTruncatedInsideCountFields );
	CPPUNIT_TEST_SUITE_END();

private:
	FPlayer *m_p1;

	/// Trivial FPObject::save() stub so the fixture can call the inherited
	/// protected wire helpers; not exercised as real persistence.
	const virtual int save(std::ostream &os) const;
	/// Trivial FPObject::load() stub; see save() above.
	virtual int load(std::istream &is);

public:
	void setUp();
	void tearDown();

	void testConstructor();
	void testSetName();
	void testAddFleet();
	void testGetFleetByName();
	void testGetFleetByLocation();
	void testGetFleetList();
	void testRemoveFleet();
	void testSetIcon();
	void testAddUnattachedShips();
	void testAddDestroyedShips();
	void testSerialize();
	void testDestroyedShipFreedExactlyOnceOnPlayerDestruction();
	void testSurvivingFleetShipNotDoubleDeletedByDestroyedList();

	/**
	 * @brief P5-3 AC: a player with unattached ships, a fleet holding a
	 * ship, and a non-empty destroyed-ship list round-trips through
	 * save()->load() (via std::stringstream) with every collection's
	 * counts/IDs/names/per-ship state restored. m_unattached and m_fleets
	 * are asserted directly via getShipList()/getFleetList(); m_destroyed
	 * has no public accessor (FPlayer is its sole owner), so its
	 * count/ID/name/HP round-trip is proven by re-saving the loaded player
	 * and asserting the produced byte stream is identical to the original
	 * -- the destroyed-ship section of save() is a pure function of the
	 * restored m_destroyed contents, so any count/ID/name/state mismatch
	 * after load() would change the re-saved bytes.
	 *
	 * @author Claude Sonnet 5 (medium)
	 * @date Created: Jul 17, 2026
	 * @date Last Modified: Jul 17, 2026
	 */
	void testFullRoundTripPreservesUnattachedFleetsAndDestroyedShips();

	/**
	 * @brief P5-3 H3: after load() reads a player with a large m_ID, a
	 * subsequently default-constructed FPlayer receives an ID strictly
	 * greater than the loaded ID (the static m_nextID counter is advanced
	 * past any loaded ID).
	 *
	 * @author Claude Sonnet 5 (medium)
	 * @date Created: Jul 17, 2026
	 * @date Last Modified: Jul 17, 2026
	 */
	void testLoadAdvancesNextIDPastLoadedID();

	/**
	 * @brief P5-3: FPlayer::load() returns nonzero (without a NULL
	 * dereference/crash) when the wire stream encodes an unrecognized ship
	 * type tag in the unattached-ship list that createShip(...) cannot
	 * resolve.
	 *
	 * @author Claude Sonnet 5 (medium)
	 * @date Created: Jul 17, 2026
	 * @date Last Modified: Jul 17, 2026
	 */
	void testLoadReturnsNonzeroOnUnknownUnattachedShipType();

	/**
	 * @brief P5-3 F2: FPlayer::load() returns nonzero (without a NULL
	 * dereference/crash) when the wire stream encodes an unrecognized ship
	 * type tag in the destroyed-ship list that createShip(...) cannot
	 * resolve.
	 *
	 * @author Claude Sonnet 5 (medium)
	 * @date Created: Jul 17, 2026
	 * @date Last Modified: Jul 17, 2026
	 */
	void testLoadReturnsNonzeroOnUnknownDestroyedShipType();

	/**
	 * @brief FF2-3 (FR-D): a stream truncated strictly inside
	 * FPlayer::load()'s own scalar region -- after m_ID and m_name are
	 * fully present, but before m_iconName's own length-prefixed record can
	 * be read -- must make load() return nonzero instead of silently
	 * continuing with whatever default-constructed values the untouched
	 * fields (m_iconName and the unattached/fleet/destroyed counts) already
	 * held. Before FF2-3, every one of those reads' return values was
	 * discarded, so this exact truncation point returned 0. Also asserts
	 * that no sub-object list was populated (the abort happens before any
	 * ship/fleet is allocated), so the player remains safe to query and
	 * destruct after a failed load().
	 *
	 * @author Claude Sonnet 5 (medium)
	 * @date Created: Jul 19, 2026
	 * @date Last Modified: Jul 19, 2026
	 */
	void testLoadReturnsNonzeroWhenTruncatedInsideOwnScalarRegion();

	/**
	 * @brief FF2-3 (FR-D): exercises the return-checks on the later
	 * container-level scalar reads specifically -- m_ID, m_name, and
	 * m_iconName are all fully present and readable, but the fleet-count
	 * (fSize) field is truncated mid-way through its own 4 bytes. Before
	 * FF2-3, this truncation point also returned 0 (fSize's discarded read
	 * left the local counter at its zero-initialized default, so the fleet
	 * loop silently ran zero iterations and the destroyed-count read failed
	 * the same way).
	 *
	 * @author Claude Sonnet 5 (medium)
	 * @date Created: Jul 19, 2026
	 * @date Last Modified: Jul 19, 2026
	 */
	void testLoadReturnsNonzeroWhenTruncatedInsideCountFields();
};

}

#endif /* FPlayerTest_H_ */
