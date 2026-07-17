/**
 * @file FSystemTest.cpp
 * @brief Implementation file for the FSystemTest class
 * @author Tom Stephens
 * @date Created: Jul 17, 2009
 */

#include "FSystemTest.h"
#include <cstdio>
#include <sstream>

namespace FrontierTests {
using namespace Frontier;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FSystemTest );

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
// allocation convention). Uses the real save()/load() round trip rather than
// a hand-crafted stream since FSystem exposes no public ID setter.
void FSystemTest::testLoadAdvancesNextIdPastLoadedID(){
	FSystem s("High ID Star", 1, 2, 3, 0);

	std::stringstream buf;
	CPPUNIT_ASSERT(s.save(buf) == 0);

	FSystem s2;
	CPPUNIT_ASSERT(s2.load(buf) == 0);
	unsigned int loadedID = s2.getID();
	CPPUNIT_ASSERT(loadedID == s.getID());

	// A freshly constructed FSystem must be allocated an ID strictly greater
	// than the loaded one, proving m_nextID was advanced by load().
	FSystem s3;
	CPPUNIT_ASSERT(s3.getID() > loadedID);
}

// P5-4 AC5 (supplement): FSystem::save writes the ID as a fixed-width
// little-endian uint32_t via writeU32, not the host's native
// representation. This behaviorally inspects the raw bytes emitted at the
// known ID offset (byte 0, since the ID is the first field written) rather
// than relying on source-text inspection alone.
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
