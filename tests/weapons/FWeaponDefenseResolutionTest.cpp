/**
 * @file FWeaponDefenseResolutionTest.cpp
 * @brief Implementation file for the FWeaponDefenseResolutionTest class
 */

#include "FWeaponDefenseResolutionTest.h"

#include <cstdlib>

#include "ships/FBattleship.h"
#include "ships/FFrigate.h"
#include "weapons/weapons.h"

namespace FrontierTests {
using namespace Frontier;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FWeaponDefenseResolutionTest );

void FWeaponDefenseResolutionTest::setUp(){
}

void FWeaponDefenseResolutionTest::tearDown(){
}

void FWeaponDefenseResolutionTest::testMostEffectiveDefenseResolvesAgainstHullOverRaisedProtonScreenForLaserBattery(){
	// FBattleship defenses: 0=RH, 1=ES, 2=PS, 3=SS, 4=ICM
	FBattleship *attacker = new FBattleship();
	FBattleship *target = new FBattleship();
	target->setCurrentDefense(2); // raise Proton Screen

	// FBattleship weapons: index 1 is a Laser Battery
	FWeapon *lb = attacker->getWeapon(1);
	CPPUNIT_ASSERT(lb->getType() == FWeapon::LB);
	lb->setTarget(target, 0, false);

	FTacticalAttackResult result = lb->fire();

	// RH (0) is more effective than the raised PS (+15) against a Laser
	// Battery, and PS does not attract LB, so the hull's modifier is used.
	CPPUNIT_ASSERT(result.toHitProbability == 50);
	CPPUNIT_ASSERT(result.fired());

	delete attacker;
	delete target;
}

void FWeaponDefenseResolutionTest::testAttractOverrideResolvesElectronScreenForProtonBeamOverHull(){
	FBattleship *attacker = new FBattleship();
	FBattleship *target = new FBattleship();
	target->setCurrentDefense(1); // raise Electron Screen

	// FBattleship weapons: index 8 is a Proton Battery
	FWeapon *pb = attacker->getWeapon(8);
	CPPUNIT_ASSERT(pb->getType() == FWeapon::PB);
	pb->setTarget(target, 0, false);

	FTacticalAttackResult result = pb->fire();

	// Electron Screen attracts Proton Beam (+10), which overrides the
	// otherwise-more-effective hull (0).
	CPPUNIT_ASSERT(result.toHitProbability == 70);
	CPPUNIT_ASSERT(result.fired());

	delete attacker;
	delete target;
}

void FWeaponDefenseResolutionTest::testAttractOverrideResolvesProtonScreenForElectronBeamOverHull(){
	FBattleship *attacker = new FBattleship();
	FBattleship *target = new FBattleship();
	target->setCurrentDefense(2); // raise Proton Screen

	// FBattleship weapons: index 6 is an Electron Battery
	FWeapon *eb = attacker->getWeapon(6);
	CPPUNIT_ASSERT(eb->getType() == FWeapon::EB);
	eb->setTarget(target, 0, false);

	FTacticalAttackResult result = eb->fire();

	// Proton Screen attracts Electron Beam (+10), which overrides the
	// otherwise-more-effective hull (0).
	CPPUNIT_ASSERT(result.toHitProbability == 70);
	CPPUNIT_ASSERT(result.fired());

	delete attacker;
	delete target;
}

void FWeaponDefenseResolutionTest::testAttractOverrideResolvesStasisScreenForTorpedoSeekerAndMineOverHull(){
	FBattleship *attacker = new FBattleship();
	FBattleship *target = new FBattleship();
	target->setCurrentDefense(3); // raise Stasis Screen

	// Torpedo: FBattleship weapons index 4
	FWeapon *torpedo = attacker->getWeapon(4);
	CPPUNIT_ASSERT(torpedo->getType() == FWeapon::T);
	torpedo->setTarget(target, 0, false);
	FTacticalAttackResult torpedoResult = torpedo->fire();
	CPPUNIT_ASSERT(torpedoResult.toHitProbability == 75); // base 50 + attract 25
	CPPUNIT_ASSERT(torpedoResult.fired());

	// Seeker Missile: FBattleship weapons index 9
	FWeapon *seeker = attacker->getWeapon(9);
	CPPUNIT_ASSERT(seeker->getType() == FWeapon::SM);
	seeker->setTarget(target, 0, false);
	FTacticalAttackResult seekerResult = seeker->fire();
	CPPUNIT_ASSERT(seekerResult.toHitProbability == 90); // base 75 + attract 15
	CPPUNIT_ASSERT(seekerResult.fired());

	// Mine: not carried by FBattleship, so build one directly
	FWeapon *mine = createWeapon(FWeapon::M);
	mine->setParent(attacker);
	mine->setTarget(target, 0, false);
	FTacticalAttackResult mineResult = mine->fire();
	CPPUNIT_ASSERT(mineResult.toHitProbability == 80); // base 60 + attract 20
	CPPUNIT_ASSERT(mineResult.fired());

	delete mine;
	delete attacker;
	delete target;
}

void FWeaponDefenseResolutionTest::testMaskingScreenLaserOutUsesAttackerModifierAndHalvesDamage(){
	// FFrigate defenses: 0=RH, 1=MS, 2=ICM; weapons: 0=LC, 1=LB, 2=T, 3=RB
	FFrigate *attacker = new FFrigate();
	attacker->setCurrentDefense(1); // raise attacker's Masking Screen
	FFrigate *target = new FFrigate(); // target's defense stays at RH (index 0)

	FWeapon *lb = attacker->getWeapon(1);
	CPPUNIT_ASSERT(lb->getType() == FWeapon::LB);
	lb->setTarget(target, 0, false);

	srand(24); // deterministic: to-hit roll=9, damage die=9

	FTacticalAttackResult result = lb->fire();

	// LB base (50) + attacker's raised MS laser modifier (-30) = 20.
	CPPUNIT_ASSERT(result.toHitProbability == 20);
	CPPUNIT_ASSERT(result.outcome == TAO_Hit);
	// raw damage 9, halved rounded up: 9/2 + 9%2 = 4 + 1 = 5
	CPPUNIT_ASSERT(result.damageRolled == 5);

	delete attacker;
	delete target;
}

void FWeaponDefenseResolutionTest::testMaskingScreenLaserInUsesTargetModifierAndHalvesDamage(){
	FFrigate *attacker = new FFrigate(); // attacker's defense stays at RH (index 0)
	FFrigate *target = new FFrigate();
	target->setCurrentDefense(1); // raise target's Masking Screen

	FWeapon *lb = attacker->getWeapon(1);
	CPPUNIT_ASSERT(lb->getType() == FWeapon::LB);
	lb->setTarget(target, 0, false);

	srand(34); // deterministic: to-hit roll=17, damage die=1

	FTacticalAttackResult result = lb->fire();

	// Target's operating defenses are RH (0) and raised MS (-30); MS is the
	// most-effective (minimum) modifier, so LB base (50) + (-30) = 20.
	CPPUNIT_ASSERT(result.toHitProbability == 20);
	CPPUNIT_ASSERT(result.outcome == TAO_Hit);
	// raw damage 1, halved rounded up: 1/2 + 1%2 = 0 + 1 = 1
	CPPUNIT_ASSERT(result.damageRolled == 1);

	delete attacker;
	delete target;
}

void FWeaponDefenseResolutionTest::testParentlessMineFireDoesNotCrashAndResolvesAgainstTargetDefense(){
	FBattleship *target = new FBattleship();
	target->setCurrentDefense(3); // raise Stasis Screen (attracts Mine)

	FWeapon *mine = createWeapon(FWeapon::M);
	// deliberately no setParent(...): mines are placed on the hex map and
	// fire without an owning ship parent
	mine->setTarget(target, 0, false);

	FTacticalAttackResult result = mine->fire(); // must not crash

	CPPUNIT_ASSERT(result.skipReason == TASR_None);
	CPPUNIT_ASSERT(result.fired());
	// Mine base (60) + Stasis Screen attract modifier (+20) = 80.
	CPPUNIT_ASSERT(result.toHitProbability == 80);

	delete mine;
	delete target;
}

}
