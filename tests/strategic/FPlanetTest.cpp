/**
 * @file FPlanetTest.cpp
 * @brief Implementation file for the FPlanetTest class
 * @author Tom Stephens, Claude Sonnet 5 (medium)
 * @date Created: Jun 24, 2009
 * @date Last Modified: Jul 19, 2026
 */

#include "FPlanetTest.h"
#include "ships/FVehicle.h"
#include <cstdio>
#include <sstream>

namespace FrontierTests {
using namespace Frontier;

namespace {

/**
 * @brief Builds a real FPlanet::save() byte stream from the given planet, so
 * FF2-2 tests can truncate/corrupt specific fields at known offsets computed
 * from the field sizes/lengths chosen by the test itself.
 *
 * @author Claude Sonnet 5 (medium)
 * @date Created: Jul 19, 2026
 * @date Last Modified: Jul 19, 2026
 */
std::string savePlanetBytes(const FPlanet &p){
	std::ostringstream os;
	CPPUNIT_ASSERT_EQUAL(0, p.save(os));
	return os.str();
}

}  // namespace

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FPlanetTest );

void FPlanetTest::setUp(){
	m_p1 = new FPlanet();
}

void FPlanetTest::tearDown(){
	delete m_p1;
}

void FPlanetTest::testConstructor(){
	CPPUNIT_ASSERT(m_p1 != NULL);
	CPPUNIT_ASSERT(m_p1->getName() == "Unknown");
	delete m_p1;
	m_p1 = new FPlanet("New Planet");
	CPPUNIT_ASSERT(m_p1->getName() == "New Planet");
	FPlanet p2;
	CPPUNIT_ASSERT(p2.getName() == "Unknown");
	FPlanet p3("Odie");
	CPPUNIT_ASSERT(p3.getName() == "Odie");
}

void FPlanetTest::testAddDestroyStation(){
	FVehicle *s1 = createShip("ArmedStation");
	m_p1->addStation(s1);
	CPPUNIT_ASSERT( m_p1->getStation() == s1 );
	m_p1->destroyStation();
	CPPUNIT_ASSERT(NULL == m_p1->getStation());
}

void FPlanetTest::testOverwriteStation(){
	FVehicle *s1 = createShip("ArmedStation");
	m_p1->addStation(s1);
	CPPUNIT_ASSERT( m_p1->getStation() == s1 );
	FVehicle *s2 = createShip("Fortress");
	m_p1->addStation(s2);
	CPPUNIT_ASSERT( m_p1->getStation() == s2 );
}

void FPlanetTest::testSerialize(){
	delete m_p1;
	m_p1 = new FPlanet("Odie");
	FVehicle *s = createShip("FortifiedStation");
	m_p1->addStation(s);
	const char *filename = "FPlanetTest.tmp";
	std::remove(filename);
	std::ofstream os(filename,std::ios::binary);
	m_p1->save(os);
	os.close();
	FPlanet p2;
	std::ifstream is(filename,std::ios::binary);
	p2.load(is);
	is.close();
	CPPUNIT_ASSERT( p2.getName() == "Odie" );
	CPPUNIT_ASSERT( p2.getStation()->getType() == "FortifiedStation");
	m_p1->destroyStation();
	std::ofstream os2(filename,std::ios::binary);
	m_p1->save(os2);
	os2.close();
	FPlanet p3;
	std::ifstream is2(filename,std::ios::binary);
	p3.load(is2);
	is2.close();
	std::remove(filename);
	CPPUNIT_ASSERT( p3.getName() == "Odie" );
	CPPUNIT_ASSERT( p3.getStation() == NULL );


}

void FPlanetTest::testLoadReturnsNonzeroOnTruncatedID(){
	FPlanet p("TruncID Planet");
	std::string bytes = savePlanetBytes(p);
	// FPlanet::save() writes m_ID via the native-representation `write<T>`
	// template (sizeof(int) bytes); 2 bytes is not enough for that read to
	// complete.
	CPPUNIT_ASSERT(bytes.size() > 2);
	std::string truncated = bytes.substr(0, 2);

	FPlanet loaded;
	std::istringstream is(truncated);
	int rc = loaded.load(is);

	CPPUNIT_ASSERT(rc != 0);
}

void FPlanetTest::testLoadReturnsNonzeroOnTruncatedName(){
	FPlanet p("TruncName Planet");
	std::string bytes = savePlanetBytes(p);
	size_t idSize = sizeof(int);
	// m_ID is complete but only 2 bytes of m_name's 4-byte length prefix
	// survive, so readString(is,m_name) fails.
	CPPUNIT_ASSERT(bytes.size() > idSize + 2);
	std::string truncated = bytes.substr(0, idSize + 2);

	FPlanet loaded;
	std::istringstream is(truncated);
	int rc = loaded.load(is);

	CPPUNIT_ASSERT(rc != 0);
}

void FPlanetTest::testLoadReturnsNonzeroOnTruncatedStationCount(){
	FPlanet p("TruncCount Planet");
	std::string bytes = savePlanetBytes(p);
	size_t idSize = sizeof(int);
	size_t nameFieldSize = 4 + p.getName().size();
	size_t afterName = idSize + nameFieldSize;
	// m_ID and m_name are complete but only 2 of stationCount's 4 bytes
	// survive, so read(is,stationCount) fails.
	CPPUNIT_ASSERT(bytes.size() > afterName + 2);
	std::string truncated = bytes.substr(0, afterName + 2);

	FPlanet loaded;
	std::istringstream is(truncated);
	int rc = loaded.load(is);

	CPPUNIT_ASSERT(rc != 0);
}

void FPlanetTest::testLoadReturnsNonzeroOnTruncatedStationTypeTag(){
	FPlanet p("TruncType Planet");
	FVehicle *station = createShip("ArmedStation");
	p.addStation(station);
	std::string bytes = savePlanetBytes(p);
	size_t idSize = sizeof(int);
	size_t nameFieldSize = 4 + p.getName().size();
	size_t stationFlagSize = sizeof(int);
	size_t afterFlag = idSize + nameFieldSize + stationFlagSize;
	// m_ID, m_name, and stationCount (nonzero) are complete, but only 2 of
	// the station's type-tag length prefix's 4 bytes survive, so
	// readString(is,type) fails.
	CPPUNIT_ASSERT(bytes.size() > afterFlag + 2);
	std::string truncated = bytes.substr(0, afterFlag + 2);

	FPlanet loaded;
	std::istringstream is(truncated);
	int rc = loaded.load(is);

	CPPUNIT_ASSERT(rc != 0);
}

void FPlanetTest::testLoadReturnsNonzeroOnUnknownStationType(){
	FPlanet p("UnknownType Planet");
	FVehicle *station = createShip("ArmedStation");
	p.addStation(station);
	std::string bytes = savePlanetBytes(p);
	size_t idSize = sizeof(int);
	size_t nameFieldSize = 4 + p.getName().size();
	size_t stationFlagSize = sizeof(int);
	size_t typeLenPrefixPos = idSize + nameFieldSize + stationFlagSize;
	size_t typeStrPos = typeLenPrefixPos + 4;

	// Sanity: the 12 bytes at typeStrPos are exactly "ArmedStation", the
	// type this test's station was created with.
	CPPUNIT_ASSERT(bytes.size() >= typeStrPos + 12);
	CPPUNIT_ASSERT(bytes.compare(typeStrPos, 12, "ArmedStation") == 0);

	// Overwrite just the 12 type-name bytes (leaving the 4-byte length
	// prefix untouched) with a same-length string createShip() does not
	// recognize, so FPlanet::load()'s `m_station = createShip(type)` comes
	// back NULL.
	std::string bytesCopy = bytes;
	const std::string bogusType = "NotAStation1";
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(12), bogusType.size());
	bytesCopy.replace(typeStrPos, bogusType.size(), bogusType);

	FPlanet loaded;
	std::istringstream is(bytesCopy);
	int rc = loaded.load(is);

	// No NULL virtual-call crash (reaching this assertion is part of the
	// proof), a nonzero return, and the station stays NULL rather than
	// dangling or partially constructed.
	CPPUNIT_ASSERT(rc != 0);
	CPPUNIT_ASSERT(loaded.getStation() == NULL);
}

void FPlanetTest::testLoadReturnsNonzeroOnTruncatedStationRecord(){
	FPlanet p("TruncStation Planet");
	FVehicle *station = createShip("ArmedStation");
	p.addStation(station);
	std::string bytes = savePlanetBytes(p);
	size_t idSize = sizeof(int);
	size_t nameFieldSize = 4 + p.getName().size();
	size_t stationFlagSize = sizeof(int);
	size_t typeFieldSize = 4 + 12;  // 4-byte length prefix + "ArmedStation"
	size_t afterType = idSize + nameFieldSize + stationFlagSize + typeFieldSize;
	// The station's type tag is fully present and consumed by
	// createShip(type), but only 2 of the nested station's own fixed-width
	// m_ID (readU32, 4 bytes) survive, so m_station->load(is) fails on its
	// own leading read.
	CPPUNIT_ASSERT(bytes.size() > afterType + 2);
	std::string truncated = bytes.substr(0, afterType + 2);

	FPlanet loaded;
	std::istringstream is(truncated);
	int rc = loaded.load(is);

	CPPUNIT_ASSERT(rc != 0);
}

void FPlanetTest::testLoadValidSaveWithStationReturnsZeroAndPreservesData(){
	FPlanet p("ValidWithStation");
	FVehicle *station = createShip("FortifiedStation");
	p.addStation(station);
	std::string bytes = savePlanetBytes(p);

	FPlanet loaded;
	std::istringstream is(bytes);
	int rc = loaded.load(is);

	CPPUNIT_ASSERT_EQUAL(0, rc);
	CPPUNIT_ASSERT(loaded.getName() == "ValidWithStation");
	CPPUNIT_ASSERT(loaded.getStation() != NULL);
	CPPUNIT_ASSERT(loaded.getStation()->getType() == "FortifiedStation");
}

void FPlanetTest::testLoadValidSaveWithoutStationReturnsZeroAndPreservesData(){
	FPlanet p("ValidNoStation");
	std::string bytes = savePlanetBytes(p);

	FPlanet loaded;
	std::istringstream is(bytes);
	int rc = loaded.load(is);

	CPPUNIT_ASSERT_EQUAL(0, rc);
	CPPUNIT_ASSERT(loaded.getName() == "ValidNoStation");
	CPPUNIT_ASSERT(loaded.getStation() == NULL);
}

}
