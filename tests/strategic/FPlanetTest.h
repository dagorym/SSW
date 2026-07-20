/**
 * @file FPlanetTest.h
 * @brief Header file for the FPlanetTest class
 *
 * Also covers FF2-2 (FR-C): behavioral coverage for FPlanet::load()'s
 * hardened return-checking in isolation -- every own read (m_ID, m_name,
 * stationCount, and, when a station is present, its type tag) must make
 * load() return nonzero on failure; an unknown/corrupt station type must
 * make load() return nonzero without a NULL virtual-call crash (m_station
 * stays NULL); a stream truncated inside the nested station's own
 * FVehicle::load() record must make load() return nonzero; and valid saves
 * (with and without a station) must still load unchanged.
 *
 * @author Tom Stephens, Claude Sonnet 5 (medium)
 * @date Created: Jun 24, 2009
 * @date Last Modified: Jul 19, 2026
 */

#ifndef FPlanetTest_H_
#define FPlanetTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "strategic/FPlanet.h"

namespace FrontierTests {
using namespace Frontier;

class FPlanetTest : public CppUnit::TestFixture{
	CPPUNIT_TEST_SUITE( FPlanetTest );
	CPPUNIT_TEST( testConstructor );
	CPPUNIT_TEST( testAddDestroyStation );
	CPPUNIT_TEST( testOverwriteStation );
	CPPUNIT_TEST( testSerialize );
	CPPUNIT_TEST( testLoadReturnsNonzeroOnTruncatedID );
	CPPUNIT_TEST( testLoadReturnsNonzeroOnTruncatedName );
	CPPUNIT_TEST( testLoadReturnsNonzeroOnTruncatedStationCount );
	CPPUNIT_TEST( testLoadReturnsNonzeroOnTruncatedStationTypeTag );
	CPPUNIT_TEST( testLoadReturnsNonzeroOnUnknownStationType );
	CPPUNIT_TEST( testLoadReturnsNonzeroOnTruncatedStationRecord );
	CPPUNIT_TEST( testLoadValidSaveWithStationReturnsZeroAndPreservesData );
	CPPUNIT_TEST( testLoadValidSaveWithoutStationReturnsZeroAndPreservesData );
	CPPUNIT_TEST_SUITE_END();

private:
	FPlanet *m_p1;

public:
	void setUp();
	void tearDown();

	void testConstructor();
	void testAddDestroyStation();
	void testOverwriteStation();
	void testSerialize();

	/// FR-C: a stream that ends before FPlanet::load()'s own leading
	/// read(is,m_ID) can complete must make load() return nonzero.
	void testLoadReturnsNonzeroOnTruncatedID();

	/// FR-C: a stream with a complete m_ID but a truncated m_name field
	/// (readString(is,m_name) fails partway through the length prefix)
	/// must make load() return nonzero.
	void testLoadReturnsNonzeroOnTruncatedName();

	/// FR-C: a stream with a complete m_ID and m_name but a truncated
	/// stationCount field must make load() return nonzero.
	void testLoadReturnsNonzeroOnTruncatedStationCount();

	/// FR-C: a stream with a complete m_ID, m_name, and a nonzero
	/// stationCount, but a truncated station type-tag field, must make
	/// load() return nonzero.
	void testLoadReturnsNonzeroOnTruncatedStationTypeTag();

	/// FR-C: a stream whose station type tag names an unrecognized
	/// createShip() type must make load() return nonzero without a NULL
	/// virtual-call crash, leaving the planet's station NULL.
	void testLoadReturnsNonzeroOnUnknownStationType();

	/// FR-C: a stream with a complete, recognized station type tag but
	/// truncated partway through the nested station's own FVehicle::load()
	/// record must make load() return nonzero.
	void testLoadReturnsNonzeroOnTruncatedStationRecord();

	/// FR-C positive control: a complete, valid save of a planet with a
	/// station must make load() return 0 and preserve the planet's name and
	/// station type unchanged.
	void testLoadValidSaveWithStationReturnsZeroAndPreservesData();

	/// FR-C positive control: a complete, valid save of a planet without a
	/// station must make load() return 0, preserve the planet's name, and
	/// leave the station NULL.
	void testLoadValidSaveWithoutStationReturnsZeroAndPreservesData();
};

}

#endif /* FPlanetTest_H_ */
