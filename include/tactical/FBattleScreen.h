/**
 * @file FBattleScreen.h
 * @brief Header file for BattleScreen class
 * @author Tom Stephens, Claude Sonnet 4.6 (medium), gpt-5.3-codex (standard), gpt-5.4 (high), claude-sonnet-4-6 (medium), claude-opus-4-8 (medium)
 * @date Created:  Jul 11, 2008
 * @date Last Modified: Jun 22, 2026
 *
 */

#ifndef __FBattleScreen__
#define __FBattleScreen__

#include "Frontier.h"
#include "gui/GuiTypes.h"
#include "tactical/FBattleBoard.h"
#include "tactical/FBattleDisplay.h"
#include "tactical/FTacticalCombatReport.h"
#include "tactical/FTacticalGame.h"

namespace Frontier {

class FTacticalGame;
class WXTacticalUI;

/**
 * @brief Class for the Main tactical combat board
 *
 * This class implements the code for the FBattleScreen, the main
 * top-level board used for the tactical combat game. The runtime surface is
 * now frame-backed so tactical flows can attach a menu bar, while class-owned
 * modal shim APIs preserve the legacy blocking launch contract used by
 * stack-allocated battle-screen call sites. The native top-level now installs
 * `File`, `Settings`, and `Help` menus, with `File -> Quit` acting as the
 * initial active command while the remaining tactical menu entries stay
 * present as disabled placeholders. Both the menu quit command and the native
 * title-bar close vector now funnel through the same screen-owned close
 * lifecycle so non-modal tactical screens hide before wx pending-delete
 * destruction and modal callers unwind back to their launch sites without
 * allowing default frame destruction to touch stack-owned instances.
 *
 * @author Tom Stephens, gpt-5.4 (high), Claude Sonnet 4.6 (medium), gpt-5.3-codex (standard)
 * @date Created:  Jul 11, 2008
 * @date Last Modified:  May 27, 2026
 */
class FBattleScreen : public wxFrame
{
public:
	/**
	 * @brief Construct the tactical battle screen and install its initial menu bar.
	 *
	 * Builds the frame-backed tactical top-level with the shared battle map and
	 * lower display panel, then installs the native `File`, `Settings`, and
	 * `Help` menus used by the current tactical runtime. Only `File -> Quit`
	 * is active in this initial menu set; the remaining entries stay visible as
	 * disabled placeholders for future tactical commands.
	 *
	 * @param title Initial window title.
	 * @param pos Initial top-left window position.
	 * @param size Initial top-level window size.
	 * @param style wxWidgets top-level style flags for the frame-backed surface.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created:  Jul 11, 2008
	 * @date Last Modified:  May 23, 2026
	 */
	FBattleScreen(const wxString& title = "Star Frontiers Knight Hawks Battle Board", const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1200,900 ), long style = wxDEFAULT_DIALOG_STYLE|wxMINIMIZE_BOX|wxMAXIMIZE_BOX|wxRESIZE_BORDER );
//	FBattleScreen(const wxString& title = "Star Frontiers Knight Hawks Battle Board", const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 750,550 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
	/// Default destructor
	~FBattleScreen();
	/// Runs a class-owned event loop so legacy blocking launch sites stay source-compatible on wxFrame. On GTK, calls gtk_window_set_modal(TRUE) before Show() so the window acquires the input grab during the show phase, ensuring menu events are delivered even when launched from a modal dialog.
	int ShowModal();
	/// Ends frame-backed modal compatibility mode: exits the event loop first, then hides the frame (matching wxDialog::EndModal ordering) without destroying stack-owned instances.
	void EndModal(int returnCode);
	/// Returns true when modal compatibility mode is active.
	bool IsModal() const;
	/// Stores return code for modal and non-modal close paths.
	void SetReturnCode(int returnCode);
	/// Gets return code for modal and non-modal close paths.
	int GetReturnCode() const;
	static void resetLifecycleCounters();
	static int getConstructedCount();
	static int getDestroyedCount();
	static int getLiveInstanceCount();

	void draw();
	void onPaint(wxPaintEvent & event);

	/**
	 * @brief Sets up the board with battle fleets
	 *
	 * This method sets up the players fleets
	 *
	 * @param aList List of attacking fleets
	 * @param dList List of defending fleets
	 * @param planet Flag for whether there is a planet or not
	 * @param station Pointer to station battle is near
	 *
	 * @author Tom Stephens
	 * @date Created:  Jul 31, 2008
	 * @date Last Modified:  July 31, 2008
	 */
	int setupFleets(FleetList *aList, FleetList *dList, bool planet = false, FVehicle * station = NULL);

	/**
	 * @brief Return a copy of the list of the current side's ships
	 *
	 * This method looks at the m_activePlayer variable and returns a list of that
	 * sides ships.  This version of the method is now depricated in favor of the
	 * new version that takes the player's ID value
	 *
	 * @author Tom Stephens
	 * @date Created:  Sep 8, 2008
	 * @date Last Modified:  Sep 8, 2008
	 */
	VehicleList getShipList() const;

	/**
	 * @brief Return a copy of the ship list for the player with the specified ID
	 *
	 * This version of the method takes the player's ID and returns the list of ships
	 * for that particular player.
	 *
	 * @author Tom Stephens
	 * @date Created:  Mar 31, 2009
	 * @date Last Modified:  Mar 31, 2009
	 */
	VehicleList getShipList(unsigned int id) const;

	/**
	 * @brief Removes all destroyed ships from the map
	 *
	 * This runs through all the ships of the player that was just fired
	 * upon and removes them from the game map
	 *
	 * @author Tom Stephens
	 * @date Created:  Apr 30, 2009
	 * @date Last Modified:  Apr 30, 2009
	 */
	void clearDestroyedShips();

	/**
	 * determine ICM's being fired
	 *
	 * This method loops over all the weapons being fired and
	 * determines the ones that can have ICMs fired at them.  The
	 * defending player is then given the option to target ICMs
	 * at these weapons
	 *
	 * @author Tom Stephens
	 * @date Created:  Apr 22, 2010
	 * @date Last Modified:  Feb 20, 2011
	 */
	void fireICM();

	/// get the battle board state
	const int & getState() const;
	/// set the battle board state
	void setState(int s);
	/// toggle the control state
	void toggleControlState();
	/// get the control state
	const bool & getControlState() const;
	/// sets the id of the planet chosen
	void setPlanet(int c);
	/// get the value of the choice variable
	const int & getPlanetChoice() const;
	/// set planet position
	void setPlanetPosition(FPoint h);
	/// get the station's current position
	const FPoint & getPlanetPos() const;
	/// set Station position
	void setStationPosition(FPoint h);
	/// get the station's current position
	const FPoint & getStationPos() const;
	/// get pointer to the station object
	FVehicle * getStation() const;
	/// change the scale of the map
	void setScale(double factor);
	/// get the current ship selection
	FVehicle * getShip() const;
	/// set the current ship selection
	void setShip(FVehicle * s);
	/// get done state
	bool getDone();
	/// set done state
	void setDone(bool f);
	/// get side
	bool getActivePlayer();
	/// set side
	void setActivePlayer(bool f);
	/// toggle side
	void toggleActivePlayer();
	/// get the current battle phase
	int getPhase();
	/**
	 * @brief Forward a phase-transition request to the tactical model.
	 *
	 * Requests for `PH_MOVE` preserve source compatibility for legacy callers
	 * while allowing `FTacticalGame` to insert the new seeker-activation prephase
	 * before movement when required.
	 *
	 * @param p Requested tactical phase constant.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: Jul 11, 2008
	 * @date Last Modified: May 25, 2026
	 */
	void setPhase(int p);
	/// redraw the screen
	void reDraw() { m_map->Refresh(); m_display->Refresh(); }
	/// returns the ID of the attacking player
	const unsigned int & getAttackerID() const;
	/// returns the ID of the defending player
	const unsigned int & getDefenderID() const;
	/// returns the ID of the player who's turn it currently is
	const unsigned int & getActivePlayerID() const;
	/// sets the movement status
	void setMoveComplete(bool s);
	/// get the movement status
	bool isMoveComplete() const;
	/// returns the ID of the player who's moving this turn
	const unsigned int & getMovingPlayerID() const;
	/// toggles the moving player flag
	void toggleMovingPlayer();
	/// set the current weapon
	void setWeapon(FWeapon * w);
	/// get the current weapon
	FWeapon * getWeapon();
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
	 * @brief Enter the mine-only deployment setup phase (BS_PlaceMines).
	 *
	 * Forwards to FTacticalGame::beginMinePlacement() which rebuilds the
	 * deployable source list filtered to FWeapon::M slots only.
	 *
	 * @return True when mine placement mode was entered successfully.
	 *
	 * @author Tom Stephens, gpt-5.4 (high), claude-sonnet-4-6 (medium)
	 * @date Created: Jul 11, 2008
	 * @date Last Modified: Jun 02, 2026
	 */
	bool beginMinePlacement();
	/**
	 * @brief Enter the seeker-only deployment setup phase (BS_PlaceSeekers).
	 *
	 * Forwards to FTacticalGame::beginSeekerPlacement() which rebuilds the
	 * deployable source list filtered to FWeapon::SM slots only.
	 *
	 * @return True when seeker placement mode was entered successfully.
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 02, 2026
	 * @date Last Modified: Jun 02, 2026
	 */
	bool beginSeekerPlacement();
	/**
	 * @brief Forward generalized setup ordnance placement entry to FTacticalGame.
	 *
	 * @return True when a deployable source with ammo is available and placement
	 *         mode is entered.
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
	 * @return True when the requested source becomes current.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: May 24, 2026
	 * @date Last Modified: May 24, 2026
	 */
	bool selectPlacementSource(unsigned int shipID, unsigned int weaponIndex);
	/**
	 * @brief Select the active placement source by deployable-source index.
	 *
	 * @param sourceIndex Zero-based index into getDeployablePlacementSources().
	 *
	 * @return True when the requested source becomes current.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: May 24, 2026
	 * @date Last Modified: May 24, 2026
	 */
	bool selectPlacementSourceByIndex(unsigned int sourceIndex);
	int getSelectedPlacementSourceIndex() const;
	/// get the current deployable mine/seeker placement-source list
	const std::vector<FTacticalDeploymentSource> & getDeployablePlacementSources() const;
	/**
	 * @brief Complete mine placement and advance to the seeker phase (or skip it).
	 *
	 * Forwards to FTacticalGame::completeMinePlacement() which attempts seeker
	 * placement and, when none are available, advances to BS_SetupAttackFleet.
	 *
	 * @author Tom Stephens, gpt-5.4 (high), claude-sonnet-4-6 (medium)
	 * @date Created: Jul 11, 2008
	 * @date Last Modified: Jun 02, 2026
	 */
	void completeMinePlacement();
	/**
	 * @brief Complete seeker placement and advance to attacker setup.
	 *
	 * Forwards to FTacticalGame::completeSeekerPlacement() which transitions to
	 * BS_SetupAttackFleet and toggles the active player.
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 02, 2026
	 * @date Last Modified: Jun 02, 2026
	 */
	void completeSeekerPlacement();
	void completeMovePhase();
	FTacticalCombatReportSummary resolveCurrentFirePhase();
	void completeDefensiveFirePhase();
	void completeOffensiveFirePhase();
	void computeWeaponRange();
	bool assignTargetFromHex(const FPoint & hex);
	/**
	 * @brief Forward generalized ordnance placement or undo at a tactical hex.
	 *
	 * @param hex Tactical hex to place into or undo from.
	 *
	 * @return True when the model placement state changed.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: May 24, 2026
	 * @date Last Modified: May 24, 2026
	 */
	bool placeOrdnanceAtHex(const FPoint & hex);
	/**
	 * @brief Preserve the legacy mine-placement forwarder on the screen seam.
	 *
	 * @param hex Tactical hex to place into or undo from.
	 *
	 * @return True when the delegated ordnance-placement state changed.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: Jul 11, 2008
	 * @date Last Modified: May 24, 2026
	 */
	bool placeMineAtHex(const FPoint & hex);
	/**
	 * @brief Query whether the selected placement source can interact with a hex.
	 *
	 * @param hex Tactical hex to inspect.
	 *
	 * @return True when the delegated generalized placement rules allow the
	 *         interaction.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: May 24, 2026
	 * @date Last Modified: May 24, 2026
	 */
	bool isHexDeployable(const FPoint & hex);
	/**
	 * @brief Preserve the legacy mine-only placement query on the screen seam.
	 *
	 * @param hex Tactical hex to inspect.
	 *
	 * @return True when the delegated placement rules allow mine interaction.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: Jul 11, 2008
	 * @date Last Modified: May 24, 2026
	 */
	bool isHexMinable(const FPoint & hex);
	const VehicleList & getHexOccupants(const FPoint & hex) const;
	const std::vector<FPoint> & getMovementHexes() const;
	const std::vector<FPoint> & getLeftTurnHexes() const;
	const std::vector<FPoint> & getRightTurnHexes() const;
	const std::vector<FTacticalMovePreviewRoute> & getStoppedShipPreviewRoutes() const;
	const std::vector<int> & getStoppedShipPreviewHeadingsForHex(const FPoint & hex) const;
	const PointSet & getTargetHexes() const;
	const PointSet & getHeadOnHexes() const;
	const PointSet & getMinedHexes() const;
	/// SMFR-03: get triggered mine hexes pending board highlight while damage summary is shown
	const PointSet & getLastTriggeredMineHexes() const;
	/// get all source-tracked placed ordnance records
	const std::vector<FTacticalPlacedOrdnance> & getPlacedOrdnance() const;
	/// get placed ordnance records for a specific tactical hex
	std::vector<FTacticalPlacedOrdnance> getPlacedOrdnanceAtHex(const FPoint & hex) const;
	/// get all seeker missile model records (includes movementPath render state)
	const std::vector<FTacticalSeekerMissileState> & getSeekerMissiles() const;
	/// get seeker missile records in a specific tactical hex
	std::vector<FTacticalSeekerMissileState> getSeekerMissilesAtHex(const FPoint & hex, bool activeOnly = false) const;
	/**
	 * @brief Get inactive seeker-stack hexes for the current moving player.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: May 25, 2026
	 * @date Last Modified: May 25, 2026
	 */
	std::vector<FPoint> getInactiveSeekerActivationHexes() const;
	/**
	 * @brief Select the inactive seeker stack hex shown by later wx surfaces.
	 *
	 * @param hex Tactical hex to select.
	 *
	 * @return True when the delegated model selection changed.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: May 25, 2026
	 * @date Last Modified: May 25, 2026
	 */
	bool selectSeekerActivationHex(const FPoint & hex);
	/// get the currently selected inactive seeker activation stack hex
	const FPoint & getSelectedSeekerActivationHex() const;
	/**
	 * @brief Get the selected inactive seeker stack records from the model.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: May 25, 2026
	 * @date Last Modified: May 25, 2026
	 */
	std::vector<FTacticalSeekerMissileState> getSelectedInactiveSeekerActivationStack() const;
	/**
	 * @brief Report whether the selected weapon is in offensive-fire seeker deployment mode.
	 *
	 * When true, the board and lower panel should treat the selected `SM`
	 * launcher as a path-based seeker deployment source instead of a normal
	 * target-assignment weapon.
	 *
	 * @return True when offensive-fire seeker deployment UI should be active.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: May 25, 2026
	 * @date Last Modified: May 25, 2026
	 */
	bool isOffensiveSeekerDeploymentMode() const;
	/**
	 * @brief Get grouped pending offensive-fire seeker rows for the selected launcher.
	 *
	 * Each returned row groups the selected launcher's current-phase pending
	 * seekers by hex so the lower panel can render explicit one-at-a-time recall
	 * actions while allowing multiple seekers to remain stacked in the same hex.
	 *
	 * @return Pending seeker groups for the selected offensive-fire launcher.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: May 25, 2026
	 * @date Last Modified: May 25, 2026
	 */
	std::vector<FTacticalPendingSeekerHexGroup> getSelectedOffensivePendingSeekerHexGroups() const;
	/**
	 * @brief Recall one pending offensive-fire seeker from a grouped lower-panel row.
	 *
	 * @param hex Hex represented by the clicked grouped recall row.
	 *
	 * @return True when one current-phase pending seeker was removed and ammo restored.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: May 25, 2026
	 * @date Last Modified: May 25, 2026
	 */
	bool recallSelectedOffensivePendingSeekerAtHex(const FPoint & hex);
	/**
	 * @brief Get placed inactive seekers grouped by (hex, source) for the pre-game undeploy list.
	 *
	 * Delegation to FTacticalGame read-only accessor used by FBattleDisplay during
	 * BS_PlaceSeekers to render the centered placed-seeker recall list.
	 *
	 * @return Grouped placed seeker rows for the pre-game placed-seeker list.
	 *
	 * @author claude-opus-4-8 (medium)
	 * @date Created: Jun 22, 2026
	 * @date Last Modified: Jun 22, 2026
	 */
	std::vector<FTacticalPreGameSeekerHexGroup> getPlacedSeekerHexGroups() const;
	/**
	 * @brief Recall one placed pre-game seeker at a (hex, source) pair.
	 *
	 * Delegation to FTacticalGame recall method.
	 *
	 * @param hex Tactical hex to recall from.
	 * @param source Source ship/weapon-slot to match.
	 *
	 * @return True when one seeker was recalled and ammo was restored.
	 *
	 * @author claude-opus-4-8 (medium)
	 * @date Created: Jun 22, 2026
	 * @date Last Modified: Jun 22, 2026
	 */
	bool recallPlacedSeekerAtHexSource(const FPoint & hex, const FTacticalOrdnanceSource & source);
	/**
	 * @brief Get all unique hexes that hold any current-phase pending offensive-fire seeker.
	 *
	 * Delegation to the model read-only query used by the board renderer during
	 * `PH_ATTACK_FIRE` to draw the seeker icon on every hex that contains at
	 * least one pending deployment from any launcher.
	 *
	 * @return Unique hex positions of all current-phase pending seeker deployments.
	 *
	 * @author claude-sonnet-4-6 (standard)
	 * @date Created: May 30, 2026
	 * @date Last Modified: May 30, 2026
	 */
	std::vector<FPoint> getAllPendingOffensiveFireSeekerHexes() const;
	/**
	 * @brief Activate one seeker in the selected inactive stack.
	 *
	 * @param seekerID Unique seeker identifier to activate.
	 *
	 * @return True when the delegated model activation changed state.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: May 25, 2026
	 * @date Last Modified: May 25, 2026
	 */
	bool activateSelectedInactiveSeeker(unsigned int seekerID);
	/**
	 * @brief Deactivate one active seeker by ID for the moving player.
	 *
	 * Pass-through delegation to FTacticalGame model method. Transitions an active
	 * seeker owned by the moving player to inactive state.
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
	 * Pass-through delegation to FTacticalGame model method. Selects and activates
	 * exactly one inactive seeker located at the requested hex owned by the moving
	 * player. When multiple inactive seekers share the hex, chooses deterministically
	 * (lowest seekerID).
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
	 * Pass-through delegation to FTacticalGame model method. Returns a copy of all
	 * active seeker records owned by the current moving player across all hexes,
	 * suitable for UI listing and navigation.
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
	 * Pass-through delegation to FTacticalGame model method. Returns only seekers
	 * activated during the current seeker-activation phase, for use by the changeable
	 * list in the seeker-activation UI. Does not include seekers activated in prior
	 * phases.
	 *
	 * @return Vector of active seeker missile state records activated this phase.
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 02, 2026
	 * @date Last Modified: Jun 02, 2026
	 */
	std::vector<FTacticalSeekerMissileState> getActiveSeekersByMovingPlayerThisPhase() const;
	/**
	 * @brief Finish seeker activation and refresh the post-resolution movement view.
	 *
	 * Delegates the moving-side-only active seeker resolution step to
	 * `FTacticalGame`, allowing same-hex contacts, movement-step contacts,
	 * immediate seeker-detonation summary reporting, and 12-hex expiry removal
	 * to settle before the screen redraws in normal movement state. When seeker
	 * damage destroys ships, the screen consumes that deferred cleanup before the
	 * redraw and exits early if the summary path produced a winner.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: May 25, 2026
	 * @date Last Modified: May 27, 2026
	 */
	void completeSeekerActivationPhase();
	const FHexMap & getMineTargets() const;
	unsigned int getMineOwner() const;
	const std::map<unsigned int, FTacticalTurnData> & getTurnInfo() const;
	bool hasShipPlacementPendingRotation() const;
	const FPoint & getSelectedShipHex() const;
	const VehicleList & getShipsWithMines() const;
	bool isHexInBounds(const FPoint & hex) const;
	bool isHexOccupied(const FPoint & hex) const;

	void beginTacticalReport(const FTacticalCombatReportContext & context);
	void appendTacticalAttackReport(const FTacticalAttackReport & attack);
	void appendTacticalReportEvent(const FTacticalReportEvent & event);
	FTacticalCombatReportSummary buildCurrentTacticalReportSummary() const;
	int showTacticalDamageSummaryDialog(const FTacticalCombatReportSummary & summary);
	void clearTacticalReport();
	const FTacticalCombatReport & getCurrentTacticalReport() const;

protected:
	/**
	 * @brief Apply tactical screen height allocation policy.
	 *
	 * Keeps the map dominant by reserving at least sixty percent of client height
	 * for the map while giving the lower display row a baseline height and
	 * honoring larger display-height requests when space allows.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created:  May 16, 2026
	 * @date Last Modified:  May 16, 2026
	 */
	void applyLayoutPolicy();

	/**
	 * @brief Recompute tactical screen layout policy on resize.
	 *
	 * @param event wxWidgets size event raised by top-level resizing.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created:  May 16, 2026
	 * @date Last Modified:  May 16, 2026
	 */
	void onSize(wxSizeEvent & event);

	/// The map window
	FBattleBoard * m_map;
	/// The game display window
	FBattleDisplay * m_display;
	/// remembers the current display-row height request observed from FBattleDisplay
	int m_displayRequestedMinHeight;
	/// tactical model owner during delegation migration
	FTacticalGame * m_tacticalGame;
	/// wx tactical UI adapter owner during delegation migration
	WXTacticalUI * m_tacticalUI;
	/// modal shim event loop pointer for wxFrame modal compatibility paths
	wxEventLoopBase * m_modalEventLoop;
	/// modal shim return code storage for wxFrame modal compatibility paths
	int m_modalReturnCode;
	/// true while wxFrame modal compatibility mode is running
	bool m_modalActive;
	/// true while the screen-owned shared close lifecycle is actively unwinding this surface
	bool m_closeInProgress;

	/// Print a winner message and exit the battle screen through the shared close path.
	void declareWinner();
	/**
	 * @brief Close the tactical top-level through the shared modal and non-modal lifecycle.
	 *
	 * Stack-owned launch paths unwind through EndModal(returnCode) and return
	 * without entering wx default frame destruction. Non-modal launch paths hide
	 * the frame first so users see immediate closure, then schedule Destroy() for
	 * wx pending-delete cleanup. The close-in-progress guard is owned here on
	 * FBattleScreen rather than in FTacticalGame, and it is cleared if
	 * destruction is not scheduled so the first valid close request cannot leave
	 * the tactical top-level stuck open.
	 *
	 * @param returnCode Return code propagated back to modal compatibility callers.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created:  May 22, 2026
	 * @date Last Modified:  May 23, 2026
	 */
	void closeBattleScreen(int returnCode = 0);
	/**
	 * @brief Handle title-bar close events through the shared battle-screen lifecycle.
	 *
	 * The native close event reuses closeBattleScreen(GetReturnCode()) so menu and
	 * title-bar shutdown stay centralized in FBattleScreen. Accepted close events
	 * are fully handled in this method and do not continue into wx default close
	 * handling afterward.
	 *
	 * @param event wxWidgets top-level close event for the tactical frame.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created:  May 22, 2026
	 * @date Last Modified:  May 23, 2026
	 */
	void onClose(wxCloseEvent & event);
	/**
	 * @brief Handle `File -> Quit` by requesting the same close path as title-bar close.
	 *
	 * Keeps tactical menu shutdown aligned with the native title-bar close event
	 * by routing through wxWidgets Close(true) and the shared onClose(...) handler.
	 *
	 * @param event wxWidgets menu event raised for the tactical quit command.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created:  May 22, 2026
	 * @date Last Modified:  May 23, 2026
	 */
	void onMenuQuit(wxCommandEvent & event);

	/**
	 * @brief applies damage to ships from electrical fires
	 *
	 * This method is run at the start of the moving player's combat phase.
	 * It loops through all the non-moving player's ships and checks to see
	 * if they are suffering from an electrical fire.  If so, it generates a
	 * damage roll and calls the ship's takeDamage() method.
	 *
	 * @author Tom Stephens
	 * @date Created:  Jan 28, 2011
	 * @date Last Modified:  Jan 28, 2011
	 */
	void applyFireDamage();

};

}

#endif //__FBattleScreen__
