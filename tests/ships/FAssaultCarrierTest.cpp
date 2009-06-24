/**
 * @file FAssaultCarrierTest.cpp
 * @brief Implementation file for the FAssaultCarrierTest class
 * @author Tom Stephens
 * @date Created: Jun 22, 2009
 */

#include "FAssaultCarrierTest.h"

namespace FrontierTests {
using namespace Frontier;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FAssaultCarrierTest );

void FAssaultCarrierTest::setUp(){
	m_v1 = createShip("AssaultCarrier");
}

void FAssaultCarrierTest::tearDown(){
	delete m_v1;
}

void FAssaultCarrierTest::testConstructor(){
	CPPUNIT_ASSERT( m_v1->getMaxADF() == 2);
	CPPUNIT_ASSERT( m_v1->getADF() == 2);
	CPPUNIT_ASSERT( m_v1->getMaxMR() == 1);
	CPPUNIT_ASSERT( m_v1->getMR() == 1);
	CPPUNIT_ASSERT( m_v1->getMaxDCR() == 150);
	CPPUNIT_ASSERT( m_v1->getDCR() == 150);
	CPPUNIT_ASSERT( m_v1->getMaxHP() == 75);
	CPPUNIT_ASSERT( m_v1->getHP() == 75);
	CPPUNIT_ASSERT( m_v1->getHeading() == 0);
	CPPUNIT_ASSERT( m_v1->getIcon() != NULL);
	unsigned int id = m_v1->getID();
	CPPUNIT_ASSERT( id >= 0);
	std::ostringstream os;
	os << "Assault Carrier " << id;
	CPPUNIT_ASSERT( m_v1->getName() == os.str());
	CPPUNIT_ASSERT( m_v1->getOwner() == 0);
	CPPUNIT_ASSERT( m_v1->getSpeed() == 0);
	CPPUNIT_ASSERT( m_v1->getType() == "AssaultCarrier");
	CPPUNIT_ASSERT( m_v1->getWeaponCount() == 3);
	CPPUNIT_ASSERT( m_v1->getWeaponString() == "LB  PB  RB(x8)  ");

	FAssaultCarrier v;
}

}
