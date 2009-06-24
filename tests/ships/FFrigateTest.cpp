/**
 * @file FFrigateTest.cpp
 * @brief Implementation file for the FFrigateTest class
 * @author Tom Stephens
 * @date Created: Jun 23, 2009
 */

#include "FFrigateTest.h"

namespace FrontierTests {
using namespace Frontier;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FFrigateTest );

void FFrigateTest::setUp(){
	m_v1 = createShip("Frigate");
}

void FFrigateTest::tearDown(){
	delete m_v1;
}

void FFrigateTest::testConstructor(){
	CPPUNIT_ASSERT( m_v1->getMaxADF() == 4);
	CPPUNIT_ASSERT( m_v1->getADF() == 4);
	CPPUNIT_ASSERT( m_v1->getMaxMR() == 3);
	CPPUNIT_ASSERT( m_v1->getMR() == 3);
	CPPUNIT_ASSERT( m_v1->getMaxDCR() == 70);
	CPPUNIT_ASSERT( m_v1->getDCR() == 70);
	CPPUNIT_ASSERT( m_v1->getMaxHP() == 40);
	CPPUNIT_ASSERT( m_v1->getHP() == 40);
	CPPUNIT_ASSERT( m_v1->getHeading() == 0);
	CPPUNIT_ASSERT( m_v1->getIcon() != NULL);
	unsigned int id = m_v1->getID();
	CPPUNIT_ASSERT( id >= 0);
	std::ostringstream os;
	os << "Frigate " << id;
	CPPUNIT_ASSERT( m_v1->getName() == os.str());
	CPPUNIT_ASSERT( m_v1->getOwner() == 0);
	CPPUNIT_ASSERT( m_v1->getSpeed() == 0);
	CPPUNIT_ASSERT( m_v1->getType() == "Frigate");
	CPPUNIT_ASSERT( m_v1->getWeaponCount() == 4);
	CPPUNIT_ASSERT( m_v1->getWeaponString() == "LC  LB  T(x2)  RB(x4)  ");

	FFrigate v;
}

}
