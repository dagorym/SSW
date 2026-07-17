/**
 * @file FMapTest.cpp
 * @brief Implementation file for the FMapTest class
 * @author Tom Stephens
 * @date Created: Jul 18, 2009
 */

#include "FMapTest.h"
#include <cstdio>
#include <sstream>
#include <string>
#include <map>

namespace FrontierTests {
using namespace Frontier;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FMapTest );

namespace {
// Byte-packing helpers used to hand-craft raw FMap/FSystem/FJumpRoute wire
// buffers for cases (system IDs above 65535, an unknown system-ID
// reference) that have no public API path to produce that wire state.
// FPObject's writeU32/readU32 are protected, so these helpers build the
// little-endian byte layout by hand instead of reaching into FPObject's
// protected members, per P5-4 tester guidance.

void appendU32LE(std::string &buf, unsigned int v){
	buf.push_back(static_cast<char>(v & 0xFF));
	buf.push_back(static_cast<char>((v >> 8) & 0xFF));
	buf.push_back(static_cast<char>((v >> 16) & 0xFF));
	buf.push_back(static_cast<char>((v >> 24) & 0xFF));
}

void appendString(std::string &buf, const std::string &s){
	appendU32LE(buf, static_cast<unsigned int>(s.size()));
	buf += s;
}

// Builds a wire-format FSystem record with 0 planets: ID, name, owner,
// coords[3] (native float layout, matching FPObject::write<T>), planet
// count (0). Mirrors FSystem::save's field order exactly.
void appendSystem(std::string &buf, unsigned int id, const std::string &name,
                   unsigned int owner, float x, float y, float z){
	appendU32LE(buf, id);
	appendString(buf, name);
	appendU32LE(buf, owner);
	float coords[3] = { x, y, z };
	buf.append(reinterpret_cast<const char*>(coords), sizeof(coords));
	appendU32LE(buf, 0u); // planet count
}

// Builds a wire-format FJumpRoute record with 0 known players: ID, length
// (native float), start system ID, end system ID, player count (0).
// Mirrors FJumpRoute::save's field order exactly.
void appendJumpRoute(std::string &buf, unsigned int id, float length,
                      unsigned int startID, unsigned int endID){
	appendU32LE(buf, id);
	buf.append(reinterpret_cast<const char*>(&length), sizeof(float));
	appendU32LE(buf, startID);
	appendU32LE(buf, endID);
	appendU32LE(buf, 0u); // player count
}
}

void FMapTest::setUp(){
	m_pList.push_back(0);
	m_pList.push_back(1);
}

void FMapTest::tearDown(){
}

void FMapTest::testConstructor(){
	FMap *m = &(FMap::create());
	CPPUNIT_ASSERT( m != NULL);
	delete m;

	m = &(FMap::create(true,m_pList));
	CPPUNIT_ASSERT( m != NULL);
	delete m;
}

void FMapTest::testCreateFrontier(){
	FMap *m = &(FMap::create(false,m_pList));
	CPPUNIT_ASSERT( m!=NULL );
	delete m;
	m = &(FMap::create(false,m_pList));
}

void FMapTest::testGetSystemByName(){
	FMap &m = FMap::getMap();
	CPPUNIT_ASSERT(m.getSystem("Prenglar") != NULL);
	CPPUNIT_ASSERT(m.getSystem("Joe's Pool Hall") == NULL);
}

void FMapTest::testGetSystemByID(){
	FMap &m = FMap::getMap();
	unsigned int id = m.getSystem("Prenglar")->getID();
	FSystem *s = m.getSystem(id);
	CPPUNIT_ASSERT(s->getName() == "Prenglar");
	CPPUNIT_ASSERT(m.getSystem(9999) == NULL);
}

void FMapTest::getJumpRouteBySystems(){
	FMap &m = FMap::getMap();
	CPPUNIT_ASSERT(m.getJumpRoute("Prenglar","Cassidine") != NULL );
	CPPUNIT_ASSERT(m.getJumpRoute("Prenglar","White Light") == NULL );
}

void FMapTest::getJumpRouteByID(){
	FMap &m = FMap::getMap();
	FJumpRoute *j = m.getJumpRoute("Prenglar","Cassidine");
	unsigned int id = j->getID();
	CPPUNIT_ASSERT(m.getJumpRoute(id)->getStart()->getName() == "Prenglar");
	CPPUNIT_ASSERT(m.getJumpRoute(id)->getEnd()->getName() == "Cassidine");
	CPPUNIT_ASSERT(m.getJumpRoute(22222) == NULL);
}

void FMapTest::testGetConnectedSystems(){
	FFleet f;
	f.setOwner(0);
	FMap &m = FMap::getMap();
	std::vector<std::string> sList = m.getConnectedSystems("Cassidine",0,&f);
	CPPUNIT_ASSERT( sList.size() == 3 );
	f.setMilitia(true,"Timeon");
	sList = m.getConnectedSystems("Prenglar",0,&f);
	CPPUNIT_ASSERT( sList.size() == 1 );
	f.setMilitia(true,"Dramune");
	sList = m.getConnectedSystems("Dramune",0,&f);
	CPPUNIT_ASSERT( sList.size() == 2 );
}

void FMapTest::testSelectSystemValid(){
	FMap &m = FMap::getMap();
	unsigned int id = m.getSystem("Prenglar")->getID();
	FSystem *s = m.getSystem(id);
	double x = s->getCoord(0);
	double y = s->getCoord(1);
	FSystem *s2 = m.selectSystem(x,y);
	CPPUNIT_ASSERT(s2->getName() == s->getName());
	x+=0.6;
	s2 = m.selectSystem(x,y);
	CPPUNIT_ASSERT(s2->getName() == s->getName());
}

void FMapTest::testSelectSystemFail(){
	FMap &m = FMap::getMap();
	FSystem *s2 = m.selectSystem(9999,9999);
	CPPUNIT_ASSERT(s2 == NULL);
	unsigned int id = m.getSystem("Prenglar")->getID();
	FSystem *s = m.getSystem(id);
	double x = s->getCoord(0)+0.71;
	double y = s->getCoord(1);
	s2 = m.selectSystem(x,y);
	CPPUNIT_ASSERT(s2 == NULL);
}

void FMapTest::testSerialize(){
	FMap *m = &(FMap::getMap());
	const char *filename = "FMapTest.tmp";
	std::remove(filename);
	std::ofstream os(filename,std::ios::binary);
	m->save(os);
	os.close();
	delete m;
	m = &(FMap::create());
	std::ifstream is(filename,std::ios::binary);
	m->load(is);
	is.close();
	std::remove(filename);
	CPPUNIT_ASSERT(m->getSystem("White Light") != NULL);
	FJumpRoute *j = m->getJumpRoute("Prenglar","Cassidine");
	CPPUNIT_ASSERT(j != NULL);
	CPPUNIT_ASSERT(j->getStart() != NULL);
	CPPUNIT_ASSERT(j->getEnd() != NULL);
	CPPUNIT_ASSERT(j->getStart()->getName() == "Prenglar");
	CPPUNIT_ASSERT(j->getEnd()->getName() == "Cassidine");
	}

// P5-4 AC1: a map with multiple systems and jump routes must round-trip
// through save->load with every jump route's start/end pointers correctly
// re-resolved to the matching FSystem, verified by name AND ID (not just
// non-NULL) for every jump route in the map, not just one.
void FMapTest::testSerializeRoundTripAllJumpRoutesResolveByNameAndID(){
	FMap &existing = FMap::getMap();
	delete &existing;
	FMap *m = &(FMap::create(false, m_pList));

	std::map<std::string, unsigned int> nameToID;
	const SystemList &origSystems = m->getSystemList();
	for (unsigned int i = 0; i < origSystems.size(); i++){
		nameToID[origSystems[i]->getName()] = origSystems[i]->getID();
	}

	struct RouteNames { std::string start; std::string end; };
	std::vector<RouteNames> origRoutes;
	const JumpRouteList &origJumps = m->getJumpList();
	for (unsigned int i = 0; i < origJumps.size(); i++){
		RouteNames rn;
		rn.start = origJumps[i]->getStart()->getName();
		rn.end = origJumps[i]->getEnd()->getName();
		origRoutes.push_back(rn);
	}
	CPPUNIT_ASSERT(origSystems.size() > 1);
	CPPUNIT_ASSERT(origJumps.size() > 1);

	std::stringstream buf;
	CPPUNIT_ASSERT(m->save(buf) == 0);
	delete m;

	m = &(FMap::create());
	CPPUNIT_ASSERT(m->load(buf) == 0);

	const JumpRouteList &loadedJumps = m->getJumpList();
	CPPUNIT_ASSERT(loadedJumps.size() == origRoutes.size());
	for (unsigned int i = 0; i < loadedJumps.size(); i++){
		FJumpRoute *j = loadedJumps[i];
		CPPUNIT_ASSERT(j->getStart() != NULL);
		CPPUNIT_ASSERT(j->getEnd() != NULL);
		CPPUNIT_ASSERT(j->getStart()->getName() == origRoutes[i].start);
		CPPUNIT_ASSERT(j->getEnd()->getName() == origRoutes[i].end);
		CPPUNIT_ASSERT(j->getStart()->getID() == nameToID[origRoutes[i].start]);
		CPPUNIT_ASSERT(j->getEnd()->getID() == nameToID[origRoutes[i].end]);
	}
}

// P5-4 AC2 (part 2): FMap::load's getSystem(id) resolution path must
// correctly resolve system IDs above 65535 end-to-end (no truncation from
// the removed pointer-smuggling-plus-0x0000FFFFL-mask approach). FSystem
// exposes no public ID setter, so this hand-crafts a raw FMap wire buffer
// (see appendSystem/appendJumpRoute) containing two systems whose IDs are
// forced above 65535 and a jump route referencing them, then drives the
// real FMap::load(is) path and asserts the resolved FSystem pointers match
// by both name and exact ID. Assumption: hand-crafting the stream is the
// only way to inject an out-of-range system ID since neither FSystem nor
// FMap expose a public ID-setting API.
void FMapTest::testLoadResolvesSystemIdAbove65535ViaHandCraftedStream(){
	const unsigned int sysAID = 70005u;   // > 65535
	const unsigned int sysBID = 131079u;  // > 65535, and != (sysAID & 0xFFFF)

	std::string buf;
	appendU32LE(buf, 40u); // maxCoord
	appendU32LE(buf, 2u);  // system count
	appendSystem(buf, sysAID, "Alpha-70005", 0, 1.0f, 2.0f, 0.0f);
	appendSystem(buf, sysBID, "Beta-131079", 0, 3.0f, 4.0f, 0.0f);
	appendU32LE(buf, 1u); // jump route count
	appendJumpRoute(buf, 1u, 2.0f, sysBID, sysAID);

	FMap &existing = FMap::getMap();
	delete &existing;
	FMap *m = &(FMap::create());

	std::istringstream is(buf);
	CPPUNIT_ASSERT(m->load(is) == 0);

	FSystem *a = m->getSystem(sysAID);
	FSystem *b = m->getSystem(sysBID);
	CPPUNIT_ASSERT(a != NULL);
	CPPUNIT_ASSERT(b != NULL);
	CPPUNIT_ASSERT(a->getName() == "Alpha-70005");
	CPPUNIT_ASSERT(b->getName() == "Beta-131079");
	CPPUNIT_ASSERT(a->getID() == sysAID);
	CPPUNIT_ASSERT(b->getID() == sysBID);

	const JumpRouteList &jumps = m->getJumpList();
	CPPUNIT_ASSERT(jumps.size() == 1);
	CPPUNIT_ASSERT(jumps[0]->getStart() == b);
	CPPUNIT_ASSERT(jumps[0]->getEnd() == a);
	CPPUNIT_ASSERT(jumps[0]->getStart()->getID() == sysBID);
	CPPUNIT_ASSERT(jumps[0]->getEnd()->getID() == sysAID);
}

// P5-4 AC3: FMap::load must return nonzero and store no invalid pointer
// (and must not grow getJumpList()) when a jump route references a system
// ID that is not present in the loaded system set. Hand-crafts a stream
// with one valid system followed by a jump route referencing a system ID
// that does not match any saved system.
void FMapTest::testLoadRejectsJumpRouteWithUnknownSystemIdAndLeavesJumpListEmpty(){
	const unsigned int knownID = 500u;
	const unsigned int unknownID = 999u; // not present in the system set

	std::string buf;
	appendU32LE(buf, 40u); // maxCoord
	appendU32LE(buf, 1u);  // system count
	appendSystem(buf, knownID, "OnlyKnownSystem", 0, 0.0f, 0.0f, 0.0f);
	appendU32LE(buf, 1u); // jump route count
	appendJumpRoute(buf, 1u, 1.0f, unknownID, knownID);

	FMap &existing = FMap::getMap();
	delete &existing;
	FMap *m = &(FMap::create());

	std::istringstream is(buf);
	int rc = m->load(is);
	CPPUNIT_ASSERT(rc != 0);
	CPPUNIT_ASSERT(m->getJumpList().size() == 0);
}

// P5-4 AC5 (supplement): FMap::save writes the map size and system count as
// fixed-width little-endian uint32_t fields via writeU32, not the host's
// native representation. This behaviorally inspects the raw bytes emitted
// at the known field offsets (bytes 0-3 for maxCoord, bytes 4-7 for the
// system count) rather than relying on source-text inspection alone.
void FMapTest::testSaveWiresMaxCoordAndCountsLittleEndian(){
	FMap &existing = FMap::getMap();
	delete &existing;
	FMap *m = &(FMap::create(false, m_pList));

	unsigned int sysCount = static_cast<unsigned int>(m->getSystemList().size());
	CPPUNIT_ASSERT(sysCount > 0);

	std::ostringstream os;
	CPPUNIT_ASSERT(m->save(os) == 0);
	std::string data = os.str();
	CPPUNIT_ASSERT(data.size() >= 8);

	unsigned int maxCoord = 40u; // set by FMap(false, pList)
	CPPUNIT_ASSERT(static_cast<unsigned char>(data[0]) == (maxCoord & 0xFF));
	CPPUNIT_ASSERT(static_cast<unsigned char>(data[1]) == ((maxCoord >> 8) & 0xFF));
	CPPUNIT_ASSERT(static_cast<unsigned char>(data[2]) == ((maxCoord >> 16) & 0xFF));
	CPPUNIT_ASSERT(static_cast<unsigned char>(data[3]) == ((maxCoord >> 24) & 0xFF));

	CPPUNIT_ASSERT(static_cast<unsigned char>(data[4]) == (sysCount & 0xFF));
	CPPUNIT_ASSERT(static_cast<unsigned char>(data[5]) == ((sysCount >> 8) & 0xFF));
	CPPUNIT_ASSERT(static_cast<unsigned char>(data[6]) == ((sysCount >> 16) & 0xFF));
	CPPUNIT_ASSERT(static_cast<unsigned char>(data[7]) == ((sysCount >> 24) & 0xFF));
}

}
