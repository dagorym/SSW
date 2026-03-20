/**
 * @file FWeaponFireResultTest.h
 * @brief Header file for the FWeaponFireResultTest class
 */

#ifndef FWeaponFireResultTest_H_
#define FWeaponFireResultTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "weapons/FWeapon.h"

namespace FrontierTests {
using namespace Frontier;

class FWeaponFireResultTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE( FWeaponFireResultTest );
	CPPUNIT_TEST( testFireReturnsStructuredSkippedStateWhenOutOfAmmo );
	CPPUNIT_TEST( testFireReturnsStructuredSkippedStateWhenWeaponDamaged );
	CPPUNIT_TEST( testFireReturnsStructuredSkippedStateWhenNoTargetOrRangeInvalid );
	CPPUNIT_TEST( testFireReturnsStructuredMissAndPreservesLegacyFireSideEffects );
	CPPUNIT_TEST_SUITE_END();

private:
	FWeapon *m_weapon;
	FVehicle *m_attacker;
	FVehicle *m_target;

public:
	void setUp();
	void tearDown();

	void testFireReturnsStructuredSkippedStateWhenOutOfAmmo();
	void testFireReturnsStructuredSkippedStateWhenWeaponDamaged();
	void testFireReturnsStructuredSkippedStateWhenNoTargetOrRangeInvalid();
	void testFireReturnsStructuredMissAndPreservesLegacyFireSideEffects();
};

}

#endif
