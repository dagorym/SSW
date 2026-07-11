/**
 * @file FPlayerTest.cpp
 * @brief Implementation file for the FPlayerTest class
 * @author Tom Stephens
 * @date Created: Jul 18, 2009
 */

#include "FPlayerTest.h"
#include <algorithm>
#include <cstdio>
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

}
