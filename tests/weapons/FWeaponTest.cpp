/**
 * @file FWeaponTest.cpp
 * @brief Implementation file for the FWeaponTest class
 * @author Tom Stephens
 * @date Created: DATE
 */

#include "FWeaponTest.h"
#include "ships/FVehicle.h"

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
	CPPUNIT_ASSERT( m_w1->getType() == NONE );
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

	std::ofstream os("test",std::ios::binary);
	m_w1->save(os);
	os.close();
	FWeapon w2;
	std::ifstream is("test",std::ios::binary);
	unsigned int type;
	is.read((char*)&type,sizeof(unsigned int));
	w2.load(is);
	is.close();
	CPPUNIT_ASSERT( w2.getMaxAmmo() == 4 );
	CPPUNIT_ASSERT( w2.getAmmo() == 1 );
	CPPUNIT_ASSERT( w2.isDamaged() == true );
}

void FWeaponTest::testSetTarget(){
	FVehicle *v = new FVehicle;
	m_w1->setTarget(v,3,true);
	CPPUNIT_ASSERT( m_w1->getTarget() == v );
	CPPUNIT_ASSERT( m_w1->getTargetRange() == 3);
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
	FVehicle *v = new FVehicle;
	m_w1->setTarget(v,3,true);
//	m_w1->fire();
	m_w1->setMaxAmmo(3);
	m_w1->setCurrentAmmo(3);
	m_w1->fire();
	CPPUNIT_ASSERT( m_w1->getMaxAmmo() == 3);
	CPPUNIT_ASSERT( m_w1->getAmmo() == 2);
	CPPUNIT_ASSERT( m_w1->getTarget() == NULL);
	CPPUNIT_ASSERT( m_w1->getTargetRange() == -1);
	m_w1->setTarget(v,3,true);
	m_w1->fire();
	CPPUNIT_ASSERT( m_w1->getAmmo() == 1);
	CPPUNIT_ASSERT( m_w1->getTargetRange() == -1);
	m_w1->setTarget(v,3,true);
	m_w1->fire();
	CPPUNIT_ASSERT( m_w1->getAmmo() == 0);
	CPPUNIT_ASSERT( m_w1->getTargetRange() == -1);
	m_w1->setTarget(v,3,true);
	m_w1->fire();
	CPPUNIT_ASSERT( m_w1->getAmmo() == 0);
	CPPUNIT_ASSERT( m_w1->getTarget() == v);
	CPPUNIT_ASSERT( m_w1->getTargetRange() == 3);
	delete v;
}

void FWeaponTest::testBadIdentifier(){
	m_w1 = createWeapon(-1);
	CPPUNIT_ASSERT( m_w1 == NULL );
}


}
