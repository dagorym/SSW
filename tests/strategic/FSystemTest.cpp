/**
 * @file FSystemTest.cpp
 * @brief Implementation file for the FSystemTest class
 * @author Tom Stephens
 * @date Created: Jul 17, 2009
 */

#include "FSystemTest.h"
#include <cstdio>
#include <sstream>
#include <string>

namespace FrontierTests {
using namespace Frontier;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FSystemTest );

namespace {
// Appends a 4-byte, explicit little-endian encoding of v to buf. Used to
// hand-craft a raw FSystem::load wire buffer with a deliberately large ID
// (see testLoadAdvancesNextIdPastLoadedID), mirroring the equivalent helper
// in FJumpRouteTest.cpp: FPObject's writeU32/readU32 helpers are protected,
// so there is no public API to make FSystem::load see an arbitrarily large
// ID, and the byte layout is built by hand here instead.
void appendU32LE(std::string &buf, unsigned int v){
	buf.push_back(static_cast<char>(v & 0xFF));
	buf.push_back(static_cast<char>((v >> 8) & 0xFF));
	buf.push_back(static_cast<char>((v >> 16) & 0xFF));
	buf.push_back(static_cast<char>((v >> 24) & 0xFF));
}
}

void FSystemTest::setUp(){
	m_s1 = new FSystem();
}

void FSystemTest::tearDown(){
	delete m_s1;
}

void FSystemTest::testConstructor(){
	CPPUNIT_ASSERT(m_s1->getID() > 0 );

	FSystem s("Tom's Star", 12, 20, 3, 1);
	CPPUNIT_ASSERT(s.getName() == "Tom's Star");
	CPPUNIT_ASSERT(s.getCoord(0) == 12 );
	CPPUNIT_ASSERT(s.getCoord(1) == 20 );
	CPPUNIT_ASSERT(s.getCoord(2) == 3 );
	CPPUNIT_ASSERT(s.getOwner() == 1 );
}

void FSystemTest::testAddPlanet(){
	FPlanet *p = new FPlanet("Stephens's Rest");
	m_s1->addPlanet(p);
	CPPUNIT_ASSERT(m_s1->getPlanet("Stephens's Rest") != NULL);
	CPPUNIT_ASSERT(m_s1->getPlanetList().size() == 1);
	CPPUNIT_ASSERT(m_s1->getPlanet("Joe's Pool Hall") == NULL);
}

void FSystemTest::testAddRemoveFleet(){
	FFleet *f = new FFleet();
	m_s1->addFleet(f);
	FleetList list = m_s1->getFleetList();
	CPPUNIT_ASSERT(list.size() == 1);
	CPPUNIT_ASSERT(list[0]->getID() == f->getID() );
	FFleet *f2 = m_s1->removeFleet(f->getID());
	list = m_s1->getFleetList();
	CPPUNIT_ASSERT(list.size() == 0);
	CPPUNIT_ASSERT( f2->getID() == f->getID());
	f2 = m_s1->removeFleet(f->getID());
	CPPUNIT_ASSERT(f2 == NULL);
	delete f;
}

void FSystemTest::testSerialize(){
	FSystem s("Tom's Star", 12, 20, 3, 1);
	FPlanet *p = new FPlanet("Stephens's Rest");
	s.addPlanet(p);
	FFleet *f = new FFleet();
	s.addFleet(f);

	const char *filename = "FSystemTest.tmp";
	std::remove(filename);
	std::ofstream os(filename,std::ios::binary);
	s.save(os);
	os.close();
	FSystem s2;
	std::ifstream is(filename,std::ios::binary);
	s2.load(is);
	is.close();
	std::remove(filename);

	CPPUNIT_ASSERT(s.getID() == s2.getID());
	CPPUNIT_ASSERT(s.getName() == "Tom's Star");
	CPPUNIT_ASSERT(s.getCoord(0) == s2.getCoord(0));
	CPPUNIT_ASSERT(s.getCoord(1) == s2.getCoord(1));
	CPPUNIT_ASSERT(s.getCoord(2) == s2.getCoord(2));
	CPPUNIT_ASSERT(s.getOwner() == s2.getOwner());
	CPPUNIT_ASSERT(s.getPlanetList().size() == s2.getPlanetList().size());
	CPPUNIT_ASSERT(s2.getPlanet("Stephens's Rest") != NULL);
	CPPUNIT_ASSERT(s2.getFleetList().size() == 0);
}

// P5-4 AC4: after loading a system, a subsequently constructed FSystem must
// receive an ID strictly greater than every ID loaded so far (H3
// non-colliding guard, respecting FSystem's pre-increment m_ID = ++m_nextID
// allocation convention).
//
// This hand-crafts the raw wire buffer FSystem::load consumes (ID, name,
// owner, coords, planet count) with a deliberately large ID (5,000,000),
// mirroring FJumpRouteTest::testLoadAdvancesNextIdPastLoadedID. This is
// required to make the test non-vacuous: a round-trip via the real
// save()/load() path (as the previous version of this test did) constructs
// an intervening default FSystem *before* calling load() on it, and that
// default construction alone advances the shared static m_nextID counter
// past the just-saved ID -- so the final getID() > loadedID assertion would
// hold whether or not the H3 guard in FSystem::load actually fires. Using a
// deliberately large loaded ID (5,000,000) that is far above whatever small
// value the shared m_nextID counter has reached from other constructions in
// this test process makes the assertion depend on the guard actually
// firing. Assumption: this is the only way to inject an arbitrarily large
// ID since FSystem exposes no public ID setter and load() is the only entry
// point that accepts a raw stream (same assumption FJumpRouteTest makes).
//
// Confirmed non-vacuous: temporarily commenting out the H3 guard in
// src/strategic/FSystem.cpp (the `if (m_ID > ...) { m_nextID = ...; }`
// block) and rebuilding causes this test to fail (s3.getID() is not
// greater than 5,000,000 without the guard); restoring the guard makes it
// pass again.
void FSystemTest::testLoadAdvancesNextIdPastLoadedID(){
	// Field order matches FSystem::save exactly: ID (writeU32), name
	// (writeString: writeU32 length + bytes), owner (writeU32), coords
	// (native write<float[3]>, 12 bytes), planet count (writeU32).
	std::string buf;
	appendU32LE(buf, 5000000u); // deliberately large system ID
	appendU32LE(buf, 0u);       // name length = 0 (empty name)
	appendU32LE(buf, 0u);       // owner
	float coords[3] = {0.0f, 0.0f, 0.0f};
	buf.append(reinterpret_cast<const char*>(coords), sizeof(coords));
	appendU32LE(buf, 0u);       // planet count

	std::istringstream is(buf);
	FSystem s2; // freshly-constructed object; load() is called directly on
	            // it next, with no other FSystem constructed in between.
	CPPUNIT_ASSERT(s2.load(is) == 0);
	CPPUNIT_ASSERT(s2.getID() == 5000000u);

	// A freshly constructed FSystem must be allocated an ID strictly greater
	// than the loaded one, proving m_nextID was advanced by load(). No other
	// FSystem is constructed between the load() call above and this check.
	FSystem s3;
	CPPUNIT_ASSERT(s3.getID() > 5000000u);
}

// P5-4 AC5 (supplement): FSystem::save writes the ID as a fixed-width
// little-endian uint32_t via writeU32, not the host's native
// representation. This behaviorally inspects the raw bytes emitted at the
// known ID offset (byte 0, since the ID is the first field written) rather
// than relying on source-text inspection alone.
//
// The ID-field assertion above is byte-identical between the old
// native-representation write<T> path and the new fixed-width writeU32 path
// on hosts where sizeof(int) == 4, so by itself it does not discriminate a
// regression back to the old path. As a width-sensitive supplement, this
// also inspects the planet-count field's raw bytes at its known offset
// (mirroring how FMapTest asserts its system-count bytes), which likewise
// exercises writeU32 and gives the same-host coverage a second, independent
// field to check.
void FSystemTest::testSaveWiresIdLittleEndian(){
	FSystem s("LE Star", 0, 0, 0, 0);
	unsigned int id = s.getID();

	std::stringstream buf;
	CPPUNIT_ASSERT(s.save(buf) == 0);
	std::string data = buf.str();

	CPPUNIT_ASSERT(data.size() >= 4);
	unsigned char b0 = static_cast<unsigned char>(data[0]);
	unsigned char b1 = static_cast<unsigned char>(data[1]);
	unsigned char b2 = static_cast<unsigned char>(data[2]);
	unsigned char b3 = static_cast<unsigned char>(data[3]);

	CPPUNIT_ASSERT(b0 == (id & 0xFF));
	CPPUNIT_ASSERT(b1 == ((id >> 8) & 0xFF));
	CPPUNIT_ASSERT(b2 == ((id >> 16) & 0xFF));
	CPPUNIT_ASSERT(b3 == ((id >> 24) & 0xFF));

	// Planet-count field offset: ID(4) + name length-prefix(4) + name
	// bytes(len) + owner(4) + coords(12 native bytes), immediately followed
	// by the planet-count writeU32 field. s has zero planets, so all four
	// bytes must be zero.
	size_t nameLen = s.getName().size();
	size_t planetCountOffset = 4 + 4 + nameLen + 4 + 12;
	CPPUNIT_ASSERT(data.size() >= planetCountOffset + 4);
	unsigned char pc0 = static_cast<unsigned char>(data[planetCountOffset]);
	unsigned char pc1 = static_cast<unsigned char>(data[planetCountOffset + 1]);
	unsigned char pc2 = static_cast<unsigned char>(data[planetCountOffset + 2]);
	unsigned char pc3 = static_cast<unsigned char>(data[planetCountOffset + 3]);
	CPPUNIT_ASSERT(pc0 == 0);
	CPPUNIT_ASSERT(pc1 == 0);
	CPPUNIT_ASSERT(pc2 == 0);
	CPPUNIT_ASSERT(pc3 == 0);
}

// FSystem::load must propagate a stream failure (nonzero return) instead of
// silently ignoring it. A stream truncated immediately after the owner
// field (before the coordinate and planet-count fields) must cause load()
// to fail rather than leave a half-populated FSystem silently accepted.
void FSystemTest::testLoadReturnsNonzeroOnTruncatedStream(){
	FSystem s("Truncated Star", 4, 5, 6, 2);

	std::stringstream buf;
	CPPUNIT_ASSERT(s.save(buf) == 0);
	std::string full = buf.str();

	// Keep only the ID (4 bytes), name length+bytes, and owner (4 bytes);
	// drop the coordinate and planet-count fields entirely so load() runs
	// out of stream data partway through.
	CPPUNIT_ASSERT(full.size() > 16);
	std::string truncated = full.substr(0, full.size() - 16);

	std::stringstream truncBuf(truncated);
	FSystem s2;
	int rc = s2.load(truncBuf);
	CPPUNIT_ASSERT(rc != 0);
}

}
