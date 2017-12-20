/**
 * @file FMap.cpp
 * @brief Implementation file for FMap class
 * @author Tom Stephens
 * @date Created:  Jan 17, 2005
 *
 */

#include "strategic/FMap.h"
#include <cmath>

namespace Frontier
{

FMap * FMap::m_map = 0;

FMap & FMap::create(bool random, std::vector<unsigned int> pList){
	if (!m_map) {
		m_map = new FMap(random, pList);
	}
	return *m_map;
}

FMap & FMap::create(){
	if (!m_map) {
		m_map = new FMap();
	}
	return *m_map;
}

FMap & FMap::getMap(){
	return *m_map;
}

FMap::FMap(bool random, std::vector<unsigned int> pList){
	if(random){
		m_maxCoord=0;
	} else {
		m_maxCoord=40;
		setUpFrontier(pList);
	}
}

FMap::~FMap(){
  if(m_systems.size() > 0) {  //delete the systems
    for (unsigned int i=0; i< m_systems.size(); i++) {
      delete m_systems[i];
    }
  }
  if(m_jumps.size() > 0) {  //delete the systems
    for (unsigned int i=0; i< m_jumps.size(); i++) {
      delete m_jumps[i];
    }
  }
  m_map = 0;
}

FSystem * FMap::getSystem(std::string name) const {
	for (unsigned int i = 0; i < m_systems.size(); i++){
		if (m_systems[i]->getName()==name){
			return (m_systems[i]);
		}
	}
	return NULL;
}

FSystem * FMap::getSystem(unsigned int id) const {
	for (unsigned int i = 0; i < m_systems.size(); i++){
		if (m_systems[i]->getID()==id){
			return (m_systems[i]);
		}
	}
	return NULL;
}


void FMap::setUpFrontier(std::vector<unsigned int> pList){
	FPlanet *p;
	FSystem *s,*s2,*s3,*s4,*s5;
	FJumpRoute *j;

	// Make a list of player id's to assign to the jump routes
	std::vector<unsigned int> sathar;
	sathar.push_back(pList[1]);

	s = new FSystem("Prenglar",19,20,0,pList[0]);
	p = new FPlanet("Gran Quivera");
	s->addPlanet(p);
	p = new FPlanet("Morgaine's World");
	s->addPlanet(p);
	m_systems.push_back(s);

	s5 = new FSystem("Sathar Start Circle 2",24,19,0,pList[1]);
	m_systems.push_back(s5);
	j = new FJumpRoute(s5,s,sathar);
	m_jumps.push_back(j);

	s2 = new FSystem("Cassidine",19,13,0,pList[0]);
	p = new FPlanet("Triad");
	s2->addPlanet(p);
	p = new FPlanet("Rupert's Hole");
	s2->addPlanet(p);
	m_systems.push_back(s2);

	j = new FJumpRoute(s,s2,pList);
	m_jumps.push_back(j);

	s3 = new FSystem("Dixon's Star",15,17,0,pList[0]);
	p = new FPlanet("Laco");
	s3->addPlanet(p);
	m_systems.push_back(s3);
	j = new FJumpRoute(s,s3,pList);
	m_jumps.push_back(j);

	s4 = new FSystem("Truane's Star",14,12,0,pList[0]);
	p = new FPlanet("Pale");
	s4->addPlanet(p);
	p = new FPlanet("New Pale");
	s4->addPlanet(p);
	m_systems.push_back(s4);
	j = new FJumpRoute(s3,s4,pList);
	m_jumps.push_back(j);

	s3 = new FSystem("Zebulon",10,9,0,pList[0]);
	p = new FPlanet("Volturnus");
	s3->addPlanet(p);
	m_systems.push_back(s3);
	j = new FJumpRoute(s4,s3,pList);
	m_jumps.push_back(j);

	s5 = new FSystem("Sathar Start Circle 1",9,5,0,pList[1]);
	m_systems.push_back(s5);
	j = new FJumpRoute(s5,s3,sathar);
	m_jumps.push_back(j);

	s3 = new FSystem("Athor",8,21,0,pList[0]);
	p = new FPlanet("Yast");
	s3->addPlanet(p);
	m_systems.push_back(s3);
	j = new FJumpRoute(s,s3,pList);
	m_jumps.push_back(j);

	s4 = new FSystem("Araks",2,26,0,pList[0]);
	p = new FPlanet("Hentz");
	s4->addPlanet(p);
	m_systems.push_back(s4);
	j = new FJumpRoute(s3,s4,pList);
	m_jumps.push_back(j);

	s3 = new FSystem("Scree Fron",2,15,0,pList[0]);
	p = new FPlanet("Histran");
	s3->addPlanet(p);
	p = new FPlanet("Hakosoar");
	s3->addPlanet(p);
	m_systems.push_back(s3);
	j = new FJumpRoute(s4,s3,pList);
	m_jumps.push_back(j);

	s3 = new FSystem("Gruna Garu",10,31,0,pList[0]);
	p = new FPlanet("Hargut");
	s3->addPlanet(p);
	m_systems.push_back(s3);
	j = new FJumpRoute(s4,s3,pList);
	m_jumps.push_back(j);

	j = new FJumpRoute(s,s3,pList);
	m_jumps.push_back(j);

	s3 = new FSystem("Timeon",23,27,0,pList[0]);
	p = new FPlanet("Lossend");
	s3->addPlanet(p);
	m_systems.push_back(s3);
	j = new FJumpRoute(s,s3,pList);
	m_jumps.push_back(j);

	s4 = new FSystem("White Light",31,21,0,pList[0]);
	p = new FPlanet("Clarion");
	s4->addPlanet(p);
	m_systems.push_back(s4);
	j = new FJumpRoute(s3,s4,pList);
	m_jumps.push_back(j);

	s3 = new FSystem("Theseus",35,30,0,pList[0]);
	p = new FPlanet("Minotaur");
	s3->addPlanet(p);
	m_systems.push_back(s3);
	j = new FJumpRoute(s4,s3,pList);
	m_jumps.push_back(j);

	s3 = new FSystem("Madderly's Star",26,15,0,pList[0]);
	p = new FPlanet("Kdikit");
	s3->addPlanet(p);
	m_systems.push_back(s3);
	j = new FJumpRoute(s4,s3,pList);
	m_jumps.push_back(j);

	j = new FJumpRoute(s3,s2,pList);
	m_jumps.push_back(j);

	s3 = new FSystem("K'tsa-Kar",33,15,0,pList[0]);
	p = new FPlanet("Kawdl-Kit");
	s3->addPlanet(p);
	m_systems.push_back(s3);
	j = new FJumpRoute(s4,s3,pList);
	m_jumps.push_back(j);

	s5 = new FSystem("Sathar Start Circle 5",34,19,0,pList[1]);
	m_systems.push_back(s5);
	j = new FJumpRoute(s5,s3,sathar);
	m_jumps.push_back(j);

	s4 = new FSystem("K'aken-Kar",36,11,0,pList[0]);
	p = new FPlanet("Ken'zah Kit");
	s4->addPlanet(p);
	m_systems.push_back(s4);
	j = new FJumpRoute(s3,s4,pList);
	m_jumps.push_back(j);

	s3 = new FSystem("Kizk-Kar",32,5,0,pList[0]);
	p = new FPlanet("Zik-Kit");
	s3->addPlanet(p);
	m_systems.push_back(s3);
	j = new FJumpRoute(s4,s3,pList);
	m_jumps.push_back(j);

	s5 = new FSystem("Sathar Start Circle 4",33,1,0,pList[1]);
	m_systems.push_back(s5);
	j = new FJumpRoute(s5,s3,sathar);
	m_jumps.push_back(j);

	s4 = new FSystem("Fromeltar",24,4,0,pList[0]);
	p = new FPlanet("Groth");
	s4->addPlanet(p);
	p = new FPlanet("Terledrom");
	s4->addPlanet(p);
	m_systems.push_back(s4);
	j = new FJumpRoute(s3,s4,pList);
	m_jumps.push_back(j);

	s5 = new FSystem("Sathar Start Circle 3",20,2,0,pList[1]);
	m_systems.push_back(s5);
	j = new FJumpRoute(s5,s4,sathar);
	m_jumps.push_back(j);

	s3 = new FSystem("Dramune",22,11,0,pList[0]);
	p = new FPlanet("Inner Reach");
	s3->addPlanet(p);
	p = new FPlanet("Outer Reach");
	s3->addPlanet(p);
	m_systems.push_back(s3);
	j = new FJumpRoute(s4,s3,pList);
	m_jumps.push_back(j);

	j = new FJumpRoute(s3,s2,pList);
	m_jumps.push_back(j);
}

FSystem * FMap::selectSystem(double x, double y) const{
	if (m_systems.size()>0){
		for (unsigned int i = 0; i < m_systems.size(); i++){
			double x2 = m_systems[i]->getCoord(0);
			double y2 = m_systems[i]->getCoord(1);
			if(sqrt((x-x2)*(x-x2)+(y-y2)*(y-y2)) < 0.7){
				return m_systems[i];
			}
		}
	}
	return NULL;
}

std::vector<std::string> FMap::getConnectedSystems(std::string system, unsigned int player, FFleet * fleet){
	std::vector<std::string> sList;
	std::vector<FJumpRoute *>::iterator itr;
	for(itr = m_jumps.begin(); itr < m_jumps.end();itr++){
		if((*itr)->isKnown(player)){ // make sure the jump route is known to the player
			if (fleet->isMilitia()==false    // it's a UPF or Sathar fleet
					|| (fleet->isMilitia()    //  or it's a militia fleet and the jump connects to it's home system
							&& ((*itr)->getStart()->getName()==fleet->getHomeSystem()  // either at the start
									||(*itr)->getEnd()->getName()==fleet->getHomeSystem()))) {  // or the end
				if((*itr)->getStart()->getName()==system){
					sList.push_back((*itr)->getEnd()->getName());
				}
				if((*itr)->getEnd()->getName()==system){
					sList.push_back((*itr)->getStart()->getName());
				}
			}
		}
	}
	return sList;
}

FJumpRoute * FMap::getJumpRoute(std::string start, std::string end){
	std::vector<FJumpRoute *>::iterator itr;
	std::string sName,eName;
	for(itr = m_jumps.begin(); itr < m_jumps.end();itr++){
		sName = (*itr)->getStart()->getName();
		eName = (*itr)->getEnd()->getName();
		if((sName==start && eName==end)	|| (sName==end && eName==start)){
			return (*itr);
		}
	}
	return NULL;
}

FJumpRoute * FMap::getJumpRoute(unsigned int id){
	std::vector<FJumpRoute *>::iterator itr;
	std::string sName,eName;
	for(itr = m_jumps.begin(); itr < m_jumps.end();itr++){
//		std::cerr << (*itr)->getID() << " " (*itr)->getStart()->getName() << " "
//		          << (*itr)->getEnd()->getName() << std::endl;
		if((*itr)->getID()==id){
			return (*itr);
		}
	}
	return NULL;
}

const int FMap::save(std::ostream &os) const{
	// save the size of the map
	write(os,m_maxCoord);
	// save the number of systems
	write(os,m_systems.size());
	// save each system
	for (unsigned int i = 0; i < m_systems.size(); i++){
		m_systems[i]->save(os);
	}
	// save the number of jump routes
	write(os,m_jumps.size());
	// save the individual jump routes
	for (unsigned int i = 0; i < m_jumps.size(); i++){
		m_jumps[i]->save(os);
	}
	return 0;
}

int FMap::load(std::istream &is){
//	std::cerr << "Entering FMap::load" << std::endl;
	read(is,m_maxCoord);
	size_t sysCount,jumpCount;
	read(is,sysCount);
	for (unsigned int i = 0; i < sysCount; i++){
		FSystem *s = new FSystem;
		s->load(is);
		m_systems.push_back(s);
	}
	read(is,jumpCount);
	for (unsigned int i = 0; i < jumpCount; i++){
		FJumpRoute *j = new FJumpRoute;
		j->load(is);
		j->setStart(getSystem((j->getStart())->getID()));
		j->setEnd(getSystem((j->getEnd())->getID()));
		m_jumps.push_back(j);
	}

	return 0;
}

};
