/**
 * @file FTacticalAttackIntegrationTest.cpp
 * @brief Implementation file for the FTacticalAttackIntegrationTest class
 */

#include "FTacticalAttackIntegrationTest.h"

#include <cstdlib>

#include "tactical/FTacticalCombatReport.h"

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
		m_baseToHitProb = 100;
	}

	void setDamageTableModifier(int modifier) { m_damageTableMod = modifier; }
	void assignTargetDirectly(FVehicle *target, int range, bool headOn = false) {
		m_target = target;
		m_targetRange = range;
		m_isHeadOn = headOn;
	}
};

class FCombatVehicleHarness : public FVehicle {
public:
	FCombatVehicleHarness() {
		m_name = "Combat Harness";
		m_type = "CombatHarness";
	}

	~FCombatVehicleHarness() {}

	void configureStats(int hp, int adf, unsigned int mr, unsigned int dcr) {
		m_maxHP = hp;
		m_currentHP = hp;
		m_maxADF = adf;
		m_currentADF = adf;
		m_maxMR = mr;
		m_currentMR = mr;
		m_maxDCR = dcr;
		m_currentDCR = dcr;
	}

	void setCombatControlDamaged(bool value) { m_combatControlDamaged = value; }
	void setOnFire(bool value) { m_onFire = value; }
};

}

CPPUNIT_TEST_SUITE_REGISTRATION( FTacticalAttackIntegrationTest );

void FTacticalAttackIntegrationTest::setUp() {
	m_weapon = new FWeaponFireHarness();
	m_attacker = new FCombatVehicleHarness();
	m_target = new FCombatVehicleHarness();
	m_attacker->setName("Attacker");
	m_attacker->setOwner(1);
	m_target->setName("Target");
	m_target->setOwner(2);
	static_cast<FCombatVehicleHarness *>(m_attacker)->configureStats(20, 4, 3, 8);
	static_cast<FCombatVehicleHarness *>(m_target)->configureStats(20, 4, 3, 8);
	m_weapon->setParent(m_attacker);
	m_weapon->setMaxAmmo(1);
	m_weapon->setCurrentAmmo(1);
}

void FTacticalAttackIntegrationTest::tearDown() {
	delete m_weapon;
	delete m_attacker;
	delete m_target;
}

void FTacticalAttackIntegrationTest::testFireHitPreservesLegacyCleanupSideEffects() {
	// AC: successful hits still clear the assigned target and consume ammo after firing.
	srand(123);
	static_cast<FWeaponFireHarness *>(m_weapon)->assignTargetDirectly(m_target, 3);

	FTacticalAttackResult result = m_weapon->fire();

	CPPUNIT_ASSERT(result.outcome == TAO_Hit);
	CPPUNIT_ASSERT(result.skipReason == TASR_None);
	CPPUNIT_ASSERT(result.fired());
	CPPUNIT_ASSERT(result.hit());
	CPPUNIT_ASSERT(result.targetID == m_target->getID());
	CPPUNIT_ASSERT(m_weapon->getAmmo() == 0);
	CPPUNIT_ASSERT(m_weapon->getTarget() == NULL);
	CPPUNIT_ASSERT(m_weapon->getTargetRange() == -1);
}

void FTacticalAttackIntegrationTest::testFireCopiesAppliedHullDamageIntoAttackResult() {
	// AC: successful hits report the actual hull damage applied by the target damage resolution.
	srand(123);
	static_cast<FWeaponFireHarness *>(m_weapon)->setDamageTableModifier(-200);
	static_cast<FWeaponFireHarness *>(m_weapon)->assignTargetDirectly(m_target, 3);

	FTacticalAttackResult result = m_weapon->fire();

	CPPUNIT_ASSERT(result.outcome == TAO_Hit);
	CPPUNIT_ASSERT(result.skipReason == TASR_None);
	CPPUNIT_ASSERT(result.fired());
	CPPUNIT_ASSERT(result.hit());
	CPPUNIT_ASSERT(result.damageRolled == 8);
	CPPUNIT_ASSERT(result.usedAdvancedDamageTable);
	CPPUNIT_ASSERT(result.damageTableModifier == -200);
	CPPUNIT_ASSERT(result.damageTableRoll <= 10);
	CPPUNIT_ASSERT(result.totalHullDamageApplied == 16);
	CPPUNIT_ASSERT(m_target->getHP() == 4);
	CPPUNIT_ASSERT(result.effects.size() == 1);
	CPPUNIT_ASSERT(result.effects[0].effectType == TDET_HullDamage);
	CPPUNIT_ASSERT(result.effects[0].rollValue == result.damageTableRoll);
	CPPUNIT_ASSERT(result.effects[0].previousValue == 20);
	CPPUNIT_ASSERT(result.effects[0].newValue == 4);
	CPPUNIT_ASSERT(result.effects[0].amount == 16);
	CPPUNIT_ASSERT(result.effects[0].hullDamageApplied == 16);
}

void FTacticalAttackIntegrationTest::testFireCopiesAppliedNonHullEffectsIntoAttackResult() {
	// AC: successful hits report the actual non-hull effects applied by the target damage resolution.
	srand(123);
	static_cast<FWeaponFireHarness *>(m_weapon)->setDamageTableModifier(85);
	static_cast<FWeaponFireHarness *>(m_weapon)->assignTargetDirectly(m_target, 3);

	FTacticalAttackResult result = m_weapon->fire();

	CPPUNIT_ASSERT(result.outcome == TAO_Hit);
	CPPUNIT_ASSERT(result.skipReason == TASR_None);
	CPPUNIT_ASSERT(result.fired());
	CPPUNIT_ASSERT(result.hit());
	CPPUNIT_ASSERT(result.damageRolled == 8);
	CPPUNIT_ASSERT(result.usedAdvancedDamageTable);
	CPPUNIT_ASSERT(result.damageTableModifier == 85);
	CPPUNIT_ASSERT(result.damageTableRoll == 106);
	CPPUNIT_ASSERT(result.totalHullDamageApplied == 0);
	CPPUNIT_ASSERT(m_target->getHP() == 20);
	CPPUNIT_ASSERT(m_target->getDCR() == 4);
	CPPUNIT_ASSERT(result.effects.size() == 1);
	CPPUNIT_ASSERT(result.effects[0].effectType == TDET_DCRLoss);
	CPPUNIT_ASSERT(result.effects[0].rollValue == 106);
	CPPUNIT_ASSERT(result.effects[0].previousValue == 8);
	CPPUNIT_ASSERT(result.effects[0].newValue == 4);
	CPPUNIT_ASSERT(result.effects[0].amount == 4);
	CPPUNIT_ASSERT(result.effects[0].hullDamageApplied == 0);
}

void FTacticalAttackIntegrationTest::testFireReportsConvertedADFHitAsHullDamage() {
	// AC: attack-result output for converted subsystem hits reports hull damage, not ADF loss.
	srand(123);
	static_cast<FCombatVehicleHarness *>(m_target)->configureStats(20, 0, 3, 8);
	static_cast<FWeaponFireHarness *>(m_weapon)->setDamageTableModifier(28);
	static_cast<FWeaponFireHarness *>(m_weapon)->assignTargetDirectly(m_target, 3);

	FTacticalAttackResult result = m_weapon->fire();

	CPPUNIT_ASSERT(result.outcome == TAO_Hit);
	CPPUNIT_ASSERT(result.skipReason == TASR_None);
	CPPUNIT_ASSERT(result.fired());
	CPPUNIT_ASSERT(result.hit());
	CPPUNIT_ASSERT(result.damageRolled == 8);
	CPPUNIT_ASSERT(result.usedAdvancedDamageTable);
	CPPUNIT_ASSERT(result.damageTableModifier == 28);
	CPPUNIT_ASSERT(result.damageTableRoll == 49);
	CPPUNIT_ASSERT(result.totalHullDamageApplied == 8);
	CPPUNIT_ASSERT(m_target->getADF() == 0);
	CPPUNIT_ASSERT(m_target->getHP() == 12);
	CPPUNIT_ASSERT(result.effects.size() == 1);
	CPPUNIT_ASSERT(result.effects[0].effectType == TDET_HullDamage);
	CPPUNIT_ASSERT(result.effects[0].rollValue == 49);
	CPPUNIT_ASSERT(result.effects[0].previousValue == 20);
	CPPUNIT_ASSERT(result.effects[0].newValue == 12);
	CPPUNIT_ASSERT(result.effects[0].amount == 8);
	CPPUNIT_ASSERT(result.effects[0].hullDamageApplied == 8);
}

void FTacticalAttackIntegrationTest::testFireReportsDisastrousFireFallbackAsHullDamage() {
	// AC: full Disastrous Fire fallback reaches attack-result consumers as hull damage only.
	srand(123);
	FCombatVehicleHarness *target = static_cast<FCombatVehicleHarness *>(m_target);
	target->configureStats(20, 0, 0, 8);
	target->setDCR(4);
	target->setCombatControlDamaged(true);
	target->setOnFire(true);
	static_cast<FWeaponFireHarness *>(m_weapon)->setDamageTableModifier(99);
	static_cast<FWeaponFireHarness *>(m_weapon)->assignTargetDirectly(m_target, 3);

	FTacticalAttackResult result = m_weapon->fire();

	CPPUNIT_ASSERT(result.outcome == TAO_Hit);
	CPPUNIT_ASSERT(result.skipReason == TASR_None);
	CPPUNIT_ASSERT(result.fired());
	CPPUNIT_ASSERT(result.hit());
	CPPUNIT_ASSERT(result.damageRolled == 8);
	CPPUNIT_ASSERT(result.usedAdvancedDamageTable);
	CPPUNIT_ASSERT(result.damageTableModifier == 99);
	CPPUNIT_ASSERT(result.damageTableRoll == 120);
	CPPUNIT_ASSERT(result.totalHullDamageApplied == 8);
	CPPUNIT_ASSERT(m_target->getHP() == 12);
	CPPUNIT_ASSERT(m_target->getADF() == 0);
	CPPUNIT_ASSERT(m_target->getMR() == 0);
	CPPUNIT_ASSERT(m_target->getDCR() == 4);
	CPPUNIT_ASSERT(m_target->isCombatControlDamaged());
	CPPUNIT_ASSERT(m_target->isOnFire());
	CPPUNIT_ASSERT(result.effects.size() == 1);
	CPPUNIT_ASSERT(result.effects[0].effectType == TDET_HullDamage);
	CPPUNIT_ASSERT(result.effects[0].rollValue == 120);
	CPPUNIT_ASSERT(result.effects[0].previousValue == 20);
	CPPUNIT_ASSERT(result.effects[0].newValue == 12);
	CPPUNIT_ASSERT(result.effects[0].amount == 8);
	CPPUNIT_ASSERT(result.effects[0].hullDamageApplied == 8);
}

}
