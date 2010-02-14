/*
 * @file WXMapDisplay.h
 * @brief Header file for the WXMapDisplay class
 * @author Tom Stephens
 * @date Created:  Aug 2, 2009
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
	 * This method draws the map on the display
	 *
	 * @param dc the Device Context to draw to
	 *
	 * @author Tom Stephens
	 * @date Created:  Jan 17, 2005
	 * @date Last Modified:  Aug 2, 2009
	 *
	 * Note:  Aug 2, 2009 - moved from FMap class here.
	 */
	void draw(wxDC &dc/*, unsigned int id*/);

	/**
	 * @brief returns the scale for the current window dispaly
	 *
	 * This method determines the current scale based on the map size
	 * and the size of the current window
	 *
	 * @param dc The device context of the window.
	 *
	 * @author Tom Stephens
	 * @date Created:  Feb 11, 2008
	 * @date Last Modified:  Aug 2, 2009
	 *
	 * Note:  Aug 2, 2009 - moved from FMap class here.
	 */
	const double getScale(wxDC &dc) const;



};

}
#endif /* WXMAPDISPALY_H_ */
