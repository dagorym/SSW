/**
 * @file FGame.h
 * @brief Header file for FGame class
 * @author Tom Stephens
 * @date Created:  Jan 14, 2005
 *
 */

#ifndef _FGAME_H_
#define _FGAME_H_

#include "core/FPObject.h"
#include "StrategicTypes.h"
#include "FPlayer.h"
#include "FMap.h"
#include "strategic/IStrategicUI.h"
#include <vector>

class wxWindow;

namespace Frontier
{
/**
 * @brief Main game class
 *
 * This is the main class of the game that contains pointers to
 * the players and the maps.  Game creation is managed through the
 * static factory methods create(), create(IStrategicUI*), and
 * create(wxWindow*).  All UI notifications are routed through an
 * IStrategicUI adapter so no wx types are required in the model layer.
 *
 * The FGame class is responsible for destroying the FMap object and the
 * FPlayer objects when it is destroyed.
 *
 * @author Tom Stephens, gpt-5.3-codex (medium), Claude Sonnet 5 (medium)
 * @date Created:  Jan 14, 2005
 * @date Last Modified:  Jul 17, 2026
 */
class FGame : public Frontier::FPObject {
private:
	/// Static instance of the FGame class
	static FGame * m_game;
	/// Private copy construtor to prevent copies
	FGame(const FGame&);
	/// Private assignment operator to prevent copies
	FGame& operator=(FGame&);

protected:
	/// Constructor accepting an optional strategic UI adapter; pass NULL for non-GUI use.
	FGame(IStrategicUI * ui);

public:
	/**
	 * @brief Creates the game singleton (no UI) or returns the existing instance.
	 *
	 * This method creates the game object if it doesn't exist and
	 * returns a reference to the object once it is created.  If called
	 * and the object already exists, it just returns a reference to
	 * the object.  The game is created in non-GUI mode when called
	 * without arguments.
	 *
	 * @author Tom Stephens, gpt-5.3-codex (medium)
	 * @date Created:  May 30, 2008
	 * @date Last Modified:  Mar 28, 2026
	 */
	static FGame & create();
	/**
	 * @brief Creates the game singleton with a strategic UI adapter or returns the existing instance.
	 *
	 * Passes @p ui to the protected constructor so all subsequent UI notifications
	 * are routed through the IStrategicUI seam.  Pass NULL for non-GUI mode.
	 *
	 * @param ui Pointer to the IStrategicUI adapter, or NULL for non-GUI use.
	 *
	 * @author Tom Stephens, gpt-5.3-codex (medium)
	 * @date Created:  Mar 28, 2026
	 * @date Last Modified:  Mar 28, 2026
	 */
	static FGame & create(IStrategicUI * ui);
	/**
	 * @brief Compatibility shim — ignores @p win and delegates to create().
	 *
	 * Retained so existing call sites that pass a wxWindow pointer still compile.
	 * The window argument is discarded; all UI routing now goes through IStrategicUI.
	 *
	 * @param win Ignored; accepted only for backwards-compatible call sites.
	 *
	 * @author Tom Stephens, gpt-5.3-codex (medium)
	 * @date Created:  Mar 28, 2026
	 * @date Last Modified:  Mar 28, 2026
	 */
	static FGame & create(wxWindow * win);

  /// Default Destructor
	virtual ~FGame();
  /**
   * @brief Initialises the game: loads players, map, and fleets.
   *
   * This method initialises all the data for the game so that it is ready
   * to play.  All UI interaction is routed through the IStrategicUI adapter
   * installed at construction time.  It returns a zero if all is well and a
   * positive error code if there was a problem.
   *
   * The Sathar retreat condition returned by IStrategicUI::selectRetreatCondition()
   * is validated against the valid 1..5 range before being stored in
   * m_satharRetreat.  If the UI returns an out-of-range value (for example
   * wxID_CANCEL from an X-close/cancel on the selection dialog), the UI is
   * re-invoked until a valid 1..5 value is returned, so a cancelled or
   * dismissed dialog can never silently disable UPF victory for the rest of
   * the game.  When m_ui is NULL, this validation/re-prompt loop is skipped
   * entirely (there is no UI to re-invoke), preserving the existing no-UI
   * behavior.  The re-prompt loop is bounded by a generous internal attempt
   * cap so a degenerate but non-NULL UI (for example a headless
   * WXStrategicUI with no live wx runtime) that returns an invalid value on
   * every single call cannot hang init() forever; if the cap is exhausted
   * without ever seeing a valid value, m_satharRetreat is left at its
   * prior/default value rather than being set to a bogus out-of-range value.
   *
   * @param w Accepted for backwards-compatible call sites; ignored internally.
   *
   * @author Tom Stephens, gpt-5.3-codex (medium), Claude Sonnet 5 (medium)
   * @date Created:  Jan 14, 2005
   * @date Last Modified:  Jul 11, 2026
   */
	  int init(wxWindow *w);

  /**
   * @brief Method to show a dialog with a list of players
   *
   * This method prints out a list of players to the screen with
   * thier player id number and name.
   *
   * @author Tom Stephens
   * @date Created:  May 25, 2005
   * @date Last Modified:  Feb 07, 2008
   */
  void showPlayers();

  /**
   * @brief Handle map click using logical map coordinates.
   *
   * Translates model-space coordinates supplied by the GUI layer into
   * a system or fleet selection.  If a system is found at the given
   * coordinates, showSystemDialog() is called on the IStrategicUI adapter.
   * If a fleet is found, showFleetDialog() is called instead.  Does
   * nothing when no UI adapter is installed.
   *
   * @param mapX Logical map X coordinate.
   * @param mapY Logical map Y coordinate.
   *
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created:  Mar 28, 2026
   * @date Last Modified:  Mar 28, 2026
   */
  void handleMapClick(double mapX, double mapY);

  /**
   * @brief Creates and places Strike Force Nova
   *
   * This method creates the Strike Force Nova fleet and determines its position on
   * on the board if it is the UPF player's turn.  If not it does nothing.
   *
   * @author Tom Stephens
   * @date Created:  Feb 19, 2008
   * @date Last Modified:  Feb 19, 2008
   */
  bool placeNova();

  /**
   * @brief Updates position at end of sathar movement
   *
   * This method does the post orders bookkeeping for the Sathar player.
   *
   * @author Tom Stephens
   * @date Created:  Feb 19, 2008
   * @date Last Modified:  Feb 19, 2008
   */
  void endSatharTurn();

  /**
   * @brief Updates position at end of UPF movement
   *
   * This method does the post orders bookkeeping for the UPF player.
   *
   * @author Tom Stephens
   * @date Created:  Feb 19, 2008
   * @date Last Modified:  Feb 19, 2008
   */
  void endUPFTurn();

  /**
   * @brief Method to save the game
   *
   * This method implements the FPObject base class virtual write method to
   * save all the game state. Writes the fixed-width @c kSaveMagic /
   * @c kSaveFormatVersion header (via @c writeU32) before any game data, so
   * a loader can identify and validate the save-file format up front. The
   * player count and the current-player ID are written with the
   * fixed-width little-endian @c writeU32 helper rather than the native
   * @c write template, matching the portable wire-format convention used by
   * @c FMap, @c FSystem, @c FPlayer, @c FFleet, and @c FVehicle.
   *
   * @param os The output stream to write to
   *
   * @author Tom Stephens, Claude Sonnet 5 (medium)
   * @date Created:  Mar 03, 2008
   * @date Last Modified:  Jul 17, 2026
   */
  const virtual int save(std::ostream &os) const;

	/**
	 * @brief Method to read data contents
	 *
	 * This method is the inverse of the save method.  It reads the data for
	 * the class from the designated input stream.  The magic tag is read
	 * and validated first (mismatch aborts the load), then the format
	 * version (an unsupported version aborts the load); every subsequent
	 * header field read, the nested @c m_universe->load(is) call, and each
	 * player's @c load(is) call are all checked, and the first failure
	 * aborts the load without leaving a half-built game committed as the
	 * live singleton. (FF-1/FF2-1 / SF-located-object-ids) After each player
	 * loads successfully, every one of that player's fleets has its
	 * location, jump-route, and destination reference IDs validated against
	 * the already-loaded @c m_universe before being wired into a system's
	 * fleet list: a fleet's location (system) ID must resolve via
	 * @c FMap::getSystem(id) unless it is the documented @c 0 "not yet in a
	 * system" sentinel, its jump-route ID must resolve via
	 * @c FMap::getJumpRoute(id) unless it is the @c FFleet::NO_ROUTE
	 * sentinel, and its destination (system) ID must resolve via
	 * @c FMap::getSystem(id) unless it is the @c FFleet::NO_DESTINATION
	 * sentinel; the first unresolved reference aborts the load the same way
	 * a truncated/corrupt read does, since an out-of-range ID would
	 * otherwise reach a gui draw path (@c WXPlayerDisplay::drawFleets()) or
	 * @c FGame::moveFleets() and dereference a NULL
	 * @c FMap::getSystem(id)/@c getJumpRoute(id) result, crashing. A fleet
	 * whose @c getInTransit() is true but whose location is the @c 0
	 * sentinel is also rejected as an illegal state (an in-transit fleet
	 * must have a real origin system), since that combination would
	 * otherwise reach the same NULL @c FMap::getSystem(0) dereference in
	 * @c FGame::moveFleets(). Every failure is reported via
	 * @c m_ui->showMessage(...) when an @c IStrategicUI is installed, or a
	 * console fallback otherwise, via the private @c reportLoadError(...)
	 * helper. This method returns 0 if everything is okay and a positive
	 * integer error code if there is a failure.
	 *
	 * @author Tom Stephens, Claude Sonnet 5 (medium)
	 * @date Created:  Mar 07, 2008
	 * @date Last Modified:  Jul 19, 2026
	 */
	virtual int load(std::istream &is);

	/**
	 * @brief Process end turn for the active player.
	 *
	 * Invokes endUPFTurn() or endSatharTurn() depending on which player
	 * is currently active and advances the game state accordingly.
	 *
	 * @return 1 if the UPF turn was ended, 2 if the Sathar turn was ended.
	 *
	 * @author Tom Stephens, gpt-5.3-codex (medium)
	 * @date Created:  Mar 28, 2026
	 * @date Last Modified:  Mar 28, 2026
	 */
	int processEndTurn();

	/// Returns the current game round number.
	unsigned int getRound() const;

	/// Returns the ID of the currently active player.
	unsigned int getCurrentPlayerID() const;

	/// Returns a read-only reference to the list of players.
	const PlayerList & getPlayers() const;

	/**
	 * @brief Determines if it is the UPF turn or not
	 *
	 * This method determines if it is the UPF turn or not.  If it
	 * is it ruturns true otherwise it returns false
	 *
	 * @author Tom Stephens
	 * @date Created:  Mar 25, 2008
	 * @date Last Modified:  Mar 25, 2008
	 */
	bool isUPFTurn();

	/**
	 * @brief Allows Sathar player to add more fleets
	 *
	 * This method processes the selection of the Add Sathar Ships
	 * menu item to allow the Sathar player to place additional fleets
	 * on the board composed of ships from their unattached ships list.
	 *
	 * @author Tom Stephens
	 * @date Created:  Mar 25, 2008
	 * @date Last Modified:  Mar 25, 2008
	 */
	void onAddSatharShips();

	/**
	 * @brief Shows what the Sathar retreat conditions are
	 *
	 * This method pops up a dialog box showing what the sathar retreat
	 * conditions are so the Sathar player can remind himself during the game
	 *
	 * @author Tom Stephens
	 * @date Created:  Apr 15, 2008
	 * @date Last Modified:  Apr 15, 2008
	 */
	void showRetreatConditions();

	/**
	 * @brief Returns a pointer to a player object
	 *
	 * This method take the player ID as input and returns a point to that
	 * player's object.  If no player with that ID exists it returns a NULL
	 * pointer
	 *
	 * @param id The id of the player to retrieve
	 *
	 * @author Tom Stephens
	 * @date Created:  May 30, 2008
	 * @date Last Modified:  May 30, 2008
	 */
	FPlayer * getPlayer(unsigned int id) const;


private:
  /// Vector containing pointers to all players
  PlayerList m_players;
  /// Pointer to game FMap object
  FMap *m_universe;
  /// Turn counter
  unsigned int m_round;
  /// Flag for graphical output
  bool m_gui;
  /// current player ID
  unsigned int m_currentPlayer;
  /// pointer to strategic UI adapter
  IStrategicUI * m_ui;
  /// Sathar retreat condition
  int m_satharRetreat;
  /// number of Heavy Curisers the Sathar have lost
  int m_lostHC;
  /// number of Assault Carriers the Sathar have lost
  int m_lostAC;
  /// number of ships the Sathar have lost total
  int m_lostSatharShips;
  /// number of ships Sathar have lost in last tenday
  int m_lostTendaySathar;
  /// number of ships UPF have lost in the last tenday
  int m_lostTendayUPF;
  /// number of stations destroyed in the last tenday
  int m_stationsDestroyed;

  /**
   * @brief Reports a load() failure through the installed strategic UI.
   *
   * Routes @p detail to @c m_ui->showMessage(...) when an @c IStrategicUI
   * is installed; otherwise falls back to writing @p detail to
   * @c std::cout, matching the console-fallback convention already used
   * elsewhere in this class (see init()). Used by load() to report a wrong
   * magic tag, an unsupported format version, a truncated/corrupt stream,
   * or an unknown factory type surfaced by a nested load() call.
   *
   * @param detail Human-readable description of the load failure.
   *
   * @author Claude Sonnet 5 (medium)
   * @date Created: Jul 17, 2026
   * @date Last Modified: Jul 17, 2026
   */
  void reportLoadError(const std::string &detail) const;

  /**
   * @brief Method to get the players' info
   *
   * This method queries the user for the player information and then
   * initalizes the player information.
   *
   * @param gui  Flag for whether or not to use graphical output.
   *
   * @author Tom Stephens
   * @date Created: Jan 21, 2005
   * @date Last Modified:  Feb 10, 2008
   */
  int getPlayers(bool gui);

  /**
   * @brief Method to set up the game map
   *
   * This method queries the user for the map size and then
   * initalizes the map information information.
   *
   * @param gui  Flag for whether or not to use graphical output.
   *
   * @author Tom Stephens
   * @date Created: Jan 21, 2005
   * @date Last Modified:  Feb 19, 2008
   */
  int initMap(bool gui);

  /**
   * @brief Set up the inital fleets
   *
   * This method initalized the fleets for each of the players and
   * places them in thier starting locations.  It returns a 0 if there
   * were no problems and a 1 otherwise.
   *
   * @author Tom Stephens
   * @date Created:  Feb 07, 2008
   * @date Last Modified:  Mar 11, 2008
   */
  int initFleets();

  /**
   * @brief Updates position of the specified player's fleets
   *
   * This method moves the fleets along their jumps and shifts them between
   * systems as they completel jumps.
   *
   * @param p pointer to the player object of the fleets to move
   *
   * @author Tom Stephens
   * @date Created:  Feb 19, 2008
   * @date Last Modified:  Mar 14, 2008
   */
  void moveFleets(FPlayer * p);

  /// Create Task Force Prenglar
  void createTFPrenglar();
  /// Create Task Force Cassidine
  void createTFCassidine();
  /// Create Strike Force Nova
  void createSFNova();
  /// record the unattached ships
  int addUPFUnattached();
  /// record all the sathar ships at the start of the game
  int addSatharShips();
  /// Create all the milita fleets and add them to the board
  void createMilita();
  /// Set up the White Light Milita
  void createWhiteLightMilita();
  /// Set up the Gruna Garu Militia
  void createGrunaGaruMilita();
  /// Set up the Araks Militia
  void createAraksMilita();
  /// Set up the Dramune Militia
  void createDramuneMilita();
  /// Set up the Kaken-Kar Militia
  void createKakenKarMilita();
  /// Set up the Theseus Militia
  void createTheseusMilita();
  /// Set up the Truane's Star Militia
  void createTruanesStarMilita();
  /// Set up the Fromelatr Militia
  void createFromeltarMilita();
  /// Set up the Kizk-KarMilitia
  void createKizkKarMilita();
  /// add in all the space stations
  void addStations();

  /**
   * @brief Check to see of someone  has won the game
   *
   * This method checks to see if the Sathar have destroyed enough stations to win or
   * if the sathar retreat condition has been met granting a possible sathar victory.
   * If there is no winner this round it returns a 0.  If the UPF win, it returns a 1.
   * If the Sathar win it returns a 2.  And if it is a draw, it returns a 3.
   *
   * The Sathar retreat condition switch is keyed by @c m_satharRetreat and each case
   * number must evaluate the condition matching the text shown by showRetreatConditions():
   * case 4 is the station/fortress-destruction condition (UPF wins a tenday boundary if
   * fewer than two stations were destroyed that tenday, via @c m_stationsDestroyed, which
   * is reset at the boundary), and case 5 is the ship-loss condition (UPF wins a tenday
   * boundary if Sathar tenday ship losses exceed UPF losses, via @c m_lostTendaySathar /
   * @c m_lostTendayUPF, both reset at the boundary). These two cases were previously
   * evaluated swapped relative to their displayed condition text (defect C2); fixed here.
   *
   * @author Tom Stephens, Claude Sonnet 5 (medium)
   * @date Created:  Apr 15, 2008
   * @date Last Modified:  Jul 11, 2026
   */
  int checkForVictory();

  /**
   * @brief Resolves combat for a turn
   *
   * This method checks each of the systems after both sides have moved to see
   * if there are any chances for combat.  If so, it calls the resolveCombat()
   * method to see what happens.
   *
   * @author Tom Stephens
   * @date Created:  Apr 16, 2008
   * @date Last Modified:  May 28, 2009
   */
  int checkForCombat();

  /**
   * @brief Resolves the combat in the specified systems
   *
   * This method resolves combat in the specified system.
   *
   * @author Tom Stephens
   * @date Created:  Apr 16, 2008
   * @date Last Modified:  Apr 16, 2008
   */
  void resolveCombat(std::string sysName);

  /**
   *
   * @brief Removes destroyed ships from active fleets
   *
   * This method loops over the game objects at the end of a strategic
   * combat round and removes all destroyed ships from the active fleets
   * and places them in the player's destroyed ship list.  If a fleet is
   * empty it will be removed from the game.
   *
   * The method also checks all the stations and removes the destroyed
   * ones from the game.
   *
   * Operates directly on the live per-player fleet lists and live
   * per-fleet ship lists (rather than by-value copies), so removals and
   * loss-counter increments apply to the real game state exactly once
   * per destroyed ship. Destroyed ship IDs are collected before any
   * mutation to avoid an erase-while-iterating skip bug. A fleet emptied
   * by this pass is removed from both its owning player and its system,
   * then deleted, since neither retains any other live reference to it.
   *
   * Loss-counter rules match the Sathar retreat condition text shown by
   * showRetreatConditions(): condition 3 ("40 ships, including fighters")
   * means @c m_lostSatharShips counts every destroyed Sathar ship,
   * including fighters. Condition 5 ("Fighters and Militia ships are not
   * counted") means @c m_lostTendaySathar / @c m_lostTendayUPF only count
   * a destroyed ship when it is neither a fighter nor a member of a
   * militia fleet (per FFleet::isMilitia()); this exclusion applies to
   * both sides.
   *
   * @author Tom Stephens, Claude Sonnet 5 (medium)
   * @date Created:  May 28, 2009
   * @date Last Modified:  Jul 11, 2026
   */
  void cleanUpShips();


};

};

#endif //_FGAME_H_
