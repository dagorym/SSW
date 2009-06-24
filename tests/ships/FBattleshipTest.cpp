/**
 * @file FBattleshipTest.cpp
 * @brief Implementation file for the FBattleshipTest class
 * @author Tom Stephens
 * @date Created: Jun 23, 2009
 */

#include "FBattleshipTest.h"

namespace FrontierTests {
using namespace Frontier;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FBattleshipTest );

void FBattleshipTest::setUp(){
	m_v1 = createShip("Battleship");
}

void FBattleshipTest::tearDown(){
	delete m_v1;
}

void FBattleshipTest::testConstructor(){
	CPPUNIT_ASSERT( m_v1->getMaxADF() == 2);
	CPPUNIT_ASSERT( m_v1->getADF() == 2);
	CPPUNIT_ASSERT( m_v1->getMaxMR() == 2);
	CPPUNIT_ASSERT( m_v1->getMR() == 2);
	CPPUNIT_ASSERT( m_v1->getMaxDCR() == 200);
	CPPUNIT_ASSERT( m_v1->getDCR() == 200);
	CPPUNIT_ASSERT( m_v1->getMaxHP() == 120);
	CPPUNIT_ASSERT( m_v1->getHP() == 120);
	CPPUNIT_ASSERT( m_v1->getHeading() == 0);
	CPPUNIT_ASSERT( m_v1->getIcon() != NULL);
	unsigned int id = m_v1->getID();
	CPPUNIT_ASSERT( id >= 0);
	std::ostringstream os;
	os << "Battleship " << id;
	CPPUNIT_ASSERT( m_v1->getName() == os.str());
	CPPUNIT_ASSERT( m_v1->getOwner() == 0);
	CPPUNIT_ASSERT( m_v1->getSpeed() == 0);
	CPPUNIT_ASSERT( m_v1->getType() == "Battleship");
	CPPUNIT_ASSERT( m_v1->getWeaponCount() == 10);
	CPPUNIT_ASSERT( m_v1->getWeaponString() == "DC  LB  LB  LB  T(x8)  RB(x10)  EB  EB  PB  SM(x4)  ");

	FBattleship v;
}

}
