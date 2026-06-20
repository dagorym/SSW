/**
 * @file FTacticalGame.h
 * @brief Header file for FTacticalGame class
 * @author Tom Stephens, gpt-5.4 (high), gpt-5.3-codex (standard), claude-sonnet-4-6 (medium), claude-sonnet-4-6 (standard)
 * @date Created:  Mar 29, 2026
 * @date Last Modified: Jun 19, 2026
 *
 * SMFR-03: Added m_lastTriggeredMineHexes, getLastTriggeredMineHexes(),
 * and clearLastTriggeredMineHexes() to expose triggered minefield hexes
 * for board highlight while the mine damage summary dialog is shown.
 * SMFR-05: Added clearNonImpactingSeekerMovementPaths() called from
 * completeMovePhase() after applyMovementSeekerDamage() so non-impacting
 * seeker displayed paths clear with ship movement routes.
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
 * The `movementPath` field is render-supporting state: it records the
 * stepped path traversed by this seeker during the most recent movement
 * resolution (start hex through final hex), populated by
 * `resolveActiveSeekersForMovingPlayer` and cleared at the start of each
 * resolution so it reflects only the current move. It is not persisted and
 * carries no wx types.
 *
 * The `activationPhaseIndex` field is stamped at activation time with the
 * current `m_seekerActivationPhaseIndex` counter value. It allows
 * `getActiveSeekersByMovingPlayerThisPhase()` to distinguish seekers activated
 * in the current phase from those activated in earlier phases of the same turn.
 * The field is not persisted.
 *
 * @author Tom Stephens, gpt-5.4 (high), claude-sonnet-4-6 (standard)
 * @date Created: May 24, 2026
 * @date Last Modified: Jun 02, 2026
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
/// stepped path from start through final hex for the most recent movement resolution
std::vector<FPoint> movementPath;
/// activation-phase index stamped when this seeker was activated (not persisted)
int activationPhaseIndex;
} FTacticalSeekerMissileState;

/**
 * @brief Model-owned seeker contact outcome captured during activation resolve.
 *
 * Stores the minimal non-wx seam data needed for follow-on damage/ICM/report
 * integration work after the moving player's seeker-activation completion
 * detects either an immediate same-hex contact or a later movement-step
 * contact.
 *
 * @author gpt-5.4 (high)
 * @date Created: May 27, 2026
 * @date Last Modified: May 27, 2026
 */
typedef struct {
	unsigned int seekerID;
	unsigned int seekerOwnerID;
	unsigned int targetShipID;
	unsigned int targetOwnerID;
	FPoint contactHex;
	bool preMovementContact;
	unsigned int movementStep;
	int movementTurn;
} FTacticalSeekerContactOutcome;

/**
 * @brief Lightweight seeker-target snapshot for deterministic helper tests.
 *
 * Captures only the model attributes needed by the seeker closest-target
 * helper seam so behavioral tests can validate filtering and closest-distance
 * ties deterministically without constructing full tactical occupancy state or
 * wx runtime objects.
 *
 * @author gpt-5.3-codex (standard)
 * @date Created: May 27, 2026
 * @date Last Modified: May 27, 2026
 */
typedef struct {
	unsigned int shipID;
	FPoint hex;
	int hp;
	bool isStation;
} FTacticalSeekerTargetSnapshot;

/**
 * @brief One pending offensive-fire seeker deployment tied to a launcher slot.
 *
 * Tracks the seeker instance and source launcher for undo/recall during the
 * current offensive-fire phase only.
 *
 * @author gpt-5.4 (high)
 * @date Created: May 25, 2026
 * @date Last Modified: May 25, 2026
 */
typedef struct {
	unsigned int seekerID;
	FTacticalOrdnanceSource source;
	FPoint hex;
	int offensiveFirePhaseID;
} FTacticalPendingSeekerDeployment;

/**
 * @brief Grouped pending-seeker row data for the selected launcher.
 *
 * Each record represents one lower-panel recall row, combining all pending
 * seeker deployments from the current offensive-fire phase that share the same
 * legal path hex for the selected launcher.
 *
 * @author gpt-5.4 (high)
 * @date Created: May 25, 2026
 * @date Last Modified: May 25, 2026
 */
typedef struct {
	FPoint hex;
	unsigned int count;
} FTacticalPendingSeekerHexGroup;

/**
 * @brief Pure C++ tactical mechanics state container.
 *
 * Owns non-wx tactical battle state, including ship placement, movement,
 * combat-report bookkeeping, and lightweight source-tracked ordnance,
 * seeker-missile records, deterministic seeker helper seams used by
 * behavioral tests, and pending offensive-fire deployment state used by the
 * wx rendering and interaction layers.
 *
 * @author Tom Stephens, gpt-5.4 (high), gpt-5.3-codex (standard), claude-sonnet-4-6 (medium)
 * @date Created: Mar 29, 2026
 * @date Last Modified: May 28, 2026
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

/**
 * @brief Transition the tactical model into a new combat phase.
 *
 * Requests for `PH_MOVE` route through the additive seeker-activation entry
 * logic so inactive seeker stacks can be surfaced for the moving player before
 * normal movement begins. When no inactive stacks exist, the model still runs
 * the active-seeker resolution seam and then enters `PH_MOVE` automatically.
 *
 * @param p Requested tactical phase constant.
 *
 * @author Tom Stephens, gpt-5.4 (high)
 * @date Created: Mar 29, 2026
 * @date Last Modified: May 25, 2026
 */
void setPhase(int p);
int getPhase() const { return m_phase; }
/**
 * @brief Finish seeker activation and enter normal movement.
 *
 * Resolves only the moving player's active seekers, recording any same-hex or
 * movement-step contacts in the pending seeker-contact seam, removing seekers
 * that contact or expire on their 12-hex movement turn, and, when an
 * `ITacticalUI` is installed, resolves those pending contacts into the
 * immediate seeker-damage summary before destroyed-ship cleanup is handed back
 * to wx callers. The method then clears the selected activation stack and
 * advances into the ordinary movement-phase entry path.
 *
 * @author Tom Stephens, gpt-5.4 (high)
 * @date Created: May 25, 2026
 * @date Last Modified: May 27, 2026
 */
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
	/**
	 * @brief Dispatch a board hex click to the appropriate model action for the current state.
	 *
	 * Routes clicks to ship/planet/station placement, mine placement, seeker placement, or
	 * battle-phase actions depending on the current battle state. During BS_PlaceMines, M weapon
	 * clicks route to placeMineAtHex() and SM weapon clicks route to placeOrdnanceAtHex().
	 * During BS_PlaceSeekers, SM weapon clicks route to placeOrdnanceAtHex(). Hex clicks for
	 * out-of-bounds hexes or states with no valid action return false immediately.
	 *
	 * @param hex Tactical hex that was clicked.
	 *
	 * @return True when the click produced a model state change.
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 19, 2026
	 * @date Last Modified: Jun 19, 2026
	 */
	bool handleHexClick(const FPoint & hex);
	bool placePlanet(const FPoint & hex);
	bool placeStation(const FPoint & hex);
	bool placeShip(const FPoint & hex);
	bool setShipPlacementHeading(int heading);
	bool setShipPlacementHeadingByHex(const FPoint & hex);
	/**
	 * @brief Enter the mine-only deployment setup phase (BS_PlaceMines).
	 *
	 * Preserves the legacy mine-placement entry point by delegating to
	 * beginOrdnancePlacement(), which filters the deployable source list to
	 * FWeapon::M slots only before entering BS_PlaceMines.
	 *
	 * @return True when mine placement mode was entered successfully.
	 *
	 * @author Tom Stephens, gpt-5.4 (high), claude-sonnet-4-6 (medium)
	 * @date Created: May 24, 2026
	 * @date Last Modified: Jun 02, 2026
	 */
	bool beginMinePlacement();
	/**
	 * @brief Enter the seeker-only deployment setup phase (BS_PlaceSeekers).
	 *
	 * Rebuilds the deployable source list filtered to FWeapon::SM slots only.
	 * When at least one seeker source with ammo exists, selects the first one
	 * and transitions to BS_PlaceSeekers. Returns false and does not change
	 * state when no seeker sources with ammo are found.
	 *
	 * @return True when seeker placement mode was entered successfully.
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 02, 2026
	 * @date Last Modified: Jun 02, 2026
	 */
	bool beginSeekerPlacement();
	/**
	 * @brief Discover deployable mine sources and enter mine-placement mode (BS_PlaceMines).
	 *
	 * Rebuilds the full ship/weapon-slot deployment list, then filters it to
	 * FWeapon::M sources only so BS_PlaceMines exposes only mine slots to the
	 * UI. Selects the first mine source with ammo and transitions to BS_PlaceMines.
	 * Seeker (SM) sources are deferred to the seeker phase via beginSeekerPlacement().
	 *
	 * @return True when mine placement mode was entered successfully.
	 *
	 * @author Tom Stephens, gpt-5.4 (high), claude-sonnet-4-6 (medium)
	 * @date Created: May 24, 2026
	 * @date Last Modified: Jun 02, 2026
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
	/**
	 * @brief Complete mine placement and advance to the seeker phase (or skip it).
	 *
	 * Attempts to enter seeker placement (BS_PlaceSeekers) via beginSeekerPlacement().
	 * When no seeker sources with ammo exist, advances directly to BS_SetupAttackFleet.
	 * In either case resets the current ship/weapon selection.
	 *
	 * @author Tom Stephens, gpt-5.4 (high), claude-sonnet-4-6 (medium)
	 * @date Created: May 24, 2026
	 * @date Last Modified: Jun 02, 2026
	 */
	void completeMinePlacement();
	/**
	 * @brief Complete seeker placement and advance to attacker setup.
	 *
	 * Transitions the model to BS_SetupAttackFleet, toggles the active player,
	 * and resets the current ship and weapon selection.
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 02, 2026
	 * @date Last Modified: Jun 02, 2026
	 */
	void completeSeekerPlacement();
	/**
	 * @brief Canonical post-move resolution seam; PH_FINALIZE_MOVE delegates here.
	 *
	 * Finalizes ship positions, checks for active seeker contacts and mine
	 * interactions, applies movement seeker damage (removing impacting seekers and
	 * their paths via applyMovementSeekerDamage()), then clears the displayed
	 * movement path on all surviving non-impacting seekers via
	 * clearNonImpactingSeekerMovementPaths() so paths disappear together with ship
	 * movement routes (SMFR-05). The seekers' movementAllowance and movementTurn
	 * bookkeeping are preserved for subsequent turns.
	 *
	 * @author Tom Stephens, claude-sonnet-4-6 (medium)
	 * @date Created: Mar 29, 2026
	 * @date Last Modified: Jun 19, 2026
	 */
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
	 * source clicks an existing marker it owns. Successful placements and undos
	 * rebuild the deployable-source list using a type-filtered rebuild when the
	 * current state is BS_PlaceMines (M-only filter) or BS_PlaceSeekers (SM-only
	 * filter), and use the unfiltered rebuild for all other states. The source
	 * is then reselected so the active source index stays aligned with the current
	 * ship/weapon pointers.
	 *
	 * @param hex Tactical hex to place into or undo from.
	 *
	 * @return True when model placement state changed.
	 *
	 * @author Tom Stephens, gpt-5.4 (high), claude-sonnet-4-6 (medium)
	 * @date Created: May 24, 2026
	 * @date Last Modified: Jun 19, 2026
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
	/// SMFR-03: get triggered mine hexes pending highlight while damage summary is shown
	const PointSet & getLastTriggeredMineHexes() const { return m_lastTriggeredMineHexes; }
	/// SMFR-03: clear triggered mine hexes after the damage summary dialog closes
	void clearLastTriggeredMineHexes() { m_lastTriggeredMineHexes.clear(); }
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
	 * Returns only inactive seeker records owned by the current moving player for
	 * the currently selected activation hex.
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
	 * @brief Deactivate one active seeker by ID for the moving player.
	 *
	 * Deactivation is the inverse of activateSelectedInactiveSeeker; transitions an
	 * active seeker owned by the moving player to inactive state.
	 *
	 * @param seekerID Unique model seeker identifier to deactivate.
	 *
	 * @return True when an active, moving-player-owned seeker with that ID was
	 *         found and flipped to inactive.
	 *
	 * @author Claude Haiku 4.5 (standard)
	 * @date Created: May 30, 2026
	 * @date Last Modified: May 30, 2026
	 */
	bool deactivateActiveSeekerByID(unsigned int seekerID);
	/**
	 * @brief Activate exactly one inactive seeker at a given hex.
	 *
	 * Selects and activates exactly one inactive seeker located at the requested hex
	 * owned by the moving player. When multiple inactive seekers share the hex,
	 * chooses deterministically (lowest seekerID).
	 *
	 * @param hex Tactical hex where the seeker to activate is located.
	 *
	 * @return True when an inactive, moving-player-owned seeker at that hex was
	 *         found and activated.
	 *
	 * @author Claude Haiku 4.5 (standard)
	 * @date Created: May 30, 2026
	 * @date Last Modified: May 30, 2026
	 */
	bool activateInactiveSeekerAtHex(const FPoint & hex);
	/**
	 * @brief Get all active seekers owned by the moving player.
	 *
	 * Returns a copy of all active seeker records owned by the current moving player
	 * across all hexes, suitable for UI listing and navigation.
	 *
	 * @return Vector of active seeker missile state records for the moving player.
	 *
	 * @author Claude Haiku 4.5 (standard)
	 * @date Created: May 30, 2026
	 * @date Last Modified: May 30, 2026
	 */
	std::vector<FTacticalSeekerMissileState> getActiveSeekersByMovingPlayer() const;
	/**
	 * @brief Get active seekers owned by the moving player that were activated in the current activation phase.
	 *
	 * Returns only seekers whose activationPhaseIndex matches the current
	 * m_seekerActivationPhaseIndex. Used by the UI to show the changeable list —
	 * seekers from prior phases are excluded. Leave getActiveSeekersByMovingPlayer()
	 * unchanged for driving actual movement.
	 *
	 * @return Vector of active seeker missile state records activated this phase.
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 02, 2026
	 * @date Last Modified: Jun 02, 2026
	 */
	std::vector<FTacticalSeekerMissileState> getActiveSeekersByMovingPlayerThisPhase() const;
	/**
	 * @brief Report whether the current selection is offensive-fire seeker deployment.
	 *
	 * This mode is available only while the moving player has selected one of its
	 * own undamaged `SM` launchers during `PH_ATTACK_FIRE`. In that state the
	 * board highlights legal deployment hexes along the current-turn path instead
	 * of normal target-selection ranges.
	 *
	 * @return True when board clicks should place or recall pending offensive-fire
	 *         seekers for the selected launcher.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: May 25, 2026
	 * @date Last Modified: May 25, 2026
	 */
	bool isOffensiveSeekerDeploymentMode() const;
	/**
	 * @brief Group pending offensive-fire seeker deployments by hex for the selected launcher.
	 *
	 * Returns only current-phase pending seekers owned by the selected `SM`
	 * launcher. Multiple pending seekers stacked in the same legal path hex are
	 * combined into one row with a count so the lower panel can render explicit
	 * recall actions without treating same-hex board clicks as undo.
	 *
	 * @return Grouped pending deployment rows for the currently selected launcher.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: May 25, 2026
	 * @date Last Modified: May 25, 2026
	 */
	std::vector<FTacticalPendingSeekerHexGroup> getSelectedOffensivePendingSeekerHexGroups() const;
	/**
	 * @brief Recall one pending seeker from the selected launcher's grouped row.
	 *
	 * Removes exactly one current-phase pending seeker from the requested hex,
	 * restores one ammo to that launcher, and leaves any earlier-turn or
	 * pre-battle seekers in the same hex untouched.
	 *
	 * @param hex Legal-path hex represented by the clicked recall row.
	 *
	 * @return True when one pending seeker was removed from that grouped row.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: May 25, 2026
	 * @date Last Modified: May 25, 2026
	 */
	bool recallSelectedOffensivePendingSeekerAtHex(const FPoint & hex);
	/**
	 * @brief Get all unique hexes that hold any pending offensive-fire seeker
	 *        for the current phase, regardless of which launcher deployed them.
	 *
	 * Used by the board renderer during `PH_ATTACK_FIRE` to draw the seeker
	 * icon on every hex that contains at least one current-phase pending
	 * deployment. Returns an empty vector outside `PH_ATTACK_FIRE` or when no
	 * pending deployments exist.
	 *
	 * @return Unique hex positions of all current-phase pending seeker deployments.
	 *
	 * @author claude-sonnet-4-6 (standard)
	 * @date Created: May 30, 2026
	 * @date Last Modified: May 30, 2026
	 */
	std::vector<FPoint> getAllPendingOffensiveFireSeekerHexes() const;
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
	/**
	 * @brief Get seeker contact outcomes captured by the last activation resolve.
	 *
	 * Returns the moving player's pending same-hex and movement-step contact
	 * outcomes produced by the most recent active-seeker resolution pass.
	 *
	 * @author gpt-5.4 (high)
	 * @date Created: May 27, 2026
	 * @date Last Modified: May 27, 2026
	 */
	const std::vector<FTacticalSeekerContactOutcome> & getPendingSeekerContactOutcomes() const { return m_pendingSeekerContactOutcomes; }
	/**
	 * @brief Clear pending seeker contact outcomes after downstream resolution.
	 *
	 * @author gpt-5.4 (high)
	 * @date Created: May 27, 2026
	 * @date Last Modified: May 27, 2026
	 */
	void clearPendingSeekerContactOutcomes();
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
	/**
	 * @brief Check whether a moving ship's path enters any active-seeker hex.
	 *
	 * Scans the ship's current-turn path for hexes that contain at least one
	 * active seeker owned by the opposing side, appending a
	 * `FTacticalSeekerContactOutcome` for the first qualifying active seeker
	 * encountered in each such hex. Inactive seekers are ignored entirely so
	 * they cannot trigger movement contact.
	 *
	 * @param ship Moving ship whose finalized turn path is being checked.
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: May 28, 2026
	 * @date Last Modified: May 28, 2026
	 */
	void checkForActiveSeekersOnPath(FVehicle * ship);
	/**
	 * @brief Resolve all pending seeker contacts gathered during movement finalization.
	 *
	 * Collects detonating seeker IDs from `m_pendingSeekerContactOutcomes`, then
	 * calls `requestRedraw()` on the UI and `resolvePendingSeekerDetonationDamage()`
	 * when an `ITacticalUI` is installed so ship-triggered seeker contacts use
	 * exactly the same SM-weapon fire, ICM allocation, immediate `TRT_SeekerDamage`
	 * report, and post-summary destroyed-ship cleanup path used for
	 * activation-phase contacts. The redraw before dialogs ensures the impacting
	 * seeker is visible at its final hex while the ICM/damage dialog is displayed
	 * (SMF-06). When no UI is installed, clears the pending outcomes directly
	 * without damage resolution. After resolution, removes each detonated seeker
	 * from `m_seekerMissiles` exactly once. The caller (`completeMovePhase()`) is
	 * responsible for clearing any pre-move leftover outcomes before building the
	 * per-ship path contact list that this method consumes.
	 * Must be called after all ships in the moving player's list have had their
	 * paths checked via `checkForActiveSeekersOnPath` and before `applyMineDamage()`
	 * executes.
	 *
	 * @author claude-sonnet-4-6 (medium), claude-sonnet-4-6 (medium)
	 * @date Created: May 28, 2026
	 * @date Last Modified: Jun 02, 2026 (SMF-06: requestRedraw before dialogs)
	 */
	void applyMovementSeekerDamage();
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
	/**
	 * @brief Rebuild the deployable placement-source list filtered to one weapon type.
	 *
	 * Like rebuildDeployablePlacementSources() but only includes sources whose
	 * weaponType matches the requested filter. Used to populate mine-only (M) or
	 * seeker-only (SM) phase source lists.
	 *
	 * @param filter Weapon type to keep; all other deployable weapon types are excluded.
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 02, 2026
	 * @date Last Modified: Jun 02, 2026
	 */
	void rebuildDeployablePlacementSourcesFiltered(FWeapon::Weapon filter);
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
	/**
	 * @brief Enter the pre-movement seeker activation phase when needed.
	 *
	 * Selects the first inactive seeker stack for the moving player when one is
	 * available, or auto-skips directly into the normal movement entry path after
	 * resolving the active-seeker seam and pending detonation damage when no
	 * inactive stacks remain.
	 *
	 * @author gpt-5.4 (high)
	 * @date Created: May 25, 2026
	 * @date Last Modified: May 27, 2026
	 */
	void beginSeekerActivationPhase();
	/**
	 * @brief Enter the normal movement phase for the current moving player.
	 *
	 * Applies end-of-fire cleanup, updates the active-player toggle, and resets
	 * movement-state bookkeeping after seeker activation has completed or been
	 * auto-skipped.
	 *
	 * @author gpt-5.4 (high)
	 * @date Created: May 25, 2026
	 * @date Last Modified: May 25, 2026
	 */
	void beginMovePhase();
	/**
	 * @brief Resolve active seeker targeting and movement before movement phase.
	 *
	 * Advances only the moving player's active seekers by first checking
	 * same-hex contact, then selecting the closest non-station ship target
	 * across both sides, applying random tie-breaking only after the
	 * deterministic closest-target helper narrows the candidate set, adjusting
	 * initial facing up to three hexsides toward the chosen target, and moving
	 * with greedy one-hexside turn limits until contact occurs or the current
	 * allowance is exhausted. Seekers that expire (12-hex allowance exhausted
	 * without contact) are dropped. All other seekers remain in `m_seekerMissiles`.
	 *
	 * Impacting seekers are kept in `m_seekerMissiles` (SMF-06) with their
	 * `movementPath` intact so they remain renderable during ICM/damage dialogs.
	 * `applyMovementSeekerDamage()` removes them after the damage summary returns.
	 *
	 * For each moving seeker, the method clears and then populates
	 * `FTacticalSeekerMissileState::movementPath` with the start hex followed by
	 * each step hex taken during this resolution pass. Non-moving seekers (inactive
	 * or belonging to the non-moving player) have their `movementPath` cleared so
	 * stale paths do not persist across turns. This render-supporting path state is
	 * read by `FBattleBoard::drawSeekerPaths()` during `PH_MOVE` and
	 * `PH_SEEKER_ACTIVATION` to draw cyan path lines on the board.
	 *
	 * @author gpt-5.4 (high), gpt-5.3-codex (standard), claude-sonnet-4-6 (standard), claude-sonnet-4-6 (medium)
	 * @date Created: May 25, 2026
	 * @date Last Modified: Jun 02, 2026 (SMF-06: impacting seekers stay until after damage summary)
	 */
	void resolveActiveSeekersForMovingPlayer();
	/**
	 * @brief Select one valid contact ship currently occupying a seeker hex.
	 *
	 * Filters the requested hex occupancy to live non-station ships, chooses the
	 * highest-max-HP target, and applies random tie-breaking only among
	 * same-max-HP candidates.
	 *
	 * @param seeker Seeker attempting contact resolution.
	 * @param hex Tactical hex being evaluated for immediate contact.
	 *
	 * @return One valid ship in that hex, or `NULL` when none qualify.
	 *
	 * @author gpt-5.4 (high)
	 * @date Created: May 27, 2026
	 * @date Last Modified: May 27, 2026
	 */
	FVehicle * selectSeekerContactTargetAtHex(const FTacticalSeekerMissileState & seeker, const FPoint & hex) const;
	/**
	 * @brief Resolve pending seeker contact outcomes into immediate damage reports.
	 *
	 * Converts each pending seeker contact to a temporary `FWeapon::SM` attack,
	 * runs the existing ICM-allocation seam, appends all fired attacks to one
	 * immediate seeker-damage report, shows the report summary through
	 * `ITacticalUI`, and only then captures destroyed-ship bookkeeping for
	 * wx-side cleanup so the summary remains visible before removal.
	 *
	 * @author gpt-5.4 (high)
	 * @date Created: May 27, 2026
	 * @date Last Modified: May 27, 2026
	 */
	void resolvePendingSeekerDetonationDamage();
	/**
	 * @brief Append one model-owned seeker contact outcome to the pending seam.
	 *
	 * @param seeker Contacting seeker state.
	 * @param target Ship chosen for contact.
	 * @param preMovementContact True when contact happened before movement steps.
	 * @param movementStep One-based step index for movement contact, zero otherwise.
	 *
	 * @author gpt-5.4 (high)
	 * @date Created: May 27, 2026
	 * @date Last Modified: May 27, 2026
	 */
	void appendSeekerContactOutcome(
		const FTacticalSeekerMissileState & seeker,
		const FVehicle * target,
		bool preMovementContact,
		unsigned int movementStep);
	/**
	 * @brief Map seeker activation turn count to the capped movement allowance.
	 *
	 * Converts the 1-based seeker movement turn counter into the tactical rules
	 * helper allowance sequence `2, 4, 6, 8, 10, 12`, clamping higher turns to
	 * the final 12-hex allowance and returning zero for inactive or invalid turn
	 * counts so live movement and deterministic helper coverage share one rule.
	 *
	 * @param movementTurn One-based seeker movement turn counter.
	 *
	 * @return The movement allowance to use for the helper step.
	 *
	 * @author gpt-5.4 (high), gpt-5.3-codex (standard)
	 * @date Created: May 25, 2026
	 * @date Last Modified: May 27, 2026
	 */
	unsigned int computeSeekerMovementAllowance(int movementTurn) const;
	/**
	 * @brief Choose one seeker-target candidate index with the existing RNG.
	 *
	 * Keeps seeker tie-breaking model-side and wx-free by narrowing the random
	 * choice to a single zero-based index within the provided candidate set.
	 *
	 * @param candidateCount Number of tied candidates available.
	 *
	 * @return Zero-based winner index, or zero when fewer than two candidates exist.
	 *
	 * @author gpt-5.4 (high), gpt-5.3-codex (standard)
	 * @date Created: May 25, 2026
	 * @date Last Modified: May 25, 2026
	 */
	unsigned int chooseRandomSeekerIndex(unsigned int candidateCount) const;
	/**
	 * @brief Locate the tactical hex currently occupied by a specific ship.
	 *
	 * Scans the model-owned hex occupancy grid so seeker targeting helpers can
	 * stay within `FTacticalGame` and avoid any wx-owned renderer state.
	 *
	 * @param shipID Model identifier for the ship to locate.
	 * @param hex Output hex set when the ship is found.
	 *
	 * @return True when the ship currently occupies a tactical hex.
	 *
	 * @author gpt-5.4 (high), gpt-5.3-codex (standard)
	 * @date Created: May 25, 2026
	 * @date Last Modified: May 25, 2026
	 */
	bool findShipHex(unsigned int shipID, FPoint & hex) const;
	/**
	 * @brief Validate whether a ship can be targeted by seeker helper logic.
	 *
	 * The helper currently accepts only live, non-station ships. This keeps
	 * planets, stations, destroyed ships, and null pointers out of the seeker
	 * closest-target calculation.
	 *
	 * @param ship Candidate ship pointer.
	 *
	 * @return True when the candidate is a live non-station ship.
	 *
	 * @author gpt-5.4 (high), gpt-5.3-codex (standard)
	 * @date Created: May 25, 2026
	 * @date Last Modified: May 25, 2026
	 */
	bool isValidSeekerTarget(const FVehicle * ship) const;
	/**
	 * @brief Select the closest valid ship target for one active seeker.
	 *
	 * Evaluates candidate ships from both sides using hex distance from the
	 * seeker's current position, then applies the narrow random-index helper to
	 * break equal-distance ties without pulling in any UI dependencies after the
	 * deterministic closest-target helper rules have been applied.
	 *
	 * @param seeker Active seeker whose target is being chosen.
	 * @param candidates Combined ship-candidate list to inspect.
	 *
	 * @return Pointer to the chosen target ship, or `NULL` when none qualify.
	 *
	 * @author gpt-5.4 (high), gpt-5.3-codex (standard)
	 * @date Created: May 25, 2026
	 * @date Last Modified: May 27, 2026
	 */
	FVehicle * selectClosestSeekerTarget(const FTacticalSeekerMissileState & seeker, const VehicleList & candidates) const;
	/**
	 * @brief Rotate a seeker's facing toward the selected target before movement.
	 *
	 * Applies the model-side pre-move turn rule by taking the shortest wrapped
	 * heading delta toward the target hex and clamping that adjustment to at most
	 * three hexsides.
	 *
	 * @param seeker Active seeker state to update in place.
	 * @param targetHex Current tactical hex of the chosen target.
	 *
	 * @author gpt-5.4 (high), gpt-5.3-codex (standard)
	 * @date Created: May 25, 2026
	 * @date Last Modified: May 25, 2026
	 */
	void adjustSeekerHeadingTowardTarget(FTacticalSeekerMissileState & seeker, const FPoint & targetHex) const;
	/**
	 * @brief Move a seeker toward its chosen target with greedy one-step turns.
	 *
	 * For each point of movement allowance, the helper evaluates straight,
	 * left-turn, and right-turn entries, chooses the candidate hex that most
	 * reduces distance to the target, and prefers the smallest turn magnitude on
	 * equal-distance results by repeatedly applying the same one-step rule
	 * exposed through computeSeekerGreedyNextStep(...).
	 *
	 * @param seeker Active seeker state to update in place.
	 * @param targetHex Current tactical hex of the chosen target.
	 *
	 * @author gpt-5.4 (high), gpt-5.3-codex (standard)
	 * @date Created: May 25, 2026
	 * @date Last Modified: May 27, 2026
	 */
	void moveSeekerTowardTarget(FTacticalSeekerMissileState & seeker, const FPoint & targetHex) const;
	/**
	 * @brief Collect highest-max-HP valid seeker contact candidates.
	 *
	 * Filters the provided ship list to live non-station targets and returns all
	 * ships tied for the largest `getMaxHP()` value. This isolates deterministic
	 * contact filtering from random tie-breaking so behavioral tests can validate
	 * mixed-size same-hex target stacks without relying on RNG outcomes.
	 *
	 * @param candidates Candidate ships sharing a contact hex.
	 *
	 * @return Valid ships tied at the highest max HP (empty when none qualify).
	 *
	 * @author gpt-5.4 (high)
	 * @date Created: May 27, 2026
	 * @date Last Modified: May 27, 2026
	 */
	VehicleList collectHighestMaxHPSeekerTargets(const VehicleList & candidates) const;
	/**
	 * @brief Collect closest valid target ship IDs from snapshot candidate data.
	 *
	 * Filters out destroyed, out-of-bounds, and station candidates, computes
	 * seeker-to-candidate hex distance, and returns all ship IDs tied for
	 * closest distance without applying random tie-breaking. This keeps the
	 * deterministic portion of seeker targeting available to behavioral tests.
	 *
	 * @param seeker Active seeker state to evaluate.
	 * @param candidates Snapshot candidate list containing hex and validity fields.
	 *
	 * @return Ship IDs for all closest valid candidates (empty when none qualify).
	 *
	 * @author gpt-5.3-codex (standard)
	 * @date Created: May 27, 2026
	 * @date Last Modified: May 27, 2026
	 */
	std::vector<unsigned int> collectClosestSeekerTargetIDs(
		const FTacticalSeekerMissileState & seeker,
		const std::vector<FTacticalSeekerTargetSnapshot> & candidates) const;
	/**
	 * @brief Compute one greedy seeker movement step toward a target hex.
	 *
	 * Evaluates left/straight/right heading options, selects the candidate that
	 * minimizes distance to the target, and returns the resulting seeker heading
	 * and hex without mutating the input state so tests can inspect a single
	 * greedy move independently of the live allowance loop.
	 *
	 * @param seeker Current seeker state.
	 * @param targetHex Target hex to approach.
	 * @param nextState Output seeker state after one legal greedy step.
	 *
	 * @return True when a legal next step exists and `nextState` was updated.
	 *
	 * @author gpt-5.3-codex (standard)
	 * @date Created: May 27, 2026
	 * @date Last Modified: May 27, 2026
	 */
	bool computeSeekerGreedyNextStep(
		const FTacticalSeekerMissileState & seeker,
		const FPoint & targetHex,
		FTacticalSeekerMissileState & nextState) const;
	bool buildSelectedPlacementSource(FTacticalDeploymentSource & source) const;
	/**
	 * @brief Check whether the current selection can deploy seekers during offensive fire.
	 *
	 * Defensive fire, non-moving ships, damaged launchers, and non-`SM` weapons
	 * never enter this mode.
	 *
	 * @return True when the selected launcher may place pending offensive-fire seekers.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: May 25, 2026
	 * @date Last Modified: May 25, 2026
	 */
	bool canUseOffensiveFireSeekerDeployment() const;
	/**
	 * @brief Check whether a hex is on the selected ship's current-turn path.
	 *
	 * Offensive-fire seeker deployment is legal only on the selected moving
	 * ship's start, traversed, and final path hexes for the current turn.
	 *
	 * @param hex Tactical hex to validate against the selected path.
	 *
	 * @return True when the requested hex is a legal offensive-fire deployment hex.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: May 25, 2026
	 * @date Last Modified: May 25, 2026
	 */
	bool isHexOnSelectedShipCurrentPath(const FPoint & hex);
	/**
	 * @brief Check whether a launcher already owns current-phase pending deployments.
	 *
	 * This keeps empty-ammo `SM` launchers selectable long enough to recall their
	 * own pending seekers before `Offensive Fire Done` commits the phase.
	 *
	 * @param source Source launcher provenance to match.
	 *
	 * @return True when that launcher already has current offensive-fire pending seekers.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: May 25, 2026
	 * @date Last Modified: May 25, 2026
	 */
	bool hasPendingOffensiveDeploymentForSource(const FTacticalOrdnanceSource & source) const;
	/**
	 * @brief Place one pending offensive-fire seeker for the selected launcher.
	 *
	 * Each successful click on a legal path hex consumes one ammo and creates one
	 * inactive seeker plus one current-phase pending record. Same-hex clicks add
	 * more seekers to that hex instead of toggling an undo path.
	 *
	 * @param hex Legal current-path hex chosen for deployment.
	 *
	 * @return True when one pending seeker was created in that hex.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: May 25, 2026
	 * @date Last Modified: May 25, 2026
	 */
	bool placeOffensiveFirePendingSeekerAtHex(const FPoint & hex);
	/**
	 * @brief Recall one pending offensive-fire seeker from a selected hex.
	 *
	 * Removes only the most recent matching current-phase pending seeker for the
	 * selected launcher in that hex, restoring one ammo and preserving any other
	 * stacked pending seekers there.
	 *
	 * @param hex Grouped pending hex to recall from.
	 *
	 * @return True when one pending seeker was removed.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: May 25, 2026
	 * @date Last Modified: May 25, 2026
	 */
	bool removeOffensiveFirePendingSeekerAtHex(const FPoint & hex);
	/**
	 * @brief Clear phase-tracking records for offensive-fire pending seekers.
	 *
	 * The inactive seeker model records remain on the board so surviving pending
	 * deployments become ordinary inactive seekers once the offensive-fire phase
	 * is completed.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: May 25, 2026
	 * @date Last Modified: May 25, 2026
	 */
	void clearPendingOffensiveFireSeekers();
	/**
	 * @brief Clear the displayed movement path on all non-impacting seekers.
	 *
	 * After `applyMovementSeekerDamage()` has removed impacting seekers, the
	 * remaining seekers in `m_seekerMissiles` are non-impacting survivors. This
	 * helper clears their `movementPath` so the rendered path disappears when the
	 * moving player's ship movement phase completes (`completeMovePhase`), matching
	 * the point at which ship movement routes are cleared (SMFR-05).
	 *
	 * Only `movementPath` is cleared. The `movementAllowance` and `movementTurn`
	 * fields are left intact because they are needed for the seeker's movement
	 * bookkeeping in subsequent turns.
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 19, 2026
	 * @date Last Modified: Jun 19, 2026
	 */
	void clearNonImpactingSeekerMovementPaths();
	void appendPlacedOrdnanceRecord(FWeapon::Weapon weaponType, const FPoint & hex, const FTacticalOrdnanceSource & source);
	bool removePlacedOrdnanceForSelection(const FPoint & hex, FTacticalPlacedOrdnance & removed);
	void removePlacedMineRecordsAtHex(const FPoint & hex);
	/**
	 * @brief Place a mine at a hex using the specified deployment source.
	 *
	 * Validates weapon type and ammo, inserts the hex into the mined-hex set,
	 * decrements launcher ammo, records the mine owner, appends a placed-ordnance
	 * record, rebuilds the deployable source list filtered to M-type weapons only,
	 * and reselects the same ship/weapon slot. Returns false when the hex is already
	 * mined, the weapon is wrong type, or ammo is exhausted.
	 *
	 * @param hex Tactical hex to mine.
	 * @param selectedSource Deployment source providing the mine launcher.
	 *
	 * @return True when a mine was placed.
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 19, 2026
	 * @date Last Modified: Jun 19, 2026
	 */
	bool placeMineFromSelection(const FPoint & hex, const FTacticalDeploymentSource & selectedSource);
	/**
	 * @brief Place an inactive seeker missile at a hex using the specified deployment source.
	 *
	 * Validates weapon type and ammo, decrements launcher ammo, creates an inactive
	 * FTacticalSeekerMissileState record, appends a placed-ordnance record, rebuilds
	 * the deployable source list filtered to SM-type weapons only, and reselects the
	 * same ship/weapon slot.
	 *
	 * @param hex Tactical hex to place the inactive seeker.
	 * @param selectedSource Deployment source providing the seeker launcher.
	 *
	 * @return True when an inactive seeker was placed.
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 19, 2026
	 * @date Last Modified: Jun 19, 2026
	 */
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
	/// SMFR-03: hexes that triggered mine damage this move — highlighted while summary is shown
	PointSet m_lastTriggeredMineHexes;
	FHexMap m_mineTargetList;
	unsigned int m_mineOwner;
	VehicleList m_shipsWithMines;
	std::vector<FTacticalDeploymentSource> m_deployablePlacementSources;
	int m_selectedPlacementSource;
	std::vector<FTacticalPlacedOrdnance> m_placedOrdnance;
	std::vector<FTacticalSeekerMissileState> m_seekerMissiles;
	std::vector<FTacticalSeekerContactOutcome> m_pendingSeekerContactOutcomes;
	int m_offensiveFirePhaseID;
	std::vector<FTacticalPendingSeekerDeployment> m_pendingOffensiveSeekerDeployments;
	FPoint m_selectedSeekerActivationHex;
	/// Counter advanced each time a new seeker-activation phase begins; seekers stamp this when activated.
	int m_seekerActivationPhaseIndex;
};

}

#endif //_FTACTICALGAME_H_
