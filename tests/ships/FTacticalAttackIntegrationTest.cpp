/**
 * @file FTacticalAttackIntegrationTest.cpp
 * @brief Implementation file for the FTacticalAttackIntegrationTest class
 */

#include "FTacticalAttackIntegrationTest.h"

#include <cstdlib>

#include "tactical/FTacticalCombatReport.h"
#include "defenses/defenses.h"

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
	/// override the weapon type (default LB) so the same harness can exercise
	/// the laser-only masking-screen "fired out of the screen" branch vs. the
	/// non-laser regression path (e.g. Torpedo)
	void setWeaponType(FWeapon::Weapon t) { m_type = t; }
	/// override the flat damage modifier so a single die roll can be made an
	/// odd number, distinguishing "halved, rounded up" from plain integer
	/// division
	void setDamageMod(unsigned int d) { m_dMod = d; }
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
	/// append a real FDefense instance to this vehicle's defense list, e.g. so
	/// a test can raise it via setCurrentDefense(...) or have it counted as an
	/// "operating" reflective hull by FVehicle::resolveToHitModifier()
	void addDefense(FDefense *d) { m_defenses.push_back(d); }
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

void FTacticalAttackIntegrationTest::testMaskingScreenTorpedoOutResolvesAgainstTargetDefenseNotAttackerMSAndDoesNotHalveDamage() {
	// AC (T3 regression): a non-laser weapon fired from inside the attacker's
	// raised Masking Screen must resolve to-hit against the TARGET's
	// most-effective operating defense (with attract override) via
	// FVehicle::resolveToHitModifier(), not the attacker's MS modifier, and
	// must not halve damage. This fails against the pre-fix code, which
	// applied the attacker's MS modifier unconditionally to every weapon type.
	FCombatVehicleHarness *attacker = new FCombatVehicleHarness();
	attacker->configureStats(20, 4, 3, 8);
	attacker->addDefense(new FMaskingScreen()); // index 1
	attacker->setCurrentDefense(1); // attacker's MS is raised

	FCombatVehicleHarness *target = new FCombatVehicleHarness();
	target->configureStats(20, 4, 3, 8);
	target->addDefense(new FReflectiveHull()); // index 1
	target->addDefense(new FStasisScreen()); // index 2
	target->setCurrentDefense(2); // target's Stasis Screen is raised (attracts Torpedo)

	FWeaponFireHarness *weapon = new FWeaponFireHarness();
	weapon->setWeaponType(FWeapon::T); // non-laser
	weapon->setDamageMod(1); // makes the raw single-die damage an odd number
	weapon->setParent(attacker);
	weapon->assignTargetDirectly(target, 3);

	srand(123); // deterministic: to-hit roll=7, damage die=8

	FTacticalAttackResult result = weapon->fire();

	// Target's operating defenses are RH (0) and raised SS (attracts T, +25);
	// the attract override means SS's +25 wins over RH's 0. The attacker's MS
	// modifier for Torpedo is 0 and must NOT be used at all.
	CPPUNIT_ASSERT(result.toHitProbability == 125);
	CPPUNIT_ASSERT(result.outcome == TAO_Hit);
	CPPUNIT_ASSERT(result.fired());
	// Torpedo is not LC/LB, so damage is never halved regardless of MS state.
	CPPUNIT_ASSERT(result.damageRolled == 9);

	delete weapon;
	delete attacker;
	delete target;
}

void FTacticalAttackIntegrationTest::testParentlessLaserFireDoesNotCrashAndDoesNotHalveDamageAgainstNonMaskedTarget() {
	// AC: no null-deref when a parentless weapon (as mines/seekers fire)
	// fires. This specifically exercises the damage-halving block's
	// `(m_parent != NULL && ...)` guard: the target's current defense is NOT
	// a Masking Screen, so the halving condition's left operand is false and
	// the right operand (the attacker/parent check) must be evaluated. Before
	// the fix, that right operand unconditionally dereferenced m_parent.
	FCombatVehicleHarness *target = new FCombatVehicleHarness();
	target->configureStats(20, 4, 3, 8);
	target->addDefense(new FReflectiveHull()); // index 1
	target->setCurrentDefense(1); // RH raised, not MS

	FWeaponFireHarness *weapon = new FWeaponFireHarness();
	weapon->setWeaponType(FWeapon::LB); // laser
	// deliberately do not call setParent(...): m_parent stays NULL, as it
	// does for mines and seekers fired from a hex rather than a ship
	weapon->assignTargetDirectly(target, 3);

	srand(24); // deterministic: to-hit roll=9, damage die=9

	FTacticalAttackResult result = weapon->fire(); // must not crash

	// Target's only operating defense is RH (modifier 0); the laser-out
	// branch never applies because m_parent is NULL.
	CPPUNIT_ASSERT(result.toHitProbability == 100);
	CPPUNIT_ASSERT(result.outcome == TAO_Hit);
	CPPUNIT_ASSERT(result.fired());
	CPPUNIT_ASSERT(result.damageRolled == 9); // unhalved: neither side has MS raised

	delete weapon;
	delete target;
}

}
