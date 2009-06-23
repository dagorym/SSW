/**
 * @file FSeekerMissileLauncherTest.cpp
 * @brief Implementation file for the FSeekerMissileLauncherTest class
 * @author Tom Stephens
 * @date Created: Jun 19, 2009
 */

#include "FSeekerMissileLauncherTest.h"

namespace FrontierTests {
using namespace Frontier;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FSeekerMissileLauncherTest );

void FSeekerMissileLauncherTest::setUp(){
	m_w1 = createWeapon(SM);
}

void FSeekerMissileLauncherTest::tearDown(){
	delete m_w1;
}

void FSeekerMissileLauncherTest::testConstructor(){
	CPPUNIT_ASSERT( m_w1->getName() == "SM" );
	CPPUNIT_ASSERT( m_w1->getLongName() == "Seeker Missile" );
	CPPUNIT_ASSERT( m_w1->getType() == SM );
	CPPUNIT_ASSERT( m_w1->getRange() == 0 );
	CPPUNIT_ASSERT( m_w1->getDice() == 5 );
	CPPUNIT_ASSERT( m_w1->getICMMod() == -8 );

	FSeekerMissileLauncher w;
	FSeekerMissileLauncher *w2 = new FSeekerMissileLauncher();
	delete w2;
}

}
