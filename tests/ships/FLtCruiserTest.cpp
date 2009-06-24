/**
 * @file FLtCruiserTest.cpp
 * @brief Implementation file for the FLtCruiserTest class
 * @author Tom Stephens
 * @date Created: Jun 23, 2009
 */

#include "FLtCruiserTest.h"

namespace FrontierTests {
using namespace Frontier;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FLtCruiserTest );

void FLtCruiserTest::setUp(){
	m_v1 = createShip("LtCruiser");
}

void FLtCruiserTest::tearDown(){
	delete m_v1;
}

void FLtCruiserTest::testConstructor(){
	CPPUNIT_ASSERT( m_v1->getMaxADF() == 3);
	CPPUNIT_ASSERT( m_v1->getADF() == 3);
	CPPUNIT_ASSERT( m_v1->getMaxMR() == 2);
	CPPUNIT_ASSERT( m_v1->getMR() == 2);
	CPPUNIT_ASSERT( m_v1->getMaxDCR() == 100);
	CPPUNIT_ASSERT( m_v1->getDCR() == 100);
	CPPUNIT_ASSERT( m_v1->getMaxHP() == 70);
	CPPUNIT_ASSERT( m_v1->getHP() == 70);
	CPPUNIT_ASSERT( m_v1->getHeading() == 0);
	CPPUNIT_ASSERT( m_v1->getIcon() != NULL);
	unsigned int id = m_v1->getID();
	CPPUNIT_ASSERT( id >= 0);
	std::ostringstream os;
	os << "Light Cruiser " << id;
	CPPUNIT_ASSERT( m_v1->getName() == os.str());
	CPPUNIT_ASSERT( m_v1->getOwner() == 0);
	CPPUNIT_ASSERT( m_v1->getSpeed() == 0);
	CPPUNIT_ASSERT( m_v1->getType() == "LtCruiser");
	CPPUNIT_ASSERT( m_v1->getWeaponCount() == 6);
	CPPUNIT_ASSERT( m_v1->getWeaponString() == "DC  LB  T(x4)  RB(x6)  EB  PB  ");

	FLtCruiser v;
}

}
