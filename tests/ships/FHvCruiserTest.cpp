/**
 * @file FHvCruiserTest.cpp
 * @brief Implementation file for the FHvCruiserTest class
 * @author Tom Stephens
 * @date Created: Jun 23, 2009
 */

#include "FHvCruiserTest.h"

namespace FrontierTests {
using namespace Frontier;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FHvCruiserTest );

void FHvCruiserTest::setUp(){
	m_v1 = createShip("HvCruiser");
}

void FHvCruiserTest::tearDown(){
	delete m_v1;
}

void FHvCruiserTest::testConstructor(){
	CPPUNIT_ASSERT( m_v1->getMaxADF() == 2);
	CPPUNIT_ASSERT( m_v1->getADF() == 2);
	CPPUNIT_ASSERT( m_v1->getMaxMR() == 1);
	CPPUNIT_ASSERT( m_v1->getMR() == 1);
	CPPUNIT_ASSERT( m_v1->getMaxDCR() == 120);
	CPPUNIT_ASSERT( m_v1->getDCR() == 120);
	CPPUNIT_ASSERT( m_v1->getMaxHP() == 80);
	CPPUNIT_ASSERT( m_v1->getHP() == 80);
	CPPUNIT_ASSERT( m_v1->getHeading() == 0);
	CPPUNIT_ASSERT( m_v1->getIcon() != NULL);
	unsigned int id = m_v1->getID();
	CPPUNIT_ASSERT( id >= 0);
	std::ostringstream os;
	os << "Heavy Cruiser " << id;
	CPPUNIT_ASSERT( m_v1->getName() == os.str());
	CPPUNIT_ASSERT( m_v1->getOwner() == 0);
	CPPUNIT_ASSERT( m_v1->getSpeed() == 0);
	CPPUNIT_ASSERT( m_v1->getType() == "HvCruiser");
	CPPUNIT_ASSERT( m_v1->getWeaponCount() == 8);
	CPPUNIT_ASSERT( m_v1->getWeaponString() == "DC  LB  LB  T(x4)  RB(x8)  EB  PB  SM(x2)  ");

	FHvCruiser v;
}

}
