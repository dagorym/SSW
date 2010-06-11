/**
 * @file FDisruptorCannonTest.cpp
 * @brief Implementation file for the FDisruptorCannonTest class
 * @author Tom Stephens
 * @date Created: Jun 19, 2009
 */

#include "FDisruptorCannonTest.h"
#include "ships/FVehicle.h"

namespace FrontierTests {
using namespace Frontier;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FDisruptorCannonTest );

void FDisruptorCannonTest::setUp(){
	m_w1 = createWeapon(FWeapon::DC);
}

void FDisruptorCannonTest::tearDown(){
	delete m_w1;
}

void FDisruptorCannonTest::testConstructor(){
	CPPUNIT_ASSERT( m_w1->getName() == "DC" );
	CPPUNIT_ASSERT( m_w1->getLongName() == "Disruptor Cannon" );
	CPPUNIT_ASSERT( m_w1->getType() == FWeapon::DC );
	CPPUNIT_ASSERT( m_w1->getRange() == 12 );
	CPPUNIT_ASSERT( m_w1->getDice() == 3 );
	CPPUNIT_ASSERT( m_w1->getDamageMod() == 0 );
	CPPUNIT_ASSERT( m_w1->isRD() == true );
	CPPUNIT_ASSERT( m_w1->isFF() == true );

	FDisruptorCannon w;
	FDisruptorCannon *w2 = new FDisruptorCannon();
	delete w2;
}

void FDisruptorCannonTest::testFireCoverage() {
	FVehicle v;
	m_w1->setParent(&v);
	m_w1->setTarget(&v,0,true);
	m_w1->fire();
	m_w1->setTarget(&v,0,true);
	m_w1->fire();
	m_w1->setTarget(&v,0,true);
	m_w1->fire();
	m_w1->setTarget(&v,0,true);
	m_w1->fire();
	m_w1->setTarget(&v,0,true);
	m_w1->fire();
}

}
