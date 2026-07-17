/**
 * @file FJumpRouteTest.cpp
 * @brief Implementation file for the FJumpRouteTest class
 * @author Tom Stephens
 * @date Created: DATE
 */

#include "FJumpRouteTest.h"
#include <cstdio>
#include <sstream>
#include <string>
#include "strategic/FSystem.h"

namespace FrontierTests {
using namespace Frontier;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FJumpRouteTest );

namespace {
// Appends a 4-byte, explicit little-endian encoding of v to buf. Used to
// hand-craft raw FJumpRoute::load wire buffers for cases (system IDs above
// 65535, deliberately truncated streams) that have no public API path to
// produce that wire state, per P5-4 tester guidance: FPObject's
// writeU32/readU32 helpers are protected, so the byte layout is built by
// hand here rather than reaching into FPObject's protected members.
void appendU32LE(std::string &buf, unsigned int v){
	buf.push_back(static_cast<char>(v & 0xFF));
	buf.push_back(static_cast<char>((v >> 8) & 0xFF));
	buf.push_back(static_cast<char>((v >> 16) & 0xFF));
	buf.push_back(static_cast<char>((v >> 24) & 0xFF));
}
}

void FJumpRouteTest::setUp(){
	m_j1 = new FJumpRoute();
}

void FJumpRouteTest::tearDown(){
	delete m_j1;
}

void FJumpRouteTest::testConstructor(){
	CPPUNIT_ASSERT(m_j1->getStart() == NULL);
	CPPUNIT_ASSERT(m_j1->getEnd() == NULL);
	CPPUNIT_ASSERT(m_j1->getID() == 0);
	CPPUNIT_ASSERT(m_j1->getLength() == 0);
	CPPUNIT_ASSERT(m_j1->isKnown(0) == false);
}

void FJumpRouteTest::testFullConstructor(){
	FSystem *s = new FSystem("a",2,0,0,0);
	FSystem *e = new FSystem("b",0,0,0,0);
	std::vector<unsigned int> p;
	p.push_back(0);
	FJumpRoute j(s,e,p);
	CPPUNIT_ASSERT(j.getStart()->getID() == s->getID());
	CPPUNIT_ASSERT(j.getEnd()->getID() == e->getID());
	CPPUNIT_ASSERT(j.getID() == 0);
	CPPUNIT_ASSERT(j.getLength() == 2);
	CPPUNIT_ASSERT(j.isKnown(0) == true);
	delete s;
	delete e;
}

void FJumpRouteTest::testGetJumpTimeSpeed0(){
	CPPUNIT_ASSERT(m_j1->getJumpTime(0,10) == 10);
	CPPUNIT_ASSERT(m_j1->getJumpTime(4,10) == 10);
}

void FJumpRouteTest::testGetJumpTimeSpeed1(){
	CPPUNIT_ASSERT(m_j1->getJumpTime(1,10) == 10);
	FSystem *s = new FSystem("a",2,0,0,0);
	FSystem *e = new FSystem("b",0,0,0,0);
	std::vector<unsigned int> p;
	p.push_back(0);
	FJumpRoute j(s,e,p);
	CPPUNIT_ASSERT(j.getJumpTime(1) == 2);
	delete s;
	delete e;
}

void FJumpRouteTest::testGetJumpTimeSpeed2(){
	CPPUNIT_ASSERT(m_j1->getJumpTime(2,10) == 6);
	CPPUNIT_ASSERT(m_j1->getJumpTime(2,5) == 3);
	CPPUNIT_ASSERT(m_j1->getJumpTime(2,8) == 4);
}

void FJumpRouteTest::testGetJumpTimeSpeed3(){
	CPPUNIT_ASSERT(m_j1->getJumpTime(3,5) == 2);
	CPPUNIT_ASSERT(m_j1->getJumpTime(3,6) == 2);
	CPPUNIT_ASSERT(m_j1->getJumpTime(3,7) == 3);
	CPPUNIT_ASSERT(m_j1->getJumpTime(3,8) == 4);
	CPPUNIT_ASSERT(m_j1->getJumpTime(3,10) == 4);
}

void FJumpRouteTest::testSerialize(){
	FSystem *s = new FSystem("a",2,0,0,0);
	FSystem *e = new FSystem("b",0,0,0,0);
	std::vector<unsigned int> p;
	p.push_back(1);
	FJumpRoute j(s,e,p);
	const char *filename = "FJumpRouteTest.tmp";
	std::remove(filename);
	std::ofstream os(filename,std::ios::binary);
	j.save(os);
	os.close();
	FJumpRoute j2;
	std::ifstream is(filename,std::ios::binary);
	j2.load(is);
	is.close();
	std::remove(filename);
	CPPUNIT_ASSERT(j2.getID() == 0);
	CPPUNIT_ASSERT(j2.getLength() == 2);
	CPPUNIT_ASSERT(j2.isKnown(0) == false);
	CPPUNIT_ASSERT(j2.isKnown(1) == true);
	delete s;
	delete e;
}

// P5-4 AC2 (part 1): FJumpRoute::load must decode the start/end system ID
// fields as real fixed-width uint32_t values with no 16-bit truncation.
// FSystem's ID counter is a simple incrementing static int, so reaching an
// ID above 65535 naturally would require constructing >65536 FSystem
// instances; instead this hand-crafts the raw wire buffer FJumpRoute::load
// consumes (ID, length, startID, endID, player count) with start/end ID
// fields encoding values above 65535 (0x00010001 = 65537 and
// 0x00020001 = 131073) and calls load() directly. Assumption: this is the
// only way to inject an out-of-range ID since FJumpRoute exposes no public
// ID setter and load() is the only entry point that accepts a raw stream.
void FJumpRouteTest::testLoadPreservesSystemIdAbove65535(){
	std::string buf;
	appendU32LE(buf, 42u); // jump route ID
	float length = 3.5f;
	buf.append(reinterpret_cast<const char*>(&length), sizeof(float));
	appendU32LE(buf, 65537u);  // start system ID > 65535 (0x00010001)
	appendU32LE(buf, 131073u); // end system ID > 65535 (0x00020001)
	appendU32LE(buf, 0u);      // player count

	std::istringstream is(buf);
	FJumpRoute j;
	CPPUNIT_ASSERT(j.load(is) == 0);

	// No masking to a 16-bit remainder: the values must round-trip exactly.
	CPPUNIT_ASSERT(j.getStartSystemID() == 65537u);
	CPPUNIT_ASSERT(j.getEndSystemID() == 131073u);
	// load() leaves the pointers NULL; only FMap::load resolves them.
	CPPUNIT_ASSERT(j.getStart() == NULL);
	CPPUNIT_ASSERT(j.getEnd() == NULL);
}

// P5-4 AC4: after loading a jump route, a subsequently constructed
// FJumpRoute must receive an ID strictly greater than every ID loaded so
// far (H3 non-colliding guard, respecting FJumpRoute's post-increment
// m_ID = m_nextID++ allocation convention, which differs from FSystem's
// pre-increment rule and is covered directly here rather than only via
// FMap). Uses a hand-crafted stream with a very large ID so the assertion
// holds regardless of how many jump routes earlier tests in this process
// have already allocated.
void FJumpRouteTest::testLoadAdvancesNextIdPastLoadedID(){
	std::string buf;
	appendU32LE(buf, 5000000u); // deliberately large jump route ID
	float length = 1.0f;
	buf.append(reinterpret_cast<const char*>(&length), sizeof(float));
	appendU32LE(buf, 1u);
	appendU32LE(buf, 2u);
	appendU32LE(buf, 0u);

	std::istringstream is(buf);
	FJumpRoute j;
	CPPUNIT_ASSERT(j.load(is) == 0);
	CPPUNIT_ASSERT(j.getID() == 5000000u);

	FSystem *s1 = new FSystem("Counter-X", 0, 0, 0, 0);
	FSystem *s2 = new FSystem("Counter-Y", 1, 0, 0, 0);
	std::vector<unsigned int> p;
	FJumpRoute j2(s1, s2, p);
	CPPUNIT_ASSERT(j2.getID() > 5000000u);
	delete s1;
	delete s2;
}

// P5-4 AC5 (supplement): FJumpRoute::save writes the start/end system IDs
// as fixed-width little-endian uint32_t fields via writeU32, not the host's
// native representation. This behaviorally inspects the raw bytes emitted
// at the known field offsets (bytes 8-11 for start, 12-15 for end, after
// the 4-byte ID and 4-byte native-float length fields) rather than relying
// on source-text inspection alone.
void FJumpRouteTest::testSaveWiresStartEndIdsLittleEndian(){
	FSystem *s = new FSystem("a", 2, 0, 0, 0);
	FSystem *e = new FSystem("b", 0, 0, 0, 0);
	std::vector<unsigned int> p;
	FJumpRoute j(s, e, p);

	std::ostringstream os;
	CPPUNIT_ASSERT(j.save(os) == 0);
	std::string data = os.str();

	unsigned int startID = s->getID();
	unsigned int endID = e->getID();
	CPPUNIT_ASSERT(data.size() >= 16);

	unsigned char sb0 = static_cast<unsigned char>(data[8]);
	unsigned char sb1 = static_cast<unsigned char>(data[9]);
	unsigned char sb2 = static_cast<unsigned char>(data[10]);
	unsigned char sb3 = static_cast<unsigned char>(data[11]);
	CPPUNIT_ASSERT(sb0 == (startID & 0xFF));
	CPPUNIT_ASSERT(sb1 == ((startID >> 8) & 0xFF));
	CPPUNIT_ASSERT(sb2 == ((startID >> 16) & 0xFF));
	CPPUNIT_ASSERT(sb3 == ((startID >> 24) & 0xFF));

	unsigned char eb0 = static_cast<unsigned char>(data[12]);
	unsigned char eb1 = static_cast<unsigned char>(data[13]);
	unsigned char eb2 = static_cast<unsigned char>(data[14]);
	unsigned char eb3 = static_cast<unsigned char>(data[15]);
	CPPUNIT_ASSERT(eb0 == (endID & 0xFF));
	CPPUNIT_ASSERT(eb1 == ((endID >> 8) & 0xFF));
	CPPUNIT_ASSERT(eb2 == ((endID >> 16) & 0xFF));
	CPPUNIT_ASSERT(eb3 == ((endID >> 24) & 0xFF));

	delete s;
	delete e;
}

// FJumpRoute::load must propagate a stream failure (nonzero return) instead
// of silently ignoring it. A stream truncated after the start system ID
// field (before the end system ID and player-count fields) must cause
// load() to fail.
void FJumpRouteTest::testLoadReturnsNonzeroOnTruncatedStream(){
	std::string buf;
	appendU32LE(buf, 7u);
	float length = 2.0f;
	buf.append(reinterpret_cast<const char*>(&length), sizeof(float));
	appendU32LE(buf, 1u); // start system ID only; end ID and player count omitted

	std::istringstream is(buf);
	FJumpRoute j;
	int rc = j.load(is);
	CPPUNIT_ASSERT(rc != 0);
}

}
