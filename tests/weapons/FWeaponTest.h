/**
 * @file FWeaponTest.h
 * @brief Header file for the FWeaponTest class
 * @author Tom Stephens
 * @date Created: Jun 18, 2009
 */

#ifndef FWeaponTest_H_
#define FWeaponTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "weapons/FWeapon.h"

namespace FrontierTests {
using namespace Frontier;

class FWeaponTest : public CppUnit::TestFixture{
	CPPUNIT_TEST_SUITE( FWeaponTest );
	CPPUNIT_TEST( testConstructor );
	CPPUNIT_TEST( setMaxAmmo );
	CPPUNIT_TEST( setCurrentAmmo );
	CPPUNIT_TEST( checkAddAmmoToName );
	CPPUNIT_TEST( reloadTest );
	CPPUNIT_TEST( setDamageStatus );
	CPPUNIT_TEST( testSerialize );
	CPPUNIT_TEST( testSetTarget );
	CPPUNIT_TEST( testFireWhenEmpty );
	CPPUNIT_TEST( testFireNoTarget );
	CPPUNIT_TEST( testFireAtTarget );
	CPPUNIT_TEST( testBadIdentifier );
	CPPUNIT_TEST_SUITE_END();

private:
	FWeapon *m_w1;

public:
	void setUp();
	void tearDown();

	void testConstructor();
	void setMaxAmmo();
	void setCurrentAmmo();
	void checkAddAmmoToName();
	void reloadTest();
	void setDamageStatus();
	void testSerialize();
	void testSetTarget();
	void testFireWhenEmpty();
	void testFireNoTarget();
	void testFireAtTarget();
	void testBadIdentifier();
};

}

#endif /* FWeaponTest_H_ */
