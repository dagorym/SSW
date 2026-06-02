/**
 * @file FBattleBoard.h
 * @brief Header file for BattleBoard class
 * @author Tom Stephens, claude-sonnet-4-6 (standard)
 * @date Created:  Jul 11, 2008
 * @date Last Modified: May 30, 2026
 *
 */

#ifndef __FBattleBoard__
#define __FBattleBoard__

#include <map>
#include <utility>

#include "gui/GuiTypes.h"
#include "Frontier.h"
#include "strategic/FFleet.h"
#include "core/FPoint.h"

namespace Frontier {
class FBattleScreen;

/**
 * @brief Tactical board renderer and hit tester.
 *
 * Owns the wx-side tactical battlefield presentation: hex geometry, scrolling,
 * ship/overlay rendering, seeker visibility rendering, and mouse hit-testing.
 * The board now also renders setup ordnance markers from model-owned
 * placed-ordnance records so mine and seeker deployment colors stay tied to the
 * exact source ship/weapon slot selected in the lower display panel, even when
 * the setup list grows beyond the legacy 12-color seed palette.
 *
 * @author Tom Stephens, gpt-5.4 (high), claude-sonnet-4-6 (standard)
 * @date Created: Jul 11, 2008
 * @date Last Modified: May 30, 2026
 */
class FBattleBoard : public wxScrolledWindow
{
public:
FBattleBoard(wxWindow * parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxHSCROLL|wxRAISED_BORDER|wxVSCROLL, const wxString &name = "BattleBoard" );
~FBattleBoard();

/**
 * @brief Draw the tactical board overlays for the current state and phase.
 *
 * Setup placement draws source-colored ordnance markers, seeker activation
 * draws only inactive seekers for the moving player, and normal battle phases
 * draw only active seekers.
 *
 * @param dc Device context used for tactical board drawing.
 *
 * @author Tom Stephens, gpt-5.4 (high)
 * @date Created: May 25, 2026
 * @date Last Modified: May 25, 2026
 */
void draw(wxDC &dc);
void onPaint(wxPaintEvent & event);
/**
 * @brief Handle tactical-board clicks for setup, battle, and seeker activation.
 *
 * During `PH_SEEKER_ACTIVATION`, a board click calls
 * `activateInactiveSeekerAtHex(hex)` to immediately activate one inactive
 * seeker at the clicked hex and then triggers a redraw so the lower panel
 * and board reflect the updated activation state. All other board clicks
 * continue through the existing delegated tactical hex handler.
 *
 * @param event Mouse-release event carrying the clicked board position.
 *
 * @author Tom Stephens, gpt-5.4 (high), claude-sonnet-4-6 (standard)
 * @date Created: May 25, 2026
 * @date Last Modified: May 30, 2026
 */
void onLeftUp(wxMouseEvent & event);
void onMotion(wxMouseEvent & event);

void setPlanetImages(ImageList iList) { m_planetImages = iList; }
void setScale(double factor);

protected:
FBattleScreen * m_parent;
double m_scale;
int m_size;
int m_trim;
double m_d;
double m_a;
int m_nCol;
int m_nRow;
int m_width;
int m_height;
FPoint m_hexCenters[100][100];
ImageList m_planetImages;
wxImage * m_maskingScreenIcon;
wxImage * m_seekerMissileIcon;

void drawGrid(wxDC &dc);
void setConstants(double scale);
void computeCenters();
bool getHex(int x, int y, int &a, int &b);
void drawCenteredOnHex(wxImage img, FPoint p, int rot = 0);
void drawShips();
void drawRoute(wxDC &dc);
void drawRouteHexes(wxDC &dc, const std::vector<FPoint> & list, int count);
void drawMovedHexes(wxDC &dc, PointList list, bool current=false);
void drawWeaponRange(wxDC &dc);
void drawShadedHex(wxDC& dc, wxColour c, FPoint p);
void drawTarget(wxDC &dc);
void drawMinedHexes(wxDC &dc);
/**
 * @brief Draw seeker missiles with phase-based visibility filtering.
 *
 * During `PH_SEEKER_ACTIVATION`, both inactive seeker stacks owned by the
 * moving player (rendered without rotation for click-to-activate targeting)
 * and already-active seekers for the moving player (rendered with heading
 * rotation for visual confirmation) are drawn. During `PH_ATTACK_FIRE`,
 * the seeker icon is drawn on every hex holding a current-phase pending
 * offensive-fire seeker deployment (sourced from
 * `FBattleScreen::getAllPendingOffensiveFireSeekerHexes()`) so the player
 * can see where seekers have been placed before committing the phase;
 * recalling a pending seeker via the lower-panel list removes its icon on
 * the next redraw. Committed active seekers from previous turns are also
 * rendered during `PH_ATTACK_FIRE` with heading rotation. During all other
 * battle phases, only active seekers are shown with their icon rotated to
 * match their current heading. The icon is loaded through the shared
 * asset-resolution policy used elsewhere in the tactical wx surfaces.
 *
 * @param dc Device context used for tactical board drawing.
 *
 * @author Tom Stephens, gpt-5.4 (high), claude-sonnet-4-6 (standard)
 * @date Created: May 25, 2026
 * @date Last Modified: May 30, 2026
 */
void drawSeekerMissiles(wxDC &dc);
/**
 * @brief Draw stepped movement paths for active seekers during movement and activation phases.
 *
 * Iterates all seeker missile records that moved this resolution pass
 * (movementPath.size() > 1) and draws their stepped path as a line in a
 * color distinct from ship paths (cyan #00CCCC, pen width 2). Called during
 * PH_MOVE and PH_SEEKER_ACTIVATION (SMF-06) so impacting seekers remain
 * visible at their final hex while ICM/damage dialogs are displayed.
 *
 * @param dc Device context used for tactical board drawing.
 *
 * @author claude-sonnet-4-6 (standard), claude-sonnet-4-6 (medium)
 * @date Created: May 30, 2026
 * @date Last Modified: Jun 02, 2026 (SMF-06: also called during PH_SEEKER_ACTIVATION)
 */
void drawSeekerPaths(wxDC &dc);
	/**
	 * @brief Draw placed setup ordnance markers using their source-specific colors.
	 *
	 * Reads the model-owned placed-ordnance list through `FBattleScreen`, rebuilds
	 * the source-slot ordinal map for the current setup view, and shades each
	 * in-bounds mine or inactive seeker hex with the deterministic color assigned
	 * to that exact source ship/weapon slot during setup placement.
	 *
	 * @param dc Device context used for tactical board drawing.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: May 24, 2026
	 * @date Last Modified: May 24, 2026
	 */
	void drawPlacementOrdnanceHexes(wxDC &dc);
	/**
	 * @brief Derive the placement color for one setup source slot.
	 *
	 * Resolves the source ship/weapon combo to a deterministic ordinal built
	 * during placement rendering, then maps that ordinal to a stable color that
	 * expands beyond the seed palette instead of wrapping back onto the legacy
	 * modulo-collision mapping.
	 *
	 * @param shipID Source ship identifier.
	 * @param weaponIndex Zero-based weapon slot on that ship.
	 *
	 * @return wxWidgets color used for source-specific setup marker shading.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: May 24, 2026
	 * @date Last Modified: May 24, 2026
	 */
	wxColour getPlacementSourceColor(unsigned int shipID, int weaponIndex) const;

	/// deterministic source-slot ordinal map rebuilt each placement draw pass
	std::map<std::pair<unsigned int, int>, unsigned int> m_placementSourceOrdinals;
};

}

#endif //__FBattleBoard__
