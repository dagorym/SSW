/**
 * @file FFleetTest.cpp
 * @brief Implementation file for the FFleetTest class
 * @author Tom Stephens
 * @date Created: DATE
 */

#include "FFleetTest.h"
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
	CPPUNIT_ASSERT( m_f1->getIcon() == NULL );
	CPPUNIT_ASSERT( m_f1->isHolding() == false );
	CPPUNIT_ASSERT( m_f1->getSpeed() == 0 );
	CPPUNIT_ASSERT( m_f1->getTransitTime() == 0 );
	CPPUNIT_ASSERT( m_f1->getHomeSystem() == "" );
	CPPUNIT_ASSERT( m_f1->getCoord(0) == 0 );
	CPPUNIT_ASSERT( m_f1->getCoord(1) == 0 );
	CPPUNIT_ASSERT( m_f1->getDestination() == 0 );
	CPPUNIT_ASSERT( m_f1->getJumpRoute() == 0 );
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
	m_f1->setIcon("../../icons/UPFFleet.png");
	CPPUNIT_ASSERT( m_f1->getIcon() != NULL );
	m_f1->setIcon("../../icons/SatharFleet.png");
	CPPUNIT_ASSERT( m_f1->getIcon() != NULL );
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
	CPPUNIT_ASSERT(m_f1->getDestination() == -1 );
	CPPUNIT_ASSERT(m_f1->getInTransit() == false );
	CPPUNIT_ASSERT(m_f1->getTransitTime() == 0 );
	CPPUNIT_ASSERT(m_f1->getSpeed() == 0 );
	CPPUNIT_ASSERT(m_f1->getJumpRoute() == -1 );
	delete s;
}

void FFleetTest::testSetLocationLong(){
	FSystem *s = new FSystem;
	m_f1->setLocation(s,true,10,13,2,12);
	CPPUNIT_ASSERT(m_f1->getLocation() >= 0 );
	CPPUNIT_ASSERT(m_f1->getDestination() == 13 );
	CPPUNIT_ASSERT(m_f1->getInTransit() == true );
	CPPUNIT_ASSERT(m_f1->getTransitTime() == 10 );
	CPPUNIT_ASSERT(m_f1->getSpeed() == 2 );
	CPPUNIT_ASSERT(m_f1->getJumpRoute() == 12 );
	delete s;
}

void FFleetTest::testAddRemoveShip(){
	FVehicle *v = createShip("AssaultScout");
	m_f1->addShip(v);
	CPPUNIT_ASSERT(m_f1->getShipCount() == 1);
	unsigned int sid = v->getID();
	CPPUNIT_ASSERT(m_f1->getShip(sid)->getType() == "AssaultScout");
	m_f1->removeShip(sid);
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
	m_f1->setLocation(s,true,10,13,2,12);
	int tTime = m_f1->decTransitTime();
	CPPUNIT_ASSERT(m_f1->getTransitTime() == 8);
	CPPUNIT_ASSERT(tTime == 8);
	tTime = m_f1->decTransitTime();
	CPPUNIT_ASSERT(tTime == 6);
	tTime = m_f1->decTransitTime();
	CPPUNIT_ASSERT(tTime == 5);
	m_f1->setLocation(m_f1->getDestination());
	tTime = m_f1->decTransitTime();
	CPPUNIT_ASSERT(tTime == 3);
	tTime = m_f1->decTransitTime();
	CPPUNIT_ASSERT(tTime == 1);
	tTime = m_f1->decTransitTime();
	CPPUNIT_ASSERT(tTime == 0);
	CPPUNIT_ASSERT(m_f1->getLocation() == 13);
	CPPUNIT_ASSERT(m_f1->getInTransit() == false);
	CPPUNIT_ASSERT(m_f1->getJumpRoute() == 0);
	CPPUNIT_ASSERT(m_f1->getDestination() == -1);
	CPPUNIT_ASSERT(m_f1->getTransitTime() == 0);
	CPPUNIT_ASSERT(m_f1->getSpeed() == 1);
	delete s;
}

void FFleetTest::testGetMaxSpeed(){
	FVehicle *v = createShip("Frigate");
	m_f1->addShip(v);
	CPPUNIT_ASSERT(m_f1->getMaxSpeed() == 4);
	v = createShip("Minelayer");
	m_f1->addShip(v);
	CPPUNIT_ASSERT(m_f1->getMaxSpeed() == 1);
}

void FFleetTest::testCancelJump(){
	FSystem *s = new FSystem;
	m_f1->setLocation(s,true,10,13,2,12);
	m_f1->decTransitTime();
	m_f1->cancelJump();
	CPPUNIT_ASSERT(m_f1->getTransitTime() == 2 );
	CPPUNIT_ASSERT(m_f1->getLocation() == m_f1->getDestination());
	m_f1->setLocation(s,true,10,13,2,12);
	m_f1->cancelJump();
	CPPUNIT_ASSERT(m_f1->getTransitTime() == 0 );
	CPPUNIT_ASSERT(m_f1->getInTransit() == false);
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
	std::ofstream os("test",std::ios::binary);
	m_f1->save(os);
	os.close();
	FFleet f2;
	std::ifstream is("test",std::ios::binary);
	f2.load(is);
	is.close();
	CPPUNIT_ASSERT(f2.getShipCount() == m_f1->getShipCount());

}

void FFleetTest::TestCopyConstructor(){
	m_f1->addShip(createShip("AssaultScout"));
	m_f1->setName("Test Fleet");
	FFleet *f2 = new FFleet(*m_f1);
	CPPUNIT_ASSERT(f2->getShipCount() == m_f1->getShipCount());
	VehicleList l1 = m_f1->getShipList();
	VehicleList l2 = f2->getShipList();
	CPPUNIT_ASSERT(l2[0]->getID() == l1[0]->getID());
	CPPUNIT_ASSERT(f2->getName() == "Copy of " + m_f1->getName());
}

}
