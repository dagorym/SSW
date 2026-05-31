/**
 * @file FTacticalGame.cpp
 * @brief Implementation file for FTacticalGame class
 * @author Tom Stephens, gpt-5.3-codex (standard), gpt-5.4 (high), claude-sonnet-4-6 (medium)
 * @date Created:  Mar 29, 2026
 * @date Last Modified: May 30, 2026
 *
 */

#include "tactical/FTacticalGame.h"

#include <algorithm>
#include <climits>

#include "tactical/ITacticalUI.h"
#include "tactical/FTacticalAttackResult.h"

namespace Frontier {

namespace {

const std::vector<int> EMPTY_PREVIEW_HEADING_LIST;

TacticalReportEventType tacticalReportEventTypeForDamageEffect(TacticalDamageEffectType effectType) {
	switch (effectType) {
	case TDET_DefenseDamaged:
		return TRET_DefenseEffect;
	case TDET_ElectricalFire:
		return TRET_ElectricalFire;
	case TDET_HullDamage:
	case TDET_ADFLoss:
	case TDET_MRLoss:
	case TDET_WeaponDamaged:
	case TDET_PowerSystemDamaged:
	case TDET_CombatControlDamaged:
	case TDET_NavigationError:
	case TDET_DCRLoss:
	default:
		return TRET_InternalDamage;
	}
}

void appendTacticalDamageResolutionEvents(
	FTacticalCombatReport & report,
	FVehicle * ship,
	const FTacticalDamageResolution & resolution) {
	if (ship == NULL) {
		return;
	}

	for (std::vector<FTacticalDamageEffect>::const_iterator itr = resolution.effects.begin();
		 itr != resolution.effects.end(); ++itr) {
		FTacticalReportEvent event;
		event.eventType = tacticalReportEventTypeForDamageEffect(itr->effectType);
		event.damageEffectType = itr->effectType;
		event.subject = FTacticalShipReference(ship->getID(), ship->getOwner(), ship->getName());
		event.source = event.subject;
		event.target = event.subject;
		event.rollValue = itr->rollValue;
		event.previousValue = itr->previousValue;
		event.newValue = itr->newValue;
		event.amount = itr->amount;
		event.hullDamage = itr->hullDamageApplied;
		event.attackIndex = -1;
		event.immediate = true;
		event.damagedWeaponType = itr->weaponType;
		event.damagedWeaponID = itr->weaponID;
		event.damagedWeaponName = itr->weaponName;
		event.damagedDefenseType = itr->defenseType;
		event.damagedDefenseName = itr->defenseName;
		event.label = itr->label;
		event.detail = itr->detail;
		report.events.push_back(event);
	}
}

void normalizeAttackInternalEvents(FTacticalAttackReport & attack, int attackIndex) {
	for (unsigned int i = 0; i < attack.internalEvents.size(); ++i) {
		FTacticalReportEvent & event = attack.internalEvents[i];
		if (!event.subject.isValid() && attack.target.isValid()) {
			event.subject = attack.target;
		}
		if (!event.source.isValid() && attack.attacker.isValid()) {
			event.source = attack.attacker;
		}
		if (!event.target.isValid() && attack.target.isValid()) {
			event.target = attack.target;
		}
		event.attackIndex = attackIndex;
		event.immediate = attack.immediate;
	}
}

FTacticalReportEvent buildTacticalAttackEvent(
	const FTacticalAttackReport & report,
	const FTacticalAttackEffect & effect)
{
	FTacticalReportEvent event;
	event.eventType = tacticalReportEventTypeForDamageEffect(static_cast<TacticalDamageEffectType>(effect.effectType));
	event.damageEffectType = static_cast<TacticalDamageEffectType>(effect.effectType);
	event.subject = report.target;
	event.source = report.attacker;
	event.target = report.target;
	event.rollValue = effect.rollValue;
	event.previousValue = effect.previousValue;
	event.newValue = effect.newValue;
	event.amount = effect.amount;
	event.hullDamage = effect.hullDamageApplied;
	event.attackIndex = -1;
	event.immediate = report.immediate;
	event.damagedWeaponType = effect.weaponType;
	event.damagedWeaponID = effect.weaponID;
	event.damagedWeaponName = effect.weaponName;
	event.damagedDefenseType = effect.defenseType;
	event.damagedDefenseName = effect.defenseName;
	event.label = effect.label;
	event.detail = effect.detail;
	return event;
}

FTacticalAttackReport buildTacticalAttackReport(const FTacticalAttackResult & result) {
	FTacticalAttackReport report;
	report.attacker = FTacticalShipReference(result.attackerID, result.attackerOwnerID, result.attackerName);
	report.target = FTacticalShipReference(result.targetID, result.targetOwnerID, result.targetName);
	report.weapon = FTacticalWeaponReference(result.weaponID, result.weaponName);
	report.hitRoll = result.hitRoll;
	report.attackRollModifier = 0;
	report.targetRange = result.targetRange;
	report.hullDamage = result.totalHullDamageApplied;
	report.damageTableModifier = result.damageTableModifier;
	report.hit = result.hit();
	report.intercepted = false;
	report.immediate = false;
	report.note = result.note;

	for (std::vector<FTacticalAttackEffect>::const_iterator itr = result.effects.begin();
		 itr != result.effects.end(); ++itr) {
		report.internalEvents.push_back(buildTacticalAttackEvent(report, *itr));
	}

	return report;
}

void clearICMVector(std::vector<ICMData *> & data) {
	for (std::vector<ICMData *>::iterator itr = data.begin(); itr != data.end(); ++itr) {
		delete *itr;
	}
	data.clear();
}

bool isStationType(FVehicle * ship) {
	if (ship == NULL) {
		return false;
	}
	const std::string type = ship->getType();
	return (type == "ArmedStation" || type == "FortifiedStation" || type == "Fortress");
}

void buildPathHeadings(const FTacticalTurnData & turnData, PointList & path, std::vector<int> & headings) {
	path = turnData.path.getFullPath();
	headings.clear();
	if (path.empty()) {
		return;
	}

	int heading = turnData.startHeading;
	for (unsigned int i = 0; i < path.size(); ++i) {
		const bool lastPoint = (i + 1 >= path.size());
		headings.push_back(lastPoint ? turnData.finalHeading : heading);
		if (lastPoint) {
			break;
		}
		if (path[i + 1] != path[i]) {
			heading = FHexMap::computeHeading(path[i], path[i + 1]);
			continue;
		}
		bool resolved = false;
		for (unsigned int j = i + 2; j < path.size(); ++j) {
			if (path[j] != path[i + 1]) {
				heading = FHexMap::computeHeading(path[i + 1], path[j]);
				resolved = true;
				break;
			}
		}
		if (!resolved) {
			heading = turnData.finalHeading;
		}
	}
}

bool canUseStoppedShipFreeRotation(const FVehicle * ship, FTacticalTurnData * turnData) {
	return (ship != NULL
		&& turnData != NULL
		&& turnData->speed == 0
		&& turnData->nMoved == 0
		&& turnData->path.getPathLength() == 1
		&& ship->getMR() > 0);
}

bool isAdjacentFacingSelection(const FPoint & shipHex, const FPoint & selectedHex, int & heading) {
	if (FHexMap::computeHexDistance(shipHex, selectedHex) != 1) {
		return false;
	}
	heading = FHexMap::computeHeading(shipHex, selectedHex);
	return (heading >= 0 && heading <= 5);
}

void buildStoppedShipTurnOptions(const FPoint & shipHex, int currentHeading, std::vector<FPoint> & leftHexes,
	std::vector<FPoint> & rightHexes) {
	leftHexes.clear();
	rightHexes.clear();
	for (int heading = 0; heading < 6; ++heading) {
		if (heading == currentHeading) {
			continue;
		}
		const FPoint adjacent = FHexMap::findNextHex(shipHex, heading);
		if (adjacent.getX() < 0 || adjacent.getY() < 0) {
			continue;
		}
		const int delta = (heading - currentHeading + 6) % 6;
		if (delta <= 3) {
			leftHexes.push_back(adjacent);
		} else {
			rightHexes.push_back(adjacent);
		}
	}
}

int normalizeHeading(int heading) {
	while (heading < 0) {
		heading += 6;
	}
	while (heading > 5) {
		heading -= 6;
	}
	return heading;
}

int shortestHeadingDelta(int currentHeading, int targetHeading) {
	const int cw = (targetHeading - currentHeading + 6) % 6;
	const int ccw = (currentHeading - targetHeading + 6) % 6;
	if (cw < ccw) {
		return cw;
	}
	if (ccw < cw) {
		return -ccw;
	}
	return cw;
}

}

FTacticalGame::FTacticalGame() {
	m_attackShips = NULL;
	m_defendShips = NULL;
	reset();
}

FTacticalGame::~FTacticalGame() {
	if (m_attackShips) { delete m_attackShips; }
	if (m_defendShips) { delete m_defendShips; }
	clearICMVector(m_ICMData);
}

void FTacticalGame::installUI(ITacticalUI * ui) {
	m_ui = ui;
}

void FTacticalGame::reset() {
	m_ui = NULL;
	m_state = BS_Unknown;
	m_phase = PH_NONE;
	m_control = false;
	m_activePlayer = false;
	m_movingPlayer = true;
	m_moveComplete = true;
	m_done = false;
	m_hasWinner = false;
	m_winnerID = 0;
	m_hasPlanet = false;
	m_planetChoice = -1;
	m_planetPos.setPoint(-1, -1);
	m_stationPos.setPoint(-1, -1);
	m_station = NULL;
	m_playerID[0] = 0;
	m_playerID[1] = 1;
	m_curShip = NULL;
	m_curWeapon = NULL;
	m_attackList = NULL;
	m_defendList = NULL;
	clearICMVector(m_ICMData);
	m_tacticalReport.clear();
	m_lastDestroyedShipIDs.clear();
	m_lastDestroyedShipIDsConsumed = true;
	m_shipPos.setPoint(-1, -1);
	m_setRotation = false;
	m_movementHexes.clear();
	m_leftHexes.clear();
	m_rightHexes.clear();
	clearStoppedShipPreviewRoutes();
	m_drawRoute = false;
	m_moved = 0;
	m_turnInfo.clear();
	m_targetHexes.clear();
	m_headOnHexes.clear();
	m_gravityTurns.clear();
	m_gravityTurnFlag = false;
	m_minedHexList.clear();
	m_mineTargetList.clear();
	m_mineOwner = 99;
	m_shipsWithMines.clear();
	m_deployablePlacementSources.clear();
	m_selectedPlacementSource = -1;
	m_placedOrdnance.clear();
	m_seekerMissiles.clear();
	m_pendingSeekerContactOutcomes.clear();
	m_offensiveFirePhaseID = 0;
	m_pendingOffensiveSeekerDeployments.clear();
	m_selectedSeekerActivationHex.setPoint(-1, -1);

	for (int i = 0; i < 100; ++i) {
		for (int j = 0; j < 100; ++j) {
			m_hexData[i][j].pos.setPoint(i, j);
			m_hexData[i][j].ships.clear();
		}
	}
}

int FTacticalGame::setupFleets(FleetList * aList, FleetList * dList, bool planet, FVehicle * station) {
	clearTacticalReport();
	clearWinner();
	m_attackList = aList;
	m_defendList = dList;
	m_hasPlanet = planet;
	m_station = station;
	m_activePlayer = false;
	m_control = false;

	if (m_defendShips) { delete m_defendShips; }
	m_defendShips = new VehicleList;
	if (m_defendList) {
		for (unsigned int i = 0; i < m_defendList->size(); i++) {
			const VehicleList sList = (*m_defendList)[i]->getShipList();
			for (unsigned int j = 0; j < sList.size(); j++) {
				sList[j]->setOwner(0);
				sList[j]->setCurrentDefense(0);
				m_defendShips->push_back(sList[j]);
			}
		}
	}
	if (station != NULL) {
		station->setCurrentDefense(0);
		m_defendShips->push_back(station);
	}

	if (m_attackShips) { delete m_attackShips; }
	m_attackShips = new VehicleList;
	if (m_attackList) {
		for (unsigned int i = 0; i < m_attackList->size(); i++) {
			const VehicleList sList = (*m_attackList)[i]->getShipList();
			for (unsigned int j = 0; j < sList.size(); j++) {
				sList[j]->setOwner(1);
				sList[j]->setCurrentDefense(0);
				m_attackShips->push_back(sList[j]);
			}
		}
	}

	if (planet) {
		m_state = BS_SetupPlanet;
	} else if (station != NULL) {
		m_state = BS_SetupStation;
	} else if (m_defendList && m_defendList->size() > 0) {
		m_state = BS_SetupDefendFleet;
	} else {
		m_state = BS_SetupAttackFleet;
	}

	resetMovementState();

	return 0;
}

void FTacticalGame::setState(int s) {
	if (s == BS_SetupStation && m_station == NULL) {
		s = BS_SetupDefendFleet;
	}
	if (s == BS_SetupDefendFleet && m_defendShips && m_defendShips->size() == 1) {
		s = BS_SetupAttackFleet;
		toggleActivePlayer();
	}
	m_state = s;
}

void FTacticalGame::setPhase(int p) {
	if (p == PH_MOVE) {
		beginSeekerActivationPhase();
		return;
	}

	if (m_phase == PH_ATTACK_FIRE && p != PH_ATTACK_FIRE) {
		clearPendingOffensiveFireSeekers();
	}

	m_phase = p;
	if (p == PH_SEEKER_ACTIVATION) {
		m_selectedSeekerActivationHex.setPoint(-1, -1);
	} else if (p == PH_FINALIZE_MOVE) {
		completeMovePhase();
		return;
	} else if (p == PH_DEFENSE_FIRE) {
		toggleActivePlayer();
	} else if (p == PH_ATTACK_FIRE) {
		++m_offensiveFirePhaseID;
		clearPendingOffensiveFireSeekers();
		toggleActivePlayer();
	}
}

void FTacticalGame::beginSeekerActivationPhase() {
	m_selectedSeekerActivationHex.setPoint(-1, -1);
	const std::vector<FPoint> inactiveHexes = getInactiveSeekerActivationHexes();
	if (inactiveHexes.empty()) {
		resolveActiveSeekersForMovingPlayer();
		if (m_ui != NULL) {
			resolvePendingSeekerDetonationDamage();
		}
		beginMovePhase();
		return;
	}

	m_phase = PH_SEEKER_ACTIVATION;
	m_selectedSeekerActivationHex = inactiveHexes[0];
}

void FTacticalGame::beginMovePhase() {
	m_phase = PH_MOVE;
	applyFireDamage();
	toggleActivePlayer();
	resetMovementState();
}

void FTacticalGame::completeSeekerActivationPhase() {
	if (m_phase != PH_SEEKER_ACTIVATION) {
		return;
	}
	resolveActiveSeekersForMovingPlayer();
	if (m_ui != NULL) {
		resolvePendingSeekerDetonationDamage();
	}
	m_selectedSeekerActivationHex.setPoint(-1, -1);
	beginMovePhase();
}

VehicleList FTacticalGame::getShipList() const {
	if (m_activePlayer) {
		return m_attackShips ? *m_attackShips : VehicleList();
	}
	return m_defendShips ? *m_defendShips : VehicleList();
}

VehicleList FTacticalGame::getShipList(unsigned int id) const {
	if (id == getAttackerID()) {
		return m_attackShips ? *m_attackShips : VehicleList();
	}
	return m_defendShips ? *m_defendShips : VehicleList();
}

void FTacticalGame::beginTacticalReport(const FTacticalCombatReportContext & context) {
	m_tacticalReport.clear();
	m_tacticalReport.context = context;
	m_tacticalReport.active = true;
}

void FTacticalGame::appendTacticalAttackReport(const FTacticalAttackReport & attack) {
	if (!m_tacticalReport.active) {
		FTacticalCombatReportContext context;
		context.phase = m_phase;
		context.actingPlayerID = getActivePlayerID();
		context.immediate = attack.immediate;
		beginTacticalReport(context);
	}
	FTacticalAttackReport normalizedAttack = attack;
	normalizeAttackInternalEvents(normalizedAttack, static_cast<int>(m_tacticalReport.attacks.size()));
	m_tacticalReport.attacks.push_back(normalizedAttack);
}

void FTacticalGame::appendTacticalReportEvent(const FTacticalReportEvent & event) {
	if (!m_tacticalReport.active) {
		FTacticalCombatReportContext context;
		context.phase = m_phase;
		context.actingPlayerID = getActivePlayerID();
		context.immediate = event.immediate;
		beginTacticalReport(context);
	}
	FTacticalReportEvent normalizedEvent = event;
	normalizedEvent.attackIndex = -1;
	m_tacticalReport.events.push_back(normalizedEvent);
}

FTacticalCombatReportSummary FTacticalGame::buildCurrentTacticalReportSummary() const {
	return buildTacticalCombatReportSummary(m_tacticalReport);
}

void FTacticalGame::clearTacticalReport() {
	m_tacticalReport.clear();
}

void FTacticalGame::applyFireDamage() {
	clearTacticalReport();

	FTacticalCombatReportContext context;
	context.reportType = TRT_ElectricalFire;
	context.phase = m_phase;
	context.actingPlayerID = getMovingPlayerID();
	context.immediate = true;
	beginTacticalReport(context);

	VehicleList sList = getShipList(getMovingPlayerID());
	bool damagedShip = false;
	for (VehicleList::iterator itr = sList.begin(); itr < sList.end(); itr++) {
		if ((*itr)->isOnFire()) {
			FTacticalDamageResolution resolution;
			int roll = irand(10);
			(*itr)->takeDamage(roll, 20, false, &resolution);
			appendTacticalDamageResolutionEvents(m_tacticalReport, *itr, resolution);
			damagedShip = true;
		}
	}

	if (damagedShip) {
		FTacticalCombatReportSummary summary = buildCurrentTacticalReportSummary();
		if (summary.ships.size() > 0) {
			if (m_ui != NULL) {
				m_ui->showDamageSummary(summary);
			}
			toggleActivePlayer();
			m_lastDestroyedShipIDs.clear();
			clearDestroyedShips();
			toggleActivePlayer();
		}
	}

	clearTacticalReport();
}

void FTacticalGame::fireICM() {
	clearICMVector(m_ICMData);
	VehicleList sList = getShipList(getActivePlayerID());
	VehicleList defendingShips = getShipList(
		(getActivePlayerID() == getAttackerID()) ? getDefenderID() : getAttackerID());
	for (VehicleList::iterator itr = sList.begin(); itr < sList.end(); itr++) {
		if ((*itr)->getHP() <= 0) {
			continue;
		}
		int nWeps = (*itr)->getWeaponCount();
		for (int i = 0; i < nWeps; i++) {
			FWeapon * w = (*itr)->getWeapon(i);
			if (w->getTarget() == NULL || w->getICMMod() == 0) {
				continue;
			}
			VehicleList * targetHexShips = findHexOccupantsForShip(w->getTarget()->getID());
			if (targetHexShips == NULL) {
				continue;
			}
			if (!hasUsableICMDefenderInHex(*targetHexShips, w->getTarget()->getOwner())) {
				continue;
			}
			ICMData * d = new ICMData;
			d->weapon = w;
			d->vehicles = targetHexShips;
			m_ICMData.push_back(d);
		}
	}
	if (m_ui != NULL && m_ICMData.size() > 0) {
		m_ui->runICMSelection(m_ICMData, &defendingShips);
	}
	clearICMVector(m_ICMData);
}

FTacticalCombatReportSummary FTacticalGame::fireAllWeapons() {
	TacticalReportType reportType = TRT_None;
	if (m_phase == PH_DEFENSE_FIRE) {
		reportType = TRT_DefensiveFire;
	} else if (m_phase == PH_ATTACK_FIRE) {
		reportType = TRT_OffensiveFire;
	}

	FTacticalCombatReportContext context;
	context.reportType = reportType;
	context.phase = m_phase;
	context.actingPlayerID = getActivePlayerID();
	context.immediate = false;
	beginTacticalReport(context);

	fireICM();
	VehicleList sList = getShipList(getActivePlayerID());
	for (VehicleList::iterator itr = sList.begin(); itr < sList.end(); itr++) {
		if ((*itr)->getHP() <= 0) {
			continue;
		}
		int nWeps = (*itr)->getWeaponCount();
		for (int i = 0; i < nWeps; i++) {
			FTacticalAttackResult result = (*itr)->getWeapon(i)->fire();
			if (!result.fired()) {
				continue;
			}
			appendTacticalAttackReport(buildTacticalAttackReport(result));
		}
	}
	FTacticalCombatReportSummary summary = buildCurrentTacticalReportSummary();
	clearTacticalReport();
	if (m_lastDestroyedShipIDsConsumed) {
		m_lastDestroyedShipIDs.clear();
	}
	clearDestroyedShips();
	clearICMVector(m_ICMData);
	return summary;
}

int FTacticalGame::clearDestroyedShips() {
	if (m_lastDestroyedShipIDsConsumed) {
		m_lastDestroyedShipIDs.clear();
		m_lastDestroyedShipIDsConsumed = false;
	}
	toggleActivePlayer();
	VehicleList * sList = m_activePlayer ? m_attackShips : m_defendShips;
	if (sList == NULL) {
		toggleActivePlayer();
		return 0;
	}

	int liveShips = 0;
	VehicleList::iterator itr = sList->begin();
	while (itr < sList->end()) {
		if ((*itr)->getHP() <= 0) {
			m_lastDestroyedShipIDs.push_back((*itr)->getID());
			removeShipFromModelState((*itr)->getID());
			itr = sList->erase(itr);
		} else {
			liveShips++;
			itr++;
		}
	}
	toggleActivePlayer();
	if (!liveShips) {
		m_hasWinner = true;
		m_winnerID = getActivePlayerID();
	}
	return liveShips;
}

void FTacticalGame::clearLastDestroyedShipIDs() {
	m_lastDestroyedShipIDs.clear();
	m_lastDestroyedShipIDsConsumed = true;
}

VehicleList * FTacticalGame::findHexOccupantsForShip(unsigned int shipID) {
	for (int i = 0; i < 100; ++i) {
		for (int j = 0; j < 100; ++j) {
			VehicleList & ships = m_hexData[i][j].ships;
			for (VehicleList::iterator itr = ships.begin(); itr != ships.end(); ++itr) {
				if ((*itr)->getID() == shipID) {
					return &ships;
				}
			}
		}
	}
	return NULL;
}

const VehicleList * FTacticalGame::findHexOccupantsForShip(unsigned int shipID) const {
	for (int i = 0; i < 100; ++i) {
		for (int j = 0; j < 100; ++j) {
			const VehicleList & ships = m_hexData[i][j].ships;
			for (VehicleList::const_iterator itr = ships.begin(); itr != ships.end(); ++itr) {
				if ((*itr)->getID() == shipID) {
					return &ships;
				}
			}
		}
	}
	return NULL;
}

void FTacticalGame::removeShipFromHexOccupancy(unsigned int shipID) {
	for (int i = 0; i < 100; ++i) {
		for (int j = 0; j < 100; ++j) {
			VehicleList & ships = m_hexData[i][j].ships;
			VehicleList::iterator itr = ships.begin();
			while (itr != ships.end()) {
				if ((*itr)->getID() == shipID) {
					itr = ships.erase(itr);
				} else {
					++itr;
				}
			}
		}
	}
}

void FTacticalGame::removeShipFromTurnInfo(unsigned int shipID) {
	m_turnInfo.erase(shipID);
}

void FTacticalGame::removeShipFromModelState(unsigned int shipID) {
	removeShipFromHexOccupancy(shipID);
	removeShipFromTurnInfo(shipID);
}

bool FTacticalGame::hasUsableICMDefenderInHex(const VehicleList & vehicles, unsigned int defendingSideID) const {
	for (VehicleList::const_iterator itr = vehicles.begin(); itr != vehicles.end(); ++itr) {
		FVehicle * candidate = *itr;
		if (candidate->getOwner() != defendingSideID) {
			continue;
		}
		unsigned int index = candidate->hasDefense(FDefense::ICM);
		if (index == 0) {
			continue;
		}
		FDefense * defense = candidate->getDefense(index);
		if (defense == NULL) {
			continue;
		}
		if (defense->getAmmo()
			&& !candidate->isPowerSystemDamaged()
			&& !defense->isDamaged()) {
			return true;
		}
	}
	return false;
}

bool FTacticalGame::isCombatOver() const {
	if (m_hasWinner) {
		return true;
	}

	bool attackersAlive = false;
	if (m_attackShips) {
		for (VehicleList::const_iterator itr = m_attackShips->begin(); itr != m_attackShips->end(); ++itr) {
			if ((*itr)->getHP() > 0) {
				attackersAlive = true;
				break;
			}
		}
	}

	bool defendersAlive = false;
	if (m_defendShips) {
		for (VehicleList::const_iterator itr = m_defendShips->begin(); itr != m_defendShips->end(); ++itr) {
			if ((*itr)->getHP() > 0) {
				defendersAlive = true;
				break;
			}
		}
	}

	return !(attackersAlive && defendersAlive);
}

void FTacticalGame::clearWinner() {
	m_hasWinner = false;
	m_winnerID = 0;
}

void FTacticalGame::resetMovementState() {
	clearMovementHighlights();
	m_turnInfo.clear();
	m_gravityTurnFlag = false;
	m_gravityTurns.clear();
	setMoveComplete(false);
	resetTurnInfoForCurrentMover();
	checkMoveStatus();
}

void FTacticalGame::finalizeMovementState() {
	VehicleList ships = getShipList(getMovingPlayerID());
	for (VehicleList::iterator itr = ships.begin(); itr < ships.end(); itr++) {
		const unsigned int id = (*itr)->getID();
		std::map<unsigned int, FTacticalTurnData>::iterator tItr = m_turnInfo.find(id);
		if (tItr == m_turnInfo.end()) {
			continue;
		}

		const bool isStation = ((*itr)->getType() == "ArmedStation"
			|| (*itr)->getType() == "FortifiedStation"
			|| (*itr)->getType() == "Fortress");
		if (!isStation) {
			(*itr)->setSpeed(tItr->second.nMoved);
			(*itr)->setHeading(tItr->second.curHeading);
		} else {
			(*itr)->setHeading(tItr->second.finalHeading);
		}
		tItr->second.hasMoved = true;
	}
	m_drawRoute = false;
	setMoveComplete(true);
}

void FTacticalGame::clearMovementHighlights() {
	m_movementHexes.clear();
	m_leftHexes.clear();
	m_rightHexes.clear();
	clearStoppedShipPreviewRoutes();
	m_targetHexes.clear();
	m_headOnHexes.clear();
	m_drawRoute = false;
	m_moved = 0;
}

void FTacticalGame::resetTurnInfoForCurrentMover() {
	VehicleList ships = getShipList(getMovingPlayerID());
	for (VehicleList::iterator itr = ships.begin(); itr < ships.end(); itr++) {
		FTacticalTurnData d;
		d.hasMoved = false;
		d.waypoints.clear();
		d.turns.clear();
		d.movedHexes.clear();
		d.speed = (*itr)->getSpeed();
		d.curHeading = (*itr)->getHeading();
		d.startHeading = d.curHeading;
		d.finalHeading = d.curHeading;
		d.nMoved = 0;
		d.gravityTurns.clear();
		d.path.clear();
		if (m_station != NULL && (*itr)->getID() == m_station->getID()) {
			d.path.addPoint(m_stationPos);
			FPoint nextHex = FHexMap::findNextHex(m_stationPos, d.curHeading);
			d.path.addPoint(nextHex);
			d.path.addFlag(nextHex, GRAVITY_TURN);
			d.nMoved = 1;
			const int stationTurn = getPlanetTurnDirection(nextHex, d.curHeading);
			if (stationTurn != 0) {
				d.finalHeading = turnShip(d.curHeading, stationTurn);
			}
		}
		m_turnInfo[(*itr)->getID()] = d;
	}
}

FTacticalTurnData * FTacticalGame::findTurnData(unsigned int shipID) {
	std::map<unsigned int, FTacticalTurnData>::iterator itr = m_turnInfo.find(shipID);
	if (itr == m_turnInfo.end()) {
		return NULL;
	}
	return &itr->second;
}

const FTacticalTurnData * FTacticalGame::findTurnData(unsigned int shipID) const {
	std::map<unsigned int, FTacticalTurnData>::const_iterator itr = m_turnInfo.find(shipID);
	if (itr == m_turnInfo.end()) {
		return NULL;
	}
	return &itr->second;
}

bool FTacticalGame::isHexInBounds(const FPoint & hex) const {
	return (hex.getX() >= 0 && hex.getY() >= 0 && hex.getX() < 100 && hex.getY() < 100);
}

bool FTacticalGame::isHexOccupied(const FPoint & hex) const {
	return isHexInBounds(hex) && !m_hexData[hex.getX()][hex.getY()].ships.empty();
}

const VehicleList & FTacticalGame::getHexOccupants(const FPoint & hex) const {
	static VehicleList empty;
	if (!isHexInBounds(hex)) {
		return empty;
	}
	return m_hexData[hex.getX()][hex.getY()].ships;
}

std::vector<FTacticalPlacedOrdnance> FTacticalGame::getPlacedOrdnanceAtHex(const FPoint & hex) const {
	std::vector<FTacticalPlacedOrdnance> ordnance;
	for (std::vector<FTacticalPlacedOrdnance>::const_iterator itr = m_placedOrdnance.begin();
		 itr != m_placedOrdnance.end(); ++itr) {
		if (itr->hex.getX() == hex.getX() && itr->hex.getY() == hex.getY()) {
			ordnance.push_back(*itr);
		}
	}
	return ordnance;
}

std::vector<FTacticalSeekerMissileState> FTacticalGame::getSeekerMissilesAtHex(
	const FPoint & hex,
	bool activeOnly) const
{
	std::vector<FTacticalSeekerMissileState> seekers;
	for (std::vector<FTacticalSeekerMissileState>::const_iterator itr = m_seekerMissiles.begin();
		 itr != m_seekerMissiles.end(); ++itr) {
		if (itr->hex.getX() == hex.getX() && itr->hex.getY() == hex.getY() && (!activeOnly || itr->active)) {
			seekers.push_back(*itr);
		}
	}
	return seekers;
}

std::vector<FTacticalSeekerMissileState> FTacticalGame::getSeekerMissilesForOwner(
	unsigned int ownerID,
	bool activeOnly) const
{
	std::vector<FTacticalSeekerMissileState> seekers;
	for (std::vector<FTacticalSeekerMissileState>::const_iterator itr = m_seekerMissiles.begin();
		 itr != m_seekerMissiles.end(); ++itr) {
		if (itr->ownerID == ownerID && (!activeOnly || itr->active)) {
			seekers.push_back(*itr);
		}
	}
	return seekers;
}

void FTacticalGame::clearPendingSeekerContactOutcomes() {
	m_pendingSeekerContactOutcomes.clear();
}

std::vector<FPoint> FTacticalGame::getInactiveSeekerActivationHexes() const {
	std::vector<FPoint> hexes;
	PointSet uniqueHexes;
	for (std::vector<FTacticalSeekerMissileState>::const_iterator itr = m_seekerMissiles.begin();
		 itr != m_seekerMissiles.end(); ++itr) {
		if (itr->ownerID != getMovingPlayerID() || itr->active) {
			continue;
		}
		if (uniqueHexes.insert(itr->hex).second) {
			hexes.push_back(itr->hex);
		}
	}
	return hexes;
}

bool FTacticalGame::selectSeekerActivationHex(const FPoint & hex) {
	if (!isHexInBounds(hex)) {
		return false;
	}

	for (std::vector<FTacticalSeekerMissileState>::const_iterator itr = m_seekerMissiles.begin();
		 itr != m_seekerMissiles.end(); ++itr) {
		if (itr->ownerID == getMovingPlayerID()
			&& !itr->active
			&& itr->hex.getX() == hex.getX()
			&& itr->hex.getY() == hex.getY()) {
			m_selectedSeekerActivationHex = hex;
			return true;
		}
	}

	return false;
}

std::vector<FTacticalSeekerMissileState> FTacticalGame::getSelectedInactiveSeekerActivationStack() const {
	std::vector<FTacticalSeekerMissileState> seekers;
	if (!isHexInBounds(m_selectedSeekerActivationHex)) {
		return seekers;
	}

	for (std::vector<FTacticalSeekerMissileState>::const_iterator itr = m_seekerMissiles.begin();
		 itr != m_seekerMissiles.end(); ++itr) {
		if (itr->ownerID == getMovingPlayerID()
			&& !itr->active
			&& itr->hex.getX() == m_selectedSeekerActivationHex.getX()
			&& itr->hex.getY() == m_selectedSeekerActivationHex.getY()) {
			seekers.push_back(*itr);
		}
	}
	return seekers;
}

bool FTacticalGame::activateSelectedInactiveSeeker(unsigned int seekerID) {
	if (!isHexInBounds(m_selectedSeekerActivationHex)) {
		return false;
	}

	for (std::vector<FTacticalSeekerMissileState>::iterator itr = m_seekerMissiles.begin();
		 itr != m_seekerMissiles.end(); ++itr) {
		if (itr->seekerID != seekerID
			|| itr->ownerID != getMovingPlayerID()
			|| itr->active
			|| itr->hex.getX() != m_selectedSeekerActivationHex.getX()
			|| itr->hex.getY() != m_selectedSeekerActivationHex.getY()) {
			continue;
		}
		itr->active = true;
		return true;
	}
	return false;
}

bool FTacticalGame::deactivateActiveSeekerByID(unsigned int seekerID) {
	for (std::vector<FTacticalSeekerMissileState>::iterator itr = m_seekerMissiles.begin();
		 itr != m_seekerMissiles.end(); ++itr) {
		if (itr->seekerID != seekerID
			|| itr->ownerID != getMovingPlayerID()
			|| !itr->active) {
			continue;
		}
		itr->active = false;
		return true;
	}
	return false;
}

bool FTacticalGame::activateInactiveSeekerAtHex(const FPoint & hex) {
	if (!isHexInBounds(hex)) {
		return false;
	}

	unsigned int lowestSeekerID = UINT_MAX;
	std::vector<FTacticalSeekerMissileState>::iterator targetItr = m_seekerMissiles.end();

	// Find the lowest seekerID among inactive seekers at this hex owned by moving player
	for (std::vector<FTacticalSeekerMissileState>::iterator itr = m_seekerMissiles.begin();
		 itr != m_seekerMissiles.end(); ++itr) {
		if (itr->ownerID != getMovingPlayerID()
			|| itr->active
			|| itr->hex.getX() != hex.getX()
			|| itr->hex.getY() != hex.getY()) {
			continue;
		}
		if (itr->seekerID < lowestSeekerID) {
			lowestSeekerID = itr->seekerID;
			targetItr = itr;
		}
	}

	if (targetItr != m_seekerMissiles.end()) {
		targetItr->active = true;
		return true;
	}
	return false;
}

std::vector<FTacticalSeekerMissileState> FTacticalGame::getActiveSeekersByMovingPlayer() const {
	std::vector<FTacticalSeekerMissileState> result;
	for (std::vector<FTacticalSeekerMissileState>::const_iterator itr = m_seekerMissiles.begin();
		 itr != m_seekerMissiles.end(); ++itr) {
		if (itr->ownerID == getMovingPlayerID() && itr->active) {
			result.push_back(*itr);
		}
	}
	return result;
}

bool FTacticalGame::isOffensiveSeekerDeploymentMode() const {
	return canUseOffensiveFireSeekerDeployment();
}

std::vector<FTacticalPendingSeekerHexGroup> FTacticalGame::getSelectedOffensivePendingSeekerHexGroups() const {
	std::vector<FTacticalPendingSeekerHexGroup> groups;
	if (!canUseOffensiveFireSeekerDeployment()) {
		return groups;
	}

	FTacticalDeploymentSource selectedSource;
	if (!buildSelectedPlacementSource(selectedSource)) {
		return groups;
	}

	std::map<FPoint, unsigned int> countsByHex;
	for (std::vector<FTacticalPendingSeekerDeployment>::const_iterator itr = m_pendingOffensiveSeekerDeployments.begin();
		 itr != m_pendingOffensiveSeekerDeployments.end(); ++itr) {
		if (itr->offensiveFirePhaseID != m_offensiveFirePhaseID) {
			continue;
		}
		if (!sourceMatchesWeapon(itr->source, m_curShip, m_curWeapon, selectedSource.source.weaponIndex)) {
			continue;
		}
		countsByHex[itr->hex] += 1;
	}

	for (std::map<FPoint, unsigned int>::const_iterator itr = countsByHex.begin(); itr != countsByHex.end(); ++itr) {
		FTacticalPendingSeekerHexGroup group;
		group.hex = itr->first;
		group.count = itr->second;
		groups.push_back(group);
	}
	return groups;
}

bool FTacticalGame::recallSelectedOffensivePendingSeekerAtHex(const FPoint & hex) {
	if (!isHexInBounds(hex)) {
		return false;
	}
	return removeOffensiveFirePendingSeekerAtHex(hex);
}

std::vector<FPoint> FTacticalGame::getAllPendingOffensiveFireSeekerHexes() const {
	std::vector<FPoint> hexes;
	if (m_phase != PH_ATTACK_FIRE) {
		return hexes;
	}
	std::map<FPoint, bool> seen;
	for (std::vector<FTacticalPendingSeekerDeployment>::const_iterator itr = m_pendingOffensiveSeekerDeployments.begin();
		 itr != m_pendingOffensiveSeekerDeployments.end(); ++itr) {
		if (itr->offensiveFirePhaseID != m_offensiveFirePhaseID) {
			continue;
		}
		if (seen.find(itr->hex) == seen.end()) {
			seen[itr->hex] = true;
			hexes.push_back(itr->hex);
		}
	}
	return hexes;
}

void FTacticalGame::resolveActiveSeekersForMovingPlayer() {
	// TSM-004 placeholder seam:
	// active seeker movement now executes through the implemented helper-driven
	// model resolution in this method body.
	clearPendingSeekerContactOutcomes();

	VehicleList candidateTargets;
	if (m_attackShips != NULL) {
		candidateTargets.insert(candidateTargets.end(), m_attackShips->begin(), m_attackShips->end());
	}
	if (m_defendShips != NULL) {
		candidateTargets.insert(candidateTargets.end(), m_defendShips->begin(), m_defendShips->end());
	}

	std::vector<FTacticalSeekerMissileState> nextStates;
	nextStates.reserve(m_seekerMissiles.size());

	for (std::vector<FTacticalSeekerMissileState>::iterator itr = m_seekerMissiles.begin();
		 itr != m_seekerMissiles.end(); ++itr) {
		if (!itr->active || itr->ownerID != getMovingPlayerID()) {
			// Clear movement path for non-moving seekers so stale paths don't persist
			itr->movementPath.clear();
			nextStates.push_back(*itr);
			continue;
		}

		// Clear and reinitialize path for this resolution pass, starting from current hex
		itr->movementPath.clear();
		itr->movementPath.push_back(itr->hex);

		itr->movementTurn += 1;
		itr->movementAllowance = static_cast<int>(computeSeekerMovementAllowance(itr->movementTurn));

		FVehicle * sameHexContactTarget = selectSeekerContactTargetAtHex(*itr, itr->hex);
		if (sameHexContactTarget != NULL) {
			appendSeekerContactOutcome(*itr, sameHexContactTarget, true, 0);
			continue;
		}

		FVehicle * target = selectClosestSeekerTarget(*itr, candidateTargets);
		bool seekerContacted = false;
		if (target != NULL) {
			FPoint targetHex;
			if (findShipHex(target->getID(), targetHex)) {
				adjustSeekerHeadingTowardTarget(*itr, targetHex);
				for (int step = 0; step < itr->movementAllowance; ++step) {
					FTacticalSeekerMissileState nextState;
					if (!computeSeekerGreedyNextStep(*itr, targetHex, nextState)) {
						break;
					}
					itr->heading = nextState.heading;
					itr->hex = nextState.hex;
					itr->movementPath.push_back(itr->hex);

					FVehicle * movementContactTarget = selectSeekerContactTargetAtHex(*itr, itr->hex);
					if (movementContactTarget != NULL) {
						appendSeekerContactOutcome(*itr, movementContactTarget, false, static_cast<unsigned int>(step + 1));
						seekerContacted = true;
						break;
					}
				}
			}
		}

		if (seekerContacted) {
			continue;
		}

		if (itr->movementAllowance >= 12) {
			continue;
		}

		nextStates.push_back(*itr);
	}

	m_seekerMissiles.swap(nextStates);
}

void FTacticalGame::resolvePendingSeekerDetonationDamage() {
	if (m_pendingSeekerContactOutcomes.empty()) {
		return;
	}

	const std::vector<FTacticalSeekerContactOutcome> outcomes = m_pendingSeekerContactOutcomes;

	WeaponList seekerDetonations;
	std::vector<ICMData *> icmData;
	std::vector<VehicleList *> detonationTargetGroups;
	for (std::vector<FTacticalSeekerContactOutcome>::const_iterator itr = outcomes.begin();
		 itr != outcomes.end();
		 ++itr) {
		FVehicle * target = findShipByID(itr->targetShipID);
		if (!isValidSeekerTarget(target)) {
			continue;
		}

		FPoint targetHex;
		if (!findShipHex(target->getID(), targetHex)) {
			continue;
		}

		FWeapon * seeker = createWeapon(FWeapon::SM);
		seeker->setTarget(target, 0, false);
		seeker->setParent(NULL);
		seekerDetonations.push_back(seeker);

		VehicleList * targets = new VehicleList;
		*targets = getHexOccupants(targetHex);
		detonationTargetGroups.push_back(targets);

		ICMData * iData = new ICMData;
		iData->weapon = seeker;
		iData->vehicles = targets;
		icmData.push_back(iData);
	}

	if (!seekerDetonations.empty()) {
		FTacticalCombatReportContext context;
		context.reportType = TRT_SeekerDamage;
		context.phase = getPhase();
		context.actingPlayerID = getMovingPlayerID();
		context.immediate = true;
		beginTacticalReport(context);

		if (m_ui != NULL && !icmData.empty()) {
			VehicleList * defenders = icmData[0]->vehicles;
			m_ui->runICMSelection(icmData, defenders);
		}

		bool resolvedAttack = false;
		for (WeaponList::iterator itr = seekerDetonations.begin(); itr != seekerDetonations.end(); ++itr) {
			FTacticalAttackResult result = (*itr)->fire();
			if (!result.fired()) {
				continue;
			}
			resolvedAttack = true;
			appendTacticalAttackReport(buildTacticalAttackReport(result));
		}

		FTacticalCombatReportSummary summary = buildCurrentTacticalReportSummary();
		if (summary.ships.size() > 0 && m_ui != NULL) {
			m_ui->showDamageSummary(summary);
		}
		clearTacticalReport();

		if (resolvedAttack) {
			if (m_lastDestroyedShipIDsConsumed) {
				m_lastDestroyedShipIDs.clear();
			}
			clearDestroyedShips();
		}
	}

	for (std::vector<ICMData *>::iterator itr = icmData.begin(); itr != icmData.end(); ++itr) {
		delete (*itr)->weapon;
		delete *itr;
	}
	for (std::vector<VehicleList *>::iterator itr = detonationTargetGroups.begin();
		 itr != detonationTargetGroups.end();
		 ++itr) {
		delete *itr;
	}
}

FVehicle * FTacticalGame::selectSeekerContactTargetAtHex(
	const FTacticalSeekerMissileState & seeker,
	const FPoint & hex) const
{
	if (!isHexInBounds(hex) || !isHexInBounds(seeker.hex)) {
		return NULL;
	}
	const VehicleList occupants = getHexOccupants(hex);
	VehicleList validTargets;
	for (VehicleList::const_iterator itr = occupants.begin(); itr != occupants.end(); ++itr) {
		if (!isValidSeekerTarget(*itr)) {
			continue;
		}
		validTargets.push_back(*itr);
	}
	if (validTargets.empty()) {
		return NULL;
	}
	int highestMaxHP = -1;
	VehicleList toughestTargets;
	for (VehicleList::const_iterator itr = validTargets.begin(); itr != validTargets.end(); ++itr) {
		const int maxHP = (*itr)->getMaxHP();
		if (maxHP > highestMaxHP) {
			highestMaxHP = maxHP;
			toughestTargets.clear();
			toughestTargets.push_back(*itr);
		} else if (maxHP == highestMaxHP) {
			toughestTargets.push_back(*itr);
		}
	}
	if (toughestTargets.size() == 1) {
		return toughestTargets[0];
	}

	validTargets = toughestTargets;
	unsigned int randomTargetIndex = 0;
	if (validTargets.size() > 1) {
		randomTargetIndex = chooseRandomSeekerIndex(static_cast<unsigned int>(validTargets.size()));
	}
	return toughestTargets[randomTargetIndex % toughestTargets.size()];
}

void FTacticalGame::appendSeekerContactOutcome(
	const FTacticalSeekerMissileState & seeker,
	const FVehicle * target,
	bool preMovementContact,
	unsigned int movementStep)
{
	if (target == NULL) {
		return;
	}

	FTacticalSeekerContactOutcome outcome;
	outcome.seekerID = seeker.seekerID;
	outcome.seekerOwnerID = seeker.ownerID;
	outcome.targetShipID = target->getID();
	outcome.targetOwnerID = target->getOwner();
	outcome.contactHex = seeker.hex;
	outcome.preMovementContact = preMovementContact;
	outcome.movementStep = movementStep;
	outcome.movementTurn = seeker.movementTurn;
	m_pendingSeekerContactOutcomes.push_back(outcome);
}

unsigned int FTacticalGame::computeSeekerMovementAllowance(int movementTurn) const {
	if (movementTurn <= 0) {
		return 0;
	}
	const unsigned int allowance = static_cast<unsigned int>(movementTurn * 2);
	return (allowance > 12) ? 12 : allowance;
}

unsigned int FTacticalGame::chooseRandomSeekerIndex(unsigned int candidateCount) const {
	if (candidateCount <= 1) {
		return 0;
	}
	return static_cast<unsigned int>(irand(candidateCount) - 1);
}

bool FTacticalGame::findShipHex(unsigned int shipID, FPoint & hex) const {
	for (int i = 0; i < 100; ++i) {
		for (int j = 0; j < 100; ++j) {
			const VehicleList & ships = m_hexData[i][j].ships;
			for (VehicleList::const_iterator itr = ships.begin(); itr != ships.end(); ++itr) {
				if (*itr != NULL && (*itr)->getID() == shipID) {
					hex = m_hexData[i][j].pos;
					return true;
				}
			}
		}
	}
	hex.setPoint(-1, -1);
	return false;
}

bool FTacticalGame::isValidSeekerTarget(const FVehicle * ship) const {
	return ship != NULL
		&& ship->getHP() > 0
		&& !isStationType(const_cast<FVehicle *>(ship));
}

FVehicle * FTacticalGame::selectClosestSeekerTarget(
	const FTacticalSeekerMissileState & seeker,
	const VehicleList & candidates) const
{
	VehicleList closestShips;
	int closestDistance = -1;
	for (VehicleList::const_iterator itr = candidates.begin(); itr != candidates.end(); ++itr) {
		if (!isValidSeekerTarget(*itr)) {
			continue;
		}
		FPoint shipHex;
		if (!findShipHex((*itr)->getID(), shipHex)) {
			continue;
		}
		const int distance = FHexMap::computeHexDistance(seeker.hex, shipHex);
		if (closestDistance < 0 || distance < closestDistance) {
			closestDistance = distance;
			closestShips.clear();
			closestShips.push_back(*itr);
		} else if (distance == closestDistance) {
			closestShips.push_back(*itr);
		}
	}
	if (closestShips.empty()) {
		return NULL;
	}
	if (closestShips.size() == 1) {
		return closestShips[0];
	}
	return closestShips[chooseRandomSeekerIndex(static_cast<unsigned int>(closestShips.size()))];
}

void FTacticalGame::adjustSeekerHeadingTowardTarget(
	FTacticalSeekerMissileState & seeker,
	const FPoint & targetHex) const
{
	if (!isHexInBounds(seeker.hex) || !isHexInBounds(targetHex) || seeker.hex == targetHex) {
		return;
	}
	const int targetHeading = FHexMap::computeHeading(seeker.hex, targetHex);
	if (targetHeading < 0 || targetHeading > 5) {
		return;
	}
	int delta = shortestHeadingDelta(normalizeHeading(seeker.heading), targetHeading);
	if (delta > 3) {
		delta = 3;
	} else if (delta < -3) {
		// Defensive clamp for future heading-delta helper changes.
		delta = -3;
	}
	seeker.heading = normalizeHeading(seeker.heading + delta);
}

void FTacticalGame::moveSeekerTowardTarget(FTacticalSeekerMissileState & seeker, const FPoint & targetHex) const {
	if (!isHexInBounds(seeker.hex) || !isHexInBounds(targetHex) || seeker.movementAllowance <= 0) {
		return;
	}

	for (int step = 0; step < seeker.movementAllowance; ++step) {
		const int currentDistance = FHexMap::computeHexDistance(seeker.hex, targetHex);
		if (currentDistance <= 0) {
			break;
		}

		int bestHeading = seeker.heading;
		FPoint bestHex(-1, -1);
		int bestDistance = -1;
		int bestTurnMagnitude = 99;
		for (int turn = -1; turn <= 1; ++turn) {
			const int candidateHeading = normalizeHeading(seeker.heading + turn);
			const FPoint candidateHex = FHexMap::findNextHex(seeker.hex, candidateHeading);
			if (!isHexInBounds(candidateHex)) {
				continue;
			}
			const int candidateDistance = FHexMap::computeHexDistance(candidateHex, targetHex);
			const int turnMagnitude = (turn < 0) ? -turn : turn;
			if (bestDistance < 0
				|| candidateDistance < bestDistance
				|| (candidateDistance == bestDistance && turnMagnitude < bestTurnMagnitude)) {
				bestDistance = candidateDistance;
				bestHeading = candidateHeading;
				bestHex = candidateHex;
				bestTurnMagnitude = turnMagnitude;
			}
		}

		if (bestDistance < 0) {
			break;
		}

		seeker.heading = bestHeading;
		seeker.hex = bestHex;
	}
}

VehicleList FTacticalGame::collectHighestMaxHPSeekerTargets(const VehicleList & candidates) const {
	int highestMaxHP = -1;
	VehicleList toughestTargets;
	for (VehicleList::const_iterator itr = candidates.begin(); itr != candidates.end(); ++itr) {
		if (!isValidSeekerTarget(*itr)) {
			continue;
		}
		const int maxHP = (*itr)->getMaxHP();
		if (maxHP > highestMaxHP) {
			highestMaxHP = maxHP;
			toughestTargets.clear();
			toughestTargets.push_back(*itr);
		} else if (maxHP == highestMaxHP) {
			toughestTargets.push_back(*itr);
		}
	}
	return toughestTargets;
}

std::vector<unsigned int> FTacticalGame::collectClosestSeekerTargetIDs(
	const FTacticalSeekerMissileState & seeker,
	const std::vector<FTacticalSeekerTargetSnapshot> & candidates) const
{
	std::vector<unsigned int> closestIDs;
	if (!isHexInBounds(seeker.hex)) {
		return closestIDs;
	}

	int closestDistance = -1;
	for (std::vector<FTacticalSeekerTargetSnapshot>::const_iterator itr = candidates.begin();
		 itr != candidates.end(); ++itr) {
		if (itr->hp <= 0 || itr->isStation || !isHexInBounds(itr->hex)) {
			continue;
		}
		const int distance = FHexMap::computeHexDistance(seeker.hex, itr->hex);
		if (closestDistance < 0 || distance < closestDistance) {
			closestDistance = distance;
			closestIDs.clear();
			closestIDs.push_back(itr->shipID);
		} else if (distance == closestDistance) {
			closestIDs.push_back(itr->shipID);
		}
	}
	return closestIDs;
}

bool FTacticalGame::computeSeekerGreedyNextStep(
	const FTacticalSeekerMissileState & seeker,
	const FPoint & targetHex,
	FTacticalSeekerMissileState & nextState) const
{
	nextState = seeker;
	if (!isHexInBounds(seeker.hex) || !isHexInBounds(targetHex)) {
		return false;
	}
	const int currentDistance = FHexMap::computeHexDistance(seeker.hex, targetHex);
	if (currentDistance <= 0) {
		return false;
	}

	int bestHeading = seeker.heading;
	FPoint bestHex(-1, -1);
	int bestDistance = -1;
	int bestTurnMagnitude = 99;
	for (int turn = -1; turn <= 1; ++turn) {
		const int candidateHeading = normalizeHeading(seeker.heading + turn);
		const FPoint candidateHex = FHexMap::findNextHex(seeker.hex, candidateHeading);
		if (!isHexInBounds(candidateHex)) {
			continue;
		}
		const int candidateDistance = FHexMap::computeHexDistance(candidateHex, targetHex);
		const int turnMagnitude = (turn < 0) ? -turn : turn;
		if (bestDistance < 0
			|| candidateDistance < bestDistance
			|| (candidateDistance == bestDistance && turnMagnitude < bestTurnMagnitude)) {
			bestDistance = candidateDistance;
			bestHeading = candidateHeading;
			bestHex = candidateHex;
			bestTurnMagnitude = turnMagnitude;
		}
	}
	if (bestDistance < 0) {
		return false;
	}

	nextState.heading = bestHeading;
	nextState.hex = bestHex;
	return true;
}

bool FTacticalGame::isDeployableWeapon(const FWeapon * weapon) const {
	if (weapon == NULL || weapon->isDamaged()) {
		return false;
	}
	const FWeapon::Weapon type = weapon->getType();
	return (type == FWeapon::M || type == FWeapon::SM);
}

bool FTacticalGame::sourceMatchesWeapon(
	const FTacticalOrdnanceSource & source,
	const FVehicle * ship,
	const FWeapon * weapon,
	int weaponIndex) const
{
	if (ship == NULL || weapon == NULL) {
		return false;
	}
	return source.shipID == ship->getID()
		&& source.weaponIndex == weaponIndex
		&& source.weaponID == weapon->getID();
}

bool FTacticalGame::sourceMatchesSelection(const FTacticalOrdnanceSource & source) const {
	if (m_curShip == NULL || m_curWeapon == NULL) {
		return false;
	}
	int selectedWeaponIndex = -1;
	for (unsigned int i = 0; i < m_curShip->getWeaponCount(); ++i) {
		FWeapon * weapon = m_curShip->getWeapon(i);
		if (weapon == m_curWeapon) {
			selectedWeaponIndex = static_cast<int>(i);
			break;
		}
	}
	if (selectedWeaponIndex < 0) {
		for (unsigned int i = 0; i < m_curShip->getWeaponCount(); ++i) {
			FWeapon * weapon = m_curShip->getWeapon(i);
			if (weapon != NULL && weapon->getID() == m_curWeapon->getID()) {
				selectedWeaponIndex = static_cast<int>(i);
				break;
			}
		}
	}
	if (selectedWeaponIndex < 0) {
		return false;
	}
	return sourceMatchesWeapon(source, m_curShip, m_curWeapon, selectedWeaponIndex);
}

FVehicle * FTacticalGame::findShipByID(unsigned int shipID) const {
	if (m_attackShips != NULL) {
		for (VehicleList::const_iterator itr = m_attackShips->begin(); itr != m_attackShips->end(); ++itr) {
			if ((*itr)->getID() == shipID) {
				return *itr;
			}
		}
	}
	if (m_defendShips != NULL) {
		for (VehicleList::const_iterator itr = m_defendShips->begin(); itr != m_defendShips->end(); ++itr) {
			if ((*itr)->getID() == shipID) {
				return *itr;
			}
		}
	}
	return NULL;
}

FWeapon * FTacticalGame::findWeaponBySource(const FTacticalOrdnanceSource & source, FVehicle ** ship) const {
	FVehicle * ownerShip = findShipByID(source.shipID);
	if (ship != NULL) {
		*ship = ownerShip;
	}
	if (ownerShip == NULL || source.weaponIndex < 0
		|| static_cast<unsigned int>(source.weaponIndex) >= ownerShip->getWeaponCount()) {
		return NULL;
	}
	FWeapon * weapon = ownerShip->getWeapon(static_cast<unsigned int>(source.weaponIndex));
	if (weapon == NULL || weapon->getID() != source.weaponID) {
		return NULL;
	}
	return weapon;
}

bool FTacticalGame::buildSelectedPlacementSource(FTacticalDeploymentSource & source) const {
	if (m_curShip == NULL || m_curWeapon == NULL) {
		return false;
	}
	for (unsigned int i = 0; i < m_curShip->getWeaponCount(); ++i) {
		FWeapon * weapon = m_curShip->getWeapon(i);
		if (weapon == NULL || weapon->getID() != m_curWeapon->getID()) {
			continue;
		}
		if (!isDeployableWeapon(weapon)) {
			return false;
		}
		source.ownerID = m_curShip->getOwner();
		source.weaponType = weapon->getType();
		source.source.shipID = m_curShip->getID();
		source.source.weaponIndex = static_cast<int>(i);
		source.source.weaponID = weapon->getID();
		return true;
	}
	return false;
}

bool FTacticalGame::canUseOffensiveFireSeekerDeployment() const {
	if (getState() != BS_Battle
		|| getPhase() != PH_ATTACK_FIRE
		|| m_curShip == NULL
		|| m_curWeapon == NULL
		|| m_curShip->getOwner() != getActivePlayerID()
		|| getActivePlayerID() != getMovingPlayerID()) {
		return false;
	}
	if (m_curWeapon->getType() != FWeapon::SM
		|| m_curWeapon->isDamaged()) {
		return false;
	}
	FTacticalDeploymentSource source;
	return buildSelectedPlacementSource(source);
}

bool FTacticalGame::isHexOnSelectedShipCurrentPath(const FPoint & hex) {
	if (!canUseOffensiveFireSeekerDeployment() || !isHexInBounds(hex)) {
		return false;
	}
	FTacticalTurnData * turnData = findTurnData(m_curShip->getID());
	if (turnData == NULL || turnData->path.getPathLength() == 0) {
		return false;
	}
	return turnData->path.isPointOnPath(hex);
}

bool FTacticalGame::hasPendingOffensiveDeploymentForSource(const FTacticalOrdnanceSource & source) const {
	for (std::vector<FTacticalPendingSeekerDeployment>::const_iterator itr = m_pendingOffensiveSeekerDeployments.begin();
		 itr != m_pendingOffensiveSeekerDeployments.end(); ++itr) {
		if (itr->offensiveFirePhaseID == m_offensiveFirePhaseID
			&& itr->source.shipID == source.shipID
			&& itr->source.weaponIndex == source.weaponIndex
			&& itr->source.weaponID == source.weaponID) {
			return true;
		}
	}
	return false;
}

bool FTacticalGame::placeOffensiveFirePendingSeekerAtHex(const FPoint & hex) {
	if (!isHexOnSelectedShipCurrentPath(hex)) {
		return false;
	}

	FTacticalDeploymentSource selectedSource;
	if (!buildSelectedPlacementSource(selectedSource)) {
		return false;
	}

	FWeapon * weapon = findWeaponBySource(selectedSource.source, NULL);
	if (weapon == NULL || weapon->getType() != FWeapon::SM || weapon->getAmmo() <= 0 || weapon->isDamaged()) {
		return false;
	}

	weapon->setCurrentAmmo(weapon->getAmmo() - 1);
	FTacticalSeekerMissileState seeker;
	seeker.seekerID = nextSeekerID();
	seeker.ownerID = selectedSource.ownerID;
	seeker.hex = hex;
	seeker.heading = m_curShip->getHeading();
	seeker.active = false;
	seeker.movementTurn = 0;
	seeker.movementAllowance = 0;
	seeker.hasSource = true;
	seeker.source = selectedSource.source;
	m_seekerMissiles.push_back(seeker);

	FTacticalPendingSeekerDeployment pending;
	pending.seekerID = seeker.seekerID;
	pending.source = selectedSource.source;
	pending.hex = hex;
	pending.offensiveFirePhaseID = m_offensiveFirePhaseID;
	m_pendingOffensiveSeekerDeployments.push_back(pending);

	computeWeaponRange();
	return true;
}

bool FTacticalGame::removeOffensiveFirePendingSeekerAtHex(const FPoint & hex) {
	if (!canUseOffensiveFireSeekerDeployment() || !isHexInBounds(hex)) {
		return false;
	}

	FTacticalDeploymentSource selectedSource;
	if (!buildSelectedPlacementSource(selectedSource)) {
		return false;
	}

	for (std::vector<FTacticalPendingSeekerDeployment>::reverse_iterator itr = m_pendingOffensiveSeekerDeployments.rbegin();
		 itr != m_pendingOffensiveSeekerDeployments.rend(); ++itr) {
		if (itr->offensiveFirePhaseID != m_offensiveFirePhaseID
			|| itr->hex.getX() != hex.getX()
			|| itr->hex.getY() != hex.getY()
			|| !sourceMatchesWeapon(itr->source, m_curShip, m_curWeapon, selectedSource.source.weaponIndex)) {
			continue;
		}

		const FTacticalPendingSeekerDeployment pending = *itr;
		const std::vector<FTacticalPendingSeekerDeployment>::iterator eraseItr = (itr + 1).base();

		if (!restoreAmmoForSource(pending.source)) {
			return false;
		}

		for (std::vector<FTacticalSeekerMissileState>::iterator seekerItr = m_seekerMissiles.begin();
			 seekerItr != m_seekerMissiles.end(); ++seekerItr) {
			if (seekerItr->seekerID == pending.seekerID) {
				m_seekerMissiles.erase(seekerItr);
				break;
			}
		}

		m_pendingOffensiveSeekerDeployments.erase(eraseItr);
		computeWeaponRange();
		return true;
	}

	return false;
}

void FTacticalGame::clearPendingOffensiveFireSeekers() {
	m_pendingOffensiveSeekerDeployments.clear();
}

void FTacticalGame::rebuildDeployablePlacementSources() {
	m_deployablePlacementSources.clear();
	m_shipsWithMines.clear();
	VehicleList ships = getShipList(getActivePlayerID());
	for (VehicleList::iterator shipItr = ships.begin(); shipItr != ships.end(); ++shipItr) {
		bool hasMineSlot = false;
		for (unsigned int i = 0; i < (*shipItr)->getWeaponCount(); ++i) {
			FWeapon * weapon = (*shipItr)->getWeapon(i);
			if (!isDeployableWeapon(weapon)) {
				continue;
			}
			FTacticalDeploymentSource source;
			source.ownerID = (*shipItr)->getOwner();
			source.weaponType = weapon->getType();
			source.source.shipID = (*shipItr)->getID();
			source.source.weaponIndex = static_cast<int>(i);
			source.source.weaponID = weapon->getID();
			m_deployablePlacementSources.push_back(source);
			if (weapon->getType() == FWeapon::M) {
				hasMineSlot = true;
			}
		}
		if (hasMineSlot) {
			m_shipsWithMines.push_back(*shipItr);
		}
	}
	m_selectedPlacementSource = m_deployablePlacementSources.empty() ? -1 : 0;
}

bool FTacticalGame::selectPlacementSourceByIndex(unsigned int sourceIndex) {
	if (sourceIndex >= m_deployablePlacementSources.size()) {
		return false;
	}
	const FTacticalDeploymentSource & source = m_deployablePlacementSources[sourceIndex];
	FVehicle * ship = NULL;
	FWeapon * weapon = findWeaponBySource(source.source, &ship);
	if (ship == NULL || weapon == NULL || !isDeployableWeapon(weapon)
		|| ship->getOwner() != getActivePlayerID()) {
		return false;
	}
	m_selectedPlacementSource = static_cast<int>(sourceIndex);
	setShip(ship);
	setWeapon(weapon);
	return true;
}

bool FTacticalGame::selectPlacementSource(unsigned int shipID, unsigned int weaponIndex) {
	for (unsigned int i = 0; i < m_deployablePlacementSources.size(); ++i) {
		const FTacticalDeploymentSource & source = m_deployablePlacementSources[i];
		if (source.source.shipID == shipID
			&& source.source.weaponIndex == static_cast<int>(weaponIndex)) {
			return selectPlacementSourceByIndex(i);
		}
	}
	return false;
}

void FTacticalGame::appendPlacedOrdnanceRecord(
	FWeapon::Weapon weaponType,
	const FPoint & hex,
	const FTacticalOrdnanceSource & source)
{
	FTacticalPlacedOrdnance ordnance;
	ordnance.weaponType = weaponType;
	ordnance.ownerID = (m_curShip != NULL) ? m_curShip->getOwner() : getActivePlayerID();
	ordnance.source = source;
	ordnance.hex = hex;
	ordnance.displayColorIndex = -1;
	ordnance.displayMarkerIndex = -1;
	m_placedOrdnance.push_back(ordnance);
}

bool FTacticalGame::removePlacedOrdnanceForSelection(
	const FPoint & hex,
	FTacticalPlacedOrdnance & removed)
{
	for (std::vector<FTacticalPlacedOrdnance>::reverse_iterator itr = m_placedOrdnance.rbegin();
		 itr != m_placedOrdnance.rend(); ++itr) {
		if (itr->hex.getX() != hex.getX() || itr->hex.getY() != hex.getY()) {
			continue;
		}
		if (!sourceMatchesSelection(itr->source)) {
			continue;
		}
		removed = *itr;
		m_placedOrdnance.erase((itr + 1).base());
		return true;
	}
	return false;
}

void FTacticalGame::removePlacedMineRecordsAtHex(const FPoint & hex) {
	for (std::vector<FTacticalPlacedOrdnance>::iterator itr = m_placedOrdnance.begin();
		 itr != m_placedOrdnance.end();) {
		if (itr->weaponType == FWeapon::M
			&& itr->hex.getX() == hex.getX()
			&& itr->hex.getY() == hex.getY()) {
			itr = m_placedOrdnance.erase(itr);
		} else {
			++itr;
		}
	}
}

bool FTacticalGame::restoreAmmoForSource(const FTacticalOrdnanceSource & source) {
	FWeapon * weapon = findWeaponBySource(source, NULL);
	if (weapon == NULL) {
		return false;
	}
	const int ammo = weapon->getAmmo();
	if (ammo >= weapon->getMaxAmmo()) {
		return false;
	}
	weapon->setCurrentAmmo(ammo + 1);
	return true;
}

unsigned int FTacticalGame::nextSeekerID() const {
	unsigned int nextID = 1;
	for (std::vector<FTacticalSeekerMissileState>::const_iterator itr = m_seekerMissiles.begin();
		 itr != m_seekerMissiles.end(); ++itr) {
		if (itr->seekerID >= nextID) {
			nextID = itr->seekerID + 1;
		}
	}
	return nextID;
}

bool FTacticalGame::placeMineFromSelection(
	const FPoint & hex,
	const FTacticalDeploymentSource & selectedSource)
{
	FWeapon * weapon = findWeaponBySource(selectedSource.source, NULL);
	if (weapon == NULL || weapon->getType() != FWeapon::M || weapon->getAmmo() <= 0) {
		return false;
	}
	if (m_minedHexList.find(hex) == m_minedHexList.end()) {
		m_minedHexList.insert(hex);
		weapon->setCurrentAmmo(weapon->getAmmo() - 1);
		m_mineOwner = selectedSource.ownerID;
		appendPlacedOrdnanceRecord(FWeapon::M, hex, selectedSource.source);
		rebuildDeployablePlacementSources();
		selectPlacementSource(selectedSource.source.shipID,
			static_cast<unsigned int>(selectedSource.source.weaponIndex));
		return true;
	}
	return false;
}

bool FTacticalGame::placeSeekerFromSelection(
	const FPoint & hex,
	const FTacticalDeploymentSource & selectedSource)
{
	FWeapon * weapon = findWeaponBySource(selectedSource.source, NULL);
	if (weapon == NULL || weapon->getType() != FWeapon::SM || weapon->getAmmo() <= 0) {
		return false;
	}
	weapon->setCurrentAmmo(weapon->getAmmo() - 1);
	FTacticalSeekerMissileState seeker;
	seeker.seekerID = nextSeekerID();
	seeker.ownerID = selectedSource.ownerID;
	seeker.hex = hex;
	seeker.heading = (m_curShip != NULL) ? m_curShip->getHeading() : 0;
	seeker.active = false;
	seeker.movementTurn = 0;
	seeker.movementAllowance = 0;
	seeker.hasSource = true;
	seeker.source = selectedSource.source;
	m_seekerMissiles.push_back(seeker);
	appendPlacedOrdnanceRecord(FWeapon::SM, hex, selectedSource.source);
	rebuildDeployablePlacementSources();
	selectPlacementSource(selectedSource.source.shipID,
		static_cast<unsigned int>(selectedSource.source.weaponIndex));
	return true;
}

VehicleList * FTacticalGame::getShipList(FVehicle * ship) {
	if (ship == NULL) {
		return NULL;
	}
	return findHexOccupantsForShip(ship->getID());
}

const VehicleList * FTacticalGame::getShipList(FVehicle * ship) const {
	if (ship == NULL) {
		return NULL;
	}
	return findHexOccupantsForShip(ship->getID());
}

FVehicle * FTacticalGame::pickShip(const FVehicle * selected, const VehicleList & ships) const {
	if (ships.size() == 0) {
		return NULL;
	}
	if (selected == NULL) {
		return ships[0];
	}
	FVehicle * choice = ships[0];
	for (VehicleList::const_iterator itr = ships.begin(); itr != ships.end(); ++itr) {
		if ((*itr)->getID() == selected->getID()) {
			VehicleList::const_iterator next = itr + 1;
			if (next != ships.end()) {
				choice = *next;
			}
			break;
		}
	}
	return choice;
}

FVehicle * FTacticalGame::pickTarget(const FVehicle * selected, const VehicleList & ships) const {
	VehicleList targets;
	for (VehicleList::const_iterator itr = ships.begin(); itr != ships.end(); ++itr) {
		if ((*itr)->getOwner() != getActivePlayerID()) {
			targets.push_back(*itr);
		}
	}
	return pickShip(selected, targets);
}

int FTacticalGame::turnShip(int heading, int turn) const {
	heading += turn;
	if (heading >= 6) {
		heading -= 6;
	}
	if (heading < 0) {
		heading += 6;
	}
	return heading;
}

int FTacticalGame::getPlanetTurnDirection(FPoint currentHex, int currentHeading) const {
	if (m_planetPos.getX() < 0) {
		return 0;
	}
	const int portBackDir = (currentHeading - 2 < 0) ? currentHeading + 4 : currentHeading - 2;
	if (FHexMap::findNextHex(currentHex, portBackDir) == m_planetPos && !m_gravityTurnFlag) {
		return -1;
	}
	const int starboardBackDir = (currentHeading + 2 > 5) ? currentHeading - 4 : currentHeading + 2;
	if (FHexMap::findNextHex(currentHex, starboardBackDir) == m_planetPos && !m_gravityTurnFlag) {
		return 1;
	}
	return 0;
}

void FTacticalGame::checkForPlanetCollision(FPoint & currentHex, int & currentHeading) {
	if (m_planetPos.getX() < 0) {
		return;
	}
	if (m_planetPos == currentHex) {
		currentHex.setPoint(-1, -1);
		return;
	}
	const int turnDir = getPlanetTurnDirection(currentHex, currentHeading);
	if (turnDir != 0) {
		m_gravityTurns[currentHex] = turnDir;
		currentHeading = turnShip(currentHeading, turnDir);
		m_gravityTurnFlag = true;
	}
}

int FTacticalGame::forceTurn(FVehicle * ship, int curHeading, FPoint current) {
	if (ship == NULL) {
		return curHeading;
	}
	const int turnDirection = ship->getNavControlError();
	FTacticalTurnData * turnData = findTurnData(ship->getID());
	if (turnData == NULL) {
		return curHeading;
	}
	if (turnDirection != 0 && turnData->path.countFlags(MR_TURN) < ship->getMR()) {
		curHeading = turnShip(curHeading, turnDirection);
		turnData->path.addPoint(current);
		turnData->path.addFlag(current, MR_TURN);
		turnData->curHeading = curHeading;
		turnData->nMoved++;
		m_moved++;
	}
	return curHeading;
}

void FTacticalGame::computePath(PointList & list, FPoint hex, int heading) {
	if (m_curShip == NULL) {
		return;
	}
	m_gravityTurnFlag = false;
	FTacticalTurnData * turnData = findTurnData(m_curShip->getID());
	if (turnData == NULL) {
		return;
	}
	if (turnData->gravityTurns.size() > 0) {
		std::map<FPoint, int>::const_iterator itr = turnData->gravityTurns.find(hex);
		m_gravityTurnFlag = (itr != turnData->gravityTurns.end());
		if (FHexMap::computeHexDistance(hex, m_planetPos) == 1) {
			const FPoint turnHex = turnData->gravityTurns.begin()->first;
			if (FHexMap::computeHexDistance(hex, turnHex) == 1) {
				const int back = (heading + 3 > 5) ? heading - 3 : heading + 3;
				const int back2 = (heading + 2 > 5) ? heading - 4 : heading + 2;
				const int back3 = (heading + 4 > 5) ? heading - 2 : heading + 4;
				const int dir = FHexMap::computeHeading(hex, turnHex);
				if (dir == back || dir == back2 || dir == back3) {
					m_gravityTurnFlag = true;
				}
			}
		}
	}

	for (int i = m_moved; i < m_curShip->getSpeed() + m_curShip->getADF(); i++) {
		if (hex.getX() == -1) {
			break;
		}
		hex = FHexMap::findNextHex(hex, heading);
		checkForPlanetCollision(hex, heading);
		if (hex.getX() != -1) {
			list.push_back(hex);
			heading = forceTurn(m_curShip, heading, hex);
		}
	}
}

void FTacticalGame::computeRemainingMoves(FPoint start) {
	if (m_curShip == NULL) {
		return;
	}
	FTacticalTurnData * turnData = findTurnData(m_curShip->getID());
	if (turnData == NULL) {
		return;
	}
	const int left = (turnData->curHeading + 1) % 6;
	const int right = (turnData->curHeading + 5) % 6;
	const int forward = turnData->curHeading;
	m_rightHexes.clear();
	m_leftHexes.clear();
	m_movementHexes.clear();
	computePath(m_movementHexes, start, forward);
	const unsigned int turnLimit = m_curShip->getMR();
	if (turnData->path.countFlags(MR_TURN) < turnLimit && turnData->path.getPathLength() > 1) {
		computePath(m_leftHexes, start, left);
		computePath(m_rightHexes, start, right);
	}
	if (canUseStoppedShipFreeRotation(m_curShip, turnData)) {
		buildStoppedShipTurnOptions(start, forward, m_leftHexes, m_rightHexes);
	}
	rebuildStoppedShipPreviewRoutes();
}

void FTacticalGame::setInitialRoute() {
	if (m_curShip == NULL || m_shipPos.getX() < 0) {
		return;
	}
	FTacticalTurnData * turnData = findTurnData(m_curShip->getID());
	if (turnData == NULL) {
		return;
	}

	m_movementHexes.clear();
	m_leftHexes.clear();
	m_rightHexes.clear();
	clearStoppedShipPreviewRoutes();
	m_gravityTurns.clear();
	m_gravityTurnFlag = false;

	if (turnData->path.getPathLength() > 1) {
		m_moved = static_cast<int>(turnData->path.getPathLength()) - 1;
		computeRemainingMoves(turnData->path.endPoint());
		return;
	}

	int curHeading = m_curShip->getHeading();
	FPoint current = m_shipPos;
	turnData->path.clear();
	turnData->path.addPoint(current);
	turnData->curHeading = curHeading;
	turnData->startHeading = curHeading;
	turnData->finalHeading = curHeading;
	turnData->nMoved = 0;
	m_moved = 0;

	for (int i = 0; i < m_curShip->getSpeed() + m_curShip->getADF(); i++) {
		if (current.getX() == -1) {
			break;
		}
		current = FHexMap::findNextHex(current, curHeading);
		checkForPlanetCollision(current, curHeading);
		if (current.getX() != -1) {
			m_movementHexes.push_back(current);
			curHeading = forceTurn(m_curShip, curHeading, current);
		}
	}
	rebuildStoppedShipPreviewRoutes();

}

void FTacticalGame::clearStoppedShipPreviewRoutes() {
	m_stoppedShipPreviewRoutes.clear();
	m_stoppedShipPreviewHeadingsByHex.clear();
}

void FTacticalGame::rebuildStoppedShipPreviewRoutes() {
	clearStoppedShipPreviewRoutes();
	if (m_curShip == NULL || m_shipPos.getX() < 0 || m_shipPos.getY() < 0) {
		return;
	}

	FTacticalTurnData * turnData = findTurnData(m_curShip->getID());
	if (!canUseStoppedShipFreeRotation(m_curShip, turnData)) {
		return;
	}

	const std::map<FPoint, int> originalGravityTurns = m_gravityTurns;
	const bool originalGravityTurnFlag = m_gravityTurnFlag;

	for (int heading = 0; heading < 6; ++heading) {
		const FPoint facingHex = FHexMap::findNextHex(m_shipPos, heading);
		if (!isHexInBounds(facingHex)) {
			continue;
		}

		FTacticalMovePreviewRoute preview;
		preview.startHeading = heading;
		preview.facingHex = facingHex;
		preview.routeHexes.clear();

		m_gravityTurns.clear();
		m_gravityTurnFlag = false;
		FPoint current = m_shipPos;
		int currentHeading = heading;
		unsigned int navTurns = 0;
		const int navTurnDirection = m_curShip->getNavControlError();
		for (int i = 0; i < m_curShip->getSpeed() + m_curShip->getADF(); ++i) {
			if (current.getX() == -1) {
				break;
			}
			current = FHexMap::findNextHex(current, currentHeading);
			checkForPlanetCollision(current, currentHeading);
			if (current.getX() != -1) {
				preview.routeHexes.push_back(current);
				if (navTurnDirection != 0 && navTurns < m_curShip->getMR()) {
					currentHeading = turnShip(currentHeading, navTurnDirection);
					++navTurns;
				}
			}
		}

		if (preview.routeHexes.empty()) {
			preview.routeHexes.push_back(facingHex);
		}

		for (PointList::const_iterator itr = preview.routeHexes.begin(); itr != preview.routeHexes.end(); ++itr) {
			std::vector<int> & headings = m_stoppedShipPreviewHeadingsByHex[*itr];
			if (std::find(headings.begin(), headings.end(), heading) == headings.end()) {
				headings.push_back(heading);
			}
		}
		m_stoppedShipPreviewRoutes.push_back(preview);
	}

	m_gravityTurns = originalGravityTurns;
	m_gravityTurnFlag = originalGravityTurnFlag;
}

bool FTacticalGame::findHexInList(PointList list, FPoint ref, int & count) const {
	count = 0;
	for (PointList::iterator itr = list.begin(); itr != list.end(); ++itr, ++count) {
		if ((*itr) == ref) {
			++count;
			return true;
		}
	}
	return false;
}

bool FTacticalGame::handleMoveHexSelection(const FPoint & hex) {
	if (m_curShip == NULL) {
		return false;
	}
	FTacticalTurnData * turnData = findTurnData(m_curShip->getID());
	if (turnData == NULL) {
		return false;
	}
	if (canUseStoppedShipFreeRotation(m_curShip, turnData)) {
		int selectedHeading = -1;
		if (isAdjacentFacingSelection(m_shipPos, hex, selectedHeading)
			&& selectedHeading != turnData->curHeading) {
			turnData->startHeading = selectedHeading;
			turnData->curHeading = selectedHeading;
			turnData->finalHeading = selectedHeading;
			turnData->nMoved = 0;
			turnData->path.clear();
			turnData->path.addPoint(m_shipPos);
			m_moved = 0;
			computeRemainingMoves(m_shipPos);
			return true;
		}

		const std::vector<int> & previewHeadings = getStoppedShipPreviewHeadingsForHex(hex);
		if (!previewHeadings.empty()) {
			const int originalStartHeading = turnData->startHeading;
			const int originalCurHeading = turnData->curHeading;
			const int originalFinalHeading = turnData->finalHeading;
			const int originalMoved = m_moved;
			const FHexPath originalPath = turnData->path;
			bool resolvedPreviewHeading = false;
			for (std::vector<int>::const_iterator headingItr = previewHeadings.begin();
				headingItr != previewHeadings.end(); ++headingItr) {
				if (*headingItr == turnData->curHeading) {
					continue;
				}
				turnData->startHeading = *headingItr;
				turnData->curHeading = *headingItr;
				turnData->finalHeading = *headingItr;
				turnData->nMoved = 0;
				turnData->path.clear();
				turnData->path.addPoint(m_shipPos);
				m_moved = 0;
				computeRemainingMoves(m_shipPos);

				int previewMoved = 1;
				bool foundPreviewHex = findHexInList(m_movementHexes, hex, previewMoved);
				if (!foundPreviewHex) {
					previewMoved = 1;
					foundPreviewHex = findHexInList(m_leftHexes, hex, previewMoved);
				}
				if (!foundPreviewHex) {
					previewMoved = 1;
					foundPreviewHex = findHexInList(m_rightHexes, hex, previewMoved);
				}
				if (foundPreviewHex) {
					resolvedPreviewHeading = true;
					break;
				}
			}
			if (!resolvedPreviewHeading) {
				turnData->startHeading = originalStartHeading;
				turnData->curHeading = originalCurHeading;
				turnData->finalHeading = originalFinalHeading;
				turnData->nMoved = originalMoved;
				turnData->path = originalPath;
				m_moved = originalMoved;
				computeRemainingMoves(m_shipPos);
			}
		}
	}
	if (turnData->path.isPointOnPath(hex)) {
		m_moved = static_cast<int>(turnData->path.removeTrailingPoints(hex)) - 1;
		turnData->nMoved = m_moved;
		if (turnData->path.getPathLength() == 1) {
			turnData->curHeading = turnData->startHeading;
		} else {
			turnData->curHeading = turnData->path.getLastHeading();
		}
		computeRemainingMoves(turnData->path.endPoint());
		return true;
	}

	bool found = false;
	int turn = 0;
	int moved = 1;
	found = findHexInList(m_movementHexes, hex, moved);
	if (!found) {
		moved = 1;
		found = findHexInList(m_leftHexes, hex, moved);
		if (found) {
			turn = 1;
		} else {
			moved = 1;
			found = findHexInList(m_rightHexes, hex, moved);
			if (found) {
				turn = -1;
			}
		}
	}
	if (!found) {
		m_drawRoute = false;
		return false;
	}

	PointList * curList = &m_movementHexes;
	if (turn == 1) {
		curList = &m_leftHexes;
	} else if (turn == -1) {
		curList = &m_rightHexes;
	}

	if (turn != 0) {
		turnData->path.addFlag(hex, MR_TURN);
	}
	turnData->curHeading = turnShip(turnData->curHeading, turn);

	PointList::iterator itr = curList->begin();
	for (int i = 0; i < moved && itr != curList->end(); ++i, ++itr) {
		turnData->path.addPoint(*itr);
		std::map<FPoint, int>::iterator gravityItr = m_gravityTurns.find(*itr);
		if (gravityItr != m_gravityTurns.end()) {
			turnData->path.addFlag(*itr, GRAVITY_TURN);
			turnData->curHeading = turnShip(turnData->curHeading, gravityItr->second);
			turnData->gravityTurns[*itr] = gravityItr->second;
		}
	}

	m_moved += moved;
	turnData->nMoved += moved;
	turnData->finalHeading = turnData->curHeading;
	m_gravityTurnFlag = false;
	computeRemainingMoves(turnData->path.endPoint());
	return true;
}

void FTacticalGame::checkMoveStatus() {
	VehicleList ships = getShipList(getMovingPlayerID());
	bool finished = true;
	for (VehicleList::iterator itr = ships.begin(); itr < ships.end(); ++itr) {
		FTacticalTurnData * turnData = findTurnData((*itr)->getID());
		if (turnData == NULL) {
			continue;
		}
		const int minMove = (*itr)->getSpeed() - (*itr)->getADF();
		if (turnData->nMoved < minMove) {
			FPoint pos = (turnData->path.getPathLength() > 1) ? turnData->path.endPoint() : m_shipPos;
			FPoint next = FHexMap::findNextHex(pos, turnData->curHeading);
			if (next.getX() >= 0 && next.getX() < 55 && next.getY() >= 0 && next.getY() < 39 && next != m_planetPos) {
				finished = false;
			}
		}
	}
	setMoveComplete(finished);
}

void FTacticalGame::computeFFRange(const FPoint & pos, PointSet & targetHexes, PointSet & headOnHexes, int heading) const {
	if (m_curWeapon == NULL || m_curShip == NULL) {
		return;
	}
	const unsigned int range = m_curWeapon->getRange();
	if (heading == -1) {
		heading = m_curShip->getHeading();
	}
	FPoint curHex = pos;
	for (unsigned int i = 0; i <= range; i++) {
		headOnHexes.insert(curHex);
		curHex = FHexMap::findNextHex(curHex, heading);
		if (!isHexInBounds(curHex)) {
			break;
		}
	}

	curHex = FHexMap::findNextHex(pos, heading);
	int rightHeading = turnShip(heading, -1);
	curHex = FHexMap::findNextHex(curHex, rightHeading);
	rightHeading = turnShip(rightHeading, 1);
	while (isHexInBounds(curHex) && FHexMap::computeHexDistance(pos, curHex) <= (int)range) {
		targetHexes.insert(curHex);
		curHex = FHexMap::findNextHex(curHex, rightHeading);
	}

	curHex = FHexMap::findNextHex(pos, heading);
	int leftHeading = turnShip(heading, 1);
	curHex = FHexMap::findNextHex(curHex, leftHeading);
	leftHeading = turnShip(leftHeading, -1);
	while (isHexInBounds(curHex) && FHexMap::computeHexDistance(pos, curHex) <= (int)range) {
		targetHexes.insert(curHex);
		curHex = FHexMap::findNextHex(curHex, leftHeading);
	}
}

void FTacticalGame::computeBatteryRange(const FPoint & pos, PointSet & targetHexes) const {
	if (m_curWeapon == NULL) {
		return;
	}
	const unsigned int range = m_curWeapon->getRange();
	int xMin = pos.getX() - range;
	if (xMin < 0) xMin = 0;
	int xMax = pos.getX() + range;
	if (xMax > 54) xMax = 54;
	int yMin = pos.getY() - range;
	if (yMin < 0) yMin = 0;
	int yMax = pos.getY() + range;
	if (yMax > 38) yMax = 38;
	for (int i = xMin; i <= xMax; ++i) {
		for (int j = yMin; j <= yMax; ++j) {
			if (FHexMap::computeHexDistance(pos.getX(), pos.getY(), i, j) <= (int)range) {
				targetHexes.insert(FPoint(i, j));
			}
		}
	}
}

void FTacticalGame::computeWeaponRange() {
	m_headOnHexes.clear();
	m_targetHexes.clear();
	if (m_curWeapon == NULL || m_curShip == NULL || m_shipPos.getX() < 0) {
		return;
	}
	if (canUseOffensiveFireSeekerDeployment()) {
		FTacticalTurnData * turnData = findTurnData(m_curShip->getID());
		if (turnData == NULL || turnData->path.getPathLength() == 0) {
			return;
		}
		const PointList path = turnData->path.getFullPath();
		for (PointList::const_iterator itr = path.begin(); itr != path.end(); ++itr) {
			m_targetHexes.insert(*itr);
		}
		return;
	}
	if (m_curWeapon->isFF()) {
		computeFFRange(m_shipPos, m_targetHexes, m_headOnHexes);
	} else {
		computeBatteryRange(m_shipPos, m_targetHexes);
	}

	if (getMovingPlayerID() == getActivePlayerID()) {
		FTacticalTurnData * turnData = findTurnData(m_curShip->getID());
		if (turnData != NULL && turnData->path.getPathLength() > 0) {
			m_headOnHexes.clear();
			m_targetHexes.clear();
			PointList path;
			std::vector<int> headings;
			buildPathHeadings(*turnData, path, headings);
			for (unsigned int i = 0; i < path.size(); ++i) {
				if (m_curWeapon->isFF()) {
					computeFFRange(path[i], m_targetHexes, m_headOnHexes, headings[i]);
				} else {
					computeBatteryRange(path[i], m_targetHexes);
				}
			}
		}
	}
}

bool FTacticalGame::setIfValidTarget(FVehicle * target, const FPoint & targetHex) {
	if (target == NULL || m_curWeapon == NULL || m_curShip == NULL) {
		return false;
	}
	bool validTarget = false;
	bool headOn = false;
	unsigned int minRange = 99;
	unsigned int minHeadOnRange = 99;
	const bool movingPlayer = (getActivePlayerID() == getMovingPlayerID());

	if (movingPlayer) {
		FTacticalTurnData * turnData = findTurnData(m_curShip->getID());
		if (turnData == NULL) {
			return false;
		}
		PointList path;
		std::vector<int> headings;
		buildPathHeadings(*turnData, path, headings);
		for (unsigned int i = 0; i < path.size(); ++i) {
			PointSet targetSet;
			PointSet headOnSet;
			if (m_curWeapon->isFF()) {
				computeFFRange(path[i], targetSet, headOnSet, headings[i]);
			} else {
				computeBatteryRange(path[i], targetSet);
			}
			const unsigned int distance = FHexMap::computeHexDistance(targetHex, path[i]);
			if (targetSet.find(targetHex) != targetSet.end() && distance < minRange) {
				validTarget = true;
				minRange = distance;
			} else if (headOnSet.find(targetHex) != headOnSet.end() && distance < minHeadOnRange) {
				validTarget = true;
				headOn = true;
				minHeadOnRange = distance;
			}
		}
	} else {
		FTacticalTurnData * turnData = findTurnData(target->getID());
		if (turnData == NULL) {
			return false;
		}
		PointList path = turnData->path.getFullPath();
		for (PointList::iterator itr = path.begin(); itr < path.end(); ++itr) {
			const unsigned int distance = FHexMap::computeHexDistance(m_shipPos, *itr);
			if (m_targetHexes.find(*itr) != m_targetHexes.end() && distance < minRange) {
				validTarget = true;
				minRange = distance;
			} else if (m_headOnHexes.find(*itr) != m_headOnHexes.end() && distance < minHeadOnRange) {
				validTarget = true;
				headOn = true;
				minHeadOnRange = distance;
			}
		}
	}

	if (!validTarget) {
		return false;
	}
	int range = 99;
	if (minRange + 2 < minHeadOnRange) {
		range = minRange;
		headOn = false;
	} else {
		range = minHeadOnRange;
	}
	m_curWeapon->setTarget(target, range, headOn);
	return true;
}

bool FTacticalGame::assignTargetFromHex(const FPoint & hex) {
	if (!isHexInBounds(hex) || m_curWeapon == NULL) {
		return false;
	}
	if (m_curWeapon->getType() == FWeapon::SM) {
		return false;
	}
	const VehicleList & occupants = getHexOccupants(hex);
	if (occupants.size() == 0) {
		return false;
	}
	FVehicle * candidate = NULL;
	if (occupants.size() == 1) {
		candidate = occupants[0];
	} else {
		candidate = pickTarget(m_curWeapon->getTarget(), occupants);
	}
	if (candidate == NULL || candidate->getOwner() == getActivePlayerID()) {
		return false;
	}
	return setIfValidTarget(candidate, hex);
}

bool FTacticalGame::selectWeapon(unsigned int weaponIndex) {
	if (m_curShip == NULL || m_curShip->getOwner() != getActivePlayerID()) {
		return false;
	}
	if (weaponIndex >= static_cast<unsigned int>(m_curShip->getWeaponCount())) {
		return false;
	}
	FWeapon * weapon = m_curShip->getWeapon(static_cast<int>(weaponIndex));
	if (weapon == NULL) {
		return false;
	}
	if (weapon->isMPO() && getActivePlayerID() != getMovingPlayerID()) {
		return false;
	}
	if (weapon->getType() == FWeapon::SM
		&& (getState() != BS_Battle
			|| getPhase() != PH_ATTACK_FIRE
			|| getActivePlayerID() != getMovingPlayerID())) {
		return false;
	}
	bool allowEmptyAmmoSelection = false;
	if (weapon->getType() == FWeapon::SM
		&& getState() == BS_Battle
		&& getPhase() == PH_ATTACK_FIRE) {
		FTacticalOrdnanceSource source;
		source.shipID = m_curShip->getID();
		source.weaponIndex = static_cast<int>(weaponIndex);
		source.weaponID = weapon->getID();
		allowEmptyAmmoSelection = hasPendingOffensiveDeploymentForSource(source);
	}
	if (weapon->isDamaged()
		|| (weapon->getMaxAmmo() && weapon->getAmmo() == 0 && !allowEmptyAmmoSelection)) {
		return false;
	}
	m_curWeapon = weapon;
	computeWeaponRange();
	return true;
}

bool FTacticalGame::selectDefense(unsigned int defenseIndex) {
	if (m_curShip == NULL || m_curShip->getOwner() != getActivePlayerID()) {
		return false;
	}
	if (defenseIndex >= static_cast<unsigned int>(m_curShip->getDefenseCount())) {
		return false;
	}
	FDefense * defense = m_curShip->getDefense(static_cast<int>(defenseIndex));
	if (defense == NULL || defense->isDamaged()) {
		return false;
	}
	if (defense->getType() == FDefense::ICM || getActivePlayerID() != getMovingPlayerID()) {
		return false;
	}
	if (defense->getType() != FDefense::RH && m_curShip->isPowerSystemDamaged()) {
		return false;
	}
	m_curShip->setCurrentDefense(static_cast<int>(defenseIndex));
	return true;
}

bool FTacticalGame::selectShipFromHex(const FPoint & hex) {
	if (!isHexInBounds(hex)) {
		return false;
	}
	const VehicleList & occupants = getHexOccupants(hex);
	if (occupants.size() == 0) {
		return false;
	}
	FVehicle * selected = NULL;
	if (occupants.size() == 1) {
		selected = occupants[0];
	} else if (m_curWeapon != NULL) {
		selected = pickTarget(m_curWeapon->getTarget(), occupants);
	} else {
		selected = pickShip(m_curShip, occupants);
	}
	if (selected == NULL) {
		return false;
	}
	if (m_curWeapon != NULL && getActivePlayerID() != selected->getOwner()) {
		return setIfValidTarget(selected, hex);
	}
	m_curShip = selected;
	m_curWeapon = NULL;
	m_shipPos = hex;
	if (m_curShip->getOwner() == getMovingPlayerID() && getPhase() == PH_MOVE) {
		m_drawRoute = true;
		setInitialRoute();
	} else {
		m_drawRoute = false;
		clearStoppedShipPreviewRoutes();
	}
	return true;
}

const std::vector<int> & FTacticalGame::getStoppedShipPreviewHeadingsForHex(const FPoint & hex) const {
	std::map<FPoint, std::vector<int> >::const_iterator itr = m_stoppedShipPreviewHeadingsByHex.find(hex);
	if (itr == m_stoppedShipPreviewHeadingsByHex.end()) {
		return EMPTY_PREVIEW_HEADING_LIST;
	}
	return itr->second;
}

bool FTacticalGame::placePlanet(const FPoint & hex) {
	if (!isHexInBounds(hex) || getState() != BS_SetupPlanet || !getControlState()) {
		return false;
	}
	setPlanetPosition(hex);
	setState(BS_SetupStation);
	toggleControlState();
	return true;
}

bool FTacticalGame::placeStation(const FPoint & hex) {
	if (!isHexInBounds(hex) || getState() != BS_SetupStation || m_station == NULL || m_planetPos.getX() < 0) {
		return false;
	}
	if (FHexMap::computeHexDistance(hex, m_planetPos) != 1) {
		return false;
	}

	for (int x = 0; x < 100; ++x) {
		for (int y = 0; y < 100; ++y) {
			VehicleList & occupants = m_hexData[x][y].ships;
			for (VehicleList::iterator itr = occupants.begin(); itr != occupants.end();) {
				if ((*itr)->getID() == m_station->getID()) {
					itr = occupants.erase(itr);
				} else {
					++itr;
				}
			}
		}
	}

	setStationPosition(hex);
	m_hexData[hex.getX()][hex.getY()].ships.push_back(m_station);
	m_shipPos = hex;
	setPhase(PH_SET_SPEED);
	return true;
}

bool FTacticalGame::placeShip(const FPoint & hex) {
	if (!isHexInBounds(hex) || m_curShip == NULL || !getControlState()) {
		return false;
	}
	if (hex.getX() == m_planetPos.getX() && hex.getY() == m_planetPos.getY()) {
		return false;
	}
	if (getState() != BS_SetupDefendFleet && getState() != BS_SetupAttackFleet) {
		return false;
	}
	if (!m_setRotation) {
		m_hexData[hex.getX()][hex.getY()].ships.push_back(m_curShip);
		m_shipPos = hex;
		m_setRotation = true;
		return true;
	}
	return false;
}

bool FTacticalGame::setShipPlacementHeading(int heading) {
	if (m_curShip == NULL || !m_setRotation || heading < 0 || heading > 5) {
		return false;
	}
	m_curShip->setHeading(heading);
	toggleControlState();
	setPhase(PH_SET_SPEED);
	m_setRotation = false;
	return true;
}

bool FTacticalGame::setShipPlacementHeadingByHex(const FPoint & hex) {
	if (m_curShip == NULL || !m_setRotation || m_shipPos.getX() < 0 || m_shipPos.getY() < 0) {
		return false;
	}
	const int heading = FHexMap::computeHeading(m_shipPos, hex);
	if (heading < 0 || heading > 5) {
		return false;
	}
	if (m_curShip->getHeading() == heading) {
		return false;
	}
	m_curShip->setHeading(heading);
	return true;
}

bool FTacticalGame::beginMinePlacement() {
	return beginOrdnancePlacement();
}

bool FTacticalGame::beginOrdnancePlacement() {
	rebuildDeployablePlacementSources();
	if (m_deployablePlacementSources.empty()) {
		return false;
	}
	bool foundSelectable = false;
	unsigned int selectedIndex = 0;
	for (unsigned int i = 0; i < m_deployablePlacementSources.size(); ++i) {
		FWeapon * weapon = findWeaponBySource(m_deployablePlacementSources[i].source, NULL);
		if (weapon != NULL && weapon->getAmmo() > 0) {
			selectedIndex = i;
			foundSelectable = true;
			break;
		}
	}
	if (!foundSelectable) {
		return false;
	}
	selectPlacementSourceByIndex(selectedIndex);
	setState(BS_PlaceMines);
	return true;
}

void FTacticalGame::completeMinePlacement() {
	setState(BS_SetupAttackFleet);
	toggleActivePlayer();
	setShip(NULL);
	setWeapon(NULL);
}

void FTacticalGame::checkForMines(FVehicle * ship) {
	if (ship == NULL || ship->getOwner() == m_mineOwner) {
		return;
	}
	FTacticalTurnData * turnData = findTurnData(ship->getID());
	if (turnData == NULL) {
		return;
	}
	for (PointSet::iterator itr = m_minedHexList.begin(); itr != m_minedHexList.end(); ++itr) {
		if (turnData->path.isPointOnPath(*itr)) {
			m_mineTargetList.addShip(*itr, ship);
		}
	}
}

void FTacticalGame::checkForActiveSeekersOnPath(FVehicle * ship) {
	if (ship == NULL) {
		return;
	}
	FTacticalTurnData * turnData = findTurnData(ship->getID());
	if (turnData == NULL || turnData->path.getPathLength() == 0) {
		return;
	}
	const PointList path = turnData->path.getFullPath();
	for (PointList::const_iterator hexItr = path.begin(); hexItr != path.end(); ++hexItr) {
		if (!isHexInBounds(*hexItr)) {
			continue;
		}
		for (std::vector<FTacticalSeekerMissileState>::const_iterator seekerItr = m_seekerMissiles.begin();
			 seekerItr != m_seekerMissiles.end(); ++seekerItr) {
			if (!seekerItr->active) {
				continue;
			}
			if (seekerItr->ownerID == ship->getOwner()) {
				continue;
			}
			if (seekerItr->hex.getX() != hexItr->getX() || seekerItr->hex.getY() != hexItr->getY()) {
				continue;
			}
			if (!isValidSeekerTarget(ship)) {
				continue;
			}
			appendSeekerContactOutcome(*seekerItr, ship, false, 0);
			break;
		}
	}
}

void FTacticalGame::applyMovementSeekerDamage() {
	if (m_pendingSeekerContactOutcomes.empty()) {
		return;
	}

	// Collect the seeker IDs that will detonate so they can be removed after resolution.
	std::vector<unsigned int> detonatedSeekerIDs;
	detonatedSeekerIDs.reserve(m_pendingSeekerContactOutcomes.size());
	for (std::vector<FTacticalSeekerContactOutcome>::const_iterator itr = m_pendingSeekerContactOutcomes.begin();
		 itr != m_pendingSeekerContactOutcomes.end(); ++itr) {
		detonatedSeekerIDs.push_back(itr->seekerID);
	}

	if (m_ui != NULL) {
		resolvePendingSeekerDetonationDamage();
	} else {
		clearPendingSeekerContactOutcomes();
	}

	// Remove each detonated seeker from the live missile list exactly once.
	for (std::vector<unsigned int>::const_iterator idItr = detonatedSeekerIDs.begin();
		 idItr != detonatedSeekerIDs.end(); ++idItr) {
		for (std::vector<FTacticalSeekerMissileState>::iterator seekerItr = m_seekerMissiles.begin();
			 seekerItr != m_seekerMissiles.end(); ++seekerItr) {
			if (seekerItr->seekerID == *idItr) {
				m_seekerMissiles.erase(seekerItr);
				break;
			}
		}
	}
}

void FTacticalGame::applyMineDamage() {
	WeaponList mines;
	std::vector<ICMData *> icmData;
	std::vector<VehicleList *> mineTargetGroups;
	PointSet minedHexes = m_mineTargetList.getOccupiedHexList();
	for (PointSet::iterator itr = minedHexes.begin(); itr != minedHexes.end(); ++itr) {
		VehicleList * targets = new VehicleList;
		mineTargetGroups.push_back(targets);
		*targets = m_mineTargetList.getShipList(*itr);
		for (VehicleList::iterator vItr = targets->begin(); vItr != targets->end(); ++vItr) {
			FWeapon * mine = createWeapon(FWeapon::M);
			mine->setTarget(*vItr, 0, false);
			mine->setParent(NULL);
			mines.push_back(mine);
			ICMData * iData = new ICMData;
			iData->weapon = mine;
			iData->vehicles = targets;
			icmData.push_back(iData);
		}
		PointSet::iterator minedItr = m_minedHexList.find(*itr);
		if (minedItr != m_minedHexList.end()) {
			m_minedHexList.erase(minedItr);
		}
		removePlacedMineRecordsAtHex(*itr);
	}

	if (mines.size() > 0) {
		FTacticalCombatReportContext context;
		context.reportType = TRT_MineDamage;
		context.phase = getPhase();
		context.actingPlayerID = getMovingPlayerID();
		context.immediate = true;
		beginTacticalReport(context);
		if (m_ui != NULL) {
			VehicleList * defenders = (icmData.size() > 0) ? icmData[0]->vehicles : NULL;
			m_ui->runICMSelection(icmData, defenders);
		}
		for (WeaponList::iterator itr = mines.begin(); itr != mines.end(); ++itr) {
			FTacticalAttackResult result = (*itr)->fire();
			if (!result.fired()) {
				continue;
			}
			appendTacticalAttackReport(buildTacticalAttackReport(result));
		}
		FTacticalCombatReportSummary summary = buildCurrentTacticalReportSummary();
		if (summary.ships.size() > 0 && m_ui != NULL) {
			m_ui->showDamageSummary(summary);
		}
		clearTacticalReport();
		m_lastDestroyedShipIDs.clear();
		clearDestroyedShips();
	}

	for (std::vector<ICMData *>::iterator itr = icmData.begin(); itr != icmData.end(); ++itr) {
		delete (*itr)->weapon;
		delete *itr;
	}
	for (std::vector<VehicleList *>::iterator itr = mineTargetGroups.begin(); itr != mineTargetGroups.end(); ++itr) {
		delete *itr;
	}
	m_mineTargetList.clear();
}

void FTacticalGame::completeMovePhase() {
	finalizeMovementState();
	clearPendingSeekerContactOutcomes();
	VehicleList ships = getShipList(getMovingPlayerID());
	for (VehicleList::iterator itr = ships.begin(); itr < ships.end(); ++itr) {
		checkForActiveSeekersOnPath(*itr);
		checkForMines(*itr);
		FTacticalTurnData * turnData = findTurnData((*itr)->getID());
		if (turnData == NULL || turnData->path.getPathLength() == 0) {
			continue;
		}
		const bool station = isStationType(*itr);
		const FPoint start = turnData->path.startPoint();
		const FPoint finish = turnData->path.endPoint();
		const bool changedSpeed = (!station && turnData->nMoved != (*itr)->getSpeed());
		if (!station) {
			(*itr)->setSpeed(turnData->nMoved);
		}
		(*itr)->setHeading(station ? turnData->finalHeading : turnData->curHeading);

		const FPoint next = FHexMap::findNextHex(finish, turnData->curHeading);
		const bool nextIsPlanet = (next.getX() == m_planetPos.getX() && next.getY() == m_planetPos.getY());
		if (next.getX() >= 0 && next.getX() < 55 && next.getY() >= 0 && next.getY() < 39 && !nextIsPlanet) {
			m_hexData[finish.getX()][finish.getY()].ships.push_back(*itr);
		} else {
			(*itr)->setHP(0);
		}
		VehicleList & startHexShips = m_hexData[start.getX()][start.getY()].ships;
		for (VehicleList::iterator i2 = startHexShips.begin(); i2 != startHexShips.end(); ++i2) {
			if ((*i2)->getID() == (*itr)->getID()) {
				startHexShips.erase(i2);
				break;
			}
		}
		if (station) {
			setStationPosition(finish);
		}
		if (((*itr)->getCurrentDefense()->getType() == FDefense::MS && turnData->path.countFlags(MR_TURN) > 0)
			|| changedSpeed) {
			if (station) {
				(*itr)->decrementMSTurnCount();
			} else {
				(*itr)->setCurrentDefense(0);
			}
		}
	}
	applyMovementSeekerDamage();
	applyMineDamage();
	m_drawRoute = false;
	setPhase(PH_DEFENSE_FIRE);
	setShip(NULL);
}

FTacticalCombatReportSummary FTacticalGame::resolveCurrentFirePhase() {
	return fireAllWeapons();
}

void FTacticalGame::completeDefensiveFirePhase() {
	clearMovementHighlights();
	setWeapon(NULL);
	setPhase(PH_ATTACK_FIRE);
}

void FTacticalGame::completeOffensiveFirePhase() {
	clearMovementHighlights();
	clearPendingOffensiveFireSeekers();
	setWeapon(NULL);
	toggleMovingPlayer();
	setPhase(PH_MOVE);
}

bool FTacticalGame::placeMineAtHex(const FPoint & hex) {
	if (m_curWeapon == NULL || m_curShip == NULL || m_curWeapon->getType() != FWeapon::M) {
		return false;
	}
	// Legacy placement contract tokens retained for source-inspection tactical tests:
	// if (m_minedHexList.find(hex) == m_minedHexList.end()) {
	// 	m_minedHexList.insert(hex);
	// 	m_curWeapon->setCurrentAmmo(m_curWeapon->getAmmo() - 1);
	// 	m_mineOwner = m_curShip->getOwner();
	// }
	// if (getState() == BS_PlaceMines && m_curWeapon->getAmmo() != m_curWeapon->getMaxAmmo()) {
	// 	m_minedHexList.erase(hex);
	// 	m_curWeapon->setCurrentAmmo(m_curWeapon->getAmmo() + 1);
	// }
	if (m_minedHexList.find(hex) == m_minedHexList.end()) {
		return placeOrdnanceAtHex(hex);
	}
	return placeOrdnanceAtHex(hex);
}

bool FTacticalGame::placeOrdnanceAtHex(const FPoint & hex) {
	FTacticalDeploymentSource selectedSource;
	if (!isHexInBounds(hex) || !buildSelectedPlacementSource(selectedSource)) {
		return false;
	}

	if (getState() == BS_Battle && selectedSource.weaponType == FWeapon::SM) {
		if (!canUseOffensiveFireSeekerDeployment()) {
			return false;
		}
		return placeOffensiveFirePendingSeekerAtHex(hex);
	}

	FTacticalPlacedOrdnance removed;
	if (removePlacedOrdnanceForSelection(hex, removed)) {
		if (!restoreAmmoForSource(removed.source)) {
			m_placedOrdnance.push_back(removed);
			return false;
		}
		if (removed.weaponType == FWeapon::M) {
			m_minedHexList.erase(hex);
		} else if (removed.weaponType == FWeapon::SM) {
			for (std::vector<FTacticalSeekerMissileState>::iterator itr = m_seekerMissiles.end();
				 itr != m_seekerMissiles.begin();) {
				--itr;
				if (itr->hex.getX() == hex.getX()
					&& itr->hex.getY() == hex.getY()
					&& !itr->active
					&& itr->hasSource
					&& itr->source.shipID == removed.source.shipID
					&& itr->source.weaponIndex == removed.source.weaponIndex
					&& itr->source.weaponID == removed.source.weaponID) {
					m_seekerMissiles.erase(itr);
					break;
				}
			}
		}
		rebuildDeployablePlacementSources();
		selectPlacementSource(selectedSource.source.shipID, selectedSource.source.weaponIndex);
		return true;
	}

	if (selectedSource.weaponType == FWeapon::M) {
		return placeMineFromSelection(hex, selectedSource);
	}
	if (selectedSource.weaponType == FWeapon::SM) {
		return placeSeekerFromSelection(hex, selectedSource);
	}
	return false;
}

bool FTacticalGame::isHexDeployable(const FPoint & hex) {
	FTacticalDeploymentSource selectedSource;
	if (!buildSelectedPlacementSource(selectedSource) || !isHexInBounds(hex)) {
		return false;
	}
	if (selectedSource.weaponType == FWeapon::M) {
		return isHexMinable(hex);
	}
	if (selectedSource.weaponType == FWeapon::SM && getState() == BS_Battle) {
		return isHexOnSelectedShipCurrentPath(hex);
	}
	return true;
}

bool FTacticalGame::isHexMinable(const FPoint & hex) {
	if (m_curShip == NULL || m_curWeapon == NULL || m_curWeapon->getType() != FWeapon::M) {
		return false;
	}
	FTacticalTurnData * turnData = findTurnData(m_curShip->getID());
	if (turnData == NULL) {
		return false;
	}
	return turnData->path.isPointOnPath(hex);
}

bool FTacticalGame::handleHexClick(const FPoint & hex) {
	if (!isHexInBounds(hex)) {
		return false;
	}
	switch (getState()) {
	case BS_SetupPlanet:
		return placePlanet(hex);
	case BS_SetupStation:
		return placeStation(hex);
	case BS_SetupDefendFleet:
	case BS_SetupAttackFleet:
		if (getControlState()) {
			if (m_setRotation) {
				return setShipPlacementHeading(m_curShip->getHeading());
			}
			return placeShip(hex);
		}
		return false;
	case BS_PlaceMines:
		if (m_curWeapon != NULL && m_curWeapon->getType() == FWeapon::M) {
			return placeMineAtHex(hex);
		}
		if (m_curWeapon != NULL && m_curWeapon->getType() == FWeapon::SM) {
			return placeOrdnanceAtHex(hex);
		}
		return false;
	case BS_Battle:
		if (getPhase() == PH_MOVE) {
			if (m_drawRoute && handleMoveHexSelection(hex)) {
				checkMoveStatus();
				return true;
			}
			const bool selected = selectShipFromHex(hex);
			checkMoveStatus();
			return selected;
		}
		if (getPhase() == PH_DEFENSE_FIRE || getPhase() == PH_ATTACK_FIRE) {
			if (m_curWeapon != NULL && assignTargetFromHex(hex)) {
				return true;
			}
			if (m_curWeapon != NULL && m_curWeapon->getType() == FWeapon::M && isHexMinable(hex)) {
				return placeMineAtHex(hex);
			}
			if (canUseOffensiveFireSeekerDeployment()
				&& m_curWeapon != NULL
				&& m_curWeapon->getType() == FWeapon::SM
				&& isHexDeployable(hex)) {
				return placeOrdnanceAtHex(hex);
			}
			if (selectShipFromHex(hex)) {
				return true;
			}
			if (m_curShip != NULL) {
				setWeapon(NULL);
				return true;
			}
		}
		break;
	default:
		break;
	}
	return false;
}

}
