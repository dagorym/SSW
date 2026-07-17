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
};

}

#endif /* FPlayerTest_H_ */
