#ifndef _FTACTICALCOMBATREPORT_H_
#define _FTACTICALCOMBATREPORT_H_

#include <string>
#include <vector>

#include "ships/FVehicle.h"
#include "weapons/FWeapon.h"

namespace Frontier {

enum TacticalReportType {
	TRT_None = 0,
	TRT_DefensiveFire,
	TRT_OffensiveFire,
	TRT_ElectricalFire,
	TRT_MineDamage
};

enum TacticalReportEventType {
	TRET_None = 0,
	TRET_InternalDamage,
	TRET_ElectricalFire,
	TRET_MineDamage,
	TRET_DefenseEffect,
	TRET_Note
};

enum TacticalDamageEffectType {
	TDET_None = 0,
	TDET_HullDamage,
	TDET_ADFLoss,
	TDET_MRLoss,
	TDET_WeaponDamaged,
	TDET_DefenseDamaged,
	TDET_PowerSystemDamaged,
	TDET_CombatControlDamaged,
	TDET_NavigationError,
	TDET_ElectricalFire,
	TDET_DCRLoss
};

struct FTacticalDamageEffect {
	TacticalDamageEffectType effectType;
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

	FTacticalDamageEffect()
		: effectType(TDET_None), rollValue(-1), previousValue(0), newValue(0), amount(0),
		  hullDamageApplied(0), weaponType(FWeapon::NONE), weaponID(0), weaponName(""),
		  defenseType(FDefense::UNDEF), defenseName(""), navigationError(0),
		  fallbackToHullDamage(false), label(""), detail("") {}
};

struct FTacticalDamageResolution {
	bool usedAdvancedDamageTable;
	int requestedDamage;
	int damageTableModifier;
	int damageTableRoll;
	int totalHullDamageApplied;
	std::vector<FTacticalDamageEffect> effects;

	FTacticalDamageResolution()
		: usedAdvancedDamageTable(false), requestedDamage(0), damageTableModifier(0),
		  damageTableRoll(-1), totalHullDamageApplied(0), effects() {}

	void clear() {
		usedAdvancedDamageTable = false;
		requestedDamage = 0;
		damageTableModifier = 0;
		damageTableRoll = -1;
		totalHullDamageApplied = 0;
		effects.clear();
	}
};

struct FTacticalShipReference {
	unsigned int shipID;
	unsigned int ownerID;
	std::string shipName;

	FTacticalShipReference() : shipID(0), ownerID(0), shipName("") {}
	FTacticalShipReference(unsigned int id, unsigned int owner, const std::string &name)
		: shipID(id), ownerID(owner), shipName(name) {}
	bool isValid() const { return shipName.size() > 0 || shipID != 0; }
};

struct FTacticalWeaponReference {
	unsigned int weaponID;
	std::string weaponName;

	FTacticalWeaponReference() : weaponID(0), weaponName("") {}
	FTacticalWeaponReference(unsigned int id, const std::string &name)
		: weaponID(id), weaponName(name) {}
	bool isValid() const { return weaponName.size() > 0 || weaponID != 0; }
};

struct FTacticalReportEvent {
	TacticalReportEventType eventType;
	FTacticalShipReference subject;
	FTacticalShipReference source;
	FTacticalShipReference target;
	int rollValue;
	int hullDamage;
	int attackIndex;
	bool immediate;
	std::string label;
	std::string detail;

	FTacticalReportEvent()
		: eventType(TRET_None), rollValue(-1), hullDamage(0), attackIndex(-1), immediate(false),
		  label(""), detail("") {}
};

struct FTacticalAttackReport {
	FTacticalShipReference attacker;
	FTacticalShipReference target;
	FTacticalWeaponReference weapon;
	int hitRoll;
	int attackRollModifier;
	int targetRange;
	int hullDamage;
	int damageTableModifier;
	bool hit;
	bool intercepted;
	bool immediate;
	std::string note;
	std::vector<FTacticalReportEvent> internalEvents;

	FTacticalAttackReport()
		: hitRoll(-1), attackRollModifier(0), targetRange(-1), hullDamage(0),
		  damageTableModifier(0), hit(false), intercepted(false), immediate(false), note("") {}
};

struct FTacticalCombatReportContext {
	TacticalReportType reportType;
	int phase;
	unsigned int actingPlayerID;
	bool immediate;
	std::string title;

	FTacticalCombatReportContext()
		: reportType(TRT_None), phase(0), actingPlayerID(0), immediate(false), title("") {}
};

struct FTacticalShipReportSummary {
	FTacticalShipReference ship;
	int attacksMade;
	int attacksReceived;
	int hitsScored;
	int hitsTaken;
	int hullDamageDealt;
	int hullDamageTaken;
	int internalEventsTriggered;
	std::vector<std::string> displayLines;

	FTacticalShipReportSummary()
		: attacksMade(0), attacksReceived(0), hitsScored(0), hitsTaken(0),
		  hullDamageDealt(0), hullDamageTaken(0), internalEventsTriggered(0) {}
};

struct FTacticalCombatReportSummary {
	FTacticalCombatReportContext context;
	std::vector<FTacticalShipReportSummary> ships;
	std::vector<std::string> displayLines;
};

struct FTacticalCombatReport {
	FTacticalCombatReportContext context;
	bool active;
	std::vector<FTacticalAttackReport> attacks;
	std::vector<FTacticalReportEvent> events;

	FTacticalCombatReport() : active(false), attacks(), events() {}

	void clear() {
		context = FTacticalCombatReportContext();
		active = false;
		attacks.clear();
		events.clear();
	}
};

inline FTacticalShipReference createTacticalShipReference(FVehicle *ship) {
	if (ship == NULL) {
		return FTacticalShipReference();
	}
	return FTacticalShipReference(ship->getID(), ship->getOwner(), ship->getName());
}

inline FTacticalWeaponReference createTacticalWeaponReference(const FWeapon *weapon) {
	if (weapon == NULL) {
		return FTacticalWeaponReference();
	}
	return FTacticalWeaponReference(weapon->getID(), weapon->getLongName());
}

}

#endif
