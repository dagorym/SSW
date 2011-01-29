/**
 * @file FBattleScreen.h
 * @brief Header file for BattleScreen class
 * @author Tom Stephens
 * @date Created:  Jul 11, 2008
 *
 */

#ifndef __FBattleScreen__
#define __FBattleScreen__

#include <wx/scrolwin.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/frame.h>
#include <wx/dialog.h>
#include <wx/utils.h>

#include "Frontier.h"
#include "tactical/FBattleBoard.h"
#include "tactical/FBattleDisplay.h"

namespace Frontier {

/**
 * @brief Class for the Main tactical combat board
 *
 * This class implements the code for the FBattleScreen, the main
 * board used for the tactical combat game..
 *
 * @author Tom Stephens
 * @date Created:  Jul 11, 2008
 * @date Last Modified:  Jan 28, 2011
 */
class FBattleScreen : public wxDialog //wxFrame
{
public:
	/**
	 * @brief FBattleScreen constructor
	 *
	 * @author Tom Stephens
	 * @date Created:  Jul 11, 2008
	 * @date Last Modified:  Jul 19, 2008
	 */
	FBattleScreen(const wxString& title = "Star Frontiers Knight Hawks Battle Board", const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 750,550 ), long style = wxDEFAULT_DIALOG_STYLE|wxMINIMIZE_BOX|wxMAXIMIZE_BOX|wxRESIZE_BORDER );
//	FBattleScreen(const wxString& title = "Star Frontiers Knight Hawks Battle Board", const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 750,550 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
	/// Default destructor
	~FBattleScreen();

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
	 * @date Last Modified:  Apr 22, 2010
	 */
	void fireICM();

	/// get the battle board state
	const int & getState() const { return m_state; }
	/// set the battle board state
	void setState(int s);
	/// toggle the control state
	void toggleControlState() { m_control = !m_control; }
	/// get the control state
	const bool & getControlState() const { return m_control; }
	/// sets the id of the planet chosen
	void setPlanet(int c) { m_planetChoice = c; }
	/// get the value of the choice variable
	const int & getPlanetChoice() const { return m_planetChoice; }
	/// set planet position
	void setPlanetPosition(FPoint h){ m_planetPos= h;}
	/// get the station's current position
	const FPoint & getPlanetPos() const { return m_planetPos; }
	/// set Station position
	void setStationPosition(FPoint h){ m_stationPos= h;}
	/// get the station's current position
	const FPoint & getStationPos() const { return m_stationPos; }
	/// get pointer to the station object
	FVehicle * getStation() const { return m_station; }
	/// change the scale of the map
	void setScale(double factor);
	/// get the current ship selection
	FVehicle * getShip() const { return m_curShip; }
	/// set the current ship selection
	void setShip(FVehicle * s) { m_curShip = s; }
	/// get done state
	bool getDone() { return m_done;}
	/// set done state
	void setDone(bool f) { m_done = f; }
	/// get side
	bool getActivePlayer() { return m_activePlayer;}
	/// set side
	void setActivePlayer(bool f) { m_activePlayer = f; }
	/// toggle side
	void toggleActivePlayer() { m_activePlayer = !m_activePlayer; /*std::cerr << "It is now side " << m_activePlayer << "'s turn." << std::endl;*/ }
	/// get the current battle phase
	int getPhase() { return m_phase;}
	/// set the battle phase
	void setPhase(int p);
	/// redraw the screen
	void reDraw() { m_map->Refresh(); m_display->Refresh(); }
	/// returns the ID of the attacking player
	const unsigned int & getAttackerID() const { return m_playerID[1]; }
	/// returns the ID of the defending player
	const unsigned int & getDefenderID() const { return m_playerID[0]; }
	/// returns the ID of the player who's turn it currently is
	const unsigned int & getActivePlayerID() const { return m_activePlayer?getAttackerID():getDefenderID(); }
	/// sets the movement status
	void setMoveComplete(bool s);
	/// get the movement status
	const bool & isMoveComplete() const { return m_moveComplete; }
	/// returns the ID of the player who's moving this turn
	const unsigned int & getMovingPlayerID() const { return m_movingPlayer?getAttackerID():getDefenderID(); }
	/// toggles the moving player flag
	void toggleMovingPlayer() { m_movingPlayer = !m_movingPlayer; /*std::cerr << "It is now side " << m_movingPlayer << "'s move." << std::endl;*/ }
	/// set the current weapon
	void setWeapon(FWeapon * w);
	/// get the current weapon
	FWeapon * getWeapon() { return m_curWeapon; }
	/// list of ICM targets
	std::vector<ICMData *> m_ICMData;

protected:
	/// The map window
	FBattleBoard * m_map;
	/// The game display window
	FBattleDisplay * m_display;
	/// list of attacking fleets
	FleetList * m_attackList;
	/// list of attacking ships
	VehicleList * m_attackShips;
	/// list of defending fleets
	FleetList * m_defendList;
	/// list of defending ships
	VehicleList * m_defendShips;
	/// planet flag
	bool m_hasPlanet;
	/// pointer to station object
	FVehicle * m_station;
	/// display state flag
	int m_state;
	/// flag for selecting which sub-window has control true = board board false = display
	bool m_control;
	/// planet choice variable
	int m_planetChoice;
	/// chosen planet position
	FPoint m_planetPos;
	/// whose turn is it, true=attacker, false=defender
	bool m_activePlayer;
	/// station position
	FPoint m_stationPos;
	/// currently selected ship
	FVehicle * m_curShip;
	/// done flag
	bool m_done;
	///  Turn phase state variable
	int m_phase;
	/// player IDs
	unsigned int m_playerID[2];
	/// flag for status of movement.  If true all ships have been moved their minimum and the player can end their turn
	bool m_moveComplete;
	/// flag to indicate player whose movement turn it is true=attacker false=defender
	bool m_movingPlayer;
	/// pointer to currently selected weapon
	FWeapon * m_curWeapon;
//	/// window disabler object
//	wxWindowDisabler *m_wd;

	// Print a dialog declaring the  winner and exit the window
	void declareWinner();

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
