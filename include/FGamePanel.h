/**
 * @file FGamePanel.h
 * @brief Header file for FGamePanel class
 * @author Tom Stephens
 * @date Created:  Jan 31, 2018
 */
#pragma once
#include <wx/wx.h>
#include "strategic/FGame.h"
#include "core/FGameConfig.h"
#include "strategic/FGame.h"

using namespace Frontier;
/**
 * @brief wxPanel derived class for drawing game map
 *
 * This class is simply a drawing class to allow the game
 * to draw and interact with the game map.  
 *
 * It was added because of issues with the wxWidgets 3.0
 * library not having consistent behavior with drawing and
 * refreshing frames on Windows & Linux.
 *
 * @author Tom Stephens
 * @date Created: Jan 31, 2018
 * @date Last Modified: Jan 31, 2018
 */
class FGamePanel : public wxPanel
{
public:
	FGamePanel(wxFrame * parent);
	~FGamePanel();

	/**
	* @brief Method to handle the onPaint events
	*
	* This method redraws the map as needed on wxEVT_PAINT events
	*
	* @param event The window event to handle
	*
	* @author Tom Stephens
	* @date Created:  Jan 31, 2018
	* @date Last Modified:  Jan 31, 2018
	*/
	void onPaint(wxPaintEvent & event);
	void setGame(FGame * game) { m_game = game; }
	void clearGame() { m_game = NULL; }

private:
	wxWindow * m_parent;
	FGame * m_game;
};

