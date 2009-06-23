/**
 * @file FArmedStationTest.cpp
 * @brief Implementation file for the FArmedStationTest class
 * @author Tom Stephens
 * @date Created: Jun 22, 2009
 */

#include "FArmedStationTest.h"

namespace FrontierTests {
using namespace Frontier;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FArmedStationTest );

void FArmedStationTest::setUp(){
	m_v1 = createShip("ArmedStation");
}

void FArmedStationTest::tearDown(){
	delete m_v1;
}

void FArmedStationTest::testConstructor(){
	CPPUNIT_ASSERT( m_v1->getMaxADF() == 0);
	CPPUNIT_ASSERT( m_v1->getADF() == 0);
	CPPUNIT_ASSERT( m_v1->getMaxMR() == 0);
	CPPUNIT_ASSERT( m_v1->getMR() == 0);
	CPPUNIT_ASSERT( m_v1->getMaxDCR() == 75);
	CPPUNIT_ASSERT( m_v1->getDCR() == 75);
	CPPUNIT_ASSERT( m_v1->getMaxHP() == 80);
	CPPUNIT_ASSERT( m_v1->getHP() == 80);
	CPPUNIT_ASSERT( m_v1->getHeading() == 0);
	CPPUNIT_ASSERT( m_v1->getIcon() != NULL);
	unsigned int id = m_v1->getID();
	CPPUNIT_ASSERT( id >= 0);
	std::ostringstream os;
	os << "Armed Station " << id;
	CPPUNIT_ASSERT( m_v1->getName() == os.str());
	CPPUNIT_ASSERT( m_v1->getOwner() == 0);
	CPPUNIT_ASSERT( m_v1->getSpeed() == 0);
	CPPUNIT_ASSERT( m_v1->getType() == "ArmedStation");
	CPPUNIT_ASSERT( m_v1->getWeaponCount() == 2);
	CPPUNIT_ASSERT( m_v1->getWeaponString() == "LB  RB(x6)  ");

	FArmedStation v;
}

}
