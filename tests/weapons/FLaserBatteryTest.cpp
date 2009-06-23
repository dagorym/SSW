/**
 * @file FLaserBatteryTest.cpp
 * @brief Implementation file for the FLaserBatteryTest class
 * @author Tom Stephens
 * @date Created: Jun 19, 2009
 */

#include "FLaserBatteryTest.h"

namespace FrontierTests {
using namespace Frontier;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FLaserBatteryTest );

void FLaserBatteryTest::setUp(){
	m_w1 = createWeapon(LB);
}

void FLaserBatteryTest::tearDown(){
	delete m_w1;
}

void FLaserBatteryTest::testConstructor(){
	CPPUNIT_ASSERT( m_w1->getName() == "LB" );
	CPPUNIT_ASSERT( m_w1->getLongName() == "Laser Battery" );
	CPPUNIT_ASSERT( m_w1->getType() == LB );
	CPPUNIT_ASSERT( m_w1->getRange() == 9 );
	CPPUNIT_ASSERT( m_w1->getDice() == 1 );
	CPPUNIT_ASSERT( m_w1->getDamageMod() == 0 );
	CPPUNIT_ASSERT( m_w1->isRD() == true );

	FLaserBattery w;
	FLaserBattery *w2 = new FLaserBattery();
	delete w2;
}

}
