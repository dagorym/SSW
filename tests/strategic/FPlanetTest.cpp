/**
 * @file FPlanetTest.cpp
 * @brief Implementation file for the FPlanetTest class
 * @author Tom Stephens
 * @date Created: Jun 24, 2009
 */

#include "FPlanetTest.h"

namespace FrontierTests {
using namespace Frontier;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FPlanetTest );

void FPlanetTest::setUp(){
	m_p1 = new FPlanet();
}

void FPlanetTest::tearDown(){
	delete m_p1;
}

void FPlanetTest::testConstructor(){
	CPPUNIT_ASSERT(m_p1 != NULL);
	CPPUNIT_ASSERT(m_p1->getName() == "Unknown");
	delete m_p1;
	m_p1 = new FPlanet("New Planet");
	CPPUNIT_ASSERT(m_p1->getName() == "New Planet");
	FPlanet p2;
	CPPUNIT_ASSERT(p2.getName() == "Unknown");
	FPlanet p3("Odie");
	CPPUNIT_ASSERT(p3.getName() == "Odie");
}

void FPlanetTest::testAddDestroyStation(){
	FVehicle *s1 = createShip("ArmedStation");
	m_p1->addStation(s1);
	CPPUNIT_ASSERT( m_p1->getStation() == s1 );
	m_p1->destroyStation();
	CPPUNIT_ASSERT(NULL == m_p1->getStation());
}

void FPlanetTest::testOverwriteStation(){
	FVehicle *s1 = createShip("ArmedStation");
	m_p1->addStation(s1);
	CPPUNIT_ASSERT( m_p1->getStation() == s1 );
	FVehicle *s2 = createShip("Fortress");
	m_p1->addStation(s2);
	CPPUNIT_ASSERT( m_p1->getStation() == s2 );
}

void FPlanetTest::testSerialize(){
	delete m_p1;
	m_p1 = new FPlanet("Odie");
	FVehicle *s = createShip("FortifiedStation");
	m_p1->addStation(s);
	std::ofstream os("test",std::ios::binary);
	m_p1->save(os);
	os.close();
	FPlanet p2;
	std::ifstream is("test",std::ios::binary);
	p2.load(is);
	is.close();
	CPPUNIT_ASSERT( p2.getName() == "Odie" );
	CPPUNIT_ASSERT( p2.getStation()->getType() == "FortifiedStation");
	m_p1->destroyStation();
	std::ofstream os2("test",std::ios::binary);
	m_p1->save(os2);
	os2.close();
	FPlanet p3;
	std::ifstream is2("test",std::ios::binary);
	p3.load(is2);
	is2.close();
	CPPUNIT_ASSERT( p3.getName() == "Odie" );
	CPPUNIT_ASSERT( p3.getStation() == NULL );


}

}
