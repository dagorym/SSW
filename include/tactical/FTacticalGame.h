/**
 * @file FTacticalGame.h
 * @brief Header file for FTacticalGame class
 * @author Tom Stephens
 * @date Created:  Mar 29, 2026
 *
 */

#ifndef _FTACTICALGAME_H_
#define _FTACTICALGAME_H_

#include <map>
#include <vector>

#include "Frontier.h"
#include "core/FHexMap.h"
#include "core/FHexPath.h"
#include "core/FPoint.h"
#include "ships/FVehicle.h"
#include "strategic/FFleet.h"
#include "tactical/FTacticalCombatReport.h"
#include "weapons/FWeapon.h"

#define MR_TURN         1
#define GRAVITY_TURN    2

namespace Frontier {

class ITacticalUI;

/**
 * @brief Tactical hex state for FTacticalGame model ownership.
 */
typedef struct {
FPoint pos;
VehicleList ships;
} FTacticalHexData;

/**
 * @brief Per-ship movement bookkeeping for FTacticalGame model ownership.
 */
typedef struct {
bool hasMoved;
PointList waypoints;
std::vector<int> turns;
std::vector<FPoint> movedHexes;
int speed;
int startHeading;
int finalHeading;
int curHeading;
int nMoved;
std::map<FPoint, int> gravityTurns;
FHexPath path;
} FTacticalTurnData;

/**
 * @brief Pure C++ tactical mechanics state container.
 */
class FTacticalGame {
public:
FTacticalGame();
virtual ~FTacticalGame();

void reset();

int setupFleets(FleetList * aList, FleetList * dList, bool planet = false, FVehicle * station = NULL);
void setState(int s);
const int & getState() const { return m_state; }

void setPhase(int p);
int getPhase() const { return m_phase; }

VehicleList getShipList() const;
VehicleList getShipList(unsigned int id) const;

bool getActivePlayer() const { return m_activePlayer; }
void setActivePlayer(bool active) { m_activePlayer = active; }
void toggleActivePlayer() { m_activePlayer = !m_activePlayer; }

bool getMovingPlayer() const { return m_movingPlayer; }
void setMovingPlayer(bool movingPlayer) { m_movingPlayer = movingPlayer; }
void toggleMovingPlayer() { m_movingPlayer = !m_movingPlayer; }

const unsigned int & getAttackerID() const { return m_playerID[1]; }
const unsigned int & getDefenderID() const { return m_playerID[0]; }
const unsigned int & getActivePlayerID() const { return m_activePlayer ? getAttackerID() : getDefenderID(); }
const unsigned int & getMovingPlayerID() const { return m_movingPlayer ? getAttackerID() : getDefenderID(); }

void setMoveComplete(bool complete) { m_moveComplete = complete; }
bool isMoveComplete() const { return m_moveComplete; }

void setShip(FVehicle * ship) { m_curShip = ship; }
FVehicle * getShip() const { return m_curShip; }
void setWeapon(FWeapon * weapon) { m_curWeapon = weapon; }
FWeapon * getWeapon() const { return m_curWeapon; }

void beginTacticalReport(const FTacticalCombatReportContext & context);
void appendTacticalAttackReport(const FTacticalAttackReport & attack);
void appendTacticalReportEvent(const FTacticalReportEvent & event);
FTacticalCombatReportSummary buildCurrentTacticalReportSummary() const;
void clearTacticalReport();
const FTacticalCombatReport & getCurrentTacticalReport() const { return m_tacticalReport; }

void applyFireDamage();
void fireICM();
FTacticalCombatReportSummary fireAllWeapons();
int clearDestroyedShips();
bool isCombatOver() const;
bool hasWinner() const { return m_hasWinner; }
unsigned int getWinnerID() const { return m_winnerID; }
void clearWinner();

void resetMovementState();
void finalizeMovementState();
void clearMovementHighlights();
void resetTurnInfoForCurrentMover();
FTacticalTurnData * findTurnData(unsigned int shipID);
const FTacticalTurnData * findTurnData(unsigned int shipID) const;

protected:
/// Placeholder for future UI abstraction (Milestone 6)
ITacticalUI * m_ui;

/// Legacy FBattleScreen battle state
int m_state;
int m_phase;
bool m_control;
bool m_activePlayer;
bool m_movingPlayer;
bool m_moveComplete;
bool m_done;
bool m_closeInProgress;
bool m_hasWinner;
unsigned int m_winnerID;

/// Tactical setup and scenario state
bool m_hasPlanet;
int m_planetChoice;
FPoint m_planetPos;
FPoint m_stationPos;
FVehicle * m_station;
unsigned int m_playerID[2];

/// Tactical selection state
FVehicle * m_curShip;
FWeapon * m_curWeapon;

/// Fleet and ship ownership state
FleetList * m_attackList;
VehicleList * m_attackShips;
FleetList * m_defendList;
VehicleList * m_defendShips;

/// ICM and report lifecycle state
std::vector<ICMData *> m_ICMData;
FTacticalCombatReport m_tacticalReport;

/// Tactical hex-map mechanics state (legacy FBattleBoard model data)
FTacticalHexData m_hexData[100][100];
FPoint m_shipPos;
std::vector<FPoint> m_movementHexes;
std::vector<FPoint> m_leftHexes;
std::vector<FPoint> m_rightHexes;
bool m_drawRoute;
int m_moved;
std::map<unsigned int, FTacticalTurnData> m_turnInfo;
PointSet m_targetHexes;
PointSet m_headOnHexes;
std::map<FPoint, int> m_gravityTurns;
bool m_gravityTurnFlag;
PointSet m_minedHexList;
FHexMap m_mineTargetList;
unsigned int m_mineOwner;
};

}

#endif //_FTACTICALGAME_H_
