/**
 * @file FTacticalAttackIntegrationTest.h
 * @brief Header file for the FTacticalAttackIntegrationTest class
 */

#ifndef FTacticalAttackIntegrationTest_H_
#define FTacticalAttackIntegrationTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "weapons/FWeapon.h"

namespace FrontierTests {
using namespace Frontier;

class FTacticalAttackIntegrationTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE( FTacticalAttackIntegrationTest );
	CPPUNIT_TEST( testFireHitPreservesLegacyCleanupSideEffects );
	CPPUNIT_TEST( testFireCopiesAppliedHullDamageIntoAttackResult );
	CPPUNIT_TEST( testFireCopiesAppliedNonHullEffectsIntoAttackResult );
	CPPUNIT_TEST( testFireReportsConvertedADFHitAsHullDamage );
	CPPUNIT_TEST( testFireReportsDisastrousFireFallbackAsHullDamage );
	CPPUNIT_TEST_SUITE_END();

private:
	FWeapon *m_weapon;
	FVehicle *m_attacker;
	FVehicle *m_target;

public:
	void setUp();
	void tearDown();

	void testFireHitPreservesLegacyCleanupSideEffects();
	void testFireCopiesAppliedHullDamageIntoAttackResult();
	void testFireCopiesAppliedNonHullEffectsIntoAttackResult();
	void testFireReportsConvertedADFHitAsHullDamage();
	void testFireReportsDisastrousFireFallbackAsHullDamage();
};

}

#endif
