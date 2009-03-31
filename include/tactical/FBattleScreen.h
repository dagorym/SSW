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
 * @date Last Modified:  Oct 26, 2008
 */
class FBattleScreen : public wxFrame
{
public:
	/**
	 * @brief FBattleScreen constructor
	 *
	 * @author Tom Stephens
	 * @date Created:  Jul 11, 2008
	 * @date Last Modified:  Jul 19, 2008
	 */
	FBattleScreen(const wxString& title = "Star Frontiers Knight Hawks Battle Board", const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 750,550 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
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
	 * @brief return a copy of the list of the current side's ships
	 *
	 * This method looks at the m_side variable and returns a list of that
	 * sides ships
	 *
	 * @author Tom Stephens
	 * @date Created:  Sep 8, 2008
	 * @date Last Modified:  Sep 8, 2008
	 */
	VehicleList getShipList() const;

	/**
	 * @brief Returns heading from one hex to another
	 *
	 * This method takes as input a source and destination hex (grid positions) and computes
	 * the closest heading (hex facing) from the source to the destination.  The computed
	 * heading is returned to the user
	 *
	 * @param s source hex
	 * @param d destination hex
	 *
	 * @author Tom Stephens
	 * @date Created:  Oct 26, 2008
	 * @date Last Modified:  Oct 26, 2008
	 */
	int computeHeading(FPoint s, FPoint d);

	/**
	 * @brief Computes angle between two hexes
	 *
	 * This returns the angle in degrees between the two passed hexes
	 *
	 * @param s the source hex
	 * @param d the destination hex
	 *
	 * @author Tom Stephens
	 * @date Created:  Nov 1, 2008
	 * @date Last Modified:  Nov 1, 2008
	 */
	double computeHexAngle(FPoint s, FPoint d);

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
	bool getSide() { return m_side;}
	/// set side
	void setSide(bool f) { m_side = f; }
	/// toggle side
	void toggleSide() { m_side = !m_side; std::cerr << "It is now side " << m_side << "'s turn." << std::endl; }
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
	const unsigned int & getCurPlayerID() const { return m_side?getAttackerID():getDefenderID(); }
	/// sets the movement status
	void setMoveComplete(bool s);
	/// get the movement status
	const bool & isMoveComplete() const { return m_moveComplete; }


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
	bool m_side;
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

};

}

#endif //__FBattleScreen__
