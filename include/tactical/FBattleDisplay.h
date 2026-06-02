/**
 * @file FBattleDisplay.h
 * @brief Header file for BattleDisplay class
 * @author Tom Stephens, gpt-5.4 (high), claude-sonnet-4-6 (standard), claude-sonnet-4-6 (medium)
 * @date Created:  Jul 11, 2008
 * @date Last Modified: Jun 02, 2026
 *
 */

#ifndef __FBattleDisplay__
#define __FBattleDisplay__

#include <vector>

#include "gui/GuiTypes.h"
#include "Frontier.h"

namespace Frontier {
class FBattleScreen;

/**
 * @brief Class for the Main tactical combat board
 *
 * This class implements the lower tactical display panel used for combat
 * prompts, ship status, setup placement controls, and seeker-activation UI.
 *
 * @author Tom Stephens, gpt-5.4 (high), claude-sonnet-4-6 (medium)
 * @date Created:  Jul 11, 2008
 * @date Last Modified:  Jun 02, 2026
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

	/**
	 * @brief Draw the current lower-panel surface for the active tactical state.
	 *
	 * This includes the existing movement/fire/setup prompts plus the seeker
	 * activation instructions, per-seeker activation rows, and completion button
	 * during `PH_SEEKER_ACTIVATION`.
	 *
	 * @param dc Device context used for lower-panel drawing.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: May 25, 2026
	 * @date Last Modified: May 25, 2026
	 */
	void draw(wxDC &dc);
	void onPaint(wxPaintEvent & event);
	/**
	 * @brief Handle lower-panel clicks for the current tactical phase.
	 *
	 * Setup placement clicks still select deployment-source rows, while seeker
	 * activation clicks now prioritize per-seeker activation rows so the panel
	 * can refresh after each one-way activation.
	 *
	 * @param event Mouse-release event carrying the click position.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: May 25, 2026
	 * @date Last Modified: May 25, 2026
	 */
	void onLeftUp(wxMouseEvent & event);

	/**
	 * @brief Recompute lower-panel geometry and minimum height for current size.
	 *
	 * Runs prompt reservation and ship-stat placement calculations using the
	 * current widget geometry so parent resize handlers can force deterministic
	 * lower-panel reflow before applying screen-level sizing policy.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created:  May 16, 2026
	 * @date Last Modified:  May 16, 2026
	 */
	void reflowLowerPanelLayout();

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
	/// generic input spin control for entering numerical values
	wxSpinCtrl* m_spinCtrl1;
	/// generic button control
	wxButton* m_button1;
	/// flag for marking first time through a loop
	bool m_first;
	/// button for signaling completion of movement phase
	wxButton* m_buttonMoveDone;
	/// button for signaling completion of movement phase
	wxButton* m_buttonDefensiveFireDone;
	/// button for signaling completion of movement phase
	wxButton* m_buttonOffensiveFireDone;
	/// list of active regions for weapons of currently selected ship
	std::vector<wxRect> m_weaponRegions;
	/// list of active regions for defenses of currently selected ship
	std::vector<wxRect> m_defenseRegions;
	/// button for signaling completion of placing mines phase
	wxButton* m_buttonMinePlacementDone;
	/// button for signaling completion of placing seeker missiles phase
	wxButton* m_buttonSeekerPlacementDone;
	/// button for signaling completion of seeker activation phase
	wxButton* m_buttonSeekerActivationDone;
	/// list of active regions for selection of a placement-source row
	std::vector<wxRect> m_shipNameRegions;
	/// placement-source index for each selectable row region
	std::vector<int> m_shipSelectionSourceIndices;
	/// list of active regions for selecting inactive seekers in activation stack
	std::vector<wxRect> m_seekerActivationRegions;
	/// seeker ID for each activation row region
	std::vector<unsigned int> m_seekerActivationSeekerIDs;
	/// list of active regions for recalling pending offensive-fire seekers
	std::vector<wxRect> m_pendingSeekerRecallRegions;
	/// grouped hex entry for each pending offensive-fire recall region
	std::vector<wxPoint> m_pendingSeekerRecallHexes;

	/// top pixel where tactical action prompts begin
	static const int ACTION_PROMPT_TOP_MARGIN = 5;
	/// vertical spacing between tactical action prompt lines
	static const int ACTION_PROMPT_LINE_HEIGHT = 16;
	/// maximum number of prompt lines above tactical action buttons
	static const int ACTION_PROMPT_MAX_LINES = 3;
	/// minimum vertical gap between prompt block and tactical action buttons
	static const int ACTION_PROMPT_BUTTON_GAP = 8;

	/// minimum width needed to render ship stats in a right-side split
	static const int SHIP_STATS_MIN_WIDTH = 320;
	/// minimum width to preserve for prompt text while ship stats are right-aligned
	static const int ACTION_PROMPT_MIN_WIDTH = 280;

	/// lower-panel layout modes for tactical prompts and ship stats
	enum LowerPanelLayoutMode {
		LOWER_PANEL_LAYOUT_RIGHT_SPLIT,
		LOWER_PANEL_LAYOUT_STACKED
	};

	/**
	 * @brief Shared lower-panel layout state across tactical phases.
	 *
	 * @author Tom Stephens, GPT-5 (high)
	 * @date Created: May 16, 2026
	 * @date Last Modified: May 16, 2026
	 */
	struct LowerPanelLayoutState {
		LowerPanelLayoutMode mode;
		int shipStatsLeftMargin;
		int shipStatsTop;
		int reservedPromptLines;
		int requestedDisplayHeight;
		bool initialized;
	};

	/// measured ship-stat block dimensions for lower-panel layout decisions
	struct ShipStatsLayoutRequirements {
		int width;
		int height;
	};

	/// Event handler for setting the ship's speed
	void onSetSpeed( wxCommandEvent& event );

	/// event handler for movement complete button
	void onMoveDone( wxCommandEvent& event );

	/// event handler for defensive fire complete button
	void onDefensiveFireDone( wxCommandEvent& event );

	/// event handler for mine placement complete button
	void onMinePlacementDone( wxCommandEvent& event );

	/**
	 * @brief Finish the seeker-missile placement phase and advance to attacker setup.
	 *
	 * Hides and disconnects the seeker placement done button, then delegates
	 * completion to `FBattleScreen::completeSeekerPlacement()`.
	 *
	 * @param event Button-click event from the seeker placement completion control.
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 02, 2026
	 * @date Last Modified: Jun 02, 2026
	 */
	void onSeekerPlacementDone( wxCommandEvent& event );
	/**
	 * @brief Finish the visible seeker-activation stop and return to movement flow.
	 *
	 * Hides and disconnects the button using the existing tactical action-button
	 * lifecycle, then delegates completion to `FBattleScreen`.
	 *
	 * @param event Button-click event from the activation completion control.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: May 25, 2026
	 * @date Last Modified: May 25, 2026
	 */
	void onSeekerActivationDone( wxCommandEvent& event );

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

	/**
	 * @brief Draws prompt for ships initial speed
	 *
	 * This draws a control to allow the user to enter the initial speed
	 * for the ship and a "Set Speed" button to signify it is done.
	 *
	 * @param dc The device context to draw on
	 *
	 * @author Tom Stephens
	 * @date Created:  Oct 17, 2008
	 * @date Last Modified:  Oct 17, 2008
	 */
	void drawGetSpeed(wxDC &dc);

	/**
	 * @brief Draws prompt for station' initial rotation direction
	 *
	 * This method draws a selection to choose CW or CCW rotation
	 * for the station
	 *
	 * @param dc The device context to draw on
	 *
	 * @author Tom Stephens
	 * @date Created:  Oct 20, 2008
	 * @date Last Modified:  Oct 20, 2008
	 */
	void drawSelectRotation(wxDC &dc);

	/**
	 * @brief check to see if one of the rotation buttons were selected
	 *
	 * This method verifies that one of the rotation direction buttons
	 * were selected and properly sets the station's heading and speed
	 * based on the selection.
	 *
	 * The method returns true if a button was clicked and false if missed.
	 *
	 * @param event The mouse event that contains the click coordinates
	 *
	 * @author Tom Stephens
	 * @date Created:  Oct 26, 2008
	 * @date Last Modified:  Oct 26, 2008
	 */
	bool setStationRotation(wxMouseEvent &event);

	///Draws prompt to select ship to move
	void drawMoveShip(wxDC &dc);

	/// Draws the stats for the currently selected ship
	void drawCurrentShipStats(wxDC & dc);

	/// returns a string giving the heading direction
	std::string getHeadingStr();

	/// Draws prompt to select ship to fire defensive shots
	void drawDefensiveFire(wxDC &dc);

	/**
	 * @brief Draw the offensive-fire lower-panel prompt and seeker deployment status.
	 *
	 * Normal offensive fire still prompts for ship and weapon targeting, but when
	 * an `SM` launcher is selected this prompt switches to seeker deployment
	 * instructions and the lower panel adds grouped pending recall rows for that
	 * launcher.
	 *
	 * @param dc The device context to draw on.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: Apr 15, 2009
	 * @date Last Modified: May 25, 2026
	 */
	void drawAttackFire(wxDC &dc);
	/**
	 * @brief Draw grouped recall rows for pending offensive-fire seekers.
	 *
	 * Each row corresponds to one legal path hex for the currently selected `SM`
	 * launcher and recalls exactly one current-phase pending seeker from that hex.
	 * Same-hex stacks are shown through the per-row pending count instead of
	 * treating same-hex board clicks as undo.
	 *
	 * @param dc The device context to draw on.
	 * @param lMargin The x position to start drawing text.
	 * @param startY The y position to start drawing the pending rows.
	 * @param textSize The font size to use.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: May 25, 2026
	 * @date Last Modified: May 25, 2026
	 */
	void drawOffensiveSeekerPendingRows(wxDC &dc, int lMargin, int startY, int textSize);

	/**
	 * @brief Draws the ships weapons in the tactical display
	 *
	 * This method draws the ships weapons in tactical display.  The weapons are higlighted
	 * depending on the current player, the ship's owner and the status of the weapon.
	 *
	 * In all cases, damaged weapons are colored red.  If the ship does not belong to the
	 * active player, other weapons are white.  If it does belong to the active player,
	 * untargeted weapons are yellow and targeted weapons are green.
	 *
	 * This method also sets up the active blocks on the display used for selecting the weapons.
	 *
	 * @param dc The device context to draw on
	 * @param lMargin The x position on the canvas to start drawing the text
	 * @param tMargin The y position on the canvas to start drawing the text
	 * @param tSize The font size to use.
	 *
	 * @author Tom Stephens
	 * @date Created:  Apr 15, 2009
	 * @date Last Modified:  Jan 28, 2011
	 */
	void drawWeaponList(wxDC &dc, int lMargin, int tMargin, int tSize);

	/**
	 * @brief Draws the ships defenses in the tactical display
	 *
	 * This method draws the ships defenses in tactical display.  The defenses are higlighted
	 * depending on the current player, the ship's owner and the status of the defense.
	 *
	 * Damaged defenses are colored red,
	 * inactive defenses are white and the currently active defense is green.
	 *
	 * This method also sets up the active blocks on the display used for selecting the weapons.
	 *
	 * @param dc The device context to draw on
	 * @param lMargin The x position on the canvas to start drawing the text
	 * @param tMargin The y position on the canvas to start drawing the text
	 * @param tSize The font size to use.
	 *
	 * @author Tom Stephens
	 * @date Created:  Mar 28, 2010
	 * @date Last Modified:  Jan 28, 2011
	 */
	void drawDefenseList(wxDC &dc, int lMargin, int tMargin, int tSize);

	/**
	 * @brief runs through current list of weapons to find if the user selected one
	 *
	 * This method runs through the current active weapon regions to see if the user
	 * selected one of the weapons.  If so it sets the current weapon pointer to the
	 * selected weapon.
	 *
	 * @param event The mouse event with the click position.
	 *
	 * @author Tom Stephens
	 * @date Created:  Apr 15, 2009
	 * @date Last Modified:  Jan 28, 2011
	 */
	void checkWeaponSelection(wxMouseEvent &event);

	/**
	 * @brief runs through current list of defenses to find if the user selected one
	 *
	 * This method runs through the current active defense regions to see if the user
	 * selected one of the defenses.  If so it sets the current defense pointer to the
	 * selected defense.
	 *
	 * @param event The mouse event with the click position.
	 *
	 * @author Tom Stephens
	 * @date Created:  Mar 28, 2010
	 * @date Last Modified:  Jan 28, 2011
	 */
	void checkDefenseSelection(wxMouseEvent &event);

	/// event handler for defensive fire complete button
	void onOffensiveFireDone( wxCommandEvent& event );

	/**
	 * @brief Draws the ships defenses in the tactical display
	 *
	 * This method draws the ships final status line showing the other miscellaneous
	 * damage status that the ship might have such as electrical fire, power system
	 * short circuit, etc.  If no systems are damage it just displays "none".
	 *	 *
	 * @param dc The device context to draw on
	 * @param lMargin The x position on the canvas to start drawing the text
	 * @param tMargin The y position on the canvas to start drawing the text
	 * @param tSize The font size to use.
	 *
	 * @author Tom Stephens
	 * @date Created:  Jan 28, 2011
	 * @date Last Modified:  Jan 28, 2011
	 */
	void drawOtherStatus(wxDC &dc, int lMargin, int tMargin, int textSize);

	/**
	 * @brief Draws the display for placing mines and seekers
	 *
	 * This method draws the setup placement panel for deployable mine and seeker
	 * weapon slots. It renders one row per ship/weapon source and shows ammo for
	 * that exact slot.
	 *
	 * The instruction text (left side) and "Done" button occupy the upper region
	 * of the panel. The placement source list (right side, at lMargin=310) is
	 * positioned below getActionButtonRowBottom() so neither region overlaps the
	 * other vertically.
	 *
	 * @param dc The device context to draw on
	 *
	 * @author Tom Stephens, claude-sonnet-4-6 (medium), claude-sonnet-4-6 (standard)
	 * @date Created:  Feb 22, 2011
	 * @date Last Modified:  Jun 02, 2026
	 */
	void drawPlaceMines(wxDC &dc);

	/**
	 * @brief Draws the display for placing seeker missiles (BS_PlaceSeekers phase).
	 *
	 * Shows seeker-missile-only deployment source rows with the seeker-specific
	 * prompt text and the "Seeker Placement Done" button. Mirrors drawPlaceMines
	 * but filters to SM-type sources only and routes completion through
	 * FBattleScreen::completeSeekerPlacement().
	 *
	 * @param dc The device context to draw on.
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 02, 2026
	 * @date Last Modified: Jun 02, 2026
	 */
	void drawPlaceSeekers(wxDC &dc);

	/**
	 * @brief Draw seeker activation panel content and deactivate rows.
	 *
	 * Renders the activation instructions, an "Activated seekers" list, and one
	 * clickable deactivate row for each seeker already activated by the moving
	 * player (via `getActiveSeekersByMovingPlayer()`). Board clicks activate
	 * additional inactive seekers; panel row clicks deactivate individual active
	 * seekers via `deactivateActiveSeekerByID(id)`. Each row maps to exactly one
	 * seeker ID so deactivation is one-way and per-seeker.
	 *
	 * @param dc The device context to draw on.
	 *
	 * @author Tom Stephens, gpt-5.4 (high), claude-sonnet-4-6 (standard)
	 * @date Created: May 25, 2026
	 * @date Last Modified: May 30, 2026
	 */
	void drawSeekerActivation(wxDC &dc);

	/**
	 * @brief runs through placement source rows to find if the user selected one
	 *
	 * This method checks the current placement-source rows and updates the
	 * selected source when the user clicks a row.
	 *
	 * @param event The mouse event with the click position.
	 *
	 * @author Tom Stephens
	 * @date Created:  Feb 22, 2011
	 * @date Last Modified:  May 24, 2026
	 */
	void checkShipSelection(wxMouseEvent &event);

	/**
	 * @brief Deactivate one active seeker when the user clicks a panel deactivate row.
	 *
	 * Checks each region in `m_seekerActivationRegions` against the click
	 * position and calls `deactivateActiveSeekerByID(id)` for the matching
	 * seeker, then triggers a redraw. Board clicks activate seekers;
	 * this method handles the inverse deactivation action from the lower panel.
	 *
	 * @param event Mouse click event from the lower panel.
	 *
	 * @author Tom Stephens, gpt-5.4 (high), claude-sonnet-4-6 (standard)
	 * @date Created: May 25, 2026
	 * @date Last Modified: May 30, 2026
	 */
	void checkSeekerActivationSelection(wxMouseEvent &event);
	/**
	 * @brief Check whether the user clicked a grouped pending-seeker recall row.
	 *
	 * @param event The mouse event with the click position.
	 *
	 * @return True when one pending seeker was recalled for the selected launcher.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: May 25, 2026
	 * @date Last Modified: May 25, 2026
	 */
	bool checkOffensiveSeekerPendingSelection(wxMouseEvent &event);

	/// returns y-position for the indexed action prompt line
	int getActionPromptLineY(int lineIndex) const;

	/// returns explicit spacer height that keeps action buttons below prompt text
	int getActionButtonTopSpacerHeight() const;

	/// reserve prompt lines for the lower-panel action layout state
	void reserveActionPromptLines(int lineCount);

	/// returns additional prompt lines beyond baseline action prompt reservation
	int getActionPromptExtraLines() const;

	/// returns extra spacer height needed before action buttons for wrapped prompts
	int getActionButtonExtraSpacerHeight() const;

	/// updates dynamic spacer above action buttons from current prompt reservation
	void refreshActionButtonSpacer();

	/// counts wrapped lines needed to render a prompt within the given width
	int countWrappedActionPromptLines(wxDC &dc, const wxString &promptText, int maxWidth) const;

	/// draws wrapped prompt text into action-prompt lines and returns consumed lines by reference
	void drawWrappedActionPrompt(wxDC &dc, const wxString &promptText, int maxWidth, int &lineCursor);

	/// validates or updates the shared lower-panel layout state for the current geometry
	void ensureLowerPanelLayoutState(int panelWidth, int panelHeight);

	/// measure width/height needed to render current ship stats without clipping
	ShipStatsLayoutRequirements measureShipStatsLayoutRequirements(wxDC &dc) const;

	/// returns the current bottom edge for the action-button row in client coordinates
	int getActionButtonRowBottom() const;

	/// apply the requested display height from the active lower-panel layout state
	void applyRequestedDisplayHeight();

	/// returns prompt width after accounting for right-split ship stat placement
	int getCurrentPromptMaxWidth(int panelWidth) const;

	/// derives move-phase prompt strings based on active turn and selected ship state
	void buildMovePromptText(wxString & turnPrompt, wxString & detailPromptOne, wxString & detailPromptTwo) const;

	/// recomputes reserved prompt lines for move phase using current constrained width
	void refreshMovePromptReservation(wxDC &dc, int panelWidth, int panelHeight);

	/// protects lower-panel resize/reflow from recursive reflow churn
	bool m_inResizeReflow;

	/// dynamic spacer above action buttons for wrapped move prompts
	wxSizerItem * m_actionButtonExtraSpacerItem;

	/// shared layout state for prompt/stats split and requested tactical-display height
	LowerPanelLayoutState m_lowerPanelLayoutState;
};

}

#endif //__FBattleDisplay__
