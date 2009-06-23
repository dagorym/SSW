/**
 * @file FRocketBatteryTest.cpp
 * @brief Implementation file for the FRocketBatteryTest class
 * @author Tom Stephens
 * @date Created: Jun 19, 2009
 */

#include "FRocketBatteryTest.h"

namespace FrontierTests {
using namespace Frontier;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FRocketBatteryTest );

void FRocketBatteryTest::setUp(){
	m_w1 = createWeapon(RB);
}

void FRocketBatteryTest::tearDown(){
	delete m_w1;
}

void FRocketBatteryTest::testConstructor(){
	CPPUNIT_ASSERT( m_w1->getName() == "RB" );
	CPPUNIT_ASSERT( m_w1->getLongName() == "Rocket Battery" );
	CPPUNIT_ASSERT( m_w1->getType() == RB );
	CPPUNIT_ASSERT( m_w1->getRange() == 3 );
	CPPUNIT_ASSERT( m_w1->getDice() == 2 );
	CPPUNIT_ASSERT( m_w1->getICMMod() == -3 );

	FRocketBattery w;
	FRocketBattery *w2 = new FRocketBattery();
	delete w2;
}

}
