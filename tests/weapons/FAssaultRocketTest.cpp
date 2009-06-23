/**
 * @file FAssaultRocketTest.cpp
 * @brief Implementation file for the FAssaultRocketTest class
 * @author Tom Stephens
 * @date Created: Jun 19, 2009
 */

#include "FAssaultRocketTest.h"

namespace FrontierTests {
using namespace Frontier;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FAssaultRocketTest );

void FAssaultRocketTest::setUp(){
	m_w1 = createWeapon(AR);
}

void FAssaultRocketTest::tearDown(){
	delete m_w1;
}

void FAssaultRocketTest::testConstructor(){
	CPPUNIT_ASSERT( m_w1->getName() == "AR" );
	CPPUNIT_ASSERT( m_w1->getLongName() == "Assault Rocket" );
	CPPUNIT_ASSERT( m_w1->getType() == AR );
	CPPUNIT_ASSERT( m_w1->getRange() == 4 );
	CPPUNIT_ASSERT( m_w1->getDice() == 2 );
	CPPUNIT_ASSERT( m_w1->getDamageMod() == 4 );
	CPPUNIT_ASSERT( m_w1->isMPO() == true );
	CPPUNIT_ASSERT( m_w1->isFF() == true );
	CPPUNIT_ASSERT( m_w1->getICMMod() == -5 );

	FAssaultRocket w;
	FAssaultRocket *w2 = new FAssaultRocket();
	delete w2;
}

}
