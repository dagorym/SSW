/*
 * @file WXPlayerDisplay.cpp
 * @brief Implementation file for the WXPlayerDisplay class
 * @author Tom Stephens
 * @date Created:  Aug 3, 2009
 */

#include "gui/WXPlayerDisplay.h"
#include "gui/WXMapDisplay.h"
#include "strategic/FPlayer.h"

namespace Frontier
{

WXPlayerDisplay::WXPlayerDisplay() {
	// TODO Auto-generated constructor stub

}

WXPlayerDisplay::~WXPlayerDisplay() {
	// TODO Auto-generated destructor stub
}

void WXPlayerDisplay::drawFleets(wxDC &dc, FPlayer *player){
	WXMapDisplay mapDisplay;
	FMap *map = &(FMap::getMap());
	const double scale = mapDisplay.getScale(dc);
	FleetList fleetList = player->getFleetList();
	if (fleetList.size()>0){
		for (FleetList::iterator itr = fleetList.begin(); itr < fleetList.end(); itr++){
			wxBitmap b((*itr)->getIcon()->Scale((int)scale,(int)scale));
			if((*itr)->getInTransit()){  // it's in a jump
				FJumpRoute *j = map->getJumpRoute((*itr)->getJumpRoute());
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
//				std::cerr << "Fraction is " << frac << std::endl;
				if ((*itr)->getTransitTime()==j->getLength()){ // we only need to offset when we assign the jump
					frac = 0.1;
				}
				wxCoord x = (wxCoord)(((*itr)->getCoord(0)-0.5-frac*dx) * scale);
				wxCoord y = (wxCoord)(((*itr)->getCoord(1)-0.5-frac*dy) * scale);
//				std::cerr << "x = " << x << "  y = " << y << std::endl;
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
