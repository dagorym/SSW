/**
 * @file FGamePanel.h
 * @brief Header file for FGamePanel class
 * @author Tom Stephens
 * @date Created:  Jan 31, 2018
 */
#pragma once
#include <wx/wx.h>
#include "strategic/FGame.h"
#include "gui/WXGameDisplay.h"
#include "core/FGameConfig.h"
#include "strategic/FGame.h"

using namespace Frontier;
/**
 * @brief wxPanel derived class for drawing the game map
 *
 * This class is a drawing surface that lets the game render and interact with
 * the strategic map through WXGameDisplay.
 *
 * It was added because of issues with the wxWidgets 3.0
 * library not having consistent behavior with drawing and
 * refreshing frames on Windows & Linux.
 *
 * @author Tom Stephens, gpt-5.3-codex (medium), claude-sonnet-4-6 (medium)
 * @date Created: Jan 31, 2018
 * @date Last Modified: Jul 12, 2026
 */
class FGamePanel : public wxPanel
{
public:
	/**
	 * @brief Construct a drawing panel for the owning frame
	 *
	 * @param parent The frame that owns this panel
	 *
	 * @author Tom Stephens, gpt-5.3-codex (medium)
	 * @date Created: Jan 31, 2018
	 * @date Last Modified: Mar 28, 2026
	 */
	FGamePanel(wxFrame * parent);

	/**
	 * @brief Release the panel resources
	 *
	 * @author Tom Stephens
	 * @date Created: Jan 31, 2018
	 * @date Last Modified: Jan 31, 2018
	 */
	~FGamePanel();

	/**
	 * @brief Method to handle the onPaint events
	 *
	 * This method redraws the map as needed on wxEVT_PAINT events through the
	 * shared WXGameDisplay renderer. Uses a single wxPaintDC on all platforms
	 * (H7): the prior Linux-only wxClientDC branch is removed so the paint
	 * path is uniform and respects the update-region clip.
	 *
	 * @param event The window event to handle
	 *
	 * @author Tom Stephens, claude-sonnet-4-6 (medium)
	 * @date Created:  Jan 31, 2018
	 * @date Last Modified:  Jul 12, 2026
	 */
	void onPaint(wxPaintEvent & event);

	/**
	 * @brief Set the game instance rendered by the panel
	 *
	 * @param game The game to draw
	 *
	 * @author Tom Stephens
	 * @date Created: Jan 31, 2018
	 * @date Last Modified: Jan 31, 2018
	 */
	void setGame(FGame * game) { m_game = game; }

	/**
	 * @brief Clear the game instance rendered by the panel
	 *
	 * @author Tom Stephens
	 * @date Created: Jan 31, 2018
	 * @date Last Modified: Jan 31, 2018
	 */
	void clearGame() { m_game = NULL; }

private:
	wxWindow * m_parent;
	FGame * m_game;
	WXGameDisplay m_gameDisplay;
};
