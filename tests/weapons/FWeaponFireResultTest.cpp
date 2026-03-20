/**
 * @file FWeaponFireResultTest.cpp
 * @brief Implementation file for the FWeaponFireResultTest class
 */

#include "FWeaponFireResultTest.h"

#include <cstdlib>

#include "ships/FVehicle.h"

namespace FrontierTests {
using namespace Frontier;

namespace {

class FWeaponFireHarness : public FWeapon {
public:
	FWeaponFireHarness() {
		m_name = "TST";
		m_fullName = "Test Weapon";
		m_type = FWeapon::LB;
		m_range = 10;
		m_nDice = 1;
		m_dMod = 0;
		m_baseToHitProb = 0;
	}

	void setBaseToHit(int probability) { m_baseToHitProb = probability; }
	void setDamageTableModifier(int modifier) { m_damageTableMod = modifier; }
	void assignTargetDirectly(FVehicle *target, int range, bool headOn = false) {
		m_target = target;
		m_targetRange = range;
		m_isHeadOn = headOn;
	}
};

}

CPPUNIT_TEST_SUITE_REGISTRATION( FWeaponFireResultTest );

void FWeaponFireResultTest::setUp() {
	m_weapon = new FWeaponFireHarness();
	m_attacker = new FVehicle();
	m_target = new FVehicle();
	m_attacker->setName("Attacker");
	m_target->setName("Target");
	m_weapon->setParent(m_attacker);
}

void FWeaponFireResultTest::tearDown() {
	delete m_weapon;
	delete m_attacker;
	delete m_target;
}

void FWeaponFireResultTest::testFireReturnsStructuredSkippedStateWhenOutOfAmmo() {
	// AC: skipped attacks report a not-fired outcome and preserve legacy no-fire side effects.
	m_weapon->setMaxAmmo(1);
	m_weapon->setCurrentAmmo(0);
	static_cast<FWeaponFireHarness *>(m_weapon)->assignTargetDirectly(m_target, 3);

	FTacticalAttackResult result = m_weapon->fire();

	CPPUNIT_ASSERT(result.outcome == TAO_NotFired);
	CPPUNIT_ASSERT(result.skipReason == TASR_NoAmmo);
	CPPUNIT_ASSERT(!result.fired());
	CPPUNIT_ASSERT(!result.hit());
	CPPUNIT_ASSERT(result.targetID == m_target->getID());
	CPPUNIT_ASSERT(m_weapon->getAmmo() == 0);
	CPPUNIT_ASSERT(m_weapon->getTarget() == m_target);
	CPPUNIT_ASSERT(m_weapon->getTargetRange() == 3);
}

void FWeaponFireResultTest::testFireReturnsStructuredSkippedStateWhenWeaponDamaged() {
	// AC: damaged weapons produce a distinct structured skipped state.
	m_weapon->setMaxAmmo(1);
	m_weapon->setCurrentAmmo(1);
	m_weapon->setDamageStatus(true);
	static_cast<FWeaponFireHarness *>(m_weapon)->assignTargetDirectly(m_target, 4);

	FTacticalAttackResult result = m_weapon->fire();

	CPPUNIT_ASSERT(result.outcome == TAO_NotFired);
	CPPUNIT_ASSERT(result.skipReason == TASR_WeaponDamaged);
	CPPUNIT_ASSERT(!result.fired());
	CPPUNIT_ASSERT(result.targetID == m_target->getID());
	CPPUNIT_ASSERT(m_weapon->getAmmo() == 1);
	CPPUNIT_ASSERT(m_weapon->getTarget() == m_target);
	CPPUNIT_ASSERT(m_weapon->getTargetRange() == 4);
}

void FWeaponFireResultTest::testFireReturnsStructuredSkippedStateWhenNoTargetOrRangeInvalid() {
	// AC: no-target and invalid-range cases remain not-fired but are distinguishable.
	m_weapon->setMaxAmmo(1);
	m_weapon->setCurrentAmmo(1);

	FTacticalAttackResult noTargetResult = m_weapon->fire();
	CPPUNIT_ASSERT(noTargetResult.outcome == TAO_NotFired);
	CPPUNIT_ASSERT(noTargetResult.skipReason == TASR_NoTarget);
	CPPUNIT_ASSERT(!noTargetResult.fired());
	CPPUNIT_ASSERT(m_weapon->getAmmo() == 1);

	static_cast<FWeaponFireHarness *>(m_weapon)->assignTargetDirectly(m_target, -1);
	FTacticalAttackResult invalidRangeResult = m_weapon->fire();
	CPPUNIT_ASSERT(invalidRangeResult.outcome == TAO_NotFired);
	CPPUNIT_ASSERT(invalidRangeResult.skipReason == TASR_InvalidTargetRange);
	CPPUNIT_ASSERT(!invalidRangeResult.fired());
	CPPUNIT_ASSERT(invalidRangeResult.targetID == m_target->getID());
	CPPUNIT_ASSERT(m_weapon->getAmmo() == 1);
	CPPUNIT_ASSERT(m_weapon->getTarget() == m_target);
	CPPUNIT_ASSERT(m_weapon->getTargetRange() == -1);
}

void FWeaponFireResultTest::testFireReturnsStructuredMissAndPreservesLegacyFireSideEffects() {
	// AC: fired attacks report misses distinctly while still consuming ammo and clearing the target.
	srand(1);
	m_weapon->setMaxAmmo(1);
	m_weapon->setCurrentAmmo(1);
	static_cast<FWeaponFireHarness *>(m_weapon)->setBaseToHit(0);
	static_cast<FWeaponFireHarness *>(m_weapon)->assignTargetDirectly(m_target, 2);

	FTacticalAttackResult result = m_weapon->fire();

	CPPUNIT_ASSERT(result.outcome == TAO_Missed);
	CPPUNIT_ASSERT(result.skipReason == TASR_None);
	CPPUNIT_ASSERT(result.fired());
	CPPUNIT_ASSERT(!result.hit());
	CPPUNIT_ASSERT(result.attackerID == m_attacker->getID());
	CPPUNIT_ASSERT(result.targetID == m_target->getID());
	CPPUNIT_ASSERT(result.hitRoll == 85);
	CPPUNIT_ASSERT(result.toHitProbability >= 5);
	CPPUNIT_ASSERT(result.hitRoll > result.toHitProbability);
	CPPUNIT_ASSERT(result.damageRolled == 0);
	CPPUNIT_ASSERT(result.totalHullDamageApplied == 0);
	CPPUNIT_ASSERT(result.effects.empty());
	CPPUNIT_ASSERT(m_weapon->getAmmo() == 0);
	CPPUNIT_ASSERT(m_weapon->getTarget() == NULL);
	CPPUNIT_ASSERT(m_weapon->getTargetRange() == -1);
}

}
