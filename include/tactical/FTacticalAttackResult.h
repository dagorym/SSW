/**
 * @file FTacticalAttackResult.h
 * @brief Tactical attack result data types returned by weapon fire resolution.
 *
 * @author Tom Stephens, gpt-5.3-codex (medium)
 * @date Created: Mar 20, 2026
 * @date Last Modified: Mar 20, 2026
 */

#ifndef _FTACTICALATTACKRESULT_H_
#define _FTACTICALATTACKRESULT_H_

#include <string>
#include <vector>

namespace Frontier {

/**
 * @brief Enumerates the high-level outcome of a weapon fire attempt.
 *
 * @author Tom Stephens, gpt-5.3-codex (medium)
 * @date Created: Mar 20, 2026
 * @date Last Modified: Mar 20, 2026
 */
enum TacticalAttackOutcome {
	TAO_NotFired = 0,
	TAO_Missed,
	TAO_Hit
};

/**
 * @brief Captures why a weapon attack did not fire.
 *
 * @author Tom Stephens, gpt-5.3-codex (medium)
 * @date Created: Mar 20, 2026
 * @date Last Modified: Mar 20, 2026
 */
enum TacticalAttackSkipReason {
	TASR_None = 0,
	TASR_NoAmmo,
	TASR_WeaponDamaged,
	TASR_NoTarget,
	TASR_InvalidTargetRange
};

/**
 * @brief Describes one internal effect produced by a resolved tactical attack.
 *
 * @author Tom Stephens, gpt-5.3-codex (medium)
 * @date Created: Mar 20, 2026
 * @date Last Modified: Mar 20, 2026
 */
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

/**
 * @brief Aggregates the full result payload emitted by a weapon fire operation.
 *
 * Includes attacker/target identity, to-hit and damage details, optional internal
 * effects, and summary status flags used by tactical reporting.
 *
 * @author Tom Stephens, gpt-5.3-codex (medium)
 * @date Created: Mar 20, 2026
 * @date Last Modified: Mar 20, 2026
 */
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

	/// Returns true when the weapon attempted to fire this attack result.
	bool fired() const { return outcome != TAO_NotFired; }
	/// Returns true when the resolved attack outcome is a hit.
	bool hit() const { return outcome == TAO_Hit; }
};

}

#endif
