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
};

}

#endif //__FBattleBoard__
