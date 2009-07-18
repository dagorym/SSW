/**
 * @file FSystemTest.cpp
 * @brief Implementation file for the FSystemTest class
 * @author Tom Stephens
 * @date Created: Jul 17, 2009
 */

#include "FSystemTest.h"

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

	std::ofstream os("test",std::ios::binary);
	s.save(os);
	os.close();
	FSystem s2;
	std::ifstream is("test",std::ios::binary);
	s2.load(is);
	is.close();

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

}
