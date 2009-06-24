/**
 * @file FFighterTest.cpp
 * @brief Implementation file for the FFighterTest class
 * @author Tom Stephens
 * @date Created: Jun 23, 2009
 */

#include "FFighterTest.h"

namespace FrontierTests {
using namespace Frontier;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FFighterTest );

void FFighterTest::setUp(){
	m_v1 = createShip("Fighter");
}

void FFighterTest::tearDown(){
	delete m_v1;
}

void FFighterTest::testConstructor(){
	CPPUNIT_ASSERT( m_v1->getMaxADF() == 5);
	CPPUNIT_ASSERT( m_v1->getADF() == 5);
	CPPUNIT_ASSERT( m_v1->getMaxMR() == 5);
	CPPUNIT_ASSERT( m_v1->getMR() == 5);
	CPPUNIT_ASSERT( m_v1->getMaxDCR() == 30);
	CPPUNIT_ASSERT( m_v1->getDCR() == 30);
	CPPUNIT_ASSERT( m_v1->getMaxHP() == 8);
	CPPUNIT_ASSERT( m_v1->getHP() == 8);
	CPPUNIT_ASSERT( m_v1->getHeading() == 0);
	CPPUNIT_ASSERT( m_v1->getIcon() != NULL);
	unsigned int id = m_v1->getID();
	CPPUNIT_ASSERT( id >= 0);
	std::ostringstream os;
	os << "Fighter " << id;
	CPPUNIT_ASSERT( m_v1->getName() == os.str());
	CPPUNIT_ASSERT( m_v1->getOwner() == 0);
	CPPUNIT_ASSERT( m_v1->getSpeed() == 0);
	CPPUNIT_ASSERT( m_v1->getType() == "Fighter");
	CPPUNIT_ASSERT( m_v1->getWeaponCount() == 1);
	CPPUNIT_ASSERT( m_v1->getWeaponString() == "AR(x3)  ");

	FFighter v;
}

}
