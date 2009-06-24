/**
 * @file FFortressTest.cpp
 * @brief Implementation file for the FFortressTest class
 * @author Tom Stephens
 * @date Created: Jun 23, 2009
 */

#include "FFortressTest.h"

namespace FrontierTests {
using namespace Frontier;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FFortressTest );

void FFortressTest::setUp(){
	m_v1 = createShip("Fortress");
}

void FFortressTest::tearDown(){
	delete m_v1;
}

void FFortressTest::testConstructor(){
	CPPUNIT_ASSERT( m_v1->getMaxADF() == 0);
	CPPUNIT_ASSERT( m_v1->getADF() == 0);
	CPPUNIT_ASSERT( m_v1->getMaxMR() == 0);
	CPPUNIT_ASSERT( m_v1->getMR() == 0);
	CPPUNIT_ASSERT( m_v1->getMaxDCR() == 200);
	CPPUNIT_ASSERT( m_v1->getDCR() == 200);
	CPPUNIT_ASSERT( m_v1->getMaxHP() == 300);
	CPPUNIT_ASSERT( m_v1->getHP() == 300);
	CPPUNIT_ASSERT( m_v1->getHeading() == 0);
	CPPUNIT_ASSERT( m_v1->getIcon() != NULL);
	unsigned int id = m_v1->getID();
	CPPUNIT_ASSERT( id >= 0);
	std::ostringstream os;
	os << "Fortress " << id;
	CPPUNIT_ASSERT( m_v1->getName() == os.str());
	CPPUNIT_ASSERT( m_v1->getOwner() == 0);
	CPPUNIT_ASSERT( m_v1->getSpeed() == 0);
	CPPUNIT_ASSERT( m_v1->getType() == "Fortress");
	CPPUNIT_ASSERT( m_v1->getWeaponCount() == 6);
	CPPUNIT_ASSERT( m_v1->getWeaponString() == "LB  LB  LB  EB  PB  RB(x12)  ");

	FFortress v;
}

}
