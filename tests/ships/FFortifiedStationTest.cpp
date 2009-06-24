/**
 * @file FFortifiedStationTest.cpp
 * @brief Implementation file for the FFortifiedStationTest class
 * @author Tom Stephens
 * @date Created: Jun 23, 2009
 */

#include "FFortifiedStationTest.h"

namespace FrontierTests {
using namespace Frontier;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FFortifiedStationTest );

void FFortifiedStationTest::setUp(){
	m_v1 = createShip("FortifiedStation");
}

void FFortifiedStationTest::tearDown(){
	delete m_v1;
}

void FFortifiedStationTest::testConstructor(){
	CPPUNIT_ASSERT( m_v1->getMaxADF() == 0);
	CPPUNIT_ASSERT( m_v1->getADF() == 0);
	CPPUNIT_ASSERT( m_v1->getMaxMR() == 0);
	CPPUNIT_ASSERT( m_v1->getMR() == 0);
	CPPUNIT_ASSERT( m_v1->getMaxDCR() == 100);
	CPPUNIT_ASSERT( m_v1->getDCR() == 100);
	CPPUNIT_ASSERT( m_v1->getMaxHP() == 140);
	CPPUNIT_ASSERT( m_v1->getHP() == 140);
	CPPUNIT_ASSERT( m_v1->getHeading() == 0);
	CPPUNIT_ASSERT( m_v1->getIcon() != NULL);
	unsigned int id = m_v1->getID();
	CPPUNIT_ASSERT( id >= 0);
	std::ostringstream os;
	os << "Fortified Station " << id;
	CPPUNIT_ASSERT( m_v1->getName() == os.str());
	CPPUNIT_ASSERT( m_v1->getOwner() == 0);
	CPPUNIT_ASSERT( m_v1->getSpeed() == 0);
	CPPUNIT_ASSERT( m_v1->getType() == "FortifiedStation");
	CPPUNIT_ASSERT( m_v1->getWeaponCount() == 3);
	CPPUNIT_ASSERT( m_v1->getWeaponString() == "LB  LB  RB(x8)  ");

	FFortifiedStation v;
}

}
