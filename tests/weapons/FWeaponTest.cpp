/**
 * @file FWeaponTest.cpp
 * @brief Implementation file for the FWeaponTest class
 * @author Tom Stephens
 * @date Created: DATE
 */

#include "FWeaponTest.h"
#include <cstdio>
#include "ships/FVehicle.h"
#include "ships/FAssaultScout.h"

namespace FrontierTests {
using namespace Frontier;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FWeaponTest );

void FWeaponTest::setUp(){
	m_w1 = new FWeapon();
}

void FWeaponTest::tearDown(){
	delete m_w1;
}

void FWeaponTest::testConstructor(){
	CPPUNIT_ASSERT( m_w1->getAmmo() == 0 );
	CPPUNIT_ASSERT( m_w1->getDamageMod() == 0 );
	CPPUNIT_ASSERT( m_w1->getDice() == 0 );
	CPPUNIT_ASSERT( m_w1->getICMMod() == 0 );
	CPPUNIT_ASSERT( m_w1->getLongName() == "Not a Weapon" );
	CPPUNIT_ASSERT( m_w1->getMaxAmmo() == 0 );
	CPPUNIT_ASSERT( m_w1->getRange() == 0 );
	CPPUNIT_ASSERT( m_w1->getTarget() == NULL );
	CPPUNIT_ASSERT( m_w1->getTargetRange() == -1 );
	CPPUNIT_ASSERT( m_w1->getType() == FWeapon::NONE );
	CPPUNIT_ASSERT( m_w1->hasAmmo() == false );
	CPPUNIT_ASSERT( m_w1->isDamaged() == false );
	CPPUNIT_ASSERT( m_w1->isFF() == false );
	CPPUNIT_ASSERT( m_w1->isMPO() == false );
	CPPUNIT_ASSERT( m_w1->isRD() == false );
	CPPUNIT_ASSERT( m_w1->getName() == "NAW" );
}

void FWeaponTest::setMaxAmmo(){
	m_w1->setMaxAmmo(3);
	CPPUNIT_ASSERT( m_w1->getMaxAmmo() == 3 );
//	CPPUNIT_ASSERT( m_w1->getAmmo() == 3 );
//	CPPUNIT_ASSERT( m_w1->getName() == "NAW(x3)" );
	//CPPUNIT_ASSERT( m_w1 );
	//CPPUNIT_ASSERT( m_w1 );
}

void FWeaponTest::setCurrentAmmo(){
	m_w1->setMaxAmmo(3);
	m_w1->setCurrentAmmo(1);
	CPPUNIT_ASSERT( m_w1->getAmmo() == 1 );
	m_w1->setCurrentAmmo(3);
	CPPUNIT_ASSERT( m_w1->getAmmo() == 3 );
	m_w1->setCurrentAmmo(4);
	CPPUNIT_ASSERT( m_w1->getAmmo() == 3 );
}

void FWeaponTest::checkAddAmmoToName(){
	m_w1->setMaxAmmo(3);
	m_w1->setCurrentAmmo(3);
	CPPUNIT_ASSERT( m_w1->getAmmo() == 3 );
	CPPUNIT_ASSERT( m_w1->getName() == "NAW(x3)" );
	//CPPUNIT_ASSERT( m_w1 );
	//CPPUNIT_ASSERT( m_w1 );
}

void FWeaponTest::reloadTest(){
	m_w1->setMaxAmmo(3);
	m_w1->reload();
	CPPUNIT_ASSERT( m_w1->getAmmo() == 3 );
}

void FWeaponTest::setDamageStatus(){
	m_w1->setDamageStatus(true);
	CPPUNIT_ASSERT( m_w1->isDamaged() == true );
}

void FWeaponTest::testSerialize(){
	m_w1->setMaxAmmo(4);
	m_w1->setCurrentAmmo(1);
	m_w1->setDamageStatus(true);

	const char *filename = "FWeaponTest.tmp";
	std::remove(filename);
	std::ofstream os(filename,std::ios::binary);
	m_w1->save(os);
	os.close();
	FWeapon w2;
	std::ifstream is(filename,std::ios::binary);
	unsigned int type;
	is.read((char*)&type,sizeof(unsigned int));
	w2.load(is);
	is.close();
	std::remove(filename);
	CPPUNIT_ASSERT( w2.getMaxAmmo() == 4 );
	CPPUNIT_ASSERT( w2.getAmmo() == 1 );
	CPPUNIT_ASSERT( w2.isDamaged() == true );
}

void FWeaponTest::testBaseWeaponZeroRange(){
	// Base FWeapon has range=0: out-of-range target assignment is rejected
	FVehicle *v = new FVehicle;
	m_w1->setTarget(v,3,true);
	CPPUNIT_ASSERT( m_w1->getRange() == 0 );
	CPPUNIT_ASSERT( m_w1->getTarget() == NULL );
	CPPUNIT_ASSERT( m_w1->getTargetRange() == -1 );

	// Concrete weapon (LB, range=9) accepts range-3 target and fires successfully
	// TASR_InvalidTargetRange: target set but range < 0, ammo available
	FWeapon *lb = createWeapon(FWeapon::LB);
	lb->setMaxAmmo(1);
	lb->setCurrentAmmo(1);
	lb->setTarget(v, -1, true);  // range -1 satisfies setTarget guard (-1 <= 9) but is invalid for fire
	FTacticalAttackResult invalidResult = lb->fire();
	CPPUNIT_ASSERT( invalidResult.skipReason == TASR_InvalidTargetRange );
	delete lb;
	delete v;
}

void FWeaponTest::testSetTarget(){
	FVehicle *v = new FVehicle;
	m_w1->setTarget(v,3,true);
	CPPUNIT_ASSERT( m_w1->getTarget() == NULL );
	CPPUNIT_ASSERT( m_w1->getTargetRange() == -1 );
	m_w1->setTarget(v,0,true);
	CPPUNIT_ASSERT( m_w1->getTarget() == v );
	CPPUNIT_ASSERT( m_w1->getTargetRange() == 0 );
	m_w1->setTarget(NULL,0,false);
	CPPUNIT_ASSERT( m_w1->getTarget() == NULL );
	CPPUNIT_ASSERT( m_w1->getTargetRange() == 0 );
	delete v;
}

void FWeaponTest::testFireWhenEmpty(){
	m_w1->setMaxAmmo(1);
	m_w1->fire();
}

void FWeaponTest::testFireNoTarget(){
	m_w1->fire();
}

void FWeaponTest::testFireAtTarget(){
	FVehicle *v = new FAssaultScout;
	FWeapon *w = createWeapon(FWeapon::LB);
	w->setParent(v);
	w->setTarget(v,3,true);
	w->setMaxAmmo(3);
	w->setCurrentAmmo(3);
	FTacticalAttackResult firstFireResult = w->fire();
	CPPUNIT_ASSERT( firstFireResult.skipReason == TASR_None );
	CPPUNIT_ASSERT( firstFireResult.fired() == true );
	CPPUNIT_ASSERT( w->getMaxAmmo() == 3);
	CPPUNIT_ASSERT( w->getAmmo() == 2);
	CPPUNIT_ASSERT( w->getTarget() == NULL);
	CPPUNIT_ASSERT( w->getTargetRange() == -1);
	w->setTarget(v,3,true);
	w->fire();
	CPPUNIT_ASSERT( w->getAmmo() == 1);
	CPPUNIT_ASSERT( w->getTargetRange() == -1);
	w->setTarget(v,3,true);
	w->fire();
	CPPUNIT_ASSERT( w->getAmmo() == 0);
	CPPUNIT_ASSERT( w->getTargetRange() == -1);
	CPPUNIT_ASSERT( w->getTarget() == NULL);
	w->setTarget(v,3,true);
	FTacticalAttackResult noAmmoResult = w->fire();
	CPPUNIT_ASSERT( noAmmoResult.skipReason == TASR_NoAmmo );
	CPPUNIT_ASSERT( w->getAmmo() == 0);
	CPPUNIT_ASSERT( w->getTarget() == v);
	CPPUNIT_ASSERT( w->getTargetRange() == 3);
	w->setTarget(v,-1,true);
	FTacticalAttackResult invalidRangeResult = w->fire();
	// ammo=0 check comes before targetRange check in fire(), so NoAmmo is returned
	CPPUNIT_ASSERT( invalidRangeResult.skipReason == TASR_NoAmmo );
	CPPUNIT_ASSERT( w->getAmmo() == 0);
	CPPUNIT_ASSERT( w->getTarget() == v);
	CPPUNIT_ASSERT( w->getTargetRange() == -1);
	delete w;
	delete v;
}

void FWeaponTest::testBadIdentifier(){
	m_w1 = createWeapon(FWeapon::NONE);
	CPPUNIT_ASSERT( m_w1 == NULL );
}


}
