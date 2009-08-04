/*
 * @file WXGameDisplay.h
 * @brief Header file for the WXGameDisplay class
 * @author Tom Stephens
 * @date Created:  Aug 3, 2009
 */

#ifndef WXGAMEDISPALY_H_
#define WXGAMEDISPALY_H_

#include <wx/wx.h>
#include "strategic/FGame.h"

namespace Frontier
{

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

};

}
#endif /* WXGAMEDISPALY_H_ */
