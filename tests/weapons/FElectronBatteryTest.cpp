/**
 * @file FElectronBatteryTest.cpp
 * @brief Implementation file for the FElectronBatteryTest class
 * @author Tom Stephens
 * @date Created: Jun 19, 2009
 */

#include "FElectronBatteryTest.h"

namespace FrontierTests {
using namespace Frontier;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FElectronBatteryTest );

void FElectronBatteryTest::setUp(){
	m_w1 = createWeapon(FWeapon::EB);
}

void FElectronBatteryTest::tearDown(){
	delete m_w1;
}

void FElectronBatteryTest::testConstructor(){
	CPPUNIT_ASSERT( m_w1->getName() == "EB" );
	CPPUNIT_ASSERT( m_w1->getLongName() == "Electron Battery" );
	CPPUNIT_ASSERT( m_w1->getType() == FWeapon::EB );
	CPPUNIT_ASSERT( m_w1->getRange() == 12 );
	CPPUNIT_ASSERT( m_w1->getDice() == 1 );
	CPPUNIT_ASSERT( m_w1->getDamageMod() == 0 );
	CPPUNIT_ASSERT( m_w1->isRD() == true );

	FElectronBattery w;
	FElectronBattery *w2 = new FElectronBattery();
	delete w2;
}

}
