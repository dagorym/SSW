/**
 * @file FProtonBatteryTest.cpp
 * @brief Implementation file for the FProtonBatteryTest class
 * @author Tom Stephens
 * @date Created: Jun 19, 2009
 */

#include "FProtonBatteryTest.h"

namespace FrontierTests {
using namespace Frontier;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FProtonBatteryTest );

void FProtonBatteryTest::setUp(){
	m_w1 = createWeapon(PB);
}

void FProtonBatteryTest::tearDown(){
	delete m_w1;
}

void FProtonBatteryTest::testConstructor(){
	CPPUNIT_ASSERT( m_w1->getName() == "PB" );
	CPPUNIT_ASSERT( m_w1->getLongName() == "Proton Battery" );
	CPPUNIT_ASSERT( m_w1->getType() == PB );
	CPPUNIT_ASSERT( m_w1->getRange() == 12 );
	CPPUNIT_ASSERT( m_w1->getDice() == 1 );
	CPPUNIT_ASSERT( m_w1->getDamageMod() == 0 );
	CPPUNIT_ASSERT( m_w1->isRD() == true );

	FProtonBattery w;
	FProtonBattery *w2 = new FProtonBattery();
	delete w2;
}

}
