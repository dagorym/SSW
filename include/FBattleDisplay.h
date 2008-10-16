/**
 * @file FBattleDisplay.h
 * @brief Header file for BattleDisplay class
 * @author Tom Stephens
 * @date Created:  Jul 11, 2008
 *
 */

#ifndef __FBattleDisplay__
#define __FBattleDisplay__

#include <wx/scrolwin.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/frame.h>

#include <vector>

#include "Frontier.h"

namespace Frontier {
class FBattleScreen;

/**
 * @brief Class for the Main tactical combat board
 *
 * This class implements the code for the FBattleDisplay, the main
 * board used for the tactical combat game..
 *
 * @author Tom Stephens
 * @date Created:  Jul 11, 2008
 * @date Last Modified:  Aug 31, 2008
 */
class FBattleDisplay : public wxPanel
{
public:
	/**
	 * @brief FBattleDisplay constructor
	 *
	 * @author Tom Stephens
	 * @date Created:  Jul 11, 2008
	 * @date Last Modified:  Jul 19, 2008
	 */
	FBattleDisplay(wxWindow * parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxSUNKEN_BORDER|wxTAB_TRAVERSAL , const wxString &name = "BattleDisplay" );
	/// Default destructor
	~FBattleDisplay();

	void draw(wxDC &dc);
	void onPaint(wxPaintEvent & event);
	void onLeftUp(wxMouseEvent & event);

	/// Set the list of planet images
	void setImageList(ImageList l) { m_imageList = l;}

protected:
	/// a list of images to display
	ImageList m_imageList;
	/// parent window
	FBattleScreen * m_parent;
	/// zoom graphic
	wxImage m_zoomImage;
	/// flag for whether or not the ship list has been loaded
	bool m_loaded;
	/// list of ships for current fleet
	VehicleList m_vList;

	/**
	 * @brief Draws choice of planet icons on display
	 *
	 * This method takes the list of possible planet icons and draws them on the screen so
	 * that the player can choose the image he wants on the map
	 *
	 * @param dc The device context to draw on
	 *
	 * @author Tom Stephens
	 * @date Created:  Aug 31, 2008
	 * @date Last Modified:  Aug 31, 2008
	 */
	void drawPlanetChoices (wxDC &dc);

	/**
	 * @brief Determine which planet icon the user selected
	 *
	 * This method looks at the mouse position of the click and determines which of the image
	 * icons the user selected for the planet.  The choice is stored in the m_choice variable.
	 *
	 * @param event The mouse event that triggered the selection.
	 *
	 * @author Tom Stephens
	 * @date Created:  Aug 31, 2008
	 * @date Last Modified:  Aug 31, 2008
	 */
	void makePlanetChoice(wxMouseEvent & event);

	/**
	 * @brief Draws the prompt to place the planet on the hex map
	 *
	 * @param dc The device context to draw on
	 *
	 * @author Tom Stephens
	 * @date Created:  Aug 31, 2008
	 * @date Last Modified:  Aug 31, 2008
	 */
	void drawPlacePlanet(wxDC &dc);

	/**
	 * @brief Draws the prompt to place the ship on the hex map
	 *
	 * @param dc The device context to draw on
	 *
	 * @author Tom Stephens
	 * @date Created:  Oct 13, 2008
	 * @date Last Modified:  Oct 13, 2008
	 */
	void drawPlaceShip(wxDC &dc);

	/**
	 * @brief Draws the prompt to place the station on the hex map
	 *
	 * @param dc The device context to draw on
	 *
	 * @author Tom Stephens
	 * @date Created:  Sep 8, 2008
	 * @date Last Modified:  Sep 8, 2008
	 */
	void drawPlaceStation(wxDC &dc);

	/**
	 * @brief Draws the ships of the fleet to be placed
	 *
	 * This method draws the ships of the fleet that still need to be
	 * placed on the battle board
	 *
	 * @param dc The device context to draw on
	 *
	 * @author Tom Stephens
	 * @date Created:  Sep 8, 2008
	 * @date Last Modified:  Sep 8, 2008
	 */
	void drawShipChoices(wxDC &dc);

	/**
	 * @brief Determine which ship icon the user selected
	 *
	 * This method looks at the mouse position of the click and determines which of the image
	 * icons the user selected for the ship.
	 *
	 * @param event The mouse event that triggered the selection.
	 *
	 * @author Tom Stephens
	 * @date Created:  Oct 13, 2008
	 * @date Last Modified:  Oct 13, 2008
	 */
	void makeShipChoice(wxMouseEvent & event);

	/**
	 * @brief Zooms the battle map
	 *
	 * This method determines whether the map should zoom in or out based on where the user
	 * clicked on the zoom icon and transmits that to the battle screen for relay to the
	 * FBattleMap class
	 *
	 * @param event The mouse event that contains the position information
	 *
	 * @author Tom Stephens
	 * @date Created:  Oct 08, 2008
	 * @date Last Modified:  Oct 08, 2008
	 */
	void zoomMap(wxMouseEvent & event);


};

}

#endif //__FBattleDisplay__
