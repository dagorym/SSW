/**
 * @file FTacticalAttackResultTest.cpp
 * @brief Implementation file for the FTacticalAttackResultTest class
 */

#include "FTacticalAttackResultTest.h"
#include "tactical/FTacticalCombatReport.h"

namespace FrontierTests {
using namespace Frontier;

CPPUNIT_TEST_SUITE_REGISTRATION( FTacticalAttackResultTest );

void FTacticalAttackResultTest::setUp() {
}

void FTacticalAttackResultTest::tearDown() {
}

void FTacticalAttackResultTest::testDefaultConstructorRepresentsNotFiredAttack() {
	// AC: the returned attack object starts in a structured not-fired state.
	FTacticalAttackResult result;

	CPPUNIT_ASSERT(result.outcome == TAO_NotFired);
	CPPUNIT_ASSERT(result.skipReason == TASR_None);
	CPPUNIT_ASSERT(!result.fired());
	CPPUNIT_ASSERT(!result.hit());
	CPPUNIT_ASSERT(result.hitRoll == -1);
	CPPUNIT_ASSERT(result.targetRange == -1);
	CPPUNIT_ASSERT(result.damageRolled == 0);
	CPPUNIT_ASSERT(result.totalHullDamageApplied == 0);
	CPPUNIT_ASSERT(result.effects.empty());

	FTacticalAttackEffect effect;
	CPPUNIT_ASSERT(effect.effectType == TDET_None);
	CPPUNIT_ASSERT(effect.rollValue == -1);
	CPPUNIT_ASSERT(effect.hullDamageApplied == 0);
	CPPUNIT_ASSERT(effect.weaponType == 0);
	CPPUNIT_ASSERT(effect.defenseType == 0);
	CPPUNIT_ASSERT(effect.fallbackToHullDamage == false);
}

void FTacticalAttackResultTest::testHelpersAndEffectsReflectMutatedOutcomeState() {
	// AC: tactical reporting can distinguish hit state and carry copied effect metadata.
	FTacticalAttackResult result;
	result.outcome = TAO_Hit;
	result.skipReason = TASR_None;
	result.hitRoll = 12;
	result.toHitProbability = 65;
	result.damageRolled = 7;
	result.totalHullDamageApplied = 4;

	FTacticalAttackEffect effect;
	effect.effectType = TDET_DCRLoss;
	effect.rollValue = 106;
	effect.previousValue = 8;
	effect.newValue = 4;
	effect.amount = 4;
	effect.label = "DCR";
	effect.detail = "4 DCR lost";
	result.effects.push_back(effect);

	CPPUNIT_ASSERT(result.fired());
	CPPUNIT_ASSERT(result.hit());
	CPPUNIT_ASSERT(result.effects.size() == 1);
	CPPUNIT_ASSERT(result.effects[0].effectType == TDET_DCRLoss);
	CPPUNIT_ASSERT(result.effects[0].rollValue == 106);
	CPPUNIT_ASSERT(result.effects[0].previousValue == 8);
	CPPUNIT_ASSERT(result.effects[0].newValue == 4);
	CPPUNIT_ASSERT(result.effects[0].amount == 4);
	CPPUNIT_ASSERT(result.effects[0].label == "DCR");
}

}
