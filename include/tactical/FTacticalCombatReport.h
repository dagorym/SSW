#ifndef _FTACTICALCOMBATREPORT_H_
#define _FTACTICALCOMBATREPORT_H_

#include <map>
#include <sstream>
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
	TacticalDamageEffectType damageEffectType;
	FTacticalShipReference subject;
	FTacticalShipReference source;
	FTacticalShipReference target;
	int rollValue;
	int hullDamage;
	int attackIndex;
	bool immediate;
	int damagedWeaponType;
	unsigned int damagedWeaponID;
	std::string damagedWeaponName;
	std::string label;
	std::string detail;

	FTacticalReportEvent()
		: eventType(TRET_None), damageEffectType(TDET_None), rollValue(-1), hullDamage(0),
		  attackIndex(-1), immediate(false), damagedWeaponType(FWeapon::NONE), damagedWeaponID(0),
		  damagedWeaponName(""), label(""), detail("") {}
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

inline std::string tacticalCombatReportTypeLabel(TacticalReportType reportType) {
	switch (reportType) {
	case TRT_DefensiveFire:
		return "Defensive Fire Results";
	case TRT_OffensiveFire:
		return "Offensive Fire Results";
	case TRT_ElectricalFire:
		return "Electrical Fire Damage";
	case TRT_MineDamage:
		return "Mine Damage";
	default:
		return "Tactical Damage Summary";
	}
}

inline std::string tacticalCombatReportContextLabel(const FTacticalCombatReportContext & context) {
	if (context.title.size() > 0) {
		return context.title;
	}
	return tacticalCombatReportTypeLabel(context.reportType);
}

inline std::string tacticalCombatReportContextModeLabel(const FTacticalCombatReportContext & context) {
	if (context.immediate) {
		return "Immediate report";
	}
	return "End-of-phase report";
}

inline std::string tacticalCombatReportDialogTitle(const FTacticalCombatReportContext & context) {
	const std::string contextLabel = tacticalCombatReportContextLabel(context);
	if (contextLabel.size() == 0 || contextLabel == "Tactical Damage Summary") {
		return "Tactical Damage Summary";
	}
	return std::string("Tactical Damage Summary - ") + contextLabel;
}

struct FTacticalShipReportSummary {
	FTacticalShipReference ship;
	int attacksMade;
	int attacksReceived;
	int hitsScored;
	int hitsTaken;
	int hullDamageDealt;
	int hullDamageTaken;
	int internalEventsTriggered;
	int damagingAttacksReceived;
	int nonHullEffectsTaken;
	std::vector<std::string> displayLines;
	std::vector<FTacticalAttackReport> rawAttacksReceived;
	std::vector<FTacticalReportEvent> rawEvents;

	FTacticalShipReportSummary()
		: attacksMade(0), attacksReceived(0), hitsScored(0), hitsTaken(0),
		  hullDamageDealt(0), hullDamageTaken(0), internalEventsTriggered(0),
		  damagingAttacksReceived(0), nonHullEffectsTaken(0) {}

	bool hasSustainedDamage() const {
		return hullDamageTaken > 0 || nonHullEffectsTaken > 0;
	}
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

namespace TacticalCombatReportDetail {

struct TacticalShipSummaryKey {
	unsigned int shipID;
	unsigned int ownerID;

	TacticalShipSummaryKey() : shipID(0), ownerID(0) {}
	TacticalShipSummaryKey(const FTacticalShipReference & ship)
		: shipID(ship.shipID), ownerID(ship.ownerID) {}

	bool operator<(const TacticalShipSummaryKey & other) const {
		if (ownerID != other.ownerID) {
			return ownerID < other.ownerID;
		}
		return shipID < other.shipID;
	}
};

inline FTacticalShipReportSummary & ensureShipSummary(
	std::map<TacticalShipSummaryKey, FTacticalShipReportSummary> & summaryMap,
	std::vector<TacticalShipSummaryKey> & summaryOrder,
	const FTacticalShipReference & ship) {
	TacticalShipSummaryKey key(ship);
	std::map<TacticalShipSummaryKey, FTacticalShipReportSummary>::iterator itr = summaryMap.find(key);
	if (itr == summaryMap.end()) {
		FTacticalShipReportSummary summary;
		summary.ship = ship;
		summaryMap[key] = summary;
		summaryOrder.push_back(key);
		itr = summaryMap.find(key);
	}
	return itr->second;
}

inline bool eventRepresentsDamageEffect(const FTacticalReportEvent & event) {
	return event.hullDamage > 0 || event.eventType == TRET_InternalDamage
		|| event.eventType == TRET_ElectricalFire || event.eventType == TRET_MineDamage
		|| event.eventType == TRET_DefenseEffect;
}

inline bool eventMatchesShipReference(
	const FTacticalShipReference & left,
	const FTacticalShipReference & right) {
	return left.shipID == right.shipID && left.ownerID == right.ownerID;
}

inline bool shouldCountInternalEventHullDamage(
	const FTacticalAttackReport & attack,
	const FTacticalReportEvent & event) {
	if (event.hullDamage <= 0) {
		return false;
	}

	// Attack-level hull damage is the canonical player-facing total for the target ship.
	// Nested hull-damage events remain available as raw detail, but only suppress
	// player-facing hull damage when they target that same ship.
	if (attack.hullDamage > 0
		&& attack.target.isValid()
		&& eventMatchesShipReference(event.subject, attack.target)) {
		return false;
	}

	return true;
}

inline std::string summarizeEventEffect(const FTacticalReportEvent & event) {
	if (event.label.size() > 0) {
		return event.label;
	}

	switch (event.eventType) {
	case TRET_InternalDamage:
		return "Internal damage";
	case TRET_ElectricalFire:
		return "Electrical fire";
	case TRET_MineDamage:
		return "Mine damage";
	case TRET_DefenseEffect:
		return "Defense effect";
	case TRET_Note:
		return "Note";
	default:
		return "Damage effect";
	}
}

inline void appendEffectSummary(
	FTacticalShipReportSummary & shipSummary,
	std::map<std::string, int> & effectCounts,
	const FTacticalReportEvent & event) {
	if (!eventRepresentsDamageEffect(event)) {
		return;
	}

	if (event.hullDamage <= 0) {
		shipSummary.nonHullEffectsTaken++;
	}

	effectCounts[summarizeEventEffect(event)]++;
}

inline std::string buildShipSummaryDisplayLine(
	const FTacticalShipReportSummary & shipSummary,
	const std::map<std::string, int> & effectCounts) {
	std::ostringstream os;
	os << shipSummary.ship.shipName << ": "
	   << shipSummary.hullDamageTaken << " hull damage";

	if (shipSummary.damagingAttacksReceived > 0) {
		os << " from " << shipSummary.damagingAttacksReceived << " attack";
		if (shipSummary.damagingAttacksReceived != 1) {
			os << "s";
		}
	}

	if (!effectCounts.empty()) {
		os << "; effects: ";
		bool first = true;
		for (std::map<std::string, int>::const_iterator itr = effectCounts.begin();
			 itr != effectCounts.end(); ++itr) {
			if (!first) {
				os << ", ";
			}
			first = false;
			os << itr->first;
			if (itr->second > 1) {
				os << " x" << itr->second;
			}
		}
	}

	return os.str();
}

} // namespace TacticalCombatReportDetail

inline FTacticalCombatReportSummary buildTacticalCombatReportSummary(const FTacticalCombatReport & report) {
	FTacticalCombatReportSummary summary;
	summary.context = report.context;

	std::map<TacticalCombatReportDetail::TacticalShipSummaryKey, FTacticalShipReportSummary> summaryMap;
	std::vector<TacticalCombatReportDetail::TacticalShipSummaryKey> summaryOrder;
	std::map<TacticalCombatReportDetail::TacticalShipSummaryKey, std::map<std::string, int> > effectSummaries;

	for (unsigned int i = 0; i < report.attacks.size(); i++) {
		const FTacticalAttackReport & attack = report.attacks[i];

		if (attack.attacker.isValid()) {
			FTacticalShipReportSummary & attackerSummary =
				TacticalCombatReportDetail::ensureShipSummary(summaryMap, summaryOrder, attack.attacker);
			attackerSummary.attacksMade++;
			if (attack.hit) {
				attackerSummary.hitsScored++;
			}
			attackerSummary.hullDamageDealt += attack.hullDamage;
		}

		if (attack.target.isValid()) {
			FTacticalShipReportSummary & targetSummary =
				TacticalCombatReportDetail::ensureShipSummary(summaryMap, summaryOrder, attack.target);
			targetSummary.attacksReceived++;
			if (attack.hit) {
				targetSummary.hitsTaken++;
			}
			if (attack.hullDamage > 0) {
				targetSummary.hullDamageTaken += attack.hullDamage;
				targetSummary.damagingAttacksReceived++;
			}
			if (attack.hit || !attack.internalEvents.empty()) {
				targetSummary.rawAttacksReceived.push_back(attack);
			}

			for (unsigned int j = 0; j < attack.internalEvents.size(); j++) {
				const FTacticalReportEvent & event = attack.internalEvents[j];
				if (!event.subject.isValid()) {
					continue;
				}

				TacticalCombatReportDetail::TacticalShipSummaryKey eventKey(event.subject);
				FTacticalShipReportSummary & eventSummary =
					TacticalCombatReportDetail::ensureShipSummary(summaryMap, summaryOrder, event.subject);
				eventSummary.internalEventsTriggered++;
				eventSummary.rawEvents.push_back(event);
				if (TacticalCombatReportDetail::shouldCountInternalEventHullDamage(attack, event)) {
					eventSummary.hullDamageTaken += event.hullDamage;
				}
				TacticalCombatReportDetail::appendEffectSummary(
					eventSummary, effectSummaries[eventKey], event);
			}
		}
	}

	for (unsigned int i = 0; i < report.events.size(); i++) {
		const FTacticalReportEvent & event = report.events[i];
		if (!event.subject.isValid()) {
			continue;
		}

		TacticalCombatReportDetail::TacticalShipSummaryKey eventKey(event.subject);
		FTacticalShipReportSummary & eventSummary =
			TacticalCombatReportDetail::ensureShipSummary(summaryMap, summaryOrder, event.subject);
		eventSummary.internalEventsTriggered++;
		eventSummary.rawEvents.push_back(event);
		if (event.hullDamage > 0) {
			eventSummary.hullDamageTaken += event.hullDamage;
		}
		TacticalCombatReportDetail::appendEffectSummary(
			eventSummary, effectSummaries[eventKey], event);
	}

	for (unsigned int i = 0; i < summaryOrder.size(); i++) {
		const TacticalCombatReportDetail::TacticalShipSummaryKey & key = summaryOrder[i];
		FTacticalShipReportSummary shipSummary = summaryMap[key];
		if (!shipSummary.hasSustainedDamage()) {
			continue;
		}

		const std::string displayLine =
			TacticalCombatReportDetail::buildShipSummaryDisplayLine(shipSummary, effectSummaries[key]);
		shipSummary.displayLines.push_back(displayLine);
		summary.displayLines.push_back(displayLine);
		summary.ships.push_back(shipSummary);
	}

	return summary;
}

}

#endif
