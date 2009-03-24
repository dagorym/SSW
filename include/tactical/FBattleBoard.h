/**
 * @file FBattleBoard.h
 * @brief Header file for BattleBoard class
 * @author Tom Stephens
 * @date Created:  Jul 11, 2008
 *
 */

#ifndef __FBattleBoard__
#define __FBattleBoard__

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
#include "strategic/FFleet.h"

#include <map>

namespace Frontier {
class FBattleScreen;

/// data structure to hold information about each hex on the map
typedef struct {
	/// x coordinate of hex center
	int cx;
	/// y coordinate of hex center
	int cy;
	/// List of ships in hex
	VehicleList ships;
} hexData;

/// data structure to hold information about the movements of a ship during a turn.
typedef struct {
	/// flag for whether or not the ship has moved yet this turn
	bool hasMoved;
	/// list of waypoint hexes along the ship's path
	std::vector<hexData> waypoints;
	/// list of turns made at the waypoints
	std::vector<int> turns;
	/// ship's final speed
	int speed;
	/// ship's final heading
	int finalHeading;
	/// ship's current heading
	int curHeading;
	/// number of hexes it has currently moved
	int nMoved;
} turnData;

/**
 * @brief Class for the Main tactical combat board
 *
 * This class implements the code for the FBattleBoard, the main
 * board used for the tactical combat game..
 *
 * @author Tom Stephens
 * @date Created:  Jul 11, 2008
 * @date Last Modified:  Nov 1, 2008
 */
class FBattleBoard : public wxScrolledWindow
{
public:
	/**
	 * @brief FBattleBoard constructor
	 *
	 * @author Tom Stephens
	 * @date Created:  Jul 11, 2008
	 * @date Last Modified:  Jul 30, 2008
	 */
	FBattleBoard(wxWindow * parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxHSCROLL|wxRAISED_BORDER|wxVSCROLL, const wxString &name = "BattleBoard" );
	/// Default destructor
	~FBattleBoard();

	void draw(wxDC &dc);
	void onPaint(wxPaintEvent & event);
	void onLeftUp(wxMouseEvent & event);

	/// sets the options for the planet icons
	void setPlanetImages(ImageList iList) { m_planetImages = iList; }
	/// sets the new scale for the map;
	void setScale(double factor);

	/**
	 * @brief Resets the turn information data
	 *
	 * This method clears and resets the m_turnInfo structure to hold information
	 * for the currently moving player.
	 *
	 * @author Tom Stephens
	 * @date Created:  Dec 03, 2008
	 * @date Last Modified:  Dec 03, 2008
	 */
	void resetMoveData();

	/**
	 * @brief Finalize movement phase
	 *
	 * This method iterates over the ships and updates their current status
	 * to reflect their movement.
	 *
	 * @author Tom Stephens
	 * @date Created:  Feb 8, 2009
	 * @date Last Modified:  Feb 8, 2009
	 */
	void finalizeMove();

protected:
	/// parent window
	FBattleScreen * m_parent;
	/// Scale of the map to draw
	double m_scale;
	/// Size of a hex across the flats (based on scale)
	int m_size;
	/// size of blank border around the map (based on scale)
	int m_trim;
	/// half the width across the flats (based on size)
	double m_d;
	/// one quarter of the height of the hex (based on size)
	double m_a;
	/// number of columns
	int m_nCol;
	/// number of rows
	int m_nRow;
	/// total width of map
	int m_width;
	/// total height of map
	int m_height;
	/// array of hex data for map
	hexData m_hexData[100][100];
	/// Flag for whether planet is present or not
	bool m_drawPlanet;
	/// position of planet
	hexData m_planetPosition;
	/// list of possible planet images to display
	ImageList m_planetImages;
	/// planet index to draw
	int m_planetChoice;
	/// flag for whether or not we are setting the rotation of a placed ship
	bool m_setRotation;
	/// m_hexData entry for hex containing currently selected ship
	hexData m_shipPos;
	/// list of hexes to highlight for movement
	std::vector<hexData> m_movementHexes;
	/// list of hexes to highlight for ADF range
	std::vector<hexData> m_movedHexes;
	/// list of hexes to highlight for left turn
	std::vector<hexData> m_leftHexes;
	/// list of hexes to highlight for right turn
	std::vector<hexData> m_rightHexes;
	/// flag for whether or not to draw the current ship's available path
	bool m_drawRoute;
	/// number of hexes moved
	int m_moved;
	/// list of turn data elements for each ship stored with the FVehicle ID value as the key
	std::map<unsigned int, turnData> m_turnInfo;


	/**
	 * @brief Draws the grid background
	 *
	 * This method draws the map grid background.  It takes into account the size of
	 * the buffer region around the map, the size of the hexes and the magnification and
	 * draws the grid and rescales the window appropriately.  It also takes into account
	 * the scrolled position of the map within the window.
	 *
	 * @param dc The device context to write to
	 *
	 * @author Tom Stephens
	 * @date Created:  Jul 19, 2008
	 * @date Last Modified:  Jul 30, 2008
	 */
	void drawGrid(wxDC &dc);

	/**
	 * @brief Sets all the map sizes based on the provided scale
	 *
	 * This method updates the scale of the map and updates all the drawing
	 * size constants based on the new scale. The scale is a simple ratio with
	 * a scale of 1.0 corresponding to hexes 50 pixels across the flats.
	 *
	 * @param scale  The new scale for the map
	 *
	 * @author Tom Stephens
	 * @date Created:  July 30, 2008
	 * @date last Modified:  July 30, 2008
	 */
	void setConstants(double scale);

	/**
	 * @brief Computes the centers of all the hexes
	 *
	 * This method uses the stored values for the various map scales to compute
	 * the center point of each hex on the map.
	 *
	 * @author Tom Stephens
	 * @date Created:  July 30, 2008
	 * @date last Modified:  July 30, 2008
	 */
	void computeCenters();

	/**
	 * @brief returns the array coordinates of the selected hex
	 *
	 * Using the mouse coordinates of the click and the scaling constants,
	 * this method determines the hex that was selected on the map.  It first
	 * estimates a small range of hexes to check and then computes the exact
	 * distance from the mouse click position to the center of each of those
	 * hexes.  The closest hex center is the one we want.
	 *
	 * If the mouse is clicked outside the hex grid the method returns false
	 * and the hex numbers are set to -1.  Otherwise the method returns true
	 * and the hex numbers correspond to the indices in the array of the
	 * selected hex
	 *
	 * @param x The x position of the mouse click
	 * @param y The y position of the mouse click
	 * @param a The x hex number
	 * @param b the y hex number
	 *
	 * @author Tom Stephens
	 * @date Created:  July 30, 2008
	 * @date last Modified:  July 30, 2008
	 */
	bool getHex(int x, int y, int &a, int &b);

	/**
	 * @brief Draws an object centered on the specified hex
	 *
	 * This method takes the specified object and draws it scaled and centered
	 * in the specified hex
	 *
	 * @param img The icon to be drawn
	 * @param pos The hex array coordinates to draw at
	 * @param rot The rotation of the image in hexsides relative to east
	 *
	 * @author Tom Stephens
	 * @date Created:  July 30, 2008
	 * @date Last Modified:  Oct 13, 2008
	 */
	void drawCenteredOnHex(wxImage img, hexData pos, int rot = 0);

	/**
	 * @brief Computes the distance between two hexes on the map
	 *
	 * This method determines the number of hexes in the shortest path
	 * between two specified hexes on the map
	 *
	 * @param sx The horizontal hex coordinate of the starting hex
	 * @param sy The vertical hex coordinate of the starting hex
	 * @param ex The horizontal hex coordinate of the ending hex
	 * @param ey The vertical hex coordinate of the ending hex
	 *
	 * @author Tom Stephens
	 * @date Created:  Sep 14, 2008
	 * @date Last Modified:  Sep 14, 2008
	 */
	int computeHexDistance(int sx, int sy, int ex, int ey);

	/**
	 * @brief Draws ships on map
	 *
	 * This method runs through all the hexes and if there are any ships
	 * there draws them on the map
	 *
	 * @author Tom Stephens
	 * @date Created:  Oct 13, 2008
	 * @date Last Modified:  Oct 13, 2008
	 */
	void drawShips();

	/**
	 * @brief Compute the heading value for the give mouse event
	 *
	 * This method compares the location of the mouse event to the location of the
	 * center of the hex the ship is in and determines the value to be stored as the heading
	 * of the currently selected ship.
	 *
	 * @param event The mouse event with the pointer coordinates
	 *
	 * @author Tom Stephens
	 * @date Created:  Oct 13, 2008
	 * @date Last Modified:  Oct 13, 2008
	 */
	int computeHeading(wxMouseEvent &event);

	/**
	 * @brief Handles responses to motions of the mouse
	 *
	 * When placing ships, this method updates the rotation of the currently selected ship as
	 * you move the mouse around to select it's initial rotation.
	 *
	 * @param event The mouse event with the pointer coordinates
	 *
	 * @author Tom Stephens
	 * @date Created:  Oct 13, 2008
	 * @date Last Modified:  Oct 13, 2008
	 */
	void onMotion(wxMouseEvent & event);

	/**
	 * @brief selects a vessel based on the clicked hex
	 *
	 * This method looks at the hex clicked and sets the current ship to the ship in
	 * the selected hex.  If more than one ship is in the hex a selection of choices
	 * is drawn
	 *
	 * @param event The mouse click event that triggered the call
	 *
	 * @author Tom Stephens
	 * @date Created:  Nov 1, 2008
	 * @date Last Modified:  Feb 13, 2009
	 */
	void selectVessel(wxMouseEvent &event);

	/**
	 * @brief Draws the current ships route
	 *
	 * This routine cycles over the m_movementHexes and m_ADFHexes vectors and highlights the appropriate
	 * hexes on the map
	 *
	 * @param dc The device context to draw on
	 *
	 * @author Tom Stephens
	 * @date Created:  Nov 21, 2008
	 * @date Last Modified:  Feb 13, 2009
	 */
	void drawRoute(wxDC &dc);

	/**
	 * @brief Sets the initial route for the selected ship
	 *
	 * This method fills in the m_movementHexes and m_ADFHexes vectors
	 * for the selected ship with a straight line going forward from
	 * it's current position.
	 *
	 * @author Tom Stephens
	 * @date Created:  Nov 21, 2008
	 * @date Last Modified:  Feb 13, 2009
	 */
	void setInitialRoute();

	/**
	 * @brief Finds the next hex in the specified direction
	 *
	 * Given the current hex array coordinates (passed in a hexData structure)
	 * and a heading, this method computes the next hex along the path.
	 *
	 * @param h hexData structure containing array coordinates of hex ship is in
	 * @param heading Hexside the ship is facing
	 *
	 * @author Tom Stephens
	 * @date Created:  Nov 21, 2008
	 * @date Last Modified:  Nov 21, 2008
	 */
	hexData findNextHex(hexData h, int heading);

	/**
	 * @brief Checks to see if the selected hex in along the valid paths
	 *
	 * This method checks to see if the selected hex is on the current ships
	 * valid path and if it has any available MR.  If so, it draws the
	 * available paths for the ship.
	 *
	 * @param event The mouse event with the coordinates
	 *
	 * @author Tom Stephens
	 * @date Created:  Nov 30, 2008
	 * @date Last Modified:  Feb 13, 2009
	 */
	void checkForTurn(wxMouseEvent &event);

	/**
	 * @brief Highlights the hexes in the specified list
	 *
	 * This method iterates over the list of passed in hexes and highlights them
	 * approrpriately based on whether or not the ship has to move that far or
	 * it falls within the ADF range of the ship.
	 *
	 * @param dc The device context to draw on
	 * @param list The list of hexes to higlight
	 * @param count The number of hexes on the path already drawn
	 *
	 * @author Tom Stephens
	 * @date Created:  Dec 7, 2008
	 * @date Last Modified:  Feb 13, 2009
	 */
	void drawRouteHexes(wxDC &dc, std::vector<hexData> list, int count=1);

	/**
	 * @brief Runs through a list of hexes to see if the current one is there
	 *
	 * This method runs through a list of hexes and checks its coordinates against
	 * a reference hex position.  If it is found, the method returns true.  If it is
	 * not found it returns false.  It also returns the position in the list of the
	 * matching hex
	 *
	 * @param list The list of hexes to check
	 * @param ref The reference hex to check against
	 * @param count The position in the list where the match occured
	 *
	 * @author Tom Stephens
	 * @date Creatd:  Dec 24, 2008
	 * @date Last Modified:  Dec 24, 2008
	 */
	bool findHexInList(std::vector<hexData> list, hexData ref, int &count);

	/**
	 * @brief draws a line connecting the hexes in the specified list
	 *
	 * This method iterates over the list of passed in hexes and highlights them
	 * approrpriately based on whether or not the ship has to move that far or
	 * it falls within the ADF range of the ship.
	 *
	 * @param dc The device context to draw on
	 * @param list The list of hexes to higlight
	 * @param current  Flag for whether or not you are drawing the movement for the currently selected ship or not
	 *
	 * @author Tom Stephens
	 * @date Created:  Feb 8, 2009
	 * @date Last Modified:  Feb 13, 2009
	 */
	void drawMovedHexes(wxDC &dc, std::vector<hexData> list, bool current=false);

	/**
	 * @brief Compute the remaining moves for the current ship
	 *
	 * @param start the hex to start computing from
	 *
	 * @author Tom Stephens
	 * @date Created:  Feb 8, 2009
	 * @date Last Modified:  Feb 8, 2009
	 */
	void computeRemainingMoves(hexData start);

	/**
	 * @brief Checks to see if all the ships are done moving
	 *
	 * This method iterates through all the current players ships and checks to see
	 * if they have moved at least the minimum distance required by their current
	 * speed and ADF.
	 *
	 * @author Tom Stephens
	 * @date Created:  Feb 8, 2009
	 * @date Last Modified:  Feb 8, 2009
	 */
	void checkMoveStatus();

	/**
	 * @brief Turns ship and keeps heading value in proper range
	 *
	 * @param heading The ships inital heading
	 * @param turn The turn direction (-1 left, 1 right)
	 *
	 * @author Tom Stephens
	 * @date Created:  Feb 13, 2009
	 * @date Last Modified:  Feb 13, 2009
	 */
	int turnShip(int & heading, int turn);

	/**
	 * @brief Draws a box on the screen with a list of ships to choose from
	 *
	 * This method draws a box on the screen that lists all the ships in the
	 * supplied Vehicle list.  Once one of the ship names are selected, it
	 * returns a pointer to the specified ship.
	 *
	 * @param list The list of ships to display.
	 *
	 * @author Tom Stephens
	 * @date Created:  Feb 13, 2009
	 * @date Last Modified:  Feb 13, 2009
	 */
FVehicle * pickShip(const VehicleList & list);
};

}

#endif //__FBattleBoard__
