/**
 * @file FMainBattleSimFrame.h
 * @brief Header file for FMainBattleSimFrame class
 * @author Tom Stephens
 * @date Created:  Jun 01, 2010
 *
 */

#ifndef _FMAINFRAME_H_
#define _FMAINFRAME_H_

#include <wx/wx.h>
#include "strategic/FGame.h"
#include "core/FGameConfig.h"

/**
 * @brief Main frame class for the SSW Battle Sim
 *
 * This class is the main user interface window for the SSW Battle Sim.  It builds
 * all the menus and handles the user interface
 *
 * @author Tom Stephens
 * @date Created:  Jun 01, 2010
 * @date Last Modified:  Jun 01, 2010
 */
class FMainBattleSimFrame : public wxFrame {
public:
  /**
   * @brief FMainBattleSimFrame Constuctor
   *
   * This is the constructor method for the FMainBattleSimFrame class.
   *
   * @param title  The frame title
   * @param pos The screen position of the top right corner for the frame
   * @param size The size, in pixels, of the window
   *
   * @author Tom Stephens
   * @date Created:  Feb 28, 2005
   * @date Last Modified:  Feb 19, 2008
   */
  FMainBattleSimFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

  /**
   * @brief Method to handle the File:Exit menu option
   *
   * This is the method called when the File:Exit menu option is selected
   *
   * @param event The wxWidget window event that triggered the function call
   *
   * @author Tom Stephens
   * @date Created:  Feb 28, 2005
   * @date Last Modified:  Feb 28, 2005
   */
  void onQuit(wxCommandEvent& event);

  /**
   * @brief Method to handle the Help:About menu option
   *
   * This is the method called when the Help:About menu option is selected
   *
   * @param event The wxWidget window event that triggered the function call
   *
   * @author Tom Stephens
   * @date Created:  Feb 28, 2005
   * @date Last Modified:  Mar 25, 2008
   */
  void onAbout(wxCommandEvent& event);

  /**
   * @brief Method to handle the File:Open menu option
   *
   * This is the method called when the File:Open menu option is selected
   *
   * @param event The wxWidget window event that triggered the function call
   *
   * @author Tom Stephens
   * @date Created:  Mar 02, 2005
   * @date Last Modified:  Mar 25, 2008
   */
  void onOpen(wxCommandEvent& event);

  /**
   * @brief Method to handle the File:Save menu option
   *
   * This is the method called when the File:Save menu option is selected
   *
   * @param event The wxWidget window event that triggered the function call
   *
   * @author Tom Stephens
   * @date Created:  Mar 02, 2005
   * @date Last Modified:  Mar 05, 2008
   */
  void onSave(wxCommandEvent& event);

  /**
   * @brief Method to handle the File:Close menu option
   *
   * This is the method called when the File:Close menu option is selected
   *
   * @param event The wxWidget window event that triggered the function call
   *
   * @author Tom Stephens
   * @date Created:  Mar 17, 2005
   * @date Last Modified:  Mar 17, 2008
   */
  void onClose(wxCommandEvent& event);

  /**
   * @brief Method to handle the File:New menu option
   *
   * This is the method called when the File:New menu option is selected
   *
   * @param event The wxWidget window event that triggered the function call
   *
   * @author Tom Stephens
   * @date Created:  Mar 02, 2005
   * @date Last Modified:  Mar 09, 2008
   */
  void onNew(wxCommandEvent& event);

  /**
   * @brief Method to handle the Show:Players menu option
   *
   * This is the method called when the Show:Players menu option is selected.  It checks to
   * see if the game has been initalized.  If so, it shows the player list.  Otherwise it
   * gives a dialog box saying that the game has not yet been initalized
   *
   * @param event The wxWidget window event that triggered the function call
   *
   * @author Tom Stephens
   * @date Created:  May 25, 2005
   * @date Last Modified:  Jan 28, 2008
   */
  void onShowPlayers(wxCommandEvent& event);

  /**
   * @brief Method to handle the onPaint events
   *
   * This method redraws the map as needed on wxEVT_PAINT events
   *
   * @param event The window event to handle
   *
   * @author Tom Stephens
   * @date Created:  Feb 03, 2008
   * @date Last Modified:  Feb 03, 2008
   */
  void onPaint(wxPaintEvent & event);

  /**
   * @brief Process the end of the UPF player's turn
   *
   * This method processes the end of the UPF player's turn and
   * updates the game information accordingly
   *
   * @param event The wxWidget window event that triggered the function call
   *
   * @author Tom Stephens
   * @date Created:  Feb 10, 2008
   * @date Last Modified:  Mar 25, 2008
   */
  void onEndUPFTurn(wxCommandEvent& event);

  /**
   * @brief Process the end of the Sathar player's turn
   *
   * This method processes the end of the Sathar player's turn and
   * updates the game information accordingly
   *
   * @param event The wxWidget window event that triggered the function call
   *
   * @author Tom Stephens
   * @date Created:  Feb 10, 2008
   * @date Last Modified:  Mar 25, 2008
   */
  void onEndSatharTurn(wxCommandEvent& event);

  /**
   * @brief Process double clicking on the window
   *
   * This method processes a double click of the left mouse button
   *
   * @param event The wxWidget window event that triggered the function call
   *
   * @author Tom Stephens
   * @date Created:  Feb 11, 2008
   * @date Last Modified:  Feb 19, 2008
   */
  void onLeftDClick(wxMouseEvent& event);

  /**
   * @brief Place Stike Force Nova on the board
   *
   * This method places Strike Force Nova on the board.  It first checks to see
   * that it is the UPF player's turn and if so, rolls the dice to see where the
   * fleet shows up.
   *
   * @param event The wxWidget window event that triggered the function call
   *
   * @author Tom Stephens
   * @date Created:  Feb 19, 2008
   * @date Last Modified:  Feb 19, 2008
   */
  void onPlaceNova(wxCommandEvent& event);

  /**
   * @brief Process double clicking on the window
   *
   * This method processes the release of the left mouse button
   *
   * @param event The wxWidget window event that triggered the function call
   *
   * @author Tom Stephens
   * @date Created:  Mar 11, 2008
   * @date Last Modified:  Mar 11, 2008
   */
  void onLeftUp(wxMouseEvent& event);

  /**
   * @brief Handles changing the size of the window
   *
   * This method catches a resizing event for the window and redaws
   * the map
   *
   * @param event The wxWidget window event that triggered the function call
   *
   * @author Tom Stephens
   * @date Created:  Mar 15, 2008
   * @date Last Modified:  Mar 15, 2008
   */
  void onSize(wxSizeEvent& event);

  /**
   * @brief Handles selection of Add Sathar Fleets menu option
   *
   * This method handles the selection of the Add Sathar Fleets menu
   * option and allow the Sathar player to place additional ships on
   * the map following the placement rules of the game
   *
   * @param event The wxWidget window event that triggered the function call
   *
   * @author Tom Stephens
   * @date Created:  Mar 25, 2008
   * @date Last Modified:  Mar 25, 2008
   */
  void onAddSatharShips(wxCommandEvent& event);

  /**
   * @brief Shows the Sathar retreat conditions
   *
   * This method call the FGame method that shows what the
   * Sathar retreat condition is for the game
   *
   * @author Tom Stephens
   * @date Created:  Apr 15, 2008
   * @date Last Modified:  Apr 15, 2008
   */
  void onShowRetreatConditions(wxCommandEvent& event);

  /**
   * @brief Shows the tactical combat board
   *
   * This method call the FBattleScreen method that allows the players to
   * play the tactical board game
   *
   * @author Tom Stephens
   * @date Created:  Jul 11, 2008
   * @date Last Modified:  Jul 11, 2008
   */
  void onShowBattleScreen(wxCommandEvent& event);

  ///Default destructor
  virtual ~FMainBattleSimFrame();

  /// Event table constuction
  DECLARE_EVENT_TABLE()

private:
  /// Pointer to FGame object
  Frontier::FGame *m_game;
  /// flag for whether or not SF Nova has been placed
  bool m_novaPlaced;
  /// Game configuration object
  Frontier::FGameConfig * m_gameConfig;

  /**
   * @brief deletes the game and resets the window
   *
   * This method deletes the current game object, clears the window and
   * resets the menu bar to the proper state.
   *
   * @author Tom Stephens
   * @date Created:  Mar 18, 2008
   * @date Last Modified:  Mar 18, 2008
   */
  void resetGame();

};

#endif //_FMAINFRAME_H_
