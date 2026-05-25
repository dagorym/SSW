/**
 * @file FTacticalGame.h
 * @brief Header file for FTacticalGame class
 * @author Tom Stephens, gpt-5.4 (high), gpt-5.3-codex (standard)
 * @date Created:  Mar 29, 2026
 * @date Last Modified: May 24, 2026
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
 *
 * Compatibility note (Milestone 5): mirrors legacy Frontier::hexData in
 * FBattleBoard.h so model-only callers can use FTacticalGame.h without
 * depending on wx tactical headers.
 */
typedef struct {
FPoint pos;
VehicleList ships;
} FTacticalHexData;

/**
 * @brief Per-ship movement bookkeeping for FTacticalGame model ownership.
 *
 * Compatibility note (Milestone 5): mirrors legacy Frontier::turnData in
 * FBattleBoard.h until Milestones 7-8 remove duplicated state from wx owners.
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
 * @brief Preview route metadata for a stopped-ship starting-facing option.
 */
typedef struct {
int startHeading;
FPoint facingHex;
PointList routeHexes;
} FTacticalMovePreviewRoute;

/**
 * @brief Source provenance metadata for placed tactical ordnance.
 *
 * Stores the source ship and exact weapon slot that created a placed
 * ordnance marker so later placement undo and rendering steps can retain
 * source identity without depending on wx types.
 *
 * @author Tom Stephens, gpt-5.4 (high)
 * @date Created: May 24, 2026
 * @date Last Modified: May 24, 2026
 */
typedef struct {
unsigned int shipID;
int weaponIndex;
unsigned int weaponID;
} FTacticalOrdnanceSource;

/**
 * @brief Deployable weapon-slot metadata for tactical placement setup.
 *
 * Captures owner, ship, exact weapon slot, and weapon type for one placement
 * source so setup placement can target either mines or seekers without
 * changing wx-owned row rendering code in this milestone.
 *
 * @author Tom Stephens, gpt-5.3-codex (standard)
 * @date Created: May 24, 2026
 * @date Last Modified: May 24, 2026
 */
typedef struct {
	unsigned int ownerID;
	FWeapon::Weapon weaponType;
	FTacticalOrdnanceSource source;
} FTacticalDeploymentSource;

/**
 * @brief Model record for a placed tactical ordnance item.
 *
 * This lightweight record is intended for tactical model ownership of placed
 * ordnance metadata regardless of eventual rendering implementation.
 *
 * @author Tom Stephens, gpt-5.4 (high)
 * @date Created: May 24, 2026
 * @date Last Modified: May 24, 2026
 */
typedef struct {
FWeapon::Weapon weaponType;
unsigned int ownerID;
FTacticalOrdnanceSource source;
FPoint hex;
int displayColorIndex;
int displayMarkerIndex;
} FTacticalPlacedOrdnance;

/**
 * @brief Model record for one tactical seeker missile instance.
 *
 * Tracks active/inactive seeker position and movement state with optional
 * source provenance so later subtasks can reconcile setup/offensive-fire
 * deployments to their launching weapon slot.
 *
 * @author Tom Stephens, gpt-5.4 (high)
 * @date Created: May 24, 2026
 * @date Last Modified: May 24, 2026
 */
typedef struct {
unsigned int seekerID;
unsigned int ownerID;
FPoint hex;
int heading;
bool active;
int movementTurn;
int movementAllowance;
bool hasSource;
FTacticalOrdnanceSource source;
} FTacticalSeekerMissileState;

/**
 * @brief Pure C++ tactical mechanics state container.
 *
 * Owns non-wx tactical battle state, including ship placement, movement,
 * combat-report bookkeeping, and lightweight source-tracked ordnance and
 * seeker-missile records used by later rendering and activation subtasks.
 *
 * @author Tom Stephens, gpt-5.4 (high), gpt-5.3-codex (standard)
 * @date Created: Mar 29, 2026
 * @date Last Modified: May 25, 2026
 */
class FTacticalGame {
public:
FTacticalGame();
virtual ~FTacticalGame();

void installUI(ITacticalUI * ui);
ITacticalUI * getUI() const { return m_ui; }

void reset();

int setupFleets(FleetList * aList, FleetList * dList, bool planet = false, FVehicle * station = NULL);
void setState(int s);
const int & getState() const { return m_state; }
void toggleControlState() { m_control = !m_control; }
const bool & getControlState() const { return m_control; }
void setControlState(bool control) { m_control = control; }

void setDone(bool done) { m_done = done; }
bool getDone() const { return m_done; }

void setPlanet(int c) { m_planetChoice = c; }
const int & getPlanetChoice() const { return m_planetChoice; }
void setPlanetPosition(FPoint h) { m_planetPos = h; }
const FPoint & getPlanetPos() const { return m_planetPos; }
void setStationPosition(FPoint h) { m_stationPos = h; }
const FPoint & getStationPos() const { return m_stationPos; }
void setStation(FVehicle * station) { m_station = station; }
FVehicle * getStation() const { return m_station; }
bool hasPlanet() const { return m_hasPlanet; }

void setPhase(int p);
int getPhase() const { return m_phase; }
/// complete seeker activation and enter normal movement for the moving player
void completeSeekerActivationPhase();

VehicleList getShipList() const;
VehicleList getShipList(unsigned int id) const;
FleetList * getAttackList() const { return m_attackList; }
FleetList * getDefendList() const { return m_defendList; }
VehicleList * getAttackShips() const { return m_attackShips; }
VehicleList * getDefendShips() const { return m_defendShips; }

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
	bool selectWeapon(unsigned int weaponIndex);
	bool selectDefense(unsigned int defenseIndex);
	bool selectShipFromHex(const FPoint & hex);
	bool handleHexClick(const FPoint & hex);
	bool placePlanet(const FPoint & hex);
	bool placeStation(const FPoint & hex);
	bool placeShip(const FPoint & hex);
	bool setShipPlacementHeading(int heading);
	bool setShipPlacementHeadingByHex(const FPoint & hex);
	/**
	 * @brief Enter setup placement through the generalized ordnance-placement flow.
	 *
	 * Preserves the legacy mine-placement entry point while routing setup
	 * placement through the shared source-tracked ordnance selection logic.
	 *
	 * @return True when at least one deployable source with ammo can enter
	 *         placement mode.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: May 24, 2026
	 * @date Last Modified: May 24, 2026
	 */
	bool beginMinePlacement();
	/**
	 * @brief Discover deployable mine and seeker sources and enter placement mode.
	 *
	 * Rebuilds the exact ship/weapon-slot placement-source list, selects the
	 * first source that still has ammo, and transitions tactical setup into the
	 * existing placement state.
	 *
	 * @return True when placement mode was entered successfully.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: May 24, 2026
	 * @date Last Modified: May 24, 2026
	 */
	bool beginOrdnancePlacement();
	/**
	 * @brief Select the active placement source by ship ID and weapon slot.
	 *
	 * @param shipID Source ship identifier.
	 * @param weaponIndex Zero-based weapon-slot index on the source ship.
	 *
	 * @return True when the requested source exists and becomes current.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: May 24, 2026
	 * @date Last Modified: May 24, 2026
	 */
	bool selectPlacementSource(unsigned int shipID, unsigned int weaponIndex);
	/**
	 * @brief Select the active placement source by deployable-source list index.
	 *
	 * @param sourceIndex Zero-based index into getDeployablePlacementSources().
	 *
	 * @return True when the requested list entry exists and becomes current.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: May 24, 2026
	 * @date Last Modified: May 24, 2026
	 */
	bool selectPlacementSourceByIndex(unsigned int sourceIndex);
	int getSelectedPlacementSourceIndex() const { return m_selectedPlacementSource; }
	/// get the current deployable mine/seeker placement-source list
	const std::vector<FTacticalDeploymentSource> & getDeployablePlacementSources() const { return m_deployablePlacementSources; }
	void completeMinePlacement();
	/// Canonical post-move resolution seam; PH_FINALIZE_MOVE delegates here.
	void completeMovePhase();
	FTacticalCombatReportSummary resolveCurrentFirePhase();
	void completeDefensiveFirePhase();
	void completeOffensiveFirePhase();
	void computeWeaponRange();
	bool assignTargetFromHex(const FPoint & hex);
	/**
	 * @brief Place or undo ordnance at a hex for the currently selected source.
	 *
	 * Creates a source-tracked mine record or inactive seeker when the selected
	 * source can deploy into the requested hex, or undoes the matching selected
	 * ship/weapon-slot source's placed item and restores ammo when that same
	 * source clicks an existing marker it owns. Successful placements rebuild the
	 * deployable-source list and reselect the same ship/weapon slot so the
	 * active source index stays aligned with the current ship/weapon pointers.
	 *
	 * @param hex Tactical hex to place into or undo from.
	 *
	 * @return True when model placement state changed.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: May 24, 2026
	 * @date Last Modified: May 24, 2026
	 */
	bool placeOrdnanceAtHex(const FPoint & hex);
	/**
	 * @brief Compatibility wrapper for legacy mine-placement callers.
	 *
	 * Delegates to the generalized ordnance-placement path while preserving the
	 * legacy tactical setup API used by existing wx flows and source-contract
	 * tests.
	 *
	 * @param hex Tactical hex to place into or undo from.
	 *
	 * @return True when the underlying ordnance-placement state changed.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: May 24, 2026
	 * @date Last Modified: May 24, 2026
	 */
	bool placeMineAtHex(const FPoint & hex);
	/**
	 * @brief Check whether the currently selected source may interact with a hex.
	 *
	 * Applies the generalized placement rules for the active source, including
	 * mine single-hex exclusivity and same-source undo of existing placed
	 * ordnance markers.
	 *
	 * @param hex Tactical hex to inspect.
	 *
	 * @return True when placement or selected-source undo is legal at hex.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: May 24, 2026
	 * @date Last Modified: May 24, 2026
	 */
	bool isHexDeployable(const FPoint & hex);
	/**
	 * @brief Compatibility wrapper for legacy mine-only placement checks.
	 *
	 * @param hex Tactical hex to inspect.
	 *
	 * @return True when the generalized placement rules allow mine interaction
	 *         for the current selection at hex.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: May 24, 2026
	 * @date Last Modified: May 24, 2026
	 */
	bool isHexMinable(const FPoint & hex);
	const VehicleList & getHexOccupants(const FPoint & hex) const;
	const std::vector<FPoint> & getMovementHexes() const { return m_movementHexes; }
	const std::vector<FPoint> & getLeftTurnHexes() const { return m_leftHexes; }
	const std::vector<FPoint> & getRightTurnHexes() const { return m_rightHexes; }
	const std::vector<FTacticalMovePreviewRoute> & getStoppedShipPreviewRoutes() const { return m_stoppedShipPreviewRoutes; }
	const std::vector<int> & getStoppedShipPreviewHeadingsForHex(const FPoint & hex) const;
	const PointSet & getTargetHexes() const { return m_targetHexes; }
	const PointSet & getHeadOnHexes() const { return m_headOnHexes; }
	const PointSet & getMinedHexes() const { return m_minedHexList; }
	const FHexMap & getMineTargets() const { return m_mineTargetList; }
	unsigned int getMineOwner() const { return m_mineOwner; }
	const std::map<unsigned int, FTacticalTurnData> & getTurnInfo() const { return m_turnInfo; }
	bool hasShipPlacementPendingRotation() const { return m_setRotation; }
	const FPoint & getSelectedShipHex() const { return m_shipPos; }
	const VehicleList & getShipsWithMines() const { return m_shipsWithMines; }
	/// get all source-tracked placed ordnance records
	const std::vector<FTacticalPlacedOrdnance> & getPlacedOrdnance() const { return m_placedOrdnance; }
	/// get all seeker missile model records
	const std::vector<FTacticalSeekerMissileState> & getSeekerMissiles() const { return m_seekerMissiles; }
	/**
	 * @brief Get all placed ordnance records in the requested hex.
	 *
	 * Returns a copy of the model-side placed-ordnance records whose stored
	 * placement hex matches the requested coordinates.
	 *
	 * @param hex Tactical hex to inspect for placed ordnance.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: May 24, 2026
	 * @date Last Modified: May 24, 2026
	 */
	std::vector<FTacticalPlacedOrdnance> getPlacedOrdnanceAtHex(const FPoint & hex) const;
	/**
	 * @brief Get seeker missiles in a specific hex.
	 *
	 * Returns a copy of the seeker records located in the requested hex, with
	 * optional filtering that limits the result to active seekers only.
	 *
	 * @param hex Tactical hex to inspect for seekers.
	 * @param activeOnly When true, only active seekers are returned.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: May 24, 2026
	 * @date Last Modified: May 24, 2026
	 */
	std::vector<FTacticalSeekerMissileState> getSeekerMissilesAtHex(const FPoint & hex, bool activeOnly = false) const;
	/**
	 * @brief Get inactive seeker stack hexes owned by the current moving player.
	 *
	 * Returns one entry per tactical hex containing at least one inactive seeker
	 * owned by getMovingPlayerID().
	 *
	 * @author gpt-5.4 (high)
	 * @date Created: May 25, 2026
	 * @date Last Modified: May 25, 2026
	 */
	std::vector<FPoint> getInactiveSeekerActivationHexes() const;
	/**
	 * @brief Set the selected seeker-activation stack hex for the moving player.
	 *
	 * @param hex Tactical hex to select.
	 *
	 * @return True when hex contains at least one inactive seeker for the moving
	 *         player and becomes the selected activation stack.
	 *
	 * @author gpt-5.4 (high)
	 * @date Created: May 25, 2026
	 * @date Last Modified: May 25, 2026
	 */
	bool selectSeekerActivationHex(const FPoint & hex);
	/// get the selected activation stack hex, or (-1, -1) when none is selected
	const FPoint & getSelectedSeekerActivationHex() const { return m_selectedSeekerActivationHex; }
	/**
	 * @brief Get inactive seeker missiles from the selected activation stack.
	 *
	 * @author gpt-5.4 (high)
	 * @date Created: May 25, 2026
	 * @date Last Modified: May 25, 2026
	 */
	std::vector<FTacticalSeekerMissileState> getSelectedInactiveSeekerActivationStack() const;
	/**
	 * @brief Activate one seeker in the selected activation stack.
	 *
	 * Activation is one-way: only currently inactive seekers can transition to
	 * active state, and already-active seekers are never deactivated here.
	 *
	 * @param seekerID Unique model seeker identifier to activate.
	 *
	 * @return True when the requested inactive owned seeker was activated.
	 *
	 * @author gpt-5.4 (high)
	 * @date Created: May 25, 2026
	 * @date Last Modified: May 25, 2026
	 */
	bool activateSelectedInactiveSeeker(unsigned int seekerID);
	/**
	 * @brief Get seeker missiles owned by a specific player.
	 *
	 * Returns a copy of the seeker records for the requested owner, with
	 * optional filtering that limits the result to active seekers only.
	 *
	 * @param ownerID Player identifier to match.
	 * @param activeOnly When true, only active seekers are returned.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: May 24, 2026
	 * @date Last Modified: May 24, 2026
	 */
	std::vector<FTacticalSeekerMissileState> getSeekerMissilesForOwner(unsigned int ownerID, bool activeOnly = false) const;
	bool isHexInBounds(const FPoint & hex) const;
	bool isHexOccupied(const FPoint & hex) const;

void beginTacticalReport(const FTacticalCombatReportContext & context);
void appendTacticalAttackReport(const FTacticalAttackReport & attack);
void appendTacticalReportEvent(const FTacticalReportEvent & event);
FTacticalCombatReportSummary buildCurrentTacticalReportSummary() const;
void clearTacticalReport();
const FTacticalCombatReport & getCurrentTacticalReport() const { return m_tacticalReport; }

void applyFireDamage();
void fireICM();
/**
 * @brief Resolve current side weapon fire and capture destroyed-ship bookkeeping.
 *
 * Milestone 8 remediation contract:
 * - Model owns fire resolution and destroyed-ship ID capture.
 * - wx callers may read getLastDestroyedShipIDs() after this method returns.
 * - wx-side cleanup must consume IDs before clearLastDestroyedShipIDs() is called.
 */
FTacticalCombatReportSummary fireAllWeapons();
/**
 * @brief Remove destroyed ships from model-owned tactical state and capture IDs.
 *
 * This method is the model-side owner of destroyed-ship ID capture in
 * m_lastDestroyedShipIDs. It should run during fire resolution before wx cleanup.
 */
int clearDestroyedShips();
const std::vector<unsigned int> & getLastDestroyedShipIDs() const { return m_lastDestroyedShipIDs; }
/**
 * @brief Clear destroyed-ship bookkeeping after wx cleanup consumes IDs.
 *
 * Canonical clear point for m_lastDestroyedShipIDs in the delegated fire flow.
 */
void clearLastDestroyedShipIDs();
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

VehicleList * findHexOccupantsForShip(unsigned int shipID);
const VehicleList * findHexOccupantsForShip(unsigned int shipID) const;
	void removeShipFromHexOccupancy(unsigned int shipID);
	void removeShipFromTurnInfo(unsigned int shipID);
	void removeShipFromModelState(unsigned int shipID);
	bool hasUsableICMDefenderInHex(const VehicleList & vehicles, unsigned int defendingSideID) const;
	FVehicle * pickShip(const FVehicle * selected, const VehicleList & ships) const;
	FVehicle * pickTarget(const FVehicle * selected, const VehicleList & ships) const;
	bool findHexInList(PointList list, FPoint ref, int & count) const;
	void setInitialRoute();
	bool handleMoveHexSelection(const FPoint & hex);
	void computeRemainingMoves(FPoint start);
	void computePath(PointList & list, FPoint hex, int heading);
	int turnShip(int heading, int turn) const;
	int forceTurn(FVehicle * ship, int curHeading, FPoint current);
	void checkForPlanetCollision(FPoint & currentHex, int & currentHeading);
	int getPlanetTurnDirection(FPoint currentHex, int currentHeading) const;
	void checkMoveStatus();
	void checkForMines(FVehicle * ship);
	void applyMineDamage();
	void clearStoppedShipPreviewRoutes();
	void rebuildStoppedShipPreviewRoutes();
	/**
	 * @brief Rebuild the deployable mine/seeker placement-source list.
	 *
	 * Refreshes the exact ship/weapon-slot entries after ammo changes and keeps
	 * the current selection aligned with the same source when that slot remains
	 * deployable.
	 *
	 * @author gpt-5.4 (high)
	 * @date Created: May 24, 2026
	 * @date Last Modified: May 24, 2026
	 */
	void rebuildDeployablePlacementSources();
	bool isDeployableWeapon(const FWeapon * weapon) const;
	bool sourceMatchesWeapon(const FTacticalOrdnanceSource & source, const FVehicle * ship, const FWeapon * weapon, int weaponIndex) const;
	/**
	 * @brief Check whether stored source provenance matches the active selection.
	 *
	 * Resolves the currently selected ship/weapon-slot identity before comparing
	 * against stored ordnance provenance so undo restores ammo only to the exact
	 * selected launcher slot, even if pointer identity has been rebuilt.
	 *
	 * @param source Stored placed-ordnance source metadata to compare.
	 *
	 * @return True when the active selection refers to the same source slot.
	 *
	 * @author gpt-5.4 (high)
	 * @date Created: May 24, 2026
	 * @date Last Modified: May 24, 2026
	 */
	bool sourceMatchesSelection(const FTacticalOrdnanceSource & source) const;
	FVehicle * findShipByID(unsigned int shipID) const;
	FWeapon * findWeaponBySource(const FTacticalOrdnanceSource & source, FVehicle ** ship = NULL) const;
	void beginSeekerActivationPhase();
	void beginMovePhase();
	void resolveActiveSeekersForMovingPlayer();
	bool buildSelectedPlacementSource(FTacticalDeploymentSource & source) const;
	void appendPlacedOrdnanceRecord(FWeapon::Weapon weaponType, const FPoint & hex, const FTacticalOrdnanceSource & source);
	bool removePlacedOrdnanceForSelection(const FPoint & hex, FTacticalPlacedOrdnance & removed);
	void removePlacedMineRecordsAtHex(const FPoint & hex);
	bool placeMineFromSelection(const FPoint & hex, const FTacticalDeploymentSource & selectedSource);
	bool placeSeekerFromSelection(const FPoint & hex, const FTacticalDeploymentSource & selectedSource);
	bool restoreAmmoForSource(const FTacticalOrdnanceSource & source);
	unsigned int nextSeekerID() const;
	void computeFFRange(const FPoint & pos, PointSet & targetHexes, PointSet & headOnHexes, int heading = -1) const;
	void computeBatteryRange(const FPoint & pos, PointSet & targetHexes) const;
	bool setIfValidTarget(FVehicle * target, const FPoint & targetHex);
	VehicleList * getShipList(FVehicle * ship);
	const VehicleList * getShipList(FVehicle * ship) const;

/// Legacy FBattleScreen battle state
int m_state;
int m_phase;
bool m_control;
bool m_activePlayer;
bool m_movingPlayer;
bool m_moveComplete;
bool m_done;
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

/// ICM and report lifecycle state (legacy fire-flow owners were FBattleScreen + FBattleDisplay)
std::vector<ICMData *> m_ICMData;
FTacticalCombatReport m_tacticalReport;
std::vector<unsigned int> m_lastDestroyedShipIDs;
/// True when destroyed-ship IDs have been consumed by wx cleanup and can be reset by model capture.
bool m_lastDestroyedShipIDsConsumed;

/// Tactical hex-map mechanics state (legacy FBattleBoard model data)
FTacticalHexData m_hexData[100][100];
	FPoint m_shipPos;
	bool m_setRotation;
	std::vector<FPoint> m_movementHexes;
	std::vector<FPoint> m_leftHexes;
	std::vector<FPoint> m_rightHexes;
	std::vector<FTacticalMovePreviewRoute> m_stoppedShipPreviewRoutes;
	std::map<FPoint, std::vector<int> > m_stoppedShipPreviewHeadingsByHex;
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
	VehicleList m_shipsWithMines;
	std::vector<FTacticalDeploymentSource> m_deployablePlacementSources;
	int m_selectedPlacementSource;
	std::vector<FTacticalPlacedOrdnance> m_placedOrdnance;
	std::vector<FTacticalSeekerMissileState> m_seekerMissiles;
	FPoint m_selectedSeekerActivationHex;
};

}

#endif //_FTACTICALGAME_H_
