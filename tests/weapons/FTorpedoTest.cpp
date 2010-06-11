/**
 * @file FTorpedoTest.cpp
 * @brief Implementation file for the FTorpedoTest class
 * @author Tom Stephens
 * @date Created: Jun 22, 2009
 */

#include "FTorpedoTest.h"

namespace FrontierTests {
using namespace Frontier;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FTorpedoTest );

void FTorpedoTest::setUp(){
	m_w1 = createWeapon(FWeapon::T);
}

void FTorpedoTest::tearDown(){
	delete m_w1;
}

void FTorpedoTest::testConstructor(){
	CPPUNIT_ASSERT( m_w1->getName() == "T" );
	CPPUNIT_ASSERT( m_w1->getLongName() == "Torpedo" );
	CPPUNIT_ASSERT( m_w1->getType() == FWeapon::T );
	CPPUNIT_ASSERT( m_w1->getRange() == 4 );
	CPPUNIT_ASSERT( m_w1->getDice() == 4 );
	CPPUNIT_ASSERT( m_w1->getICMMod() == -10 );
	CPPUNIT_ASSERT( m_w1->isMPO() == true );

	FTorpedo w;
	FTorpedo *w2 = new FTorpedo();
	delete w2;
}

}
