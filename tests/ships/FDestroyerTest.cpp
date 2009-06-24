/**
 * @file FDestroyerTest.cpp
 * @brief Implementation file for the FDestroyerTest class
 * @author Tom Stephens
 * @date Created: Jun 23, 2009
 */

#include "FDestroyerTest.h"

namespace FrontierTests {
using namespace Frontier;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FDestroyerTest );

void FDestroyerTest::setUp(){
	m_v1 = createShip("Destroyer");
}

void FDestroyerTest::tearDown(){
	delete m_v1;
}

void FDestroyerTest::testConstructor(){
	CPPUNIT_ASSERT( m_v1->getMaxADF() == 3);
	CPPUNIT_ASSERT( m_v1->getADF() == 3);
	CPPUNIT_ASSERT( m_v1->getMaxMR() == 3);
	CPPUNIT_ASSERT( m_v1->getMR() == 3);
	CPPUNIT_ASSERT( m_v1->getMaxDCR() == 75);
	CPPUNIT_ASSERT( m_v1->getDCR() == 75);
	CPPUNIT_ASSERT( m_v1->getMaxHP() == 50);
	CPPUNIT_ASSERT( m_v1->getHP() == 50);
	CPPUNIT_ASSERT( m_v1->getHeading() == 0);
	CPPUNIT_ASSERT( m_v1->getIcon() != NULL);
	unsigned int id = m_v1->getID();
	CPPUNIT_ASSERT( id >= 0);
	std::ostringstream os;
	os << "Destroyer " << id;
	CPPUNIT_ASSERT( m_v1->getName() == os.str());
	CPPUNIT_ASSERT( m_v1->getOwner() == 0);
	CPPUNIT_ASSERT( m_v1->getSpeed() == 0);
	CPPUNIT_ASSERT( m_v1->getType() == "Destroyer");
	CPPUNIT_ASSERT( m_v1->getWeaponCount() == 5);
	CPPUNIT_ASSERT( m_v1->getWeaponString() == "LC  LB  T(x2)  RB(x4)  EB  ");

	FDestroyer v;
}

}
