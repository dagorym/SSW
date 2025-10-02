/**
 * @file FBattleScreen.cpp
 * @brief Implementation file for BattleScreen class
 * @author Tom Stephens
 * @date Created:  Jul 11, 2008
 *
 */

#include "tactical/FBattleScreen.h"
#include "Frontier.h"
#include "core/FGameConfig.h"
#include "gui/ICMSelectionGUI.h"
#include <wx/wx.h>

namespace Frontier {

FBattleScreen::FBattleScreen(const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( (wxDialog *)NULL, -1, title, pos, size, style )
//FBattleScreen::FBattleScreen(const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( (wxFrame *)NULL, -1, title, pos, size, style )
{
//	m_wd = new wxWindowDisabler(this);
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
//	this->MakeModal(true);
	wxColour black(wxT("#000000"));// black
	wxColour white(wxT("#FFFFFF"));// white

//	CreateStatusBar();
//	SetStatusText( "Welcome to the BattleScreen!" );


	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer1->AddGrowableCol( 0 );
	fgSizer1->AddGrowableRow( 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_map = new FBattleBoard( this );
	fgSizer1->Add( m_map, 5, wxALIGN_CENTER|wxALL|wxEXPAND, 1 );

	m_display = new FBattleDisplay( this );

	fgSizer1->Add( m_display, 1, wxEXPAND | wxALL, 1 );

	this->SetSizer( fgSizer1 );
	this->Layout();
	Centre();
	SetOwnBackgroundColour(black);

	m_attackShips = NULL;
	m_defendShips = NULL;
	m_state = BS_Unknown;
	m_phase = PH_NONE;
	m_playerID[0]=0;
	m_playerID[1]=1;
	m_movingPlayer=true;
	m_curWeapon = NULL;
	m_curShip = NULL;
	m_activePlayer = -1;
	m_attackList = NULL;
	m_defendList = NULL;
	m_control = false;
	m_moveComplete = true;
	m_planetChoice = -1;
	m_station = NULL;
	m_hasPlanet = false;
	m_done = false;

}

FBattleScreen::~FBattleScreen(){
	if (m_attackShips) { delete m_attackShips; }
	if (m_defendShips) { delete m_defendShips; }
//	delete m_wd;
//	this->MakeModal(false);
}

void FBattleScreen::draw(){
}
void FBattleScreen::onPaint(wxPaintEvent & event){
}

int FBattleScreen::setupFleets(FleetList *aList, FleetList *dList, bool planet, FVehicle * station){
	m_attackList = aList;
	m_defendList = dList;
	m_hasPlanet = planet;
	m_station = station;
	m_activePlayer = false;
//	m_stationPos.cx=0;
//	m_stationPos.cy=0;
//	m_planetPos.cx=0;
//	m_planetPos.cy=0;

	//create a list of ships from the list of fleets for the attacker and defenders
	if (m_defendShips) { delete m_defendShips; }
//	std::cerr << "There are " << m_defendList->size() << " defending fleets" << std::endl;
	m_defendShips = new VehicleList;
	for (unsigned int i=0; i< m_defendList->size(); i++){
		const VehicleList sList = (*m_defendList)[i]->getShipList();
		for (unsigned int j=0; j< sList.size(); j++){
			sList[j]->setOwner(0);
			sList[j]->setCurrentDefense(0);
			m_defendShips->push_back(sList[j]);
		}
	}
	if (station!=NULL){  // the station always belongs to the defender
		station->setCurrentDefense(0);
		m_defendShips->push_back(station);
	}
	if (m_attackShips) { delete m_attackShips; }
	m_attackShips = new VehicleList;
//	std::cerr << "There are " << m_attackList->size() << " attacking fleets" << std::endl;
	for (unsigned int i=0; i< m_attackList->size(); i++){
		const VehicleList sList = (*m_attackList)[i]->getShipList();
		for (unsigned int j=0; j< sList.size(); j++){
			sList[j]->setOwner(1);
			sList[j]->setCurrentDefense(0);
			m_attackShips->push_back(sList[j]);
		}
	}

	m_control=false;
	if(planet){
		FGameConfig &gc = FGameConfig::create();
		m_state = BS_SetupPlanet;
		ImageList iList;
		wxImage p0(gc.getBasePath()+"icons/planet_01.png");
		iList.push_back(p0);
		wxImage p1(gc.getBasePath()+"icons/planet_02.png");
		iList.push_back(p1);
		wxImage p2(gc.getBasePath()+"icons/planet_03.png");
		iList.push_back(p2);
		m_display->setImageList(iList);
		m_map->setPlanetImages(iList);
	} else if (station != NULL){
		m_state=BS_SetupStation;
	} else if (m_defendList->size()>0) {
		m_state=BS_SetupDefendFleet;
	} else {
		m_state=BS_SetupAttackFleet;
	}
	return 0;
}

void FBattleScreen::setState(int s) {
	if (s == BS_SetupStation && m_station == NULL){
		s = BS_SetupDefendFleet;
	}
	if (s == BS_SetupDefendFleet && m_defendShips->size()==1){
		s = BS_SetupAttackFleet;
		toggleActivePlayer();
	}
	m_state = s;
	m_map->Refresh();
	m_display->Refresh();
}

VehicleList FBattleScreen::getShipList() const{
	if (m_activePlayer){
		return *m_attackShips;
	} else {
		return *m_defendShips;
	}
}

VehicleList FBattleScreen::getShipList(unsigned int id) const{
	if (id == getAttackerID()){
		return *m_attackShips;
	} else {
		return *m_defendShips;
	}
}

void FBattleScreen::setScale(double factor) {
//	std::cerr<< "calling map setScale()" << std::endl;
	m_map->setScale(factor);
}

void FBattleScreen::setPhase(int p){
	m_phase = p;
	if (p==PH_MOVE) { // we just ended a turn
		if (!m_activePlayer) {  // defender just ended
			///@todo update turn counters
			///@todo check for repair turn
//			std::cerr << "Defender just ended their movement turn" << std::endl;
		}
		toggleActivePlayer();
		m_map->resetMoveData();

	} else 	if (p==PH_FINALIZE_MOVE){
		m_map->finalizeMove();
		/// @todo drop into combat phase
//		toggleMovingPlayer();
//		setPhase(PH_MOVE);
		setPhase(PH_DEFENSE_FIRE);
		m_curShip = NULL;
	} else if (p==PH_DEFENSE_FIRE){
		toggleActivePlayer();
	} else if (p==PH_ATTACK_FIRE){
		applyFireDamage();
		toggleActivePlayer();
	} else {
	}
	m_map->Refresh();
	m_display->Refresh();
}

void FBattleScreen::setMoveComplete(bool s) {
	bool refresh = false;
	if (m_moveComplete!=s){
		refresh = true;
	}
	m_moveComplete = s;
	if (refresh){
		m_map->Refresh();
		m_display->Refresh();
	}
}

void FBattleScreen::setWeapon(FWeapon * w) {
//	if (w!=NULL) std::cerr << "Setting current weapon to " << w->getLongName() << std::endl;
	m_curWeapon = w;
//	std::cerr << "computing weapon range" << std::endl;
	m_map->computeWeaponRange();
//	std::cerr << "range computed." << std::endl;
}

void FBattleScreen::clearDestroyedShips(){
	toggleActivePlayer(); // switch to the player getting shot at
	VehicleList *sList = NULL;
	if (m_activePlayer){
		sList = m_attackShips;
	} else {
		sList = m_defendShips;
	}
	int liveShips = 0;
	VehicleList::iterator itr = sList->begin();
	while ( itr < sList->end()){
		if ((*itr)->getHP() <= 0 ){
			m_map->removeShipFromGame((*itr)->getID());
			itr = sList->erase(itr);
		} else {
			liveShips++;
			itr++;
		}
	}
	toggleActivePlayer();  // switch back
	if (!liveShips){
		declareWinner();
	}
}

void FBattleScreen::declareWinner(){
	std::string msg = "The winner of the battle is \nPlayer ";
	msg+= (getActivePlayer())?"Sathar":"UPF";
	wxMessageBox( msg, "Enemy Defeated!", wxOK | wxICON_INFORMATION );
	Destroy();
	EndModal(0);
}

void FBattleScreen::fireICM() {
	VehicleList sList = getShipList(getActivePlayerID());
	// loop over all ships and weapons to find rocket weapons that have targets
	for (VehicleList::iterator itr =sList.begin(); itr < sList.end(); itr++){
		if ((*itr)->getHP()>0){ // if the ship hasn't been destroyed
			int nWeps = (*itr)->getWeaponCount();
			for (int i = 0; i < nWeps; i++){
				FWeapon *w = (*itr)->getWeapon(i);
				// if the weapon can be affected by ICMs
				if (w->getTarget()!=NULL && w->getICMMod() != 0){
					ICMData *d = new ICMData;
					d->weapon = w;
					d->vehicles = m_map->getShipList(w->getTarget());
					if (d->vehicles != NULL){ // if we have vehicles
						// check to see if any ships in the list have ICMs to use
						bool haveICMs = false;
						for (VehicleList::iterator vItr=d->vehicles->begin(); vItr<d->vehicles->end(); vItr++){
							unsigned int index = (*vItr)->hasDefense(FDefense::ICM);
							if (index && (*vItr)->getDefense(index)->getAmmo()           // It has ICMs left,
									&& (*vItr)->isPowerSystemDamaged()==false            // it can use them,
									&& (*vItr)->getDefense(index)->isDamaged()==false
									&& w->getTarget()->getOwner() == (*vItr)->getOwner()) {  // and it's on the same team as the defending ship
								haveICMs = true;
								break;  // once at least one ship has ICMs we can stop
							}
						}
						// if so add the weapon to the list
						if (haveICMs){
							m_ICMData.push_back(d);
						}
					}
				}
			}
		}
	}
	// now display the ICM targeting panel
	if (m_ICMData.size()>0){
		ICMSelectionGUI *panel = new ICMSelectionGUI( this, &m_ICMData);
		panel->ShowModal();
	}
	m_ICMData.clear();
}

void FBattleScreen::applyFireDamage(){
	VehicleList sList = getShipList(getActivePlayerID());
	for (VehicleList::iterator itr =sList.begin(); itr < sList.end(); itr++){
		if ((*itr)->isOnFire()){
			int roll = irand(10);
			(*itr)->takeDamage(roll,20);
		}
	}
}

}
