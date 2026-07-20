/**
 * @file FPlayerTest.cpp
 * @brief Implementation file for the FPlayerTest class
 * @author Tom Stephens
 * @date Created: Jul 18, 2009
 */

#include "FPlayerTest.h"
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <sstream>
#include <vector>

namespace FrontierTests {
using namespace Frontier;

namespace {
	/**
	 * @brief FVehicle subclass instrumented to record destruction, used to
	 * prove FPlayer::m_destroyed ownership behavior behaviorally.
	 *
	 * Each instance records its own `this` pointer (as an FVehicle*) into a
	 * shared static log when it is destructed. Tests reset the log, exercise
	 * some FPlayer/FFleet ownership scenario, and then assert on exactly
	 * which pointers were destructed and how many times, which proves both
	 * "freed exactly once" and "no double-delete of an aliased pointer"
	 * without relying on undefined double-free behavior.
	 *
	 * @author Claude Sonnet 5 (medium)
	 * @date Created: Jul 11, 2026
	 */
	class CountingVehicle : public FVehicle {
	public:
		CountingVehicle() : FVehicle() {}
		virtual ~CountingVehicle() {
			s_destructedPointers.push_back(this);
		}
		static std::vector<FVehicle*> s_destructedPointers;
	};
	std::vector<FVehicle*> CountingVehicle::s_destructedPointers;
}

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FPlayerTest );

void FPlayerTest::setUp(){
	m_p1 = new FPlayer;
}

void FPlayerTest::tearDown(){
	delete m_p1;
}

void FPlayerTest::testConstructor(){
	CPPUNIT_ASSERT(m_p1->getID() == 1);
	CPPUNIT_ASSERT(m_p1->getName() == "");
}

void FPlayerTest::testSetName(){
	m_p1->setName("Destroyers");
	CPPUNIT_ASSERT(m_p1->getName() == "Destroyers");
}

void FPlayerTest::testAddFleet(){
	FFleet *f = new FFleet;
	unsigned int id = f->getID();
	f->setIcon("icons/UPF.png");
	m_p1->addFleet(f);
	CPPUNIT_ASSERT(m_p1->getLargestFleetID() == id );
}

void FPlayerTest::testGetFleetByName(){
	FFleet *f = new FFleet;
	f->setName("Tom's Fleet");
	unsigned int id = f->getID();
	f->setIcon("icons/UPF.png");
	m_p1->addFleet(f);
	FFleet *f2 = m_p1->getFleet("Tom's Fleet");
	CPPUNIT_ASSERT( f2->getID() == id);
	CPPUNIT_ASSERT( m_p1->getFleet("John's Fleet") == NULL );
}

void FPlayerTest::testGetFleetByLocation(){
	FFleet *f = new FFleet;
	unsigned int id = f->getID();
	m_p1->addFleet(f);
	CPPUNIT_ASSERT(m_p1->getFleet(0,0)->getID() == id);
	CPPUNIT_ASSERT(m_p1->getFleet(0.49,0)->getID() == id);
	CPPUNIT_ASSERT(m_p1->getFleet(0.51,0) == NULL);

}

void FPlayerTest::testGetFleetList(){
	FleetList fList = m_p1->getFleetList();
	CPPUNIT_ASSERT (fList.size() == 0);
	FFleet *f = new FFleet;
	unsigned int id = f->getID();
	f->setIcon("icons/UPF.png");
	m_p1->addFleet(f);
	fList = m_p1->getFleetList();
	CPPUNIT_ASSERT (fList.size() == 1);
	CPPUNIT_ASSERT (fList[0]->getID() == id);
}

void FPlayerTest::testRemoveFleet(){
	FFleet *f = new FFleet;
	unsigned int id = f->getID();
	f->setIcon("icons/UPF.png");
	m_p1->addFleet(f);

	FFleet *f2 = m_p1->removeFleet(id);
	CPPUNIT_ASSERT( f2->getID() == id );
	CPPUNIT_ASSERT( m_p1->getFleetList().size() == 0 );
	CPPUNIT_ASSERT( m_p1->getLargestFleetID() == 0 );

	CPPUNIT_ASSERT( m_p1->removeFleet(id+1) == NULL );

}

void FPlayerTest::testSetIcon(){
	std::string name = "icons/UPF.png";
	m_p1->setFleetIcon(name);
	CPPUNIT_ASSERT(m_p1->getFleetIconName() == name);
}

void FPlayerTest::testAddUnattachedShips(){
	CPPUNIT_ASSERT(m_p1->getShipList().size() == 0);
	FVehicle *s = createShip("AssaultScout");
	unsigned int id = s->getID();
	m_p1->addShip(s);
	VehicleList sList = m_p1->getShipList();
	CPPUNIT_ASSERT(sList.size() == 1);
	CPPUNIT_ASSERT(sList[0]->getID() == id);
}

void FPlayerTest::testAddDestroyedShips(){
	CPPUNIT_ASSERT(m_p1->getShipList().size() == 0);
	FVehicle *s = createShip("AssaultScout");
	m_p1->addDestroyedShip(s);
}

void FPlayerTest::testSerialize(){
	FFleet *f = new FFleet;
	unsigned int fID = f->getID();
	FVehicle *s = createShip("AssaultScout");
	unsigned int sID1 = s->getID();
	f->addShip(s);
	f->setIcon("icons/UPF.png");
	m_p1->addFleet(f);
	m_p1->setFleetIcon("icons/UPF.png");
	s = createShip("Frigate");
	unsigned int sID2 = s->getID();
	m_p1->addShip(s);

	const char *filename = "FPlayerTest.tmp";
	std::remove(filename);
	std::ofstream os(filename,std::ios::binary);
	m_p1->save(os);
	os.close();
	FPlayer p2;
	std::ifstream is(filename,std::ios::binary);
	p2.load(is);
	is.close();
	std::remove(filename);

	CPPUNIT_ASSERT(p2.getShipList()[0]->getID() == sID2);
	FleetList fList = p2.getFleetList();
	CPPUNIT_ASSERT(fList.size() == 1);
	CPPUNIT_ASSERT(fList[0]->getID() == fID);
	CPPUNIT_ASSERT(fList[0]->getShipList().size() == 1);
	CPPUNIT_ASSERT(fList[0]->getShipList()[0]->getID() == sID1);
}

// Reviewer follow-up F2: FPlayer must be the sole owner of ships in
// m_destroyed and free them in ~FPlayer(). Against the unfixed destructor
// (no delete loop over m_destroyed), this test fails because the
// instrumented ship's destructor never runs, so s_destructedPointers stays
// empty instead of recording exactly one entry.
void FPlayerTest::testDestroyedShipFreedExactlyOnceOnPlayerDestruction(){
	CountingVehicle::s_destructedPointers.clear();

	CountingVehicle *v = new CountingVehicle();
	FVehicle *vAsBase = v;

	{
		FPlayer p;
		p.addDestroyedShip(v);
		// p (and, per the ownership contract, v) is destructed at the
		// closing brace below.
	}

	CPPUNIT_ASSERT_EQUAL( (size_t)1, CountingVehicle::s_destructedPointers.size() );
	CPPUNIT_ASSERT( CountingVehicle::s_destructedPointers[0] == vAsBase );
}

// Reviewer follow-up F2 aliasing check: a ship removed from a fleet via
// FFleet::removeShip() (which erases without deleting) and then handed to
// addDestroyedShip() must be freed exactly once overall -- once via
// ~FPlayer()'s m_destroyed cleanup -- while a sibling ship left in the
// surviving fleet is freed exactly once via the existing FFleet destructor
// path. Neither ship may be destructed zero times (a leak) or more than
// once (a double-delete/aliasing bug).
void FPlayerTest::testSurvivingFleetShipNotDoubleDeletedByDestroyedList(){
	CountingVehicle::s_destructedPointers.clear();

	CountingVehicle *survivor = new CountingVehicle();
	CountingVehicle *destroyedShip = new CountingVehicle();

	FFleet *fleet = new FFleet;
	fleet->setIcon("icons/UPF.png");
	fleet->addShip(survivor);
	fleet->addShip(destroyedShip);

	unsigned int destroyedId = destroyedShip->getID();
	FVehicle *removed = fleet->removeShip(destroyedId);
	CPPUNIT_ASSERT( removed == destroyedShip );
	// destroyedShip is no longer reachable through the fleet's ship list.
	CPPUNIT_ASSERT( fleet->getShip(destroyedId) == NULL );

	{
		FPlayer p;
		p.addFleet(fleet);        // fleet (and its remaining ship, survivor) owned by p
		p.addDestroyedShip(removed); // destroyedShip ownership transferred to p
		// p, fleet, survivor, and destroyedShip are all destructed at the
		// closing brace below.
	}

	CPPUNIT_ASSERT_EQUAL( (size_t)2, CountingVehicle::s_destructedPointers.size() );
	CPPUNIT_ASSERT_EQUAL( (size_t)1, (size_t)std::count(
		CountingVehicle::s_destructedPointers.begin(),
		CountingVehicle::s_destructedPointers.end(),
		static_cast<FVehicle*>(survivor) ) );
	CPPUNIT_ASSERT_EQUAL( (size_t)1, (size_t)std::count(
		CountingVehicle::s_destructedPointers.begin(),
		CountingVehicle::s_destructedPointers.end(),
		static_cast<FVehicle*>(destroyedShip) ) );
}

const int FPlayerTest::save(std::ostream &os) const { return 0; }
int FPlayerTest::load(std::istream &is) { return 0; }

void FPlayerTest::testFullRoundTripPreservesUnattachedFleetsAndDestroyedShips(){
	FVehicle *unattached = createShip("AssaultScout");
	unattached->setName("Unattached One");
	unattached->setHP(7);
	unsigned int unattachedID = unattached->getID();
	m_p1->addShip(unattached);

	FFleet *fleet = new FFleet;
	fleet->setName("Strike Fleet");
	fleet->setIcon("icons/UPF.png");
	FVehicle *fleetShip = createShip("Frigate");
	fleetShip->setName("Fleet Ship One");
	fleetShip->setHP(11);
	unsigned int fleetShipID = fleetShip->getID();
	fleet->addShip(fleetShip);
	unsigned int fleetID = fleet->getID();
	m_p1->addFleet(fleet);

	FVehicle *destroyed = createShip("Destroyer");
	destroyed->setName("Destroyed One");
	destroyed->setHP(3);
	m_p1->addDestroyedShip(destroyed);

	m_p1->setName("Round Trip Player");
	m_p1->setFleetIcon("icons/UPF.png");

	std::stringstream original;
	CPPUNIT_ASSERT( m_p1->save(original) == 0 );
	std::string originalBytes = original.str();

	std::stringstream loadStream(originalBytes);
	FPlayer p2;
	CPPUNIT_ASSERT( p2.load(loadStream) == 0 );

	CPPUNIT_ASSERT( p2.getName() == "Round Trip Player" );
	CPPUNIT_ASSERT( p2.getID() == m_p1->getID() );

	VehicleList &uList = p2.getShipList();
	CPPUNIT_ASSERT( uList.size() == 1 );
	CPPUNIT_ASSERT( uList[0]->getID() == unattachedID );
	CPPUNIT_ASSERT( uList[0]->getName() == "Unattached One" );
	CPPUNIT_ASSERT( uList[0]->getHP() == 7 );

	FleetList &fList = p2.getFleetList();
	CPPUNIT_ASSERT( fList.size() == 1 );
	CPPUNIT_ASSERT( fList[0]->getID() == fleetID );
	CPPUNIT_ASSERT( fList[0]->getName() == "Strike Fleet" );
	CPPUNIT_ASSERT( fList[0]->getShipList().size() == 1 );
	CPPUNIT_ASSERT( fList[0]->getShipList()[0]->getID() == fleetShipID );
	CPPUNIT_ASSERT( fList[0]->getShipList()[0]->getName() == "Fleet Ship One" );
	CPPUNIT_ASSERT( fList[0]->getShipList()[0]->getHP() == 11 );

	// m_destroyed has no public accessor -- FPlayer is its sole owner (see
	// addDestroyedShip()'s ownership contract). Prove the destroyed-ship
	// list's count/ID/name/HP round-tripped correctly by re-saving the
	// loaded player: FPlayer::save() writes the destroyed-ship section as a
	// pure function of the current m_destroyed contents (type tag +
	// FVehicle::save()), immediately after the already-verified unattached
	// and fleet sections, so if load() had gotten the destroyed-ship count,
	// factory type, ID, name, or HP wrong, the re-saved bytes would differ
	// from the original.
	std::stringstream resaved;
	CPPUNIT_ASSERT( p2.save(resaved) == 0 );
	CPPUNIT_ASSERT( resaved.str() == originalBytes );
}

void FPlayerTest::testLoadAdvancesNextIDPastLoadedID(){
	// AC (P5-3 H3): use an ID far larger than any ID this test binary could
	// plausibly have assigned so far so the assertion is not sensitive to
	// test order. m_p1 (created in setUp) keeps FPlayer::m_classCount above
	// zero for the duration of this test, so the static m_nextID counter
	// cannot be reset back to 1 between the load() below and the
	// freshly-constructed player.
	const uint32_t loadedID = 5000000u;

	std::stringstream stream;
	writeU32(stream, loadedID);
	writeString(stream, "Loaded Player");
	writeString(stream, "icons/UPF.png");
	writeU32(stream, (uint32_t)0); // unattached ship count
	writeU32(stream, (uint32_t)0); // fleet count
	writeU32(stream, (uint32_t)0); // destroyed ship count

	FPlayer p2;
	int rc = p2.load(stream);
	CPPUNIT_ASSERT( rc == 0 );
	CPPUNIT_ASSERT( p2.getID() == loadedID );

	FPlayer p3;
	CPPUNIT_ASSERT( p3.getID() > loadedID );
}

void FPlayerTest::testLoadReturnsNonzeroOnUnknownUnattachedShipType(){
	// AC (P5-3): an unattached-ship-list entry with an unresolvable type tag
	// makes createShip(...) return NULL; FPlayer::load() must null-check and
	// return nonzero rather than dereferencing it.
	std::stringstream stream;
	writeU32(stream, (uint32_t)1);
	writeString(stream, "Corrupt Player");
	writeString(stream, "");
	writeU32(stream, (uint32_t)1); // unattached ship count = 1
	writeString(stream, "BogusShipType"); // unknown type tag; no further payload needed

	FPlayer p2;
	int rc = p2.load(stream);
	CPPUNIT_ASSERT( rc != 0 );
}

void FPlayerTest::testLoadReturnsNonzeroOnUnknownDestroyedShipType(){
	// AC (P5-3 F2): the same null-check/abort contract applies to the
	// destroyed-ship list.
	std::stringstream stream;
	writeU32(stream, (uint32_t)1);
	writeString(stream, "Corrupt Player");
	writeString(stream, "");
	writeU32(stream, (uint32_t)0); // unattached ship count
	writeU32(stream, (uint32_t)0); // fleet count
	writeU32(stream, (uint32_t)1); // destroyed ship count = 1
	writeString(stream, "BogusShipType"); // unknown type tag; no further payload needed

	FPlayer p2;
	int rc = p2.load(stream);
	CPPUNIT_ASSERT( rc != 0 );
}

void FPlayerTest::testLoadReturnsNonzeroWhenTruncatedInsideOwnScalarRegion(){
	// AC (FF2-3 / FR-D): m_ID and m_name are fully present, but the stream
	// stops before m_iconName's own length-prefixed record can be read.
	// Before FF2-3, every scalar read below this point (m_iconName plus the
	// unattached/fleet/destroyed counts) had its return value discarded, so
	// this exact truncation point returned 0 despite never reaching a valid
	// end of the player's own record.
	std::stringstream stream;
	writeU32(stream, (uint32_t)42);                 // m_ID: fully present
	writeString(stream, "Truncated Mid-Record Player"); // m_name: fully present
	// Deliberately stop here: no m_iconName, no unattached/fleet/destroyed
	// counts at all.

	FPlayer p2;
	int rc = p2.load(stream);

	CPPUNIT_ASSERT( rc != 0 );
	// Leak/dangle safety: the abort happens before any sub-object is
	// allocated, so every list must remain empty and the player must remain
	// safe to query and destruct.
	CPPUNIT_ASSERT( p2.getShipList().empty() );
	CPPUNIT_ASSERT( p2.getFleetList().empty() );
}

void FPlayerTest::testLoadReturnsNonzeroWhenTruncatedInsideCountFields(){
	// AC (FF2-3 / FR-D): m_ID, m_name, and m_iconName are all fully present
	// and readable, but the fleet-count (fSize) field is truncated mid-way
	// through its own 4 bytes -- after the unattached-ship-count (uSize=0)
	// field has already been read successfully. This isolates the
	// return-check on a *later* container-level scalar read rather than the
	// very first one. Before FF2-3, fSize's discarded read left the local
	// counter at its zero-initialized default, the fleet loop silently ran
	// zero iterations, the destroyed-count read failed the same way, and
	// load() returned 0.
	std::stringstream stream;
	writeU32(stream, (uint32_t)43);
	writeString(stream, "Truncated Fleet-Count Player");
	writeString(stream, "icons/UPF.png");
	writeU32(stream, (uint32_t)0); // unattached ship count: fully present

	// Write only 2 of the 4 bytes that make up fSize.
	unsigned char partialCount[2] = {0x07, 0x00};
	stream.write(reinterpret_cast<const char*>(partialCount), 2);

	FPlayer p2;
	int rc = p2.load(stream);

	CPPUNIT_ASSERT( rc != 0 );
	CPPUNIT_ASSERT( p2.getShipList().empty() );
	CPPUNIT_ASSERT( p2.getFleetList().empty() );
}

}
