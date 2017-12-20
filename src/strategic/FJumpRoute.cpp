/**
 * @file FJumpRoute.cpp
 * @brief Implementation file for FJumpRoute class
 * @author Tom Stephens
 * @date Created:  Jan 20, 2005
 *
 */

#include "strategic/FJumpRoute.h"
#include <cmath>

namespace Frontier
{

unsigned int FJumpRoute::m_nextID=0;
unsigned int FJumpRoute::m_classCount = 0;

FJumpRoute::FJumpRoute(){
//	m_ID = m_nextID++;
	m_classCount++;
	m_start = NULL;
	m_end = NULL;
	m_length = 0;
	m_ID = 0;
}

FJumpRoute::~FJumpRoute(){
	m_classCount--;
	if (m_classCount==0){  // if all players have been deleted
		m_nextID=0;  // reset the id counter
	}
}

FJumpRoute::FJumpRoute(FSystem * start, FSystem * end, std::vector<unsigned int> players){
	m_start = start;
	m_end = end;
	m_players = players;
	float dx = start->getCoord(0)-end->getCoord(0);
	float dy = start->getCoord(1)-end->getCoord(1);
	float dz = start->getCoord(2)-end->getCoord(2);
	float dis = sqrt(dx*dx+dy*dy+dz*dz);
	m_length = floor(dis + 0.5);//round(dis);  // Note:  to match the FDM this should be ceil instead of round
	m_ID = m_nextID++;
	m_classCount++;
}

const bool FJumpRoute::isKnown(unsigned int id) const {
	for (unsigned int i = 0; i < m_players.size(); i++){
		if (m_players[i]==id) return true;
	}
	return false;
}

int FJumpRoute::getRJ3Time(int l) {
	int dis;
	if (l){
		dis = l;
	} else {
		dis = (int)m_length;
	}
	if (dis%6 == 0){
		return dis/3;
	}
	if (dis == 8) {
		return 4;
	}
	return dis/3+1;
}

int FJumpRoute::getRJ2Time(int l){
	int dis;
	if (l){
		dis = l;
	} else {
		dis = (int)m_length;
	}
	if (dis%4 == 0){
		return dis/2;
	} else {
		return dis/2+1;
	}
}

int FJumpRoute::getJumpTime(int s, int l) {
	if (!l){
		l=(int)m_length;
	}
	switch(s){
	case 1:
		return l;
		break;
	case 2:
		return getRJ2Time(l);
		break;
	case 3:
		return getRJ3Time(l);
		break;
	default:
		return l;
		break;
	}
}

const int FJumpRoute::save(std::ostream &os) const{
	write(os,m_ID);
	write(os,m_length);
	// store the ID's of the systems at the start and the end
	// we'll restore the links when we read them back in
	write(os,m_start->getID());
	write(os,m_end->getID());
	// store the list of player IDs that know about the route.
	write(os,m_players.size());
	for (unsigned int i = 0; i < m_players.size(); i++){
		write(os,m_players[i]);
	}
	return 0;
}

int FJumpRoute::load(std::istream &is){
//	std::cerr << "Entering FJumpRoute::load" << std::endl;
	read(is,m_ID);
	read(is,m_length);
	unsigned int start,end;
	size_t pSize;
	// We'll hide the ID of the system in the pointer variable here and then
	// extracted it to get the actual reference once we get back out to a place
	// where we can look up the system using that ID.
	read(is,start);
	m_start = (FSystem *)start;
	read(is,end);
	m_end = (FSystem *)end;
	read(is,pSize);
	for (unsigned int i = 0; i < pSize; i++){
		unsigned int pID;
		read(is,pID);
		m_players.push_back(pID);
	}
	return 0;
}

};
