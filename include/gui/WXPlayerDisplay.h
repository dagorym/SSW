/*
 * @file WXPlayerDisplay.h
 * @brief Header file for the WXPlayerDisplay class
 * @author Tom Stephens
 * @date Created:  Aug 3, 2009
 */

#ifndef WXPLAYERDISPALY_H_
#define WXPLAYERDISPALY_H_

#include <wx/wx.h>
#include "strategic/FPlayer.h"

namespace Frontier
{

/**
 * @brief Interface class between the FPlayer class and wxWidgets
 *
 * This method provides the interface between the logic and data
 * of the FPlayer class and the wxWidgets display library
 *
 * @author Tom Stephens
 * @date Created:  Aug 3, 2009
 * @date Last modified:  Aug 3, 2009
 */
class WXPlayerDisplay {
public:
	WXPlayerDisplay();
	virtual ~WXPlayerDisplay();

	/**
	 * @brief Draws the player's fleets on the map
	 *
	 * This method takes in the DC from the game object and draws the player's fleets
	 *
	 * @param dc Device Context to draw to
	 * @param player Pointer to the FPlayer object to draw the fleets for
	 *
	 * @author Tom Stephens
	 * @date Created:  Feb 07, 2008
	 * @date Last Modified:  Mar 14, 2008
	 */
	void drawFleets(wxDC &dc, FPlayer *player);


};

}
#endif /* WXPLAYERDISPALY_H_ */
