/**
 * @file FPlayerTest.cpp
 * @brief Implementation file for the FPlayerTest class
 * @author Tom Stephens
 * @date Created: Jul 18, 2009
 */

#include "FPlayerTest.h"

namespace FrontierTests {
using namespace Frontier;

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
	m_p1->addFleet(f);
	CPPUNIT_ASSERT(m_p1->getLargestFleetID() == id );
}

void FPlayerTest::testGetFleetByName(){
	FFleet *f = new FFleet;
	f->setName("Tom's Fleet");
	unsigned int id = f->getID();
	m_p1->addFleet(f);
	FFleet *f2 = m_p1->getFleet("Tom's Fleet");
	CPPUNIT_ASSERT( f2->getID() == id);
	CPPUNIT_ASSERT( m_p1->getFleet("John's Fleet") == NULL );

}

void FPlayerTest::testGetFleetList(){
	FleetList fList = m_p1->getFleetList();
	CPPUNIT_ASSERT (fList.size() == 0);
	FFleet *f = new FFleet;
	unsigned int id = f->getID();
	m_p1->addFleet(f);
	fList = m_p1->getFleetList();
	CPPUNIT_ASSERT (fList.size() == 1);
	CPPUNIT_ASSERT (fList[0]->getID() == id);
}

void FPlayerTest::testRemoveFleet(){
	FFleet *f = new FFleet;
	unsigned int id = f->getID();
	m_p1->addFleet(f);

	FFleet *f2 = m_p1->removeFleet(id);
	CPPUNIT_ASSERT( f2->getID() == id );
	CPPUNIT_ASSERT( m_p1->getFleetList().size() == 0 );
	CPPUNIT_ASSERT( m_p1->getLargestFleetID() == 0 );

	CPPUNIT_ASSERT( m_p1->removeFleet(id+1) == NULL );

}

void FPlayerTest::testSetIcon(){
	std::string name = "../../icons/UPFFleet.png";
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
	m_p1->addFleet(f);
	s = createShip("Frigate");
	unsigned int sID2 = s->getID();
	m_p1->addShip(s);

	std::ofstream os("test",std::ios::binary);
	m_p1->save(os);
	os.close();
	FPlayer p2;
	std::ifstream is("test",std::ios::binary);
	p2.load(is);
	is.close();

	CPPUNIT_ASSERT(p2.getShipList()[0]->getID() == sID2);
	FleetList fList = p2.getFleetList();
	CPPUNIT_ASSERT(fList.size() == 1);
	CPPUNIT_ASSERT(fList[0]->getID() == fID);
	CPPUNIT_ASSERT(fList[0]->getShipList().size() == 1);
	CPPUNIT_ASSERT(fList[0]->getShipList()[0]->getID() == sID1);
}

}
