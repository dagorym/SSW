/**
 * @file FGame.cpp
 * @brief Implementation file for FGame class
 * @author Tom Stephens
 * @date Created:  Jan 12, 2005
 *
 */

#include "strategic/FGame.h"
#include "strategic/FFleet.h"
#include "gui/SystemDialogGUI.h"
#include "Frontier.h"
#include "gui/UPFUnattachedGUI.h"
#include "gui/SatharFleetsGUI.h"
#include "ships/FVehicle.h"
#include "gui/ViewFleetGUI.h"
#include "gui/SatharRetreatGUI.h"
#include "gui/SelectCombatGUI.h"
#include "core/FGameConfig.h"
#include "gui/WXGameDisplay.h"
#include "gui/WXMapDisplay.h"
#include "gui/WXPlayerDisplay.h"
#include <wx/wx.h>
#include <wx/numdlg.h>
#include <iostream>
#include <sstream>
#include <ctime>

namespace Frontier
{

FGame * FGame::m_game = 0;

FGame & FGame::create(wxWindow * win){
	if (!m_game) {
		m_game = new FGame(win);
	}
	return *m_game;
}

FGame::FGame(wxWindow * win){
	FGameConfig &gc = FGameConfig::create();
	m_parent = win;
	m_universe = NULL;
	m_round = 0;
	if (win == NULL){
		m_gui = false;
	} else {
		m_gui=true;
	}
	m_currentPlayer = -1;
	m_tenday = new wxImage(gc.getBasePath()+"icons/tenday.png");
	m_day = new wxImage(gc.getBasePath()+"icons/day.png");
	srand(time(NULL));  // intialize random number generator
	m_lostHC = 0;
	m_lostAC = 0;
	m_lostSatharShips = 0;
	m_lostTendaySathar = 0;
	m_lostTendayUPF = 0;
	m_stationsDestroyed = 0;
	m_satharRetreat = -1;

}

FGame::~FGame(){
	// clear the map
	if(m_universe !=NULL) delete m_universe;
	// delete the players
	if (m_players.size()>0) {
		for(unsigned int i=0; i< m_players.size();i++){
//			std::cerr << "deleting player " << i << std::endl;
			delete m_players[i];
		}
	}
	delete m_day;
	delete m_tenday;
	m_game = 0;
}

int FGame::init(wxWindow *w){
	if (w == NULL){
		m_gui = false;
//		std::cerr << "m_gui = " << m_gui << std::endl;
	}
  wxString errorMsg;
  if(getPlayers(m_gui)){
    if (m_gui) {
      errorMsg = "Error initalizing players.  Exiting.";
      wxMessageBox( errorMsg, "SSW Error", wxOK | wxICON_INFORMATION );
    } else {
      std::cout << errorMsg << std::endl;
    }
    return 1;
  }

  if(initMap(m_gui)) {
    errorMsg = "Error initalizing map.  Exiting";
    if (m_gui) {
      wxMessageBox( errorMsg, "SSW Error", wxOK | wxICON_INFORMATION );
    } else {
      std::cout << errorMsg << std::endl;
    }
    return 1;
  }
  draw();
  if (w!=NULL){
	  w->Refresh();
  }

  int result = initFleets();
  if(result) {
	  switch (result){
	  case 1:  //Cancelling startup
		  return 1;
		  break;
	  default:
		  errorMsg = "Error initalizing fleets.  Exiting";
		  if (m_gui) {
		    wxMessageBox( errorMsg, "SSW Error", wxOK | wxICON_INFORMATION );
		  } else {
		    std::cout << errorMsg << std::endl;
		  }
		  return 2;
		  break;
	  }
  }
  if (m_gui){
	  SatharRetreatGUI *d = new SatharRetreatGUI(m_parent);
	  m_satharRetreat = d->ShowModal();
  }
  return 0;
}

int FGame::getPlayers(bool gui){
//  int nPlayers;         // the number of players in the game
  std::string name;     // variable to read in names
  FPlayer *playerPtr;

//  if (gui){
//	  name = wxGetTextFromUser( _T("Enter a name for the UPF Player")
//			  ,_T("Player's Name:"),_T("UPF"));
	  name = "UPF";
	  playerPtr = new FPlayer();
	  playerPtr->setName(name);
	  playerPtr->setFleetIcon("icons/UPF.png");
	  m_players.push_back(playerPtr);
//	  name = wxGetTextFromUser( _T("Enter a name for the Sathar Player")
//			  ,_T("Player's Name:"),_T("Sathar"));
	  name = "Sathar";
	  playerPtr = new FPlayer();
	  m_currentPlayer = playerPtr->getID();
	  playerPtr->setName(name);
	  playerPtr->setFleetIcon("icons/Sathar.png");
	  m_players.push_back(playerPtr);
//  } else {
////	  std::cout << "Enter a name for the UPF player => ";
////	  std::cin >> name;
//	  name = "UPF";
//	  playerPtr = new FPlayer();
//	  playerPtr->setName(name);
//	  playerPtr->setFleetIcon("icons/UPF.png");
//	  m_players.push_back(playerPtr);
//	  name = "Sathar";
//	  playerPtr = new FPlayer();
//	  playerPtr->setName(name);
//	  playerPtr->setFleetIcon("icons/Sathar.png");
//	  m_players.push_back(playerPtr);
//  }

  return 0;
}

void FGame::showPlayers() {
  wxString msg = "";
  int i=1;
  for (PlayerList::iterator pItr=m_players.begin(); pItr<m_players.end(); pItr++){
    wxString tmp;
    tmp.Printf("Player %d:  %s\n",i,(*pItr)->getName().c_str());
    msg += tmp;
    i++;
  }
  if(m_gui){
	  wxMessageBox(msg, _T("Player List"),wxOK);
  } else {
	  std::cout << msg << std::endl;
  }
}

int FGame::initMap(bool gui){
	std::vector<unsigned int> pList;
	pList.push_back(m_players[0]->getID());
	pList.push_back(m_players[1]->getID());
//	if(gui){
		m_universe = &(FMap::create(false,pList));
//	} else {
    ///@todo Add the console interface
//	}
	return 0;
}

void FGame::draw(){
	// draw the base map
	if (m_parent != NULL){
		wxClientDC dc(m_parent);
		draw(dc);
	}
}

void FGame::draw(wxDC &dc){
	// draw the base map
	dc.Clear();
	if(m_universe!=NULL){
		WXMapDisplay md;
		md.draw(dc);
		// draw the fleets for each player
		WXPlayerDisplay pd;
		if(m_players[0]){
			pd.drawFleets(dc,m_players[0]);
		}
		if(m_players[1]){
			pd.drawFleets(dc,m_players[1]);
		}
		drawTurnCounter();
	}
}

int FGame::initFleets(){
	createTFPrenglar();
	createTFCassidine();
	createSFNova();
	int result = addUPFUnattached();
	if (result){
		return result;
	}
	createMilita();
	addStations();
	draw();
	result = addSatharShips();
	if (result){
		return result;
	}
	draw();

	return 0;
}

int FGame::addSatharShips(){
	FVehicle *sPtr;

	for (int i = 0; i < 25 ; i++){
		sPtr = createShip("Fighter");
		sPtr->setIcon("icons/SatharFighter.png");
		m_players[1]->addShip(sPtr);
	}
	for (int i = 0; i < 8 ; i++){
		sPtr = createShip("Frigate");
		sPtr->setIcon("icons/SatharFrigate.png");
		m_players[1]->addShip(sPtr);
	}
	for (int i = 0; i < 15 ; i++){
		sPtr = createShip("Destroyer");
		sPtr->setIcon("icons/SatharDestroyer.png");
		m_players[1]->addShip(sPtr);
	}
	for (int i = 0; i < 7 ; i++){
		sPtr = createShip("LtCruiser");
		sPtr->setIcon("icons/SatharLtCruiser.png");
		m_players[1]->addShip(sPtr);
	}
	for (int i = 0; i < 8 ; i++){
		sPtr = createShip("HvCruiser");
		sPtr->setIcon("icons/SatharHvCruiser.png");
		m_players[1]->addShip(sPtr);
	}
	for (int i = 0; i < 4 ; i++){
		sPtr = createShip("AssaultCarrier");
		sPtr->setIcon("icons/SatharAssaultCarrier.png");
		m_players[1]->addShip(sPtr);
	}

	int result;
	if (m_gui){
		SatharFleetsGUI * d = new SatharFleetsGUI(m_parent,m_players[1],m_universe,true);
		result = d->ShowModal();
//	std::cerr << "Made it out of the dialog box okay." << std::endl;
	} else {
		result = 0;
	}
	return result;
}

int FGame::addUPFUnattached(){
	FVehicle *sPtr;

	for (int i = 0; i < 4 ; i++){
		sPtr = createShip("Fighter");
		sPtr->setIcon("icons/UPFFighter.png");
		m_players[0]->addShip(sPtr);
	}
	for (int i = 0; i < 3 ; i++){
		sPtr = createShip("AssaultScout");
		m_players[0]->addShip(sPtr);
	}
	for (int i = 0; i < 2 ; i++){
		sPtr = createShip("Destroyer");
		sPtr->setIcon("icons/UPFDestroyer.png");
		m_players[0]->addShip(sPtr);
	}
	for (int i = 0; i < 2 ; i++){
		sPtr = createShip("LtCruiser");
		sPtr->setIcon("icons/UPFLtCruiser.png");
		m_players[0]->addShip(sPtr);
	}
	sPtr = createShip("AssaultCarrier");
	sPtr->setIcon("icons/UPFAssaultCarrier.png");
	m_players[0]->addShip(sPtr);

	int result;
	if (m_gui){
		UPFUnattachedGUI * d = new UPFUnattachedGUI(m_parent,m_players[0],m_universe);
		result = d->ShowModal();
//	std::cout << "UPF has " << m_players[0]->getShipList().size() << " unattached ships left" << std::endl;
//	std::cout << "UPF has " << m_players[0]->getFleetList().size() << " fleets" << std::endl;
	} else {
		result = 0;
	}
	return result;
}

void FGame::createTFCassidine(){
	FFleet *fPtr;
	FVehicle *sPtr;

	fPtr = new FFleet;
	fPtr->setName("Task Force Cassidine");
	fPtr->setLocation(m_universe->getSystem("Cassidine"),false);
	fPtr->setOwner(m_players[0]->getID());
	fPtr->setIcon(m_players[0]->getFleetIconName());

	for (int i = 0; i < 6 ; i++){
		sPtr = createShip("Fighter");
		sPtr->setIcon("icons/UPFFighter.png");
		fPtr->addShip(sPtr);
	}
	for (int i = 0; i < 3 ; i++){
		sPtr = createShip("AssaultScout");
		fPtr->addShip(sPtr);
	}
	sPtr = createShip("Minelayer");
	fPtr->addShip(sPtr);
	for (int i = 0; i < 2 ; i++){
		sPtr = createShip("Frigate");
		sPtr->setIcon("icons/UPFFrigate.png");
		fPtr->addShip(sPtr);
	}
	for (int i = 0; i < 1 ; i++){
		sPtr = createShip("LtCruiser");
		sPtr->setIcon("icons/UPFLtCruiser.png");
		fPtr->addShip(sPtr);
	}
	sPtr = createShip("HvCruiser");
	sPtr->setIcon("icons/UPFHvCruiser.png");
	fPtr->addShip(sPtr);
	sPtr = createShip("AssaultCarrier");
	sPtr->setIcon("icons/UPFAssaultCarrier.png");
	fPtr->addShip(sPtr);
	sPtr = createShip("Battleship");
	fPtr->addShip(sPtr);

	m_players[0]->addFleet(fPtr);
	m_universe->getSystem("Cassidine")->addFleet(fPtr);

}

void FGame::createTFPrenglar(){
	FFleet *fPtr;
	FVehicle *sPtr;

	fPtr = new FFleet;
	fPtr->setName("Task Force Prenglar");
	fPtr->setLocation(m_universe->getSystem("Prenglar"),false);
	fPtr->setOwner(m_players[0]->getID());
	fPtr->setIcon(m_players[0]->getFleetIconName());

	for (int i = 0; i < 5 ; i++){
		sPtr = createShip("AssaultScout");
		fPtr->addShip(sPtr);
	}
	for (int i = 0; i < 2 ; i++){
		sPtr = createShip("Minelayer");
		fPtr->addShip(sPtr);
	}
	for (int i = 0; i < 3 ; i++){
		sPtr = createShip("Frigate");
		sPtr->setIcon("icons/UPFFrigate.png");
		fPtr->addShip(sPtr);
	}
	for (int i = 0; i < 2 ; i++){
		sPtr = createShip("Destroyer");
		sPtr->setIcon("icons/UPFDestroyer.png");
		fPtr->addShip(sPtr);
	}
	for (int i = 0; i < 3 ; i++){
		sPtr = createShip("LtCruiser");
		sPtr->setIcon("icons/UPFLtCruiser.png");
		fPtr->addShip(sPtr);
	}
	sPtr = createShip("Battleship");
	fPtr->addShip(sPtr);

	m_players[0]->addFleet(fPtr);
	m_universe->getSystem("Prenglar")->addFleet(fPtr);

}

void FGame::createSFNova(){
	FFleet *fPtr;
	FVehicle *sPtr;

	fPtr = new FFleet;
	fPtr->setName("Strike Force Nova");
	fPtr->setOwner(m_players[0]->getID());
	fPtr->setIcon(m_players[0]->getFleetIconName());

	for (int i = 0; i < 6 ; i++){
		sPtr = createShip("Fighter");
		sPtr->setIcon("icons/UPFFighter.png");
		fPtr->addShip(sPtr);
	}
	for (int i = 0; i < 3 ; i++){
		sPtr = createShip("AssaultScout");
		fPtr->addShip(sPtr);
	}
	for (int i = 0; i < 2 ; i++){
		sPtr = createShip("Frigate");
		sPtr->setIcon("icons/UPFFrigate.png");
		fPtr->addShip(sPtr);
	}
	sPtr = createShip("Destroyer");
	sPtr->setIcon("icons/UPFDestroyer.png");
	fPtr->addShip(sPtr);
	for (int i = 0; i < 2 ; i++){
		sPtr = createShip("LtCruiser");
		sPtr->setIcon("icons/UPFLtCruiser.png");
		fPtr->addShip(sPtr);
	}
	sPtr = createShip("AssaultCarrier");
	sPtr->setIcon("icons/UPFAssaultCarrier.png");
	fPtr->addShip(sPtr);
	sPtr = createShip("Battleship");
	fPtr->addShip(sPtr);

	m_players[0]->addFleet(fPtr);

}

void FGame::onLeftDClick(wxMouseEvent& event) {
	wxClientDC dc(m_parent);
	WXMapDisplay md;
	std::cout << "m_x = " << event.m_x << ", m_y = " << event.m_y << std::endl;
	if (m_universe!=NULL){
		FSystem * sys = m_universe->selectSystem(event.m_x/md.getScale(dc),event.m_y/md.getScale(dc));
		FPlayer * player = (m_players[0]->getID()==m_currentPlayer)?m_players[0]:m_players[1];
		if (sys!=NULL){
			std::string title = sys->getName() + " System Information";
			SystemDialogGUI *d = new SystemDialogGUI(m_parent,sys,m_universe,player,title);
			d->ShowModal();
			m_parent->Refresh();
			return;
		}
	}
	if(m_players.size()>0){
		FFleet *f = NULL;
		for (unsigned int i = 0; i < m_players.size(); i++){
			f = m_players[i]->getFleet(event.m_x/md.getScale(dc),event.m_y/md.getScale(dc));
			if (f!=NULL){
				ViewFleetGUI *d = new ViewFleetGUI(m_parent,f,m_universe->getSystem(f->getLocation()),m_universe->getSystem(f->getDestination()));
				d->ShowModal();
				return;
			}
		}
	}
}

bool FGame::placeNova(){
	if (m_currentPlayer != m_players[0]->getID()){
		return false;
	} else {
		FFleet *fPtr;
//		unsigned int id;


		std::string system;
		int roll = irand(10);
		switch (roll){
		case 1:
		case 2:
		case 3:
			system = "Prenglar";
			break;
		case 4:
			system = "Truane's Star";
			break;
		case 5:
			system = "Dramune";
			break;
		case 6:
			system = "Fromeltar";
			break;
		case 7:
			system = "Kizk-Kar";
			break;
		case 8:
			system = "K'aken-Kar";
			break;
		case 9:
			system = "White Light";
			break;
		case 10:
			system = "K'tsa-Kar";
			break;
		default:
			return false;
			break;
		}
//		std::cerr << "SF Nova being placed in " << system << std::endl;
		fPtr = m_players[0]->getFleet("Strike Force Nova");
		fPtr->setLocation(m_universe->getSystem(system),false);
		m_universe->getSystem(system)->addFleet(fPtr);
		return true;
	}
}

void FGame::endSatharTurn(){
	moveFleets(m_players[1]);
	m_currentPlayer = m_players[0]->getID();
}

void FGame::endUPFTurn(){
	moveFleets(m_players[0]);
	m_currentPlayer = m_players[1]->getID();
	int combat = checkForCombat();
	if (combat){
		///@todo Do something appropriate  We need to get return values from checkForCombat() or set up flags
	}
	///@todo remove all destroyed ships & stations and update stats for victory conditions

	int victory = checkForVictory();
	if (victory){
		///@todo Do something appropriate
	}
	m_round++;
}

void FGame::moveFleets(FPlayer * p){
	FleetList fleets = p->getFleetList();
	for (unsigned int i = 0; i< fleets.size(); i++){
		if (fleets[i]->getInTransit()){  // fleet is on the move
//			std::cerr << fleets[i]->getName() << " is on the move.  Tranisit time is "
//						<< fleets[i]->getTransitTime() << std::endl;
			int time = fleets[i]->decTransitTime();
			if (time == -1){  // we failed the jump
				std::string msg = "The fleet " + fleets[i]->getName()
						+ " has failed it's risk jump and has been lost";
				if (m_gui){
					wxMessageDialog d(m_parent, msg,	"Failed Risk Jump",wxOK);
					d.ShowModal();
				} else {
					std::cout << msg << std::endl;
				}
				// add code here to remove fleet
				FFleet *f = p->removeFleet(fleets[i]->getID());
				m_universe->getSystem(fleets[i]->getLocation())->removeFleet(fleets[i]->getID());
				delete f;  // ships are gone
				continue;// and skip over the rest of the code this time through the loop
			}
//			std::cerr << "New transit time is " << fleets[i]->getTransitTime() << std::endl;
			FJumpRoute *j = m_universe->getJumpRoute(fleets[i]->getJumpRoute());
			if (time <= j->getLength()/2 && time > 0){  // we've actually made the jump but we're not there yet.
				if(fleets[i]->getLocation()!=fleets[i]->getDestination()){
					// move it into the other system
					FFleet * f = m_universe->getSystem(fleets[i]->getLocation())->removeFleet(fleets[i]->getID());
					m_universe->getSystem(fleets[i]->getDestination())->addFleet(f);
					fleets[i]->setLocation(fleets[i]->getDestination());
				}
			}
		}
	}
}

const int FGame::save(std::ostream &os) const {
	// save game state
	write(os,m_gui);
	write(os,m_round);
	write(os,m_currentPlayer);
	write(os,m_satharRetreat);
	write(os,m_lostHC);
	write(os,m_lostAC);
	write(os,m_lostSatharShips);
	write(os,m_lostTendaySathar);
	write(os,m_lostTendayUPF);
	write(os,m_stationsDestroyed);
	// save the map
	m_universe->save(os);
	// save the players
	// first save the number of players
	write(os,m_players.size());
	// then save each player's data
	for (unsigned int i=0; i< m_players.size(); i++){
		m_players[i]->save(os);
	}
	return 0;
}

int FGame::load(std::istream &is){
//	std::cerr << "Entering FGame::load" << std::endl;
	read(is,m_gui);
	read(is,m_round);
	read(is,m_currentPlayer);
	read(is,m_satharRetreat);
	read(is,m_lostHC);
	read(is,m_lostAC);
	read(is,m_lostSatharShips);
	read(is,m_lostTendaySathar);
	read(is,m_lostTendayUPF);
	read(is,m_stationsDestroyed);
	m_universe = &(FMap::create());
	m_universe->load(is);
	size_t pCount;
	read(is,pCount);
//	std::cerr << "There are " << pCount << " players" << std::endl;
	for (unsigned int i = 0; i < pCount; i++){
		FPlayer *p = new FPlayer;
		p->load(is);
		// we need to place the fleets into their respective systems
		FleetList fList = p->getFleetList();
//		std::cerr << "fList has " << fList.size() << " entries for player " << p->getID() << std::endl;
		for (FleetList::iterator f = fList.begin(); f < fList.end(); f++){
			FSystem *s = m_universe->getSystem((*f)->getLocation());
//			std::cerr << (*f)->getName() << std::endl;
			if (s != NULL){
//				std::cerr << s->getName() << std::endl;
				s->addFleet((*f));
			}
		}
		m_players.push_back(p);
	}
	return 0;
}

void FGame::createMilita(){
	createWhiteLightMilita();
	createGrunaGaruMilita();
	createAraksMilita();
	createDramuneMilita();
	createKakenKarMilita();
	createTheseusMilita();
	createTruanesStarMilita();
	createFromeltarMilita();
	createKizkKarMilita();
}

void FGame::createWhiteLightMilita(){
	FFleet *fPtr;
	FVehicle *sPtr;

	fPtr = new FFleet;
	fPtr->setName("Clarion Militia");
	fPtr->setLocation(m_universe->getSystem("White Light"),false);
	fPtr->setOwner(m_players[0]->getID());
	fPtr->setIcon("icons/WL-frigate.png");
	fPtr->setMilitia(true,"White Light");

	for (int i = 0; i < 3 ; i++){
		sPtr = createShip("AssaultScout");
		sPtr->setIcon("icons/WL-AS.png");
		fPtr->addShip(sPtr);
	}
	sPtr = createShip("Frigate");
	sPtr->setIcon("icons/WL-frigate.png");
	fPtr->addShip(sPtr);

	m_players[0]->addFleet(fPtr);
	m_universe->getSystem("White Light")->addFleet(fPtr);
}

void FGame::createGrunaGaruMilita(){
	FFleet *fPtr;
	FVehicle *sPtr;

	fPtr = new FFleet;
	fPtr->setName("Hargut Militia");
	fPtr->setLocation(m_universe->getSystem("Gruna Garu"),false);
	fPtr->setOwner(m_players[0]->getID());
	fPtr->setIcon("icons/Hargut-AS.png");
	fPtr->setMilitia(true,"Gruna Garu");

	for (int i = 0; i < 2 ; i++){
		sPtr = createShip("AssaultScout");
		sPtr->setIcon("icons/Hargut-AS.png");
		fPtr->addShip(sPtr);
	}

	m_players[0]->addFleet(fPtr);
	m_universe->getSystem("Gruna Garu")->addFleet(fPtr);
}

void FGame::createAraksMilita(){
	FFleet *fPtr;
	FVehicle *sPtr;

	fPtr = new FFleet;
	fPtr->setName("Hentz Militia");
	fPtr->setLocation(m_universe->getSystem("Araks"),false);
	fPtr->setOwner(m_players[0]->getID());
	fPtr->setIcon("icons/Hentz-frigate.png");
	fPtr->setMilitia(true,"Araks");

	for (int i = 0; i < 3 ; i++){
		sPtr = createShip("AssaultScout");
		sPtr->setIcon("icons/Hentz-AS.png");
		fPtr->addShip(sPtr);
	}
	sPtr = createShip("Frigate");
	sPtr->setIcon("icons/Hentz-frigate.png");
	fPtr->addShip(sPtr);

	m_players[0]->addFleet(fPtr);
	m_universe->getSystem("Araks")->addFleet(fPtr);
}

void FGame::createDramuneMilita(){
	FFleet *fPtr;
	FVehicle *sPtr;

	// Inner Reach
	fPtr = new FFleet;
	fPtr->setName("Inner Reach Militia");
	fPtr->setLocation(m_universe->getSystem("Dramune"),false);
	fPtr->setOwner(m_players[0]->getID());
	fPtr->setIcon("icons/Dramune-destroyer.png");
	fPtr->setMilitia(true,"Dramune");

	for (int i = 0; i < 3 ; i++){
		sPtr = createShip("AssaultScout");
		sPtr->setIcon("icons/Dramune-AS.png");
		fPtr->addShip(sPtr);
	}
	sPtr = createShip("Frigate");
	sPtr->setIcon("icons/Dramune-frigate.png");
	fPtr->addShip(sPtr);
	sPtr = createShip("Destroyer");
	sPtr->setIcon("icons/Dramune-destroyer.png");
	fPtr->addShip(sPtr);

	m_players[0]->addFleet(fPtr);
	m_universe->getSystem("Dramune")->addFleet(fPtr);

	// Outer Reach
	fPtr = new FFleet;
	fPtr->setName("Outer Reach Militia");
	fPtr->setLocation(m_universe->getSystem("Dramune"),false);
	fPtr->setOwner(m_players[0]->getID());
	fPtr->setIcon("icons/Dramune-frigate.png");
	fPtr->setMilitia(true,"Dramune");

	for (int i = 0; i < 2 ; i++){
		sPtr = createShip("AssaultScout");
		sPtr->setIcon("icons/Dramune-AS.png");
		fPtr->addShip(sPtr);
	}
	sPtr = createShip("Frigate");
	sPtr->setIcon("icons/Dramune-frigate.png");
	fPtr->addShip(sPtr);
	sPtr = createShip("Frigate");
	sPtr->setIcon("icons/Dramune-frigate.png");
	fPtr->addShip(sPtr);

	m_players[0]->addFleet(fPtr);
	m_universe->getSystem("Dramune")->addFleet(fPtr);
}

void FGame::createKakenKarMilita(){
	FFleet *fPtr;
	FVehicle *sPtr;

	fPtr = new FFleet;
	fPtr->setName("Ken'zah Kit Militia");
	fPtr->setLocation(m_universe->getSystem("K'aken-Kar"),false);
	fPtr->setOwner(m_players[0]->getID());
	fPtr->setIcon("icons/Kaken-Kar-AS.png");
	fPtr->setMilitia(true,"K'aken-Kar");

	for (int i = 0; i < 2 ; i++){
		sPtr = createShip("AssaultScout");
		sPtr->setIcon("icons/Kaken-Kar-AS.png");
		fPtr->addShip(sPtr);
	}

	m_players[0]->addFleet(fPtr);
	m_universe->getSystem("K'aken-Kar")->addFleet(fPtr);
}

void FGame::createTheseusMilita(){
	FFleet *fPtr;
	FVehicle *sPtr;

	fPtr = new FFleet;
	fPtr->setName("Minotaur Militia");
	fPtr->setLocation(m_universe->getSystem("Theseus"),false);
	fPtr->setOwner(m_players[0]->getID());
	fPtr->setIcon("icons/Theseus-destroyer.png");
	fPtr->setMilitia(true,"Theseus");

	for (int i = 0; i < 4 ; i++){
		sPtr = createShip("AssaultScout");
		sPtr->setIcon("icons/Theseus-AS.png");
		fPtr->addShip(sPtr);
	}
	sPtr = createShip("Frigate");
	sPtr->setIcon("icons/Theseus-frigate.png");
	fPtr->addShip(sPtr);
	sPtr = createShip("Destroyer");
	sPtr->setIcon("icons/Theseus-destroyer.png");
	fPtr->addShip(sPtr);

	m_players[0]->addFleet(fPtr);
	m_universe->getSystem("Theseus")->addFleet(fPtr);
}

void FGame::createTruanesStarMilita(){
	FFleet *fPtr;
	FVehicle *sPtr;

	fPtr = new FFleet;
	fPtr->setName("Pale Militia");
	fPtr->setLocation(m_universe->getSystem("Truane's Star"),false);
	fPtr->setOwner(m_players[0]->getID());
	fPtr->setIcon("icons/Truane-frigate.png");
	fPtr->setMilitia(true,"Truane's Star");

	for (int i = 0; i < 3 ; i++){
		sPtr = createShip("AssaultScout");
		sPtr->setIcon("icons/Truane-AS.png");
		fPtr->addShip(sPtr);
	}
	sPtr = createShip("Frigate");
	sPtr->setIcon("icons/Truane-frigate.png");
	fPtr->addShip(sPtr);

	m_players[0]->addFleet(fPtr);
	m_universe->getSystem("Truane's Star")->addFleet(fPtr);
}

void FGame::createFromeltarMilita(){
	FFleet *fPtr;
	FVehicle *sPtr;

	fPtr = new FFleet;
	fPtr->setName("Terledrom Militia");
	fPtr->setLocation(m_universe->getSystem("Fromeltar"),false);
	fPtr->setOwner(m_players[0]->getID());
	fPtr->setIcon("icons/Fromeltar-frigate.png");
	fPtr->setMilitia(true,"Fromeltar");

	for (int i = 0; i < 3 ; i++){
		sPtr = createShip("AssaultScout");
		sPtr->setIcon("icons/Fromeltar-AS.png");
		fPtr->addShip(sPtr);
	}
	sPtr = createShip("Frigate");
	sPtr->setIcon("icons/Fromeltar-frigate.png");
	fPtr->addShip(sPtr);

	m_players[0]->addFleet(fPtr);
	m_universe->getSystem("Fromeltar")->addFleet(fPtr);
}

void FGame::createKizkKarMilita(){
	FFleet *fPtr;
	FVehicle *sPtr;

	fPtr = new FFleet;
	fPtr->setName("Zik-kit Militia");
	fPtr->setLocation(m_universe->getSystem("Kizk-Kar"),false);
	fPtr->setOwner(m_players[0]->getID());
	fPtr->setIcon("icons/Kizk-Kar-AS.png");
	fPtr->setMilitia(true,"Kizk-Kar");

	for (int i = 0; i < 2 ; i++){
		sPtr = createShip("AssaultScout");
		sPtr->setIcon("icons/Kizk-Kar-AS.png");
		fPtr->addShip(sPtr);
	}

	m_players[0]->addFleet(fPtr);
	m_universe->getSystem("Kizk-Kar")->addFleet(fPtr);
}

void FGame::drawTurnCounter(){
	wxClientDC dc(m_parent);
	wxCoord w, h, s;
	dc.GetSize(&w, &h);
	s = ((w > h)?h:w)/20;

	wxColour white,blue,red,lgray,black,dblue,dred,lred,lblue;
	white.Set(wxT("#FFFFFF"));// white
	blue.Set(wxT("#0000FF"));// blue
	red.Set(wxT("#FF0000"));// red
	lgray.Set(wxT("#999999"));// light grey
	black.Set(wxT("#000000"));// blue
	dblue.Set(wxT("#000099"));// dark blue
	dred.Set(wxT("#770000"));// dark red
	lblue.Set(wxT("#9999FF"));// light blue
	lred.Set(wxT("#FF9999")); // light red

	// draw the turn boxes
	dc.SetBrush(wxBrush(white));
	dc.SetPen(wxPen(black));
	dc.SetTextForeground(black);
	dc.SetFont(wxFont((int)s/2,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL));
	for (int i = 0; i< 5; i++){
		for (int j=0; j<2; j++){
			dc.DrawRectangle(i*s,j*s,s,s);
			std::ostringstream os;
			os << i+5*j;
			dc.DrawText(os.str(),(wxCoord)(i*s+0.3*s),(wxCoord)(j*s+0.1*s));

		}
	}
	// now place the counters
	int day = m_round % 10;
	int tenday = m_round /10;
	int row,col;
	row = tenday/5;
	col = tenday%5;
	dc.DrawBitmap(wxBitmap(m_tenday->Scale(4*s/5,4*s/5)),(wxCoord)(col*s+0.1*s),(wxCoord)(row*s+0.1*s));
	row = day/5;
	col = day%5;
	dc.DrawBitmap(wxBitmap(m_day->Scale(4*s/5,4*s/5)),(wxCoord)(col*s+0.2*s),(wxCoord)(row*s+0.2*s));

	// draw the end turn button
	dc.SetFont(wxFont((int)(s/3.),wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL));
	if (m_currentPlayer == m_players[0]->getID()){
		dc.SetBrush(wxBrush(lblue));
		dc.DrawRoundedRectangle(0,2*s,4*s,s,s*0.25);
		dc.DrawText("End UPF Turn",(wxCoord)(0.5*s),(wxCoord)(2.25*s));
	} else {
		dc.SetBrush(wxBrush(lred));
		dc.DrawRoundedRectangle(0,2*s,4*s,s,s*0.25);
		dc.DrawText("End Sathar Turn",(wxCoord)(0.2*s),(wxCoord)(2.25*s));
	}
}

int FGame::onLeftUp(wxMouseEvent &event){
	wxClientDC dc(m_parent);
	wxCoord w, h, s, x, y;
	dc.GetSize(&w, &h);
	s = ((w > h)?h:w)/20;
	event.GetPosition(&x,&y);
	// Did they click the end turn button
	if ( x<4*s && y>2*s && y<3*s ){ // yes
		if (m_currentPlayer == m_players[0]->getID()){
			endUPFTurn();
			return 1;
		} else {
			endSatharTurn();
			return 2;
		}
		m_parent->Refresh();
	}
	return 0;
}

void FGame::addStations(){
	FVehicle *s = NULL;
	//First the fortresses
	s = createShip("Fortress");
	s->setName("Fortress Kdikit");
	m_universe->getSystem("Madderly's Star")->getPlanet("Kdikit")->addStation(s);

	s = createShip("Fortress");
	s->setName("Fortress Redoubt");
	m_universe->getSystem("White Light")->getPlanet("Clarion")->addStation(s);

	s = createShip("Fortress");
	s->setName("Fortress Gollwin");
	m_universe->getSystem("Prenglar")->getPlanet("Morgaine's World")->addStation(s);

	s = createShip("Fortress");
	s->setName("Fortress Pale");
	m_universe->getSystem("Truane's Star")->getPlanet("Pale")->addStation(s);

	// Fortified Stations
	s = createShip("FortifiedStation");
	m_universe->getSystem("Theseus")->getPlanet("Minotaur")->addStation(s);

	s = createShip("FortifiedStation");
	m_universe->getSystem("K'aken-Kar")->getPlanet("Ken'zah Kit")->addStation(s);

	s = createShip("FortifiedStation");
	m_universe->getSystem("Araks")->getPlanet("Hentz")->addStation(s);

	s = createShip("FortifiedStation");
	m_universe->getSystem("Prenglar")->getPlanet("Gran Quivera")->addStation(s);

	// Armed Stations
	s = createShip("ArmedStation");
	m_universe->getSystem("K'tsa-Kar")->getPlanet("Kawdl-Kit")->addStation(s);

	s = createShip("ArmedStation");
	m_universe->getSystem("Kizk-Kar")->getPlanet("Zik-Kit")->addStation(s);

	s = createShip("ArmedStation");
	m_universe->getSystem("Fromeltar")->getPlanet("Groth")->addStation(s);

	s = createShip("ArmedStation");
	m_universe->getSystem("Fromeltar")->getPlanet("Terledrom")->addStation(s);

	s = createShip("ArmedStation");
	m_universe->getSystem("Truane's Star")->getPlanet("New Pale")->addStation(s);

	s = createShip("ArmedStation");
	m_universe->getSystem("Dramune")->getPlanet("Inner Reach")->addStation(s);

	s = createShip("ArmedStation");
	m_universe->getSystem("Dramune")->getPlanet("Outer Reach")->addStation(s);

	s = createShip("ArmedStation");
	m_universe->getSystem("Cassidine")->getPlanet("Rupert's Hole")->addStation(s);

	s = createShip("ArmedStation");
	m_universe->getSystem("Cassidine")->getPlanet("Triad")->addStation(s);

	s = createShip("ArmedStation");
	m_universe->getSystem("Gruna Garu")->getPlanet("Hargut")->addStation(s);

	s = createShip("ArmedStation");
	m_universe->getSystem("Timeon")->getPlanet("Lossend")->addStation(s);
}

bool FGame::isUPFTurn(){
	if(m_currentPlayer == m_players[0]->getID()){
		return true;
	} else {
		return false;
	}
}

void FGame::onAddSatharShips(){
	if(m_currentPlayer == m_players[1]->getID()){  // make sure it is the Sathar player's turn
		SatharFleetsGUI * d = new SatharFleetsGUI(m_parent,m_players[1],m_universe,false);
		/*int result = */d->ShowModal();

	}
}

void FGame::showRetreatConditions(){
	std::ostringstream msg;
	msg << "The Sathar retreat condition is:\n\n";
	switch (m_satharRetreat) {
	case 1:
		msg << "The Sathar have lost two assault carriers.\n\n\n";
		msg << "Current Status:\n\n";
		msg << "  The Sathar have lost " << m_lostAC << " Assault Carriers.";
		break;
	case 2:
		msg << "The Sathar have lost five heavy cruisers.\n\n\n";
		msg << "Current Status:\n\n";
		msg << "  The Sathar have lost " << m_lostHC << " Heavy Cruisers.";
		break;
	case 3:
		msg << "The Sathar have lost 40 ships, including fighters.\n\n\n";
		msg << "Current Status:\n\n";
		msg << "  The Sathar have lost " << m_lostSatharShips << " ships.";
		break;
	case 4:
		msg << "A tenday passes without the Sathar destroying at least two\n"
			<< "stations, two fortresses or a station and a fortress.\n\n\n";
		msg << "Current Status:\n\n";
		msg << "  The Sathar have destroyed " << m_stationsDestroyed << " stations\n"
			<< "  so far this tenday.";
		break;
	case 5:
		msg << "At the end of any tenday, the Sathar have lost more ships in\n"
			<< "combat than the UPF.  Fighters and Militia ships are not counted\n"
			<< "toward this condition.\n\n\n";
		msg << "Current Status:\n\n";
		msg << "  The Sathar have lost " << m_lostTendaySathar << " ships and the UPF\n"
			<< "  have lost " << m_lostTendayUPF << " ships so far this tenday.";
		break;
	default:
		msg << "Error:  No retreat condition selected";
		break;
	}
	if (m_gui){
		wxMessageDialog d(m_parent,msg.str(),"Sathar Retreat Conditions",wxOK);
		d.ShowModal();
	} else {
		std::cout << msg.str() << std::endl;
	}
}

int FGame::checkForVictory(){
	int fortressCount = 0;
	int stationCount = 0;
	bool UPFVictory = false;
	bool SatharVictory = false;
	// first count all the remaining stations and number of Fortresses
	SystemList sysList = m_universe->getSystemList();
	for (SystemList::iterator sItr = sysList.begin(); sItr < sysList.end(); sItr++){
		PlanetList pList = (*sItr)->getPlanetList();
		for (PlanetList::iterator pItr = pList.begin(); pItr < pList.end(); pItr++){
			FVehicle *s = (*pItr)->getStation();
			if (s != NULL){
				stationCount++;
				if(s->getType() == "Fortress"){
					fortressCount++;
				}
			}
		}
	}
	// and check to see if the Sathar have destroyed enough to win.
	if (fortressCount == 0 && stationCount <=7 ){
		SatharVictory = true;
	}
	// now check for victory based on sathar retreat conditions
	switch (m_satharRetreat){
	case 1:
		if(m_lostAC>=2){
			UPFVictory = true;
		}
		break;
	case 2:
		if (m_lostHC>=5){
			UPFVictory = true;
		}
		break;
	case 3:
		if (m_lostSatharShips>=40){
			UPFVictory = true;
		}
		break;
	case 4:
		if (m_round % 10 == 0 && m_round > 0){ // its the end of a tenday
			if(m_lostTendaySathar > m_lostTendayUPF){
				UPFVictory = true;
			}
			// it's the end of the tenday so reset the counter
			m_lostTendaySathar = 0;
			m_lostTendayUPF = 0;
		}
		break;
	case 5:
		if (m_round % 10 == 0 && m_round > 0){ // its the end of a tenday
			if(m_stationsDestroyed < 2){
				UPFVictory = true;
			}
			// it's the end of the tenday so reset the counter
			m_stationsDestroyed = 0;
		}
		break;
	default:
		UPFVictory = false;
		break;
	}
	// okay, now we know if the sathar retreat conditions have been met, lets see what the game
	// status is.
	if (UPFVictory){  // the UPF have conditions for victory
		if(SatharVictory){ // but so do the Sathar
			wxMessageDialog d(m_parent,"Both the UPF and Sathar satisfied their victory\nconditions this round.\n\nThe game is a draw",
					"It's a Draw!", wxOK);
			d.ShowModal();
			return 3;  // it's a draw
		}
		if(fortressCount == 1){ // but there is only 1 Fortress left
			wxMessageDialog d(m_parent,"The Sathar have been forced to retreat but there\nis only one remaining UPF Fortress.\n\nThe game is a draw",
					"It's a Draw!", wxOK);
			d.ShowModal();
			return 3;  // it's a draw
		}
		if (fortressCount >= 2){ // Sathar didn't kill enough fortresses
			wxMessageDialog d(m_parent,"The Sathar have been forced to retreat and there\nare at least two UPF Fortress still intact.\n\nThe UPF win!",
					"UPF Victory!", wxOK);
			d.ShowModal();
			return 1; // it's a UPF victory
		}
	} else {  // UPF don't have victory conditions
		if (SatharVictory){  // but the Sathar do
			wxMessageDialog d(m_parent,"The Sathar have destroyed twelve stations including\nall four Fortresses.\n\nThe Sathar win!",
					"UPF Victory!", wxOK);
			d.ShowModal();
			return 2;  // it's a Sathar Victory
		}
	}
	// otherwise, no victory conditions met, continue onward.
	return 0;
}

int FGame::checkForCombat(){
	// loop over the all the systems to see of there are opposing fleets in the system
	SystemList sysList = m_universe->getSystemList();
	bool combatOccurred = false;

	for (SystemList::iterator sItr = sysList.begin(); sItr < sysList.end(); sItr++){
		bool sathar = false;
		bool upf = false;
		// first Check the planets for stations.
		PlanetList pList = (*sItr)->getPlanetList();
		for (PlanetList::iterator pItr = pList.begin(); pItr < pList.end(); pItr++){
			FVehicle *s = (*pItr)->getStation();
			if (s != NULL){
				upf = true;
			}
		}
		// next Check the fleets
		FleetList fList = (*sItr)->getFleetList();
		for(FleetList::iterator fItr = fList.begin(); fItr < fList.end(); fItr++){
			if (!((*fItr)->getInTransit())){
				if ((*fItr)->getOwner() == m_players[0]->getID()){
					upf = true;
				} else {
					sathar = true;
				}
			}
		}
		if (upf && sathar){ // we have a combat possibility
			resolveCombat((*sItr)->getName());
			combatOccurred=true;
		}
	}
	if (combatOccurred){
		cleanUpShips();
	}

	return 0;
}

void FGame::resolveCombat(std::string sysName){
	// first let's get a list of all the fleets on each side in the system
	FSystem *sys = m_universe->getSystem(sysName);
	FleetList allFleets, upfList, satharList;
	allFleets = sys->getFleetList();
//	std::cerr << "allFleets list has " << allFleets.size() << " entries" << std::endl;
	for(FleetList::iterator itr = allFleets.begin(); itr < allFleets.end(); itr++){
//		std::cerr << "Processing " << (*itr)->getName() << "  " << (*itr)->getOwner() << std::endl;
		if ((*itr)->getOwner() == m_players[0]->getID()){
			upfList.push_back(*itr);
		} else {
			satharList.push_back(*itr);
//			std::cerr << "Adding " << (*itr)->getName() << " to list" << std::endl;
		}
	}
	// okay now pop up a dialog showing the fleets and giving the Sathar the option to attack.
	SelectCombatGUI d(m_parent,sys,upfList,satharList,&m_players);
	d.ShowModal();
}

FPlayer * FGame::getPlayer(unsigned int id) const{
	for (unsigned int i = 0; i < m_players.size(); i++){
		if (m_players[i]->getID()==id){
			return (m_players[i]);
		}
	}
	return NULL;
}

void FGame::cleanUpShips(){
	// loop over all systems and planets and check to stations
	SystemList sysList = m_universe->getSystemList();
	for (SystemList::iterator sysItr = sysList.begin(); sysItr < sysList.end(); sysItr++){
		PlanetList pList = (*sysItr)->getPlanetList();
		for (PlanetList::iterator pItr = pList.begin(); pItr < pList.end(); pItr++){
			FVehicle *station =  (*pItr)->getStation();
			if (station!=NULL && station->getHP()<=0){ // If the station has 0 HP it's been destroyed
				m_stationsDestroyed++;  // increment the destroyed station count
				(*pItr)->destroyStation();
			}
		}
	}

	// loop over all the player's fleets and check their ships
	for (PlayerList::iterator pItr = m_players.begin(); pItr < m_players.end(); pItr++){
		FleetList fList = (*pItr)->getFleetList();
		for (FleetList::iterator fItr = fList.begin(); fItr < fList.end(); fItr++){
			VehicleList sList = (*fItr)->getShipList();
			for (VehicleList::iterator sItr = sList.begin(); sItr < sList.end(); sItr ++){
				if ((*sItr)->getHP() <= 0){  // if the ship is destroyed remove it
					(*pItr)->addDestroyedShip(*sItr);  // add the ship to the player's destroyed ship list
					// update destroyed ship counters
					if ((*pItr)->getName()=="UPF"){
						m_lostTendayUPF++;
					} else {
						m_lostTendaySathar++;
						if ((*sItr)->getType()!= "Fighter"){
							m_lostSatharShips++;
						}
						if ((*sItr)->getType() == "HvCruiser"){
							m_lostHC++;
						}
						if ((*sItr)->getType() == "AssaultCarrier"){
							m_lostAC++;
						}
					}
					sItr = sList.erase(sItr);  // remove the ship from the fleet.
				} else { // reload it's weapons
					///@todo this should eventually check for being in supply
					(*sItr)->reload();
				}
			}
			if (sList.size() == 0){  // if the fleet is now empty remove it
				unsigned int sysID = (*fItr)->getLocation();
				m_universe->getSystem(sysID)->removeFleet((*fItr)->getID());  // remove the fleet from the system it's in
				fItr = fList.erase(fItr);  // remove the fleet from the player's list.
			}
		}
	}
}

};
