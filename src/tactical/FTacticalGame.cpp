/**
 * @file FTacticalGame.cpp
 * @brief Implementation file for FTacticalGame class
 * @author Tom Stephens
 * @date Created:  Mar 29, 2026
 *
 */

#include "tactical/FTacticalGame.h"

#include "tactical/ITacticalUI.h"
#include "tactical/FTacticalAttackResult.h"

namespace Frontier {

namespace {

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
		event.subject = FTacticalShipReference(ship->getID(), ship->getOwner(), ship->getName());
		event.source = event.subject;
		event.target = event.subject;
		event.rollValue = itr->rollValue;
		event.hullDamage = itr->hullDamageApplied;
		event.attackIndex = -1;
		event.immediate = true;
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
	event.subject = report.target;
	event.source = report.attacker;
	event.target = report.target;
	event.rollValue = effect.rollValue;
	event.hullDamage = effect.hullDamageApplied;
	event.attackIndex = -1;
	event.immediate = report.immediate;
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
	m_closeInProgress = false;
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
	m_shipPos.setPoint(-1, -1);
	m_movementHexes.clear();
	m_leftHexes.clear();
	m_rightHexes.clear();
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
	m_phase = p;
	if (p == PH_MOVE) {
		applyFireDamage();
		if (!m_activePlayer) {
		}
		toggleActivePlayer();
		resetMovementState();
	} else if (p == PH_FINALIZE_MOVE) {
		finalizeMovementState();
		setPhase(PH_DEFENSE_FIRE);
		m_curShip = NULL;
		return;
	} else if (p == PH_DEFENSE_FIRE) {
		toggleActivePlayer();
	} else if (p == PH_ATTACK_FIRE) {
		toggleActivePlayer();
	}
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
	clearDestroyedShips();
	clearICMVector(m_ICMData);
	return summary;
}

int FTacticalGame::clearDestroyedShips() {
	m_lastDestroyedShipIDs.clear();
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
			d.finalHeading = d.curHeading;
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

}
