/*
 * @file WXGameDisplay.h
 * @brief Header file for the WXGameDisplay class
 * @author Tom Stephens
 * @date Created:  Aug 3, 2009
 */

#ifndef WXGAMEDISPALY_H_
#define WXGAMEDISPALY_H_

#include <wx/wx.h>

namespace Frontier
{

class FGame;

/**
 * @brief Interface class between the FGame class and wxWidgets
 *
 * This method provides the interface between the logic and data
 * of the FGame class and the wxWidgets display library
 *
 * @author Tom Stephens
 * @date Created:  Aug 3, 2009
 * @date Last modified:  Aug 3, 2009
 */
class WXGameDisplay {
public:
	WXGameDisplay();
	virtual ~WXGameDisplay();

	/**
	 * @brief Draws the strategic game state
	 *
	 * This method draws the strategic map, fleets, and turn counter using
	 * the GUI-side display helper classes.
	 *
	 * @param dc Device context to draw to
	 * @param game Strategic game state to render
	 */
	void draw(wxDC &dc, FGame &game);

private:
	void drawTurnCounter(wxDC &dc, const FGame &game);
};

}
#endif /* WXGAMEDISPALY_H_ */
