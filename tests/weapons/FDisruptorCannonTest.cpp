/**
 * @file FDisruptorCannonTest.cpp
 * @brief Implementation file for the FDisruptorCannonTest class
 * @author Tom Stephens, Claude Sonnet 5 (medium)
 * @date Created: Jun 19, 2009
 * @date Last Modified: Jul 11, 2026
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
	// P2-1 (T1): tactical_operations_manual.md RA 9 (weapon text l.951; Weapon Restrictions
	// Table l.1145) requires Disruptor Cannon range 9, not the prior defective 12.
	CPPUNIT_ASSERT( m_w1->getRange() == 9 );
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

void FDisruptorCannonTest::testSetTargetAcceptsRangeAtMax() {
	// P2-1 AC: setTarget(target, 9, ...) accepts the target since 9 is now the max range.
	FVehicle v;
	m_w1->setParent(&v);
	m_w1->setTarget(&v, 9, true);
	CPPUNIT_ASSERT( m_w1->getTarget() == &v );
	CPPUNIT_ASSERT( m_w1->getTargetRange() == 9 );
}

void FDisruptorCannonTest::testSetTargetRejectsRangeBeyondMax() {
	// P2-1 AC: setTarget(target, r, ...) for r in 10..12 rejects the target now that
	// m_range is 9 (previously accepted up to 12 under the defective range).
	FVehicle v;
	m_w1->setParent(&v);

	m_w1->setTarget(&v, 10, true);
	CPPUNIT_ASSERT( m_w1->getTarget() == NULL );
	CPPUNIT_ASSERT( m_w1->getTargetRange() == -1 );

	m_w1->setTarget(&v, 11, true);
	CPPUNIT_ASSERT( m_w1->getTarget() == NULL );
	CPPUNIT_ASSERT( m_w1->getTargetRange() == -1 );

	m_w1->setTarget(&v, 12, true);
	CPPUNIT_ASSERT( m_w1->getTarget() == NULL );
	CPPUNIT_ASSERT( m_w1->getTargetRange() == -1 );
}

}
