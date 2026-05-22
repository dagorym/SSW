/**
 * @file FBattleScreen.h
 * @brief Header file for BattleScreen class
 * @author Tom Stephens
 * @date Created:  Jul 11, 2008
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
 * stack-allocated battle-screen call sites.
 *
 * @author Tom Stephens, gpt-5.4 (high)
 * @date Created:  Jul 11, 2008
 * @date Last Modified:  May 22, 2026
 */
class FBattleScreen : public wxFrame
{
public:
	/**
	 * @brief FBattleScreen constructor
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created:  Jul 11, 2008
	 * @date Last Modified:  May 22, 2026
	 */
	FBattleScreen(const wxString& title = "Star Frontiers Knight Hawks Battle Board", const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1200,900 ), long style = wxDEFAULT_DIALOG_STYLE|wxMINIMIZE_BOX|wxMAXIMIZE_BOX|wxRESIZE_BORDER );
//	FBattleScreen(const wxString& title = "Star Frontiers Knight Hawks Battle Board", const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 750,550 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
	/// Default destructor
	~FBattleScreen();
	/// Runs a class-owned event loop so legacy blocking launch sites stay source-compatible on wxFrame.
	int ShowModal();
	/// Ends frame-backed modal compatibility mode before any non-modal destroy path runs.
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
	/// set the battle phase
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
	bool beginMinePlacement();
	void completeMinePlacement();
	void completeMovePhase();
	FTacticalCombatReportSummary resolveCurrentFirePhase();
	void completeDefensiveFirePhase();
	void completeOffensiveFirePhase();
	void computeWeaponRange();
	bool assignTargetFromHex(const FPoint & hex);
	bool placeMineAtHex(const FPoint & hex);
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
	/// modal shim disabler for wxFrame modal compatibility paths
	wxWindowDisabler * m_modalDisabler;
	/// modal shim event loop pointer for wxFrame modal compatibility paths
	wxEventLoopBase * m_modalEventLoop;
	/// modal shim return code storage for wxFrame modal compatibility paths
	int m_modalReturnCode;
	/// true while wxFrame modal compatibility mode is running
	bool m_modalActive;
//	/// window disabler object
//	wxWindowDisabler *m_wd;

	/// Print a winner message and exit the battle screen through the shared close path.
	void declareWinner();
	/**
	 * @brief Close the tactical top-level using modal-first compatibility behavior.
	 *
	 * Stack-owned launch paths must unwind through EndModal(returnCode) before the
	 * frame falls back to the non-modal destroy path used by heap-owned callers.
	 *
	 * @param returnCode Return code propagated back to modal compatibility callers.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created:  May 22, 2026
	 * @date Last Modified:  May 22, 2026
	 */
	void closeBattleScreen(int returnCode = 0);
	/// Handle top-level close events by delegating into the shared battle-screen close path.
	void onClose(wxCloseEvent & event);
	/// Handle menu quit requests by delegating into the shared battle-screen close path.
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
