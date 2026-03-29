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
