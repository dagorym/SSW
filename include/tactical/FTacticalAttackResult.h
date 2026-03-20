#ifndef _FTACTICALATTACKRESULT_H_
#define _FTACTICALATTACKRESULT_H_

#include <string>
#include <vector>

namespace Frontier {

enum TacticalAttackOutcome {
	TAO_NotFired = 0,
	TAO_Missed,
	TAO_Hit
};

enum TacticalAttackSkipReason {
	TASR_None = 0,
	TASR_NoAmmo,
	TASR_WeaponDamaged,
	TASR_NoTarget,
	TASR_InvalidTargetRange
};

struct FTacticalAttackEffect {
	int effectType;
	int rollValue;
	int previousValue;
	int newValue;
	int amount;
	int hullDamageApplied;
	int weaponType;
	unsigned int weaponID;
	std::string weaponName;
	int defenseType;
	std::string defenseName;
	int navigationError;
	bool fallbackToHullDamage;
	std::string label;
	std::string detail;

	FTacticalAttackEffect()
		: effectType(0), rollValue(-1), previousValue(0), newValue(0), amount(0),
		  hullDamageApplied(0), weaponType(0), weaponID(0), weaponName(""),
		  defenseType(0), defenseName(""), navigationError(0),
		  fallbackToHullDamage(false), label(""), detail("") {}
};

struct FTacticalAttackResult {
	TacticalAttackOutcome outcome;
	TacticalAttackSkipReason skipReason;
	unsigned int attackerID;
	unsigned int attackerOwnerID;
	std::string attackerName;
	unsigned int targetID;
	unsigned int targetOwnerID;
	std::string targetName;
	unsigned int weaponID;
	std::string weaponName;
	int weaponType;
	int hitRoll;
	int toHitProbability;
	int targetRange;
	int damageRolled;
	int damageTableModifier;
	bool usedAdvancedDamageTable;
	int damageTableRoll;
	int totalHullDamageApplied;
	std::string note;
	std::vector<FTacticalAttackEffect> effects;

	FTacticalAttackResult()
		: outcome(TAO_NotFired), skipReason(TASR_None), attackerID(0), attackerOwnerID(0),
		  attackerName(""), targetID(0), targetOwnerID(0), targetName(""), weaponID(0),
		  weaponName(""), weaponType(0), hitRoll(-1), toHitProbability(0), targetRange(-1),
		  damageRolled(0), damageTableModifier(0), usedAdvancedDamageTable(false),
		  damageTableRoll(-1), totalHullDamageApplied(0), note(""), effects() {}

	bool fired() const { return outcome != TAO_NotFired; }
	bool hit() const { return outcome == TAO_Hit; }
};

}

#endif
