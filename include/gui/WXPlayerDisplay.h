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
	 * This method takes in the DC from the game object and draws the player's fleets.
	 * It consults @c FMap::hasMap() before touching the @c FMap singleton at all
	 * (including before binding any reference/pointer to it) and returns
	 * immediately (drawing nothing) when the map does not exist, so a repaint
	 * reached while a game is unset or only partially loaded cannot dereference
	 * a NULL/half-built @c FMap. This closes the residual gap left open by the
	 * FR-2 pass-1 WXMapDisplay guard (SF-nullfmap-paint-guard); it is additive
	 * defense-in-depth and does not change behavior when a valid @c FMap exists.
	 *
	 * @param dc Device Context to draw to
	 * @param player Pointer to the FPlayer object to draw the fleets for
	 *
	 * @author Tom Stephens, Claude Sonnet 5 (medium)
	 * @date Created:  Feb 07, 2008
	 * @date Last Modified: Jul 19, 2026
	 *
	 * Note:  Jul 19, 2026 - added the FMap::hasMap() guard (SF-nullfmap-paint-guard, FR-2 pass-2).
	 */
	void drawFleets(wxDC &dc, FPlayer *player);


};

}
#endif /* WXPLAYERDISPALY_H_ */
