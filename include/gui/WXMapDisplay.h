/*
 * @file WXMapDisplay.h
 * @brief Header file for the WXMapDisplay class
 * @author Tom Stephens, Claude Sonnet 5 (medium)
 * @date Created:  Aug 2, 2009
 * @date Last Modified: Jul 19, 2026
 */

#ifndef WXMAPDISPALY_H_
#define WXMAPDISPALY_H_

#include <wx/wx.h>
//#include "strategic/FMap.h"

namespace Frontier
{

/**
 * @brief Interface class between the FMap class and wxWidgets
 *
 * This method provides the interface between the logic and data
 * of the FMap class and the wxWidgets display library
 *
 * @author Tom Stephens
 * @date Created:  Aug 2, 2009
 * @date Last modified:  Aug 2, 2009
 */
class WXMapDisplay {
public:
	WXMapDisplay();
	virtual ~WXMapDisplay();

	/**
	 * @brief Draw the map
	 *
	 * This method draws the map on the display. It consults
	 * @c FMap::hasMap() before touching the @c FMap singleton and returns
	 * immediately (drawing nothing) when the map does not exist, so a
	 * repaint reached while a game is unset or only partially loaded cannot
	 * dereference a NULL/half-built @c FMap. This guard is independent of
	 * (and in addition to) the @c FMainFrame::onOpen() call-site mitigation
	 * that defers @c setGame(...) until a load succeeds.
	 *
	 * @param dc the Device Context to draw to
	 *
	 * @author Tom Stephens, Claude Sonnet 5 (medium)
	 * @date Created:  Jan 17, 2005
	 * @date Last Modified: Jul 19, 2026
	 *
	 * Note:  Aug 2, 2009 - moved from FMap class here.
	 * Note:  Jul 19, 2026 - added the FMap::hasMap() guard (SF-nullfmap-paint-guard).
	 */
	void draw(wxDC &dc/*, unsigned int id*/);

	/**
	 * @brief returns the scale for the current window dispaly
	 *
	 * This method determines the current scale based on the map size
	 * and the size of the current window. When no @c FMap singleton exists
	 * yet (@c FMap::hasMap() is false), this returns a safe default scale
	 * of 1.0 instead of dereferencing the NULL/half-built map.
	 *
	 * @param dc The device context of the window.
	 *
	 * @author Tom Stephens, Claude Sonnet 5 (medium)
	 * @date Created:  Feb 11, 2008
	 * @date Last Modified: Jul 19, 2026
	 *
	 * Note:  Aug 2, 2009 - moved from FMap class here.
	 * Note:  Jul 19, 2026 - added the FMap::hasMap() guard (SF-nullfmap-paint-guard).
	 */
	const double getScale(wxDC &dc) const;



};

}
#endif /* WXMAPDISPALY_H_ */
