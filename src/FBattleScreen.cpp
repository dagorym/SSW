/**
 * @file FBattleScreen.cpp
 * @brief Implementation file for BattleScreen class
 * @author Tom Stephens
 * @date Created:  Jul 11, 2008
 *
 */

#include "FBattleScreen.h"
#include "Frontier.h"
#include <wx/wx.h>

namespace Frontier {

FBattleScreen::FBattleScreen(const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( (wxFrame *)NULL, -1, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	wxColour black(wxT("#000000"));// black
	wxColour white(wxT("#FFFFFF"));// white

	CreateStatusBar();
	SetStatusText( "Welcome to the BattleScreen!" );


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

}

FBattleScreen::~FBattleScreen(){
	if (m_attackShips) { delete m_attackShips; }
	if (m_defendShips) { delete m_defendShips; }
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
	m_side = false;
	m_stationPos.cx=0;
	m_stationPos.cy=0;
	m_planetPos.cx=0;
	m_planetPos.cy=0;

	//create a list of ships from the list of fleets for the attacker and defenders
	if (m_defendShips) { delete m_defendShips; }
	m_defendShips = new VehicleList;
	for (unsigned int i=0; i< m_defendList->size(); i++){
		const VehicleList sList = (*m_defendList)[i]->getShipList();
		for (unsigned int j=0; j< sList.size(); j++){
			m_defendShips->push_back(sList[j]);
		}
	}
	if (m_attackShips) { delete m_attackShips; }
	m_attackShips = new VehicleList;
	for (unsigned int i=0; i< m_attackList->size(); i++){
		const VehicleList sList = (*m_attackList)[i]->getShipList();
		for (unsigned int j=0; j< sList.size(); j++){
			m_attackShips->push_back(sList[j]);
		}
	}

	m_control=false;
	if(planet){
		m_state = BS_SetupPlanet;
		ImageList iList;
		wxImage p0("../icons/planet_01.png");
		iList.push_back(p0);
		wxImage p1("../icons/planet_02.png");
		iList.push_back(p1);
		wxImage p2("../icons/planet_03.png");
		iList.push_back(p2);
		m_display->setImageList(iList);
		m_map->setPlanetImages(iList);
	} else if (station != NULL){
		m_state=BS_SetupStation;
	}
	return 0;
}

void FBattleScreen::setState(int s) {
	m_state = s;
	if (s == BS_SetupStation && m_station == NULL){
		m_state = BS_SetupDefendFleet;
	}
	m_map->Refresh();
	m_display->Refresh();
}

VehicleList FBattleScreen::getShipList() const{
	if (m_side){
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
		if (!m_side) {  // defender just ended
			///@todo update turn counters
			///@todo check for repair turn
		}
		toggleSide();
	}
	m_map->Refresh();
	m_display->Refresh();
}

int FBattleScreen::computeHeading(hexData s, hexData d){
	double dis = 1.0;
	double a = dis/sqrt(3.);
	double sx = dis + (2 * dis * s.cx) + dis * (s.cy%2);
	double sy = 2 * a + (3 * a * s.cy);
	double dx = dis + (2 * dis * d.cx) + dis * (d.cy%2);
	double dy = 2 * a + (3 * a * d.cy);

	double angle = atan2((sy-dy),(dx-sx))*180/acos(-1.0);  // angle in degrees
	if (angle<0) { angle += 360.; }
	int ang = (int)floor(angle+30);
	if (ang>=360) { ang -= 360; }
//	std::cerr << "angle = " << angle << "  ang = " << ang << "  heading = " << ang/60 << std::endl;
	return (ang/60);
}


}
