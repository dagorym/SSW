/*
 * @file FHexPath.cpp
 * @brief Implementation file for the FHexMap class
 * @author Tom Stephens
 * @date Created: Oct 04, 2010
 */

#include "Frontier.h"
#include "core/FHexMap.h"
#include <algorithm>

namespace Frontier {

FHexMap::FHexMap() {
	// TODO Auto-generated constructor stub

}

FHexMap::~FHexMap() {
	// TODO Auto-generated destructor stub
}

int FHexMap::computeHexDistance(int sx, int sy, int ex, int ey){
	int dis=1;
    // calculate hexspace coordinates of A and B
    int x1 = sx - Floor2(sy);
    int y1 = sx + Ceil2(sy);
    int x2 = ex - Floor2(ey);
    int y2 = ex + Ceil2(ey);
    // calculate distance using hexcoords as per previous algorithm
    int dx = x2 - x1;
    int dy = y2 - y1;
    if (sign(dx) == sign(dy)) {
        dis = std::max(abs(dx),abs(dy));
    } else {
        dis = abs(dx) + abs(dy);
    }
	return dis;
}

int FHexMap::computeHexDistance(FPoint s, FPoint e){
	return computeHexDistance(s.getX(),s.getY(),e.getX(),e.getY());
}

FPoint FHexMap::findNextHex(FPoint h, int heading){
	FPoint bad(-1,-1);
	for (int i=h.getX()-1;i<=h.getX()+1;i++){
		for (int j=h.getY()-1;j<=h.getY()+1;j++){
			FPoint tmp(i,j);
			if (i>=0 && j>=0 && computeHeading(h,tmp)==heading && computeHexDistance(h.getX(),h.getY(),i,j)==1){
				return tmp;
			}
		}
	}
	return bad;
}

int FHexMap::computeHeading(FPoint s, FPoint d){

	double angle = computeHexAngle(s,d);
	if (angle<0) { angle += 360.; }
	int ang = (int)floor(angle+30);
	if (ang>=360) { ang -= 360; }
//	std::cerr << "angle = " << angle << "  ang = " << ang << "  heading = " << ang/60 << std::endl;
	return (ang/60);
}

double FHexMap::computeHexAngle(FPoint s, FPoint d){
	double dis = 1.0;
	double a = dis/sqrt(3.);
	double sx = dis + (2 * dis * s.getX()) + dis * (s.getY()%2);
	double sy = 2 * a + (3 * a * s.getY());
	double dx = dis + (2 * dis * d.getX()) + dis * (d.getY()%2);
	double dy = 2 * a + (3 * a * d.getY());
	return atan2((dy-sy),(sx-dx))*180/acos(-1.0);  // angle in degrees;
}

void FHexMap::addShip(FPoint p, FVehicle * v){
	m_hexList[p].push_back(v);
}

PointSet FHexMap::getOccupiedHexList(){
	PointSet hexList;
	HexMap::iterator itr = m_hexList.begin();
	while (itr != m_hexList.end()){
		hexList.insert(itr->first);
		itr++;
	}
	return hexList;
}

VehicleList FHexMap::getShipList(FPoint h){
	if (m_hexList.find(h)!= m_hexList.end()){
		return m_hexList[h];
	} else {
		VehicleList empty;
		return empty;
	}
}

void FHexMap::clear(){
	m_hexList.clear();
}

}
