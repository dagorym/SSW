/**
 * @file FFleetTest.cpp
 * @brief Implementation file for the FFleetTest class
 * @author Tom Stephens
 * @date Created: DATE
 */

#include "FFleetTest.h"
#include <cstdint>
#include <cstdio>
#include <sstream>
#include <type_traits>
#include "Frontier.h"
#include "strategic/FSystem.h"

namespace FrontierTests {
using namespace Frontier;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FFleetTest );

void FFleetTest::setUp(){
	m_f1 = new FFleet;
}

void FFleetTest::tearDown(){
	delete m_f1;
}

void FFleetTest::testConstructor(){
	CPPUNIT_ASSERT( m_f1->getID() >=0 );
	CPPUNIT_ASSERT( m_f1->getLocation() == 0 );
	CPPUNIT_ASSERT( m_f1->getInTransit() == false );
	CPPUNIT_ASSERT( m_f1->isMilitia() == false );
	CPPUNIT_ASSERT( m_f1->getIconName().empty() );
	CPPUNIT_ASSERT( m_f1->isHolding() == false );
	CPPUNIT_ASSERT( m_f1->getSpeed() == 0 );
	CPPUNIT_ASSERT( m_f1->getTransitTime() == 0 );
	CPPUNIT_ASSERT( m_f1->getHomeSystem() == "" );
	CPPUNIT_ASSERT( m_f1->getCoord(0) == 0 );
	CPPUNIT_ASSERT( m_f1->getCoord(1) == 0 );
	CPPUNIT_ASSERT( m_f1->getDestination() == FFleet::NO_DESTINATION );
	CPPUNIT_ASSERT( m_f1->getJumpRoute() == FFleet::NO_ROUTE );
}

void FFleetTest::testBasicSetters(){
	m_f1->setInTransit(true);
	CPPUNIT_ASSERT( m_f1->getInTransit() == true );
	m_f1->setOwner(3);
	CPPUNIT_ASSERT( m_f1->getOwner() == 3 );
	m_f1->setName("Destroyers");
	CPPUNIT_ASSERT( m_f1->getName() == "Destroyers" );
	m_f1->setSpeed(2);
	CPPUNIT_ASSERT( m_f1->getSpeed() == 2 );
	m_f1->setLocation(10);
	CPPUNIT_ASSERT( m_f1->getLocation() == 10 );
	m_f1->setDestination(12);
	CPPUNIT_ASSERT( m_f1->getDestination() == 12 );
	m_f1->setIcon("icons/UPF.png");
	CPPUNIT_ASSERT_EQUAL(std::string("icons/UPF.png"), m_f1->getIconName());
	m_f1->setIcon("icons/Sathar.png");
	CPPUNIT_ASSERT_EQUAL(std::string("icons/Sathar.png"), m_f1->getIconName());
	m_f1->setMilitia(true,"Prenglar");
	CPPUNIT_ASSERT( m_f1->isMilitia() == true );
	CPPUNIT_ASSERT( m_f1->getHomeSystem() == "Prenglar" );
	m_f1->setHolding(true);
	CPPUNIT_ASSERT( m_f1->isHolding() == true );
}

void FFleetTest::testSetLocation(){
	FSystem *s = new FSystem;
	m_f1->setLocation(s,false);
	CPPUNIT_ASSERT(m_f1->getLocation() >= 0 );
	CPPUNIT_ASSERT(m_f1->getDestination() == FFleet::NO_DESTINATION );
	CPPUNIT_ASSERT(m_f1->getInTransit() == false );
	CPPUNIT_ASSERT(m_f1->getTransitTime() == 0 );
	CPPUNIT_ASSERT(m_f1->getSpeed() == 0 );
	CPPUNIT_ASSERT(m_f1->getJumpRoute() == FFleet::NO_ROUTE );
	delete s;
}

void FFleetTest::testSetLocationLong(){
	FSystem *s = new FSystem;
	m_f1->setLocation(s,true,10,13,2,12);
	CPPUNIT_ASSERT(m_f1->getLocation() >= 0 );
	CPPUNIT_ASSERT(m_f1->getDestination() == 13 );
	CPPUNIT_ASSERT(m_f1->getDestination() != FFleet::NO_DESTINATION );
	CPPUNIT_ASSERT(m_f1->getInTransit() == true );
	CPPUNIT_ASSERT(m_f1->getTransitTime() == 10 );
	CPPUNIT_ASSERT(m_f1->getSpeed() == 2 );
	CPPUNIT_ASSERT(m_f1->getJumpRoute() == 12 );
	CPPUNIT_ASSERT(m_f1->getJumpRoute() != FFleet::NO_ROUTE );
	delete s;
}

void FFleetTest::testAddRemoveShip(){
	FVehicle *v = createShip("AssaultScout");
	m_f1->addShip(v);
	CPPUNIT_ASSERT(m_f1->getShipCount() == 1);
	unsigned int sid = v->getID();
	CPPUNIT_ASSERT(m_f1->getShip(sid)->getType() == "AssaultScout");
	FFleet f2;
	FVehicle *transferredShip = m_f1->removeShip(sid);
	CPPUNIT_ASSERT(transferredShip != NULL);
	f2.addShip(transferredShip);
	CPPUNIT_ASSERT(m_f1->getShipCount() == 0);
	CPPUNIT_ASSERT(f2.getShipCount() == 1);
	CPPUNIT_ASSERT(f2.getShip(sid) != NULL);
	f2.removeShip(sid);
	CPPUNIT_ASSERT(m_f1->getShipCount() == 0);
	FVehicle *v2 = m_f1->getShip(sid+1);
	CPPUNIT_ASSERT( v2 == NULL);
	v2 = m_f1->removeShip(sid+1);
	CPPUNIT_ASSERT( v2 == NULL);

}

void FFleetTest::testMilitiaFunctions(){
	m_f1->setMilitia(true,"Prenglar");
	CPPUNIT_ASSERT(m_f1->isMilitia() == true);
	CPPUNIT_ASSERT(m_f1->getHomeSystem() == "Prenglar");
}

void FFleetTest::testGetRJChance(){
	m_f1->setSpeed(1);
	CPPUNIT_ASSERT(m_f1->getRJChance() == 100);
	m_f1->setSpeed(2);
	CPPUNIT_ASSERT(m_f1->getRJChance() == 90);
	m_f1->setSpeed(3);
	CPPUNIT_ASSERT(m_f1->getRJChance() == 70);
	m_f1->setMilitia(true,"Prenglar");
	m_f1->setSpeed(1);
	CPPUNIT_ASSERT(m_f1->getRJChance() == 100);
	m_f1->setSpeed(2);
	CPPUNIT_ASSERT(m_f1->getRJChance() == 70);
	m_f1->setSpeed(3);
	CPPUNIT_ASSERT(m_f1->getRJChance() == 50);
	m_f1->setMilitia(false,"");
	FVehicle *v = createShip("Battleship");
	m_f1->addShip(v);
	m_f1->setSpeed(1);
	CPPUNIT_ASSERT(m_f1->getRJChance() == 100);
	m_f1->setSpeed(2);
	CPPUNIT_ASSERT(m_f1->getRJChance() == 95);
	m_f1->setSpeed(3);
	CPPUNIT_ASSERT(m_f1->getRJChance() == 90);

}

void FFleetTest::testDecTransitTime(){
	FSystem *s = new FSystem;
	const unsigned int destination = s->getID() + 1;
	m_f1->setLocation(s,true,10,destination,2,12);
	int tTime = m_f1->decTransitTime();
	CPPUNIT_ASSERT(m_f1->getTransitTime() == 8);
	CPPUNIT_ASSERT(tTime == 8);
	tTime = m_f1->decTransitTime();
	CPPUNIT_ASSERT(tTime == 6);
	// This 3rd call is the only one that lands on the risk-jump check
	// (m_transitTime <= m_jumpLength/2 while location != destination), which
	// draws irand(100) against getRJChance() (90 for speed==2, non-militia,
	// no battleship). Unseeded, that draw fails ~10% of the time (>90),
	// making decTransitTime() return -1 instead of 5 -- the documented
	// intermittent failure (reviewer follow-up F1). Seed immediately before
	// this call (not earlier, and not via constructing an FGame, which would
	// reseed from the clock) so the draw is deterministic and no other
	// irand() consumer runs in between.
	seedRandomExplicit(1);
	tTime = m_f1->decTransitTime();
	CPPUNIT_ASSERT(tTime == 5);
	m_f1->setLocation(m_f1->getDestination());
	tTime = m_f1->decTransitTime();
	CPPUNIT_ASSERT(tTime == 3);
	tTime = m_f1->decTransitTime();
	CPPUNIT_ASSERT(tTime == 1);
	tTime = m_f1->decTransitTime();
	CPPUNIT_ASSERT(tTime == 0);
	CPPUNIT_ASSERT(m_f1->getLocation() == destination);
	CPPUNIT_ASSERT(m_f1->getInTransit() == false);
	CPPUNIT_ASSERT(m_f1->getJumpRoute() == FFleet::NO_ROUTE);
	CPPUNIT_ASSERT(m_f1->getDestination() == FFleet::NO_DESTINATION);
	CPPUNIT_ASSERT(m_f1->getTransitTime() == 0);
	CPPUNIT_ASSERT(m_f1->getSpeed() == 1);
	delete s;
}

void FFleetTest::testGetMaxSpeed(){
	FVehicle *v = createShip("Frigate");
	v->setIcon("icons/UPFFrigate.png");
	m_f1->addShip(v);
	CPPUNIT_ASSERT(m_f1->getMaxSpeed() == 4);
	v = createShip("Minelayer");
	m_f1->addShip(v);
	CPPUNIT_ASSERT(m_f1->getMaxSpeed() == 1);
}

void FFleetTest::testCancelJump(){
	FSystem *s = new FSystem;
	const unsigned int destination = s->getID() + 1;
	m_f1->setLocation(s,true,10,destination,2,12);
	CPPUNIT_ASSERT(m_f1->getDestination() != FFleet::NO_DESTINATION);
	CPPUNIT_ASSERT(m_f1->getJumpRoute() != FFleet::NO_ROUTE);
	m_f1->decTransitTime();
	m_f1->cancelJump();
	CPPUNIT_ASSERT(m_f1->getTransitTime() == 2 );
	CPPUNIT_ASSERT(m_f1->getLocation() == m_f1->getDestination());
	CPPUNIT_ASSERT(m_f1->getDestination() != FFleet::NO_DESTINATION);
	CPPUNIT_ASSERT(m_f1->getJumpRoute() != FFleet::NO_ROUTE);
	CPPUNIT_ASSERT(m_f1->decTransitTime() == 0);
	CPPUNIT_ASSERT(m_f1->getDestination() == FFleet::NO_DESTINATION);
	CPPUNIT_ASSERT(m_f1->getJumpRoute() == FFleet::NO_ROUTE);
	m_f1->setLocation(s,true,10,destination,2,12);
	m_f1->cancelJump();
	CPPUNIT_ASSERT(m_f1->getTransitTime() == 0 );
	CPPUNIT_ASSERT(m_f1->getInTransit() == false);
	CPPUNIT_ASSERT(m_f1->decTransitTime() == 0);
	CPPUNIT_ASSERT(m_f1->getDestination() == FFleet::NO_DESTINATION);
	CPPUNIT_ASSERT(m_f1->getJumpRoute() == FFleet::NO_ROUTE);
	delete s;
}

void FFleetTest::testSetJumpRoute(){
	FSystem *s = new FSystem;
	FSystem *e = new FSystem;
	m_f1->setJumpRoute(10,s,e,12);
	CPPUNIT_ASSERT(m_f1->getJumpRoute() == 10);
	CPPUNIT_ASSERT(m_f1->getTransitTime() == 12);
	delete s;
	delete e;
}

void FFleetTest::testSerialize(){
	m_f1->addShip(createShip("AssaultScout"));
	const char *filename = "FFleetTest.tmp";
	std::remove(filename);
	std::ofstream os(filename,std::ios::binary);
	m_f1->setIcon("icons/UPF.png");
	m_f1->save(os);
	os.close();
	FFleet f2;
	std::ifstream is(filename,std::ios::binary);
	f2.load(is);
	is.close();
	std::remove(filename);
	CPPUNIT_ASSERT(f2.getShipCount() == m_f1->getShipCount());

}

void FFleetTest::TestCopyConstructor(){
	CPPUNIT_ASSERT(std::is_copy_constructible<FFleet>::value == false);
	CPPUNIT_ASSERT(std::is_copy_assignable<FFleet>::value == false);
}

const int FFleetTest::save(std::ostream &os) const { return 0; }
int FFleetTest::load(std::istream &is) { return 0; }

void FFleetTest::testJumpRouteZeroPreservedAcrossSaveLoad(){
	// AC (P5-3 H4): jump route ID 0 is a valid, distinct route ID (FJumpRoute
	// IDs start at 0) and must survive a save->load round trip unchanged --
	// not normalized to FFleet::NO_ROUTE by a legacy load()-side rewrite.
	FSystem *s = new FSystem;
	m_f1->setLocation(s, true, 10, 13, 2, 0); // route = 0
	CPPUNIT_ASSERT( m_f1->getJumpRoute() == 0 );
	CPPUNIT_ASSERT( m_f1->getInTransit() == true );

	std::stringstream stream;
	CPPUNIT_ASSERT( m_f1->save(stream) == 0 );

	FFleet f2;
	CPPUNIT_ASSERT( f2.load(stream) == 0 );
	CPPUNIT_ASSERT( f2.getJumpRoute() == 0 );
	CPPUNIT_ASSERT( f2.getJumpRoute() != FFleet::NO_ROUTE );
	CPPUNIT_ASSERT( f2.getInTransit() == true );

	delete s;
}

void FFleetTest::testSerializeRoundTripsMultipleShipsAndFleetState(){
	FSystem *s = new FSystem;
	m_f1->setLocation(s, true, 10, 13, 2, 12);
	m_f1->setOwner(7);
	m_f1->setName("Task Force Alpha");
	m_f1->setIcon("icons/UPF.png");

	FVehicle *shipA = createShip("AssaultScout");
	shipA->setName("Ship A");
	shipA->setHP(9);
	unsigned int idA = shipA->getID();
	m_f1->addShip(shipA);

	FVehicle *shipB = createShip("Frigate");
	shipB->setName("Ship B");
	shipB->setHP(15);
	unsigned int idB = shipB->getID();
	m_f1->addShip(shipB);

	std::stringstream stream;
	CPPUNIT_ASSERT( m_f1->save(stream) == 0 );

	FFleet f2;
	CPPUNIT_ASSERT( f2.load(stream) == 0 );

	CPPUNIT_ASSERT( f2.getID() == m_f1->getID() );
	CPPUNIT_ASSERT( f2.getName() == "Task Force Alpha" );
	CPPUNIT_ASSERT( f2.getOwner() == 7 );
	CPPUNIT_ASSERT( f2.getLocation() == m_f1->getLocation() );
	CPPUNIT_ASSERT( f2.getDestination() == 13 );
	CPPUNIT_ASSERT( f2.getJumpRoute() == 12 );
	CPPUNIT_ASSERT( f2.getIconName() == "icons/UPF.png" );

	CPPUNIT_ASSERT( f2.getShipCount() == 2 );
	FVehicle *loadedA = f2.getShip(idA);
	CPPUNIT_ASSERT( loadedA != NULL );
	CPPUNIT_ASSERT( loadedA->getName() == "Ship A" );
	CPPUNIT_ASSERT( loadedA->getHP() == 9 );
	FVehicle *loadedB = f2.getShip(idB);
	CPPUNIT_ASSERT( loadedB != NULL );
	CPPUNIT_ASSERT( loadedB->getName() == "Ship B" );
	CPPUNIT_ASSERT( loadedB->getHP() == 15 );

	delete s;
}

void FFleetTest::testLoadAdvancesNextIDPastLoadedID(){
	// AC (P5-3 H3): use an ID far larger than any ID this test binary could
	// plausibly have assigned so far so the assertion is not sensitive to
	// test order. m_f1 (created in setUp) keeps FFleet::m_classCount above
	// zero for the duration of this test, so the static, signed m_nextID
	// counter cannot be reset back to 0 between the load() below and the
	// freshly-constructed fleet.
	const uint32_t loadedID = 5000000u;

	std::stringstream stream;
	writeU32(stream, loadedID);
	writeString(stream, "Loaded Fleet");
	writeU32(stream, (uint32_t)0); // owner
	writeU32(stream, (uint32_t)0); // location
	write(stream, false); // inTransit
	writeU32(stream, (uint32_t)FFleet::NO_DESTINATION); // destination
	write(stream, (unsigned int)0); // transitTime
	write(stream, (unsigned int)0); // jumpLength
	write(stream, (unsigned int)0); // speed
	writeU32(stream, (uint32_t)FFleet::NO_ROUTE); // jumpRouteID
	writeString(stream, ""); // iconFile
	write(stream, false); // isMilitia
	writeString(stream, ""); // home
	write(stream, false); // isHolding
	write(stream, (float)0); // pos[0]
	write(stream, (float)0); // pos[1]
	write(stream, (float)0); // dx
	write(stream, (float)0); // dy
	writeU32(stream, (uint32_t)0); // ship count

	FFleet f2;
	int rc = f2.load(stream);
	CPPUNIT_ASSERT( rc == 0 );
	CPPUNIT_ASSERT( f2.getID() == loadedID );

	FFleet f3;
	CPPUNIT_ASSERT( (uint32_t)f3.getID() > loadedID );
}

void FFleetTest::testLoadReturnsNonzeroOnUnknownShipType(){
	// AC (P5-3): a ship-list entry with an unresolvable type tag makes
	// createShip(...) return NULL; FFleet::load() must null-check and
	// return nonzero rather than dereferencing it.
	std::stringstream stream;
	writeU32(stream, (uint32_t)1); // id
	writeString(stream, "Corrupt Fleet");
	writeU32(stream, (uint32_t)0); // owner
	writeU32(stream, (uint32_t)0); // location
	write(stream, false); // inTransit
	writeU32(stream, (uint32_t)FFleet::NO_DESTINATION); // destination
	write(stream, (unsigned int)0); // transitTime
	write(stream, (unsigned int)0); // jumpLength
	write(stream, (unsigned int)0); // speed
	writeU32(stream, (uint32_t)FFleet::NO_ROUTE); // jumpRouteID
	writeString(stream, ""); // iconFile
	write(stream, false); // isMilitia
	writeString(stream, ""); // home
	write(stream, false); // isHolding
	write(stream, (float)0); // pos[0]
	write(stream, (float)0); // pos[1]
	write(stream, (float)0); // dx
	write(stream, (float)0); // dy
	writeU32(stream, (uint32_t)1); // ship count = 1
	writeString(stream, "BogusShipType"); // unknown type tag; no further payload needed

	FFleet f2;
	int rc = f2.load(stream);
	CPPUNIT_ASSERT( rc != 0 );
}

}
