/**
 * @file FGame.h
 * @brief Header file for FGame class
 * @author Tom Stephens
 * @date Created:  Jan 14, 2005
 * 
 */

#ifndef _FGAME_H_
#define _FGAME_H_

#include "FPObject.h"
#include "FPlayer.h"
#include "FMap.h"
#include <vector>

namespace Frontier
{
/**
 * @brief Main game class
 * 
 * This is the main class of the game that contains pointers to 
 * the players and the maps.
 * 
 * The FGame class is responsible for destroying the FMap object and the
 * FPlayer objects when it is destroyed.
 * 
 * @author Tom Stephens
 * @date Created:  Jan 14, 2005
 * @date Last Modified:  May 30, 2008
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
	/// Default Constructor
	FGame(wxWindow * win);
	
public:
	/**
	 * @brief method to create the game object or get a reference to it
	 * 
	 * This method creates the game object if it doesn't exist and
	 * returns a reference to the object once it is created.  If called
	 * and the object already exists, it just returns a reference to 
	 * the object.
	 * 
	 * @author Tom Stephens
	 * @date Created:  May 30, 2008
	 * @date Last Modified:  May 30, 2008
	 */
	static FGame & create(wxWindow * win = NULL);
	
  /// Default Destructor
	virtual ~FGame();
  /**
   * @brief  Method to initalize the game information
   * 
   * This method initalizes all the data for the game so that it is ready
   * to play.  It returns a zero if all is well and a positive error code 
   * if there was a problem.
   * 
   * @param dc Device context to draw to.
   * @param w pointer to the main frame window
   * 
   * @author Tom Stephens
   * @date Created:  Jan 14, 2005
   * @date Last Modified:  Mar 09, 2008
   */
  int init(wxDC &dc,wxWindow *w);
  
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
   * @brief Redraw the map
   * 
   * This method redraws the game board as needed
   * 
   * @author Tom Stephens
   * @date Created:  Feb 03, 2008
   * @date Last Modified:  Mar 11, 2008
   */
  void draw();  
    
  /**
   * @brief Redraw the map
   * 
   * This method take in the DC from the main frame and passes it to the map so that it can
   * redraw itself.  This one is used when called from an wxPaintEvent
   * 
   * @author Tom Stephens
   * @date Created:  Feb 03, 2008
   * @date Last Modified:  Mar 11, 2008
   */
  void draw(wxDC &dc);  
    
  /**
   * @brief Process double clicking on the window
   * 
   * This method processes a double click of the left mouse button
   * 
   * @param event The wxWidget window event that triggered the function call
   * @param dc The device context of the window.
   * 
   * @author Tom Stephens
   * @date Created:  Feb 11, 2008
   * @date Last Modified:  Mar 11, 2008
   */
  void onLeftDClick(wxMouseEvent& event);

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
   * save all the game state.
   * 
   * @param os The output stream to write to
   * 
   * @author Tom Stephens
   * @date Created:  Mar 03, 2008
   * @aate Last Modified:  Mar 05, 2008
   */
  const virtual int save(std::ostream &os) const;

	/**
	 * @brief Method to read data contents
	 * 
	 * This method is the inverse of the save method.  It reads the data for
	 * the class from the designated input stream.  This method returns 0 if
	 * everything is okay and a positive integer error code if there is a
	 * failure
	 * 
	 * @author Tom Stephens
	 * @date Created:  Mar 07, 2008
	 * @date Last Modified:  Mar 07, 2008
	 */
	virtual int load(std::istream &is);
	   
	/**
	 * @brief Process double clicking on the window
	 * 
	 * This method processes the release of the left mouse button.  This method
	 * checks the various active regions of the screen to see if they were clicked.
	 * The return value signals what was selected:
	 * <ul>
	 * <ll> 0 = nothing selected
	 * <ll> 1 = Ended UPF turn
	 * <ll> 2 = Ended Sathar turn
	 * </ul
	 * 
	 * Mar 11, 2008 - Added End Turn button code
	 * 
	 * @param event The wxWidget window event that triggered the function call
	 * 
	 * @author Tom Stephens
	 * @date Created:  Mar 11, 2008
	 * @date Last Modified:  Mar 27, 2008
	 */
	int onLeftUp(wxMouseEvent& event);

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
  /// pointer to main window
  wxWindow * m_parent;
  /// icon for tenday marker
  wxImage * m_tenday;
  /// icon for day marker
  wxImage * m_day;
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
   * @brief Draws the turn counter on the map
   * 
   * This method draws the boxes on the map for the turn counter
   * and correctly places the day and tenday marker base on the 
   * round number
   * 
   * @author Tom Stephens
   * @date Created:  Mar 10, 2008
   * @date Last Modified:  Mar 11, 2008
   */
  void drawTurnCounter();
  
  /**
   * @brief Check to see of someone  has won the game
   * 
   * This method checks to see if the Sathar have destroyed enough stations to win or
   * if the sathar retreat condition has been met granting a possible sathar victory.
   * If there is no winner this round it returns a 0.  If the UPF win, it returns a 1.
   * If the Sathar win it returns a 2.  And if it is a draw, it returns a 3.
   * 
   * @author Tom Stephens
   * @date Created:  Apr 15, 2008
   * @date Last Modified:  Apr 15, 2008
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
   * @date Last Modified:  Apr 16, 2008
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
  
};

};

#endif //_FGAME_H_
