/*
 * @file WXMapDisplay.cpp
 * @brief Implementation file for the WXMapDisplay class
 * @author Tom Stephens, Claude Sonnet 5 (medium)
 * @date Created:  Aug 2, 2009
 * @date Last Modified: Jul 19, 2026
 */

#include "gui/WXMapDisplay.h"
#include "strategic/FMap.h"
#include <algorithm>

namespace Frontier
{

WXMapDisplay::WXMapDisplay() {
	// TODO Auto-generated constructor stub

}

WXMapDisplay::~WXMapDisplay() {
	// TODO Auto-generated destructor stub
}

void WXMapDisplay::draw(wxDC &dc/*, unsigned int id*/) {
	// Root-cause guard (SF-nullfmap-paint-guard): the FMap singleton can be
	// NULL/half-built while a FGamePanel is shown before or during an
	// unloaded/partially-loaded game. Test existence before touching the
	// singleton at all -- FMap::getMap() would otherwise return a null
	// reference and any subsequent member call (e.g. getMaxSize()) would
	// dereference this=0x0. Skip the map draw entirely in that case; this
	// is defense-in-depth independent of the FMainFrame::onOpen()
	// setGame-deferral mitigation, which remains in place.
	if (!FMap::hasMap()) {
		return;
	}

	double scale = getScale(dc);
	FMap &map = FMap::getMap();

	wxColour white,blue,red,lgray,black,dblue,dred;
	white.Set(wxT("#FFFFFF"));// white
	blue.Set(wxT("#0000FF"));// blue
	red.Set(wxT("#FF0000"));// red
	lgray.Set(wxT("#999999"));// light grey
	black.Set(wxT("#000000"));// blue
	dblue.Set(wxT("#000099"));// dark blue
	dred.Set(wxT("#770000"));// dark red

	dc.SetBackground(wxBrush(black));
	dc.Clear();

	JumpRouteList jumpList = map.getJumpList();
	if (jumpList.size()>0){
		std::vector<FJumpRoute *>::iterator itr;
		for (itr = jumpList.begin(); itr < jumpList.end(); itr++){
			wxCoord x1 = (wxCoord)((*itr)->getStart()->getCoord(0)*scale);
			wxCoord y1 = (wxCoord)((*itr)->getStart()->getCoord(1)*scale);
			wxCoord x2 = (wxCoord)((*itr)->getEnd()->getCoord(0)*scale);
			wxCoord y2 = (wxCoord)((*itr)->getEnd()->getCoord(1)*scale);
			if ((*itr)->isKnown(1)){
				dc.SetPen(wxPen(blue));
			} else {
				dc.SetPen(wxPen(red));
			}
			dc.DrawLine(x1, y1, x2, y2);
			dc.SetPen(wxPen(white));
		}
	}

	dc.SetTextForeground(white);
	dc.SetFont(wxFont((int)(scale/2),wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL));
	SystemList systemList = map.getSystemList();
	if (systemList.size()>0){
		std::vector<FSystem *>::iterator itr;
		for (itr = systemList.begin(); itr < systemList.end(); itr++){
			wxCoord x = (wxCoord)((*itr)->getCoord(0)*scale);
			wxCoord y = (wxCoord)((*itr)->getCoord(1)*scale);
			if ((*itr)->getOwner()==1){
				dc.SetBrush(wxBrush(dblue));
				dc.SetPen(wxPen(dblue));
			} else {
				dc.SetBrush(wxBrush(dred));
				dc.SetPen(wxPen(dred));
			}
			dc.DrawCircle(x,y,(wxCoord)(0.5*scale));
			dc.DrawText((*itr)->getName(),x-(wxCoord)(0.75*scale),y+(wxCoord)(0.75*scale));
		}
	}
}

const double WXMapDisplay::getScale(wxDC &dc) const {
	// Same root-cause guard as draw(): return a safe default scale instead
	// of dereferencing FMap::getMap()'s null reference when the singleton
	// does not exist yet.
	if (!FMap::hasMap()) {
		return 1.0;
	}

	FMap &map = FMap::getMap();
	wxCoord w, h;
	dc.GetSize(&w, &h);
	double scaleX=(double)w/map.getMaxSize();
	double scaleY=(double)h/map.getMaxSize();
	double minscale = 1;
	return std::max(minscale,std::min(scaleX,scaleY));
}

}
