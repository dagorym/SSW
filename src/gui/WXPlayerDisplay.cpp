/*
 * @file WXPlayerDisplay.cpp
 * @brief Implementation file for the WXPlayerDisplay class
 * @author Tom Stephens
 * @date Created:  Aug 3, 2009
 */

#include "gui/WXPlayerDisplay.h"
#include "gui/WXMapDisplay.h"
#include "gui/WXIconCache.h"
#include "strategic/FPlayer.h"
#include "strategic/FMap.h"
#include <algorithm>

namespace Frontier
{

WXPlayerDisplay::WXPlayerDisplay() {
	// TODO Auto-generated constructor stub

}

WXPlayerDisplay::~WXPlayerDisplay() {
	// TODO Auto-generated destructor stub
}

void WXPlayerDisplay::drawFleets(wxDC &dc, FPlayer *player){
	// Root-cause guard (FR-2 pass-2 remediation, closes the residual
	// WXPlayerDisplay::drawFleets() gap left open by the pass-1
	// SF-nullfmap-paint-guard fix): consult FMap::hasMap() before ever
	// binding a reference/pointer to FMap::getMap(). getMap() returns a
	// null reference when the singleton does not exist, and binding
	// `FMap *map = &(FMap::getMap())` to that null reference is itself
	// undefined behavior, independent of whether the pointer is later
	// dereferenced. Currently a located fleet cannot exist without a live
	// FMap (FGame::init()/load() always create the map before any player
	// or fleet), but that is an implicit ordering invariant rather than an
	// explicit guard, so this predicate closes the gap defensively rather
	// than relying on it. Skip the whole draw when no map exists; this is
	// additive defense-in-depth and does not change behavior when a valid
	// FMap exists.
	if (!FMap::hasMap()) {
		return;
	}

	WXMapDisplay mapDisplay;
	FMap *map = &(FMap::getMap());
	const double scale = mapDisplay.getScale(dc);
	FleetList fleetList = player->getFleetList();
	if (fleetList.size()>0){
		for (FleetList::iterator itr = fleetList.begin(); itr < fleetList.end(); itr++){
			const wxImage & icon = WXIconCache::instance().get((*itr)->getIconName());
			if (!icon.IsOk()) {
				continue;
			}
			const int iconSize = std::max(1, (int)scale);
			wxBitmap b(icon.Scale(iconSize, iconSize));
			if((*itr)->getInTransit()){  // it's in a jump
				if ((*itr)->getDestination() == FFleet::NO_DESTINATION || (*itr)->getJumpRoute() == FFleet::NO_ROUTE) {
					wxCoord x = (wxCoord)(((*itr)->getCoord(0)-0.5) * scale);
					wxCoord y = (wxCoord)(((*itr)->getCoord(1)-0.5) * scale);
					dc.DrawBitmap(b,x,y);
					continue;
				}
				FJumpRoute *j = map->getJumpRoute((*itr)->getJumpRoute());
				if (j == NULL) {
					wxCoord x = (wxCoord)(((*itr)->getCoord(0)-0.5) * scale);
					wxCoord y = (wxCoord)(((*itr)->getCoord(1)-0.5) * scale);
					dc.DrawBitmap(b,x,y);
					continue;
				}
				unsigned int start,end;
				if(j->getStart()->getID()==(*itr)->getDestination()){
					start = j->getEnd()->getID();
					end = j->getStart()->getID();
				} else {
					start = j->getStart()->getID();
					end = j->getEnd()->getID();
				}
				float dx = (map->getSystem(start)->getCoord(0)-map->getSystem(end)->getCoord(0));
				float dy = (map->getSystem(start)->getCoord(1)-map->getSystem(end)->getCoord(1));
				float frac = 0;
				if ((*itr)->getTransitTime()==j->getLength()){ // we only need to offset when we assign the jump
					frac = 0.1;
				}
				wxCoord x = (wxCoord)(((*itr)->getCoord(0)-0.5-frac*dx) * scale);
				wxCoord y = (wxCoord)(((*itr)->getCoord(1)-0.5-frac*dy) * scale);
				dc.DrawBitmap(b,x,y);
			} else {  // it's in a system
				double xoffset = -0.2*scale;
				double yoffset = -0.2*scale;
				if(player->getID()!=1){
					xoffset = 0.2*scale;
					yoffset = -0.2*scale;
				} else if((*itr)->isMilitia()){
					xoffset = 0;
					yoffset = 0.2*scale;
				}
				if((*itr)->getLocation() != 0) {
					wxCoord x = (wxCoord)(map->getSystem((*itr)->getLocation())->getCoord(0)*scale-0.5*scale+xoffset);
					wxCoord y = (wxCoord)(map->getSystem((*itr)->getLocation())->getCoord(1)*scale-0.5*scale+yoffset);
					dc.DrawBitmap(b,x,y);
				}
			}
		}
	}
}


}
