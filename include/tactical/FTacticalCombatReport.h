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
	int previousValue;
	int newValue;
	int amount;
	int hullDamage;
	int attackIndex;
	bool immediate;
	int damagedWeaponType;
	unsigned int damagedWeaponID;
	std::string damagedWeaponName;
	int damagedDefenseType;
	std::string damagedDefenseName;
	std::string label;
	std::string detail;

	FTacticalReportEvent()
		: eventType(TRET_None), damageEffectType(TDET_None), rollValue(-1),
		  previousValue(0), newValue(0), amount(0), hullDamage(0),
		  attackIndex(-1), immediate(false), damagedWeaponType(FWeapon::NONE), damagedWeaponID(0),
		  damagedWeaponName(""), damagedDefenseType(FDefense::UNDEF), damagedDefenseName(""),
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

struct FTacticalHitDetailSummary {
	FTacticalShipReference attacker;
	FTacticalWeaponReference weapon;
	FTacticalShipReference target;
	int hullDamage;
	std::vector<std::string> effects;
	std::string outcome;
	std::string displayLine;

	FTacticalHitDetailSummary() : hullDamage(0), effects(), outcome(""), displayLine("") {}
};

struct FTacticalCombatReportSummary {
	FTacticalCombatReportContext context;
	bool showHitDetails;
	std::vector<FTacticalShipReportSummary> ships;
	std::vector<FTacticalHitDetailSummary> hitDetails;
	std::vector<std::string> displayLines;

	FTacticalCombatReportSummary() : showHitDetails(true), ships(), hitDetails(), displayLines() {}
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

struct TacticalEffectSummaryAccumulator {
	std::map<std::string, int> effectCounts;
	std::vector<std::string> weaponHitAbbreviations;
	std::vector<std::string> defenseHitAbbreviations;
	int adfLossTotal;
	int mrLossTotal;

	TacticalEffectSummaryAccumulator() : effectCounts(), weaponHitAbbreviations(),
		defenseHitAbbreviations(), adfLossTotal(0), mrLossTotal(0) {}
};

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

inline bool shouldCountStandaloneEventHullDamage(
	const FTacticalShipReportSummary & shipSummary,
	const FTacticalReportEvent & event) {
	if (event.hullDamage <= 0) {
		return false;
	}

	// Attack-level hull damage remains canonical for ships that already have
	// damaging attacks in this report; report-level standalone hull-damage
	// effects still remain available as raw event detail.
	if (event.damageEffectType == TDET_HullDamage
		&& shipSummary.damagingAttacksReceived > 0) {
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

inline std::string summarizeHitDetailEffect(const FTacticalReportEvent & event) {
	std::string effect = summarizeEventEffect(event);
	if (event.detail.size() > 0) {
		effect += std::string(" (") + event.detail + ")";
	}
	return effect;
}

inline int summarizePointLoss(const FTacticalReportEvent & event) {
	if (event.amount > 0) {
		return event.amount;
	}
	if (event.previousValue > event.newValue) {
		return event.previousValue - event.newValue;
	}
	return 0;
}

inline std::string damagedWeaponAbbreviation(const FTacticalReportEvent & event) {
	switch (event.damagedWeaponType) {
	case FWeapon::LB:
		return "LB";
	case FWeapon::LC:
		return "LC";
	case FWeapon::DC:
		return "DC";
	case FWeapon::EB:
		return "EB";
	case FWeapon::PB:
		return "PB";
	case FWeapon::RB:
		return "RB";
	case FWeapon::AR:
		return "AR";
	case FWeapon::T:
		return "T";
	case FWeapon::SM:
		return "SM";
	case FWeapon::M:
		return "M";
	default:
		return "";
	}
}

inline std::string damagedDefenseAbbreviation(const FTacticalReportEvent & event) {
	switch (event.damagedDefenseType) {
	case FDefense::NONE:
		return "None";
	case FDefense::RH:
		return "RH";
	case FDefense::MS:
		return "MS";
	case FDefense::ICM:
		return "ICM";
	case FDefense::SS:
		return "SS";
	case FDefense::PS:
		return "PS";
	case FDefense::ES:
		return "ES";
	default:
		return "";
	}
}

inline void appendEffectSummary(
	FTacticalShipReportSummary & shipSummary,
	TacticalEffectSummaryAccumulator & effectSummary,
	const FTacticalReportEvent & event) {
	if (!eventRepresentsDamageEffect(event)) {
		return;
	}

	if (event.hullDamage <= 0) {
		shipSummary.nonHullEffectsTaken++;
	}

	if (event.damageEffectType == TDET_WeaponDamaged) {
		const std::string abbreviation = damagedWeaponAbbreviation(event);
		if (abbreviation.size() > 0) {
			effectSummary.weaponHitAbbreviations.push_back(abbreviation);
			return;
		}
	}

	if (event.eventType == TRET_DefenseEffect
		|| event.damageEffectType == TDET_DefenseDamaged) {
		const std::string abbreviation = damagedDefenseAbbreviation(event);
		if (abbreviation.size() > 0) {
			effectSummary.defenseHitAbbreviations.push_back(abbreviation);
			return;
		}
	}

	if (event.damageEffectType == TDET_ADFLoss) {
		effectSummary.adfLossTotal += summarizePointLoss(event);
		return;
	}

	if (event.damageEffectType == TDET_MRLoss) {
		effectSummary.mrLossTotal += summarizePointLoss(event);
		return;
	}

	if (event.hullDamage > 0 && event.damageEffectType == TDET_HullDamage) {
		return;
	}

	effectSummary.effectCounts[summarizeEventEffect(event)]++;
}

inline std::vector<std::string> buildShipSummaryDisplayLines(
	const FTacticalShipReportSummary & shipSummary,
	const TacticalEffectSummaryAccumulator & effectSummary) {
	std::vector<std::string> lines;
	lines.push_back(shipSummary.ship.shipName + ":");

	const bool hasHullDamageClause = shipSummary.hullDamageTaken > 0;
	if (hasHullDamageClause) {
		std::ostringstream os;
		os << " - " << shipSummary.hullDamageTaken << " hull damage";
		if (shipSummary.damagingAttacksReceived > 0) {
			os << " from " << shipSummary.damagingAttacksReceived << " attack";
			if (shipSummary.damagingAttacksReceived != 1) {
				os << "s";
			}
		}
		lines.push_back(os.str());
	}

	if (effectSummary.adfLossTotal > 0) {
		std::ostringstream os;
		os << " - ADF (-" << effectSummary.adfLossTotal << ")";
		lines.push_back(os.str());
	}

	if (effectSummary.mrLossTotal > 0) {
		std::ostringstream os;
		os << " - MR (-" << effectSummary.mrLossTotal << ")";
		lines.push_back(os.str());
	}

	if (!effectSummary.weaponHitAbbreviations.empty()) {
		std::ostringstream os;
		os << " - Weapon Hit: ";
		for (unsigned int i = 0; i < effectSummary.weaponHitAbbreviations.size(); i++) {
			if (i > 0) {
				os << ", ";
			}
			os << effectSummary.weaponHitAbbreviations[i];
		}
		lines.push_back(os.str());
	}

	if (!effectSummary.defenseHitAbbreviations.empty()) {
		std::ostringstream os;
		os << " - Defense Hit: ";
		for (unsigned int i = 0; i < effectSummary.defenseHitAbbreviations.size(); i++) {
			if (i > 0) {
				os << ", ";
			}
			os << effectSummary.defenseHitAbbreviations[i];
		}
		lines.push_back(os.str());
	}

	for (std::map<std::string, int>::const_iterator itr = effectSummary.effectCounts.begin();
		 itr != effectSummary.effectCounts.end(); ++itr) {
		std::ostringstream os;
		os << " - " << itr->first;
		if (itr->second > 1) {
			os << " x" << itr->second;
		}
		lines.push_back(os.str());
	}

	return lines;
}

inline std::string buildShipNameForDisplay(const FTacticalShipReference & ship, const std::string & fallback) {
	if (ship.shipName.size() > 0) {
		return ship.shipName;
	}
	if (ship.shipID != 0) {
		std::ostringstream os;
		os << fallback << " #" << ship.shipID;
		return os.str();
	}
	return fallback;
}

inline std::string buildWeaponNameForDisplay(const FTacticalWeaponReference & weapon) {
	if (weapon.weaponName.size() > 0) {
		return weapon.weaponName;
	}
	if (weapon.weaponID != 0) {
		std::ostringstream os;
		os << "Weapon #" << weapon.weaponID;
		return os.str();
	}
	return "No weapon";
}

inline std::string joinHitDetailEffects(const std::vector<std::string> & effects) {
	std::ostringstream os;
	for (unsigned int i = 0; i < effects.size(); i++) {
		if (i > 0) {
			os << ", ";
		}
		os << effects[i];
	}
	return os.str();
}

inline bool isRedundantHitDetailNote(const std::string & note) {
	return note == "Attack hit target";
}

inline FTacticalHitDetailSummary buildHitDetailSummary(const FTacticalAttackReport & attack) {
	FTacticalHitDetailSummary detail;
	detail.attacker = attack.attacker;
	detail.weapon = attack.weapon;
	detail.target = attack.target;
	detail.hullDamage = attack.hullDamage;

	for (unsigned int i = 0; i < attack.internalEvents.size(); i++) {
		const FTacticalReportEvent & event = attack.internalEvents[i];
		if (!eventRepresentsDamageEffect(event)) {
			continue;
		}
		detail.effects.push_back(summarizeHitDetailEffect(event));
	}

	std::ostringstream outcome;
	if (attack.hullDamage > 0) {
		outcome << attack.hullDamage << " hull damage";
	}
	if (!detail.effects.empty()) {
		if (outcome.str().size() > 0) {
			outcome << "; ";
		}
		outcome << "effects: " << joinHitDetailEffects(detail.effects);
	}
	if (outcome.str().size() == 0) {
		outcome << "no damage effects";
	}
	if (attack.note.size() > 0
		&& !isRedundantHitDetailNote(attack.note)) {
		outcome << " (" << attack.note << ")";
	}
	detail.outcome = outcome.str();

	std::ostringstream line;
	line << buildShipNameForDisplay(attack.attacker, "Unknown attacker")
		 << " [" << buildWeaponNameForDisplay(attack.weapon) << "] -> "
		 << buildShipNameForDisplay(attack.target, "Unknown target")
		 << ": " << detail.outcome;
	detail.displayLine = line.str();
	return detail;
}

} // namespace TacticalCombatReportDetail

inline FTacticalCombatReportSummary buildTacticalCombatReportSummary(const FTacticalCombatReport & report) {
	FTacticalCombatReportSummary summary;
	summary.context = report.context;

	std::map<TacticalCombatReportDetail::TacticalShipSummaryKey, FTacticalShipReportSummary> summaryMap;
	std::vector<TacticalCombatReportDetail::TacticalShipSummaryKey> summaryOrder;
	std::map<TacticalCombatReportDetail::TacticalShipSummaryKey, TacticalCombatReportDetail::TacticalEffectSummaryAccumulator> effectSummaries;

	for (unsigned int i = 0; i < report.attacks.size(); i++) {
		const FTacticalAttackReport & attack = report.attacks[i];

		if (summary.showHitDetails && attack.hit) {
			summary.hitDetails.push_back(TacticalCombatReportDetail::buildHitDetailSummary(attack));
		}

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
		if (TacticalCombatReportDetail::shouldCountStandaloneEventHullDamage(eventSummary, event)) {
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

		const std::vector<std::string> displayLines =
			TacticalCombatReportDetail::buildShipSummaryDisplayLines(shipSummary, effectSummaries[key]);
		for (unsigned int j = 0; j < displayLines.size(); j++) {
			shipSummary.displayLines.push_back(displayLines[j]);
			summary.displayLines.push_back(displayLines[j]);
		}
		summary.ships.push_back(shipSummary);
	}

	return summary;
}

}

#endif
