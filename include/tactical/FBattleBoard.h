/**
 * @file FBattleBoard.h
 * @brief Header file for BattleBoard class
 * @author Tom Stephens
 * @date Created:  Jul 11, 2008
 *
 */

#ifndef __FBattleBoard__
#define __FBattleBoard__

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
 * ship/overlay rendering, and mouse hit-testing. The board now also renders
 * setup ordnance markers from model-owned placed-ordnance records so mine and
 * seeker deployment colors stay tied to the exact source ship/weapon slot
 * selected in the lower display panel.
 *
 * @author Tom Stephens, gpt-5.4 (high)
 * @date Created: Jul 11, 2008
 * @date Last Modified: May 24, 2026
 */
class FBattleBoard : public wxScrolledWindow
{
public:
FBattleBoard(wxWindow * parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxHSCROLL|wxRAISED_BORDER|wxVSCROLL, const wxString &name = "BattleBoard" );
~FBattleBoard();

void draw(wxDC &dc);
void onPaint(wxPaintEvent & event);
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
	 * @brief Draw placed setup ordnance markers using their source-specific colors.
	 *
	 * Reads the model-owned placed-ordnance list through `FBattleScreen` and
	 * shades each in-bounds mine or inactive seeker hex with the deterministic
	 * color assigned to that source ship/weapon slot during setup placement.
	 *
	 * @param dc Device context used for tactical board drawing.
	 *
	 * @author Tom Stephens, gpt-5.4 (high)
	 * @date Created: May 24, 2026
	 * @date Last Modified: May 24, 2026
	 */
	void drawPlacementOrdnanceHexes(wxDC &dc);
	/**
	 * @brief Derive a deterministic placement color for one setup source slot.
	 *
	 * Uses the source ship identifier plus weapon-slot index so the lower-panel
	 * row selection and board marker colors stay stable across placement and undo
	 * operations even when ammo counts change and rows are rebuilt.
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
};

}

#endif //__FBattleBoard__
