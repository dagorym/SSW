/**
 * @file FFleetTest.h
 * @brief Header file for the FFleetTest class
 * @author Tom Stephens
 * @date Created: Jun 24, 2009
 */

#ifndef FFleetTest_H_
#define FFleetTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "strategic/FFleet.h"

namespace FrontierTests {
using namespace Frontier;

/**
 * @brief Validates FFleet construction, movement/jump-state management, and
 * the P5-3 hardened save/load wire layer.
 *
 * The fixture additionally inherits from Frontier::FPObject (with trivial
 * save()/load() stubs of its own) solely so its test bodies can call the
 * protected fixed-width wire helpers (writeU32/readU32/writeString/write) to
 * hand-craft raw streams for the H3 next-ID and unknown-ship-type
 * regression cases, mirroring the established pattern in FVehicleTest.
 *
 * @author Tom Stephens, Claude Sonnet 5 (medium)
 * @date Created: Jun 24, 2009
 * @date Last Modified: Jul 17, 2026
 */
class FFleetTest : public CppUnit::TestFixture, public Frontier::FPObject{
	CPPUNIT_TEST_SUITE( FFleetTest );
	CPPUNIT_TEST( testConstructor );
	CPPUNIT_TEST( testBasicSetters );
	CPPUNIT_TEST( testSetLocation );
	CPPUNIT_TEST( testSetLocationLong );
	CPPUNIT_TEST( testAddRemoveShip );
	CPPUNIT_TEST( testDecTransitTime );
	CPPUNIT_TEST( testGetRJChance );
	CPPUNIT_TEST( testMilitiaFunctions );
	CPPUNIT_TEST( testGetMaxSpeed );
	CPPUNIT_TEST( testCancelJump );
	CPPUNIT_TEST( testSetJumpRoute );
	CPPUNIT_TEST( testSerialize );
	CPPUNIT_TEST( TestCopyConstructor );
	CPPUNIT_TEST( testJumpRouteZeroPreservedAcrossSaveLoad );
	CPPUNIT_TEST( testSerializeRoundTripsMultipleShipsAndFleetState );
	CPPUNIT_TEST( testLoadAdvancesNextIDPastLoadedID );
	CPPUNIT_TEST( testLoadReturnsNonzeroOnUnknownShipType );
	CPPUNIT_TEST( testLoadReturnsNonzeroWhenTruncatedInsideOwnScalarRegion );
	CPPUNIT_TEST( testLoadReturnsNonzeroWhenTruncatedInsideShipCountField );
	CPPUNIT_TEST_SUITE_END();

private:
	FFleet *m_f1,*m_f2;

	/// Trivial FPObject::save() stub so the fixture can call the inherited
	/// protected wire helpers; not exercised as real persistence.
	const virtual int save(std::ostream &os) const;
	/// Trivial FPObject::load() stub; see save() above.
	virtual int load(std::istream &is);

public:
	void setUp();
	void tearDown();

	void testConstructor();
	void testBasicSetters();
	void testSetLocation();
	void testSetLocationLong();
	void testAddRemoveShip();
	void testMilitiaFunctions();
	void testGetRJChance();
	void testDecTransitTime();
	void testGetMaxSpeed();
	void testCancelJump();
	void testSetJumpRoute();
	void testSerialize();
	void TestCopyConstructor();

	/**
	 * @brief P5-3 H4: a fleet saved while in transit on jump route ID 0
	 * reloads with getJumpRoute() == 0 (a valid, distinct route ID), not
	 * normalized to FFleet::NO_ROUTE by any legacy load()-side rewrite.
	 *
	 * @author Claude Sonnet 5 (medium)
	 * @date Created: Jul 17, 2026
	 * @date Last Modified: Jul 17, 2026
	 */
	void testJumpRouteZeroPreservedAcrossSaveLoad();

	/**
	 * @brief P5-3 AC: a fleet holding multiple ships round-trips through
	 * save()->load() (via std::stringstream) with ship count/IDs/names/HP
	 * and fleet ID/name/owner/location/destination/jump-route state
	 * preserved.
	 *
	 * @author Claude Sonnet 5 (medium)
	 * @date Created: Jul 17, 2026
	 * @date Last Modified: Jul 17, 2026
	 */
	void testSerializeRoundTripsMultipleShipsAndFleetState();

	/**
	 * @brief P5-3 H3: after load() reads a fleet with a large m_ID, a
	 * subsequently default-constructed FFleet receives an ID strictly
	 * greater than the loaded ID (the static, signed m_nextID counter is
	 * advanced past any loaded ID).
	 *
	 * @author Claude Sonnet 5 (medium)
	 * @date Created: Jul 17, 2026
	 * @date Last Modified: Jul 17, 2026
	 */
	void testLoadAdvancesNextIDPastLoadedID();

	/**
	 * @brief P5-3: FFleet::load() returns nonzero (without a NULL
	 * dereference/crash) when the wire stream encodes an unrecognized ship
	 * type tag that createShip(...) cannot resolve.
	 *
	 * @author Claude Sonnet 5 (medium)
	 * @date Created: Jul 17, 2026
	 * @date Last Modified: Jul 17, 2026
	 */
	void testLoadReturnsNonzeroOnUnknownShipType();

	/**
	 * @brief FF2-3 (FR-D): a stream truncated strictly inside
	 * FFleet::load()'s own scalar region -- after m_ID and m_name are
	 * fully present, but before the m_owner field can be read -- must make
	 * load() return nonzero instead of silently continuing with whatever
	 * default-constructed values the remaining 16 own-scalar fields and the
	 * ship count already held. Before FF2-3, every one of those reads'
	 * return values was discarded, so this exact truncation point returned
	 * 0. Also asserts no ship was allocated (the abort happens before the
	 * ship loop is ever reached), so the fleet remains safe to query and
	 * destruct after a failed load().
	 *
	 * @author Claude Sonnet 5 (medium)
	 * @date Created: Jul 19, 2026
	 * @date Last Modified: Jul 19, 2026
	 */
	void testLoadReturnsNonzeroWhenTruncatedInsideOwnScalarRegion();

	/**
	 * @brief FF2-3 (FR-D): exercises the return-check on the last
	 * container-level scalar read specifically -- every field up through
	 * m_dy is fully present and readable, but the ship-count (sCount) field
	 * is truncated mid-way through its own 4 bytes. Before FF2-3, sCount's
	 * discarded read left the local counter at its zero-initialized
	 * default, so the ship loop silently ran zero iterations and load()
	 * returned 0.
	 *
	 * @author Claude Sonnet 5 (medium)
	 * @date Created: Jul 19, 2026
	 * @date Last Modified: Jul 19, 2026
	 */
	void testLoadReturnsNonzeroWhenTruncatedInsideShipCountField();
};

}

#endif /* FFleetTest_H_ */
