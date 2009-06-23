/**
 * @file FLaserCannonTest.cpp
 * @brief Implementation file for the FLaserCannonTest class
 * @author Tom Stephens
 * @date Created: Jun 19, 2009
 */

#include "FLaserCannonTest.h"

namespace FrontierTests {
using namespace Frontier;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FLaserCannonTest );

void FLaserCannonTest::setUp(){
	m_w1 = createWeapon(LC);
}

void FLaserCannonTest::tearDown(){
	delete m_w1;
}

void FLaserCannonTest::testConstructor(){
	CPPUNIT_ASSERT( m_w1->getName() == "LC" );
	CPPUNIT_ASSERT( m_w1->getLongName() == "Laser Cannon" );
	CPPUNIT_ASSERT( m_w1->getType() == LC );
	CPPUNIT_ASSERT( m_w1->getRange() == 10 );
	CPPUNIT_ASSERT( m_w1->getDice() == 2 );
	CPPUNIT_ASSERT( m_w1->isRD() == true );
	CPPUNIT_ASSERT( m_w1->isFF() == true );

	FLaserCannon w;
	FLaserCannon *w2 = new FLaserCannon();
	delete w2;
}

}
