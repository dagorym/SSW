/**
 * @file FMineLauncherTest.cpp
 * @brief Implementation file for the FMineLauncherTest class
 * @author Tom Stephens
 * @date Created: Jun 19, 2009
 */

#include "FMineLauncherTest.h"

namespace FrontierTests {
using namespace Frontier;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FMineLauncherTest );

void FMineLauncherTest::setUp(){
	m_w1 = createWeapon(M);
}

void FMineLauncherTest::tearDown(){
	delete m_w1;
}

void FMineLauncherTest::testConstructor(){
	CPPUNIT_ASSERT( m_w1->getName() == "M" );
	CPPUNIT_ASSERT( m_w1->getLongName() == "Mine" );
	CPPUNIT_ASSERT( m_w1->getType() == M );
	CPPUNIT_ASSERT( m_w1->getRange() == 0 );
	CPPUNIT_ASSERT( m_w1->getDice() == 3 );
	CPPUNIT_ASSERT( m_w1->getDamageMod() == 5);
	CPPUNIT_ASSERT( m_w1->getICMMod() == -5 );

	FMineLauncher w;
	FMineLauncher *w2 = new FMineLauncher();
	delete w2;
}

}
