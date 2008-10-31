/**
 * @file FBattleDispaly.cpp
 * @brief Implementation file for BattleDispaly class
 * @author Tom Stephens
 * @date Created:  Jul 11, 2008
 *
 */

//#include "FBattleDisplay.h"
#include "Frontier.h"
#include "FBattleScreen.h"
#include <wx/wx.h>
#include <sstream>

#define ICON_SIZE 50
#define BORDER 5
#define ZOOM_SIZE 30
const int leftOffset=2*BORDER+ZOOM_SIZE;

namespace Frontier {

FBattleDisplay::FBattleDisplay(wxWindow * parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString &name)
		: wxPanel( parent, id, pos, size, style, name ) {

	m_parent = (FBattleScreen *)parent;
	m_loaded = false;
	m_first = true;

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	wxColour black(wxT("#000000"));// black
	SetBackgroundColour(black);
	SetMinSize( wxSize( -1,120 ) );

	m_zoomImage.LoadFile("../data/zoom.png");

	/// set up the set speed controls
	m_spinCtrl1 = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxPoint(leftOffset,3*BORDER+2), wxSize( 50,-1 ), wxSP_ARROW_KEYS, 0, 55, 10 );
	m_button1 = new wxButton( this, wxID_ANY, wxT("Set Speed"), wxPoint(leftOffset+60,3*BORDER), wxDefaultSize, 0 );
	m_spinCtrl1->Hide();
	m_button1->Hide();

	this->Connect(wxEVT_PAINT, wxPaintEventHandler(FBattleDisplay::onPaint));
	this->Connect( wxEVT_LEFT_UP, wxMouseEventHandler(FBattleDisplay::onLeftUp ),NULL,this);

}

FBattleDisplay::~FBattleDisplay() {
}

void FBattleDisplay::draw(wxDC &dc){
//	wxBitmap b("../icons/ufo.png");
//	dc.DrawBitmap(b, 5, 5);
	wxColour black(wxT("#000000"));// black
	dc.SetBackground(wxBrush(black));
	dc.DrawBitmap(wxBitmap(m_zoomImage),0,0);
	switch (m_parent->getState()){
	case BS_SetupPlanet:
		if (m_parent->getControlState()){
			drawPlacePlanet(dc);
		} else {
			drawPlanetChoices(dc);
		}
		break;
	case BS_SetupStation:
		if (m_parent->getPhase()==PH_NONE){
			drawPlaceStation(dc);
		} else {
			drawSelectRotation(dc);
		}
		break;
	case BS_SetupDefendFleet:
	case BS_SetupAttackFleet:
		if (m_parent->getControlState()){
			drawPlaceShip(dc);
		} else {
			if (m_parent->getPhase()==PH_NONE){
				drawShipChoices(dc);
			} else {
				drawGetSpeed(dc);
			}
		}
		break;
	case BS_Battle:
		switch (m_parent->getPhase()){
		case PH_MOVE:
			drawMoveShip(dc);
			break;
		}
		drawCurrentShipStats(dc);
		break;
	default:
		break;
	}

}

void FBattleDisplay::onPaint(wxPaintEvent & event){
//	std::cerr << "in FBattleDispaly::onPaint()" << std::endl;
	wxPaintDC dc(this);
	draw(dc);
}

void FBattleDisplay::onLeftUp(wxMouseEvent & event) {
//	wxCoord x,y;
//	event.GetPosition(&x,&y);
//	std::cerr << "The x and y positions of the click are " << x << ", " << y << std::endl;
	if (event.GetX() < ZOOM_SIZE){
		zoomMap(event);
	}
	switch (m_parent->getState()){
	case BS_SetupPlanet:
		if (m_parent->getControlState()==false){
			makePlanetChoice(event);
		}
		break;
	case BS_SetupStation:
		if (m_parent->getPhase()==PH_SET_SPEED){
			if (!setStationRotation(event)){
				break;
			}
		}
		break;
	case BS_SetupDefendFleet:
	case BS_SetupAttackFleet:
		if (m_parent->getControlState()==false && m_parent->getPhase()==PH_NONE){
			makeShipChoice(event);
		}
		break;
	default:
		break;
	}
	event.Skip();
}

void FBattleDisplay::drawPlanetChoices (wxDC &dc){
	wxColour white(wxT("#FFFFFF"));
	for (unsigned int i=0; i< m_imageList.size(); i++){
//		std::cerr << "Drawing planet " << i << std::endl;
		wxBitmap b (m_imageList[i].Scale(ICON_SIZE,ICON_SIZE));
		dc.DrawBitmap(b,ZOOM_SIZE+BORDER+i*ICON_SIZE,BORDER);
	}
	dc.SetTextForeground(white);
	dc.SetFont(wxFont(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL));
	dc.DrawText("Please select an icon to use for the planet.",leftOffset,2*BORDER+ICON_SIZE);

}

void FBattleDisplay::makePlanetChoice(wxMouseEvent & event){
	wxCoord x,y;
	event.GetPosition(&x,&y);
	int xp = (x-leftOffset)/ICON_SIZE;
	int yp = (y-BORDER)/ICON_SIZE;
	if ( y>=BORDER && yp == 0){  // there is only one row in this case
		if ( x>=leftOffset && xp < (int)(m_imageList.size() )) { // make sure we actually hit an icon
			m_parent->setPlanet(xp);  // set the index
			m_parent->toggleControlState();
			wxClientDC dc(this);
			Refresh();
//			std::cerr << "The selection position is (" << xp << "," << yp << ")" << std::endl;
		}
	}
}

void FBattleDisplay::drawPlacePlanet(wxDC &dc){
	wxColour white(wxT("#FFFFFF"));
	dc.SetTextForeground(white);
	dc.SetFont(wxFont(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL));
	dc.DrawText("Select the map hex where you would like to place the planet.",leftOffset,BORDER);

}

void FBattleDisplay::drawPlaceShip(wxDC &dc){
	wxColour white(wxT("#FFFFFF"));
	dc.SetTextForeground(white);
	dc.SetFont(wxFont(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL));
	std::string txt = "Select the map hex where you would like to place the ship.";
	dc.DrawText(txt,leftOffset,BORDER);
	txt = "Click once to place the ship and then move the mouse to select the";
	dc.DrawText(txt,leftOffset,BORDER+16);
	txt = "desired heading and click again to finalize placement.";
	dc.DrawText(txt,leftOffset,BORDER+32);

}

void FBattleDisplay::drawPlaceStation(wxDC &dc){
	wxColour white(wxT("#FFFFFF"));
	dc.SetTextForeground(white);
	dc.SetFont(wxFont(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL));
	std::string txt = "Select the map hex where you would like to place the station.";
	dc.DrawText(txt,leftOffset,BORDER);
	txt = "It must be adjacent to the planet.";
	dc.DrawText(txt,leftOffset,BORDER+16);
}

void FBattleDisplay::drawShipChoices (wxDC &dc){
	wxColour white(wxT("#FFFFFF"));
	if (!m_loaded) {
		m_vList = m_parent->getShipList();
		m_loaded = true;
		m_parent->setDone(false);
	}
	for (unsigned int i=0; i< m_vList.size(); i++){
		if (m_vList[i]!=NULL){
		const wxImage *iptr = m_vList[i]->getIcon();
		wxImage im = iptr->Scale(ICON_SIZE,ICON_SIZE);
		wxBitmap b(im);
		dc.DrawBitmap(b,leftOffset+i*ICON_SIZE,BORDER);
		}
	}
	dc.SetTextForeground(white);
	dc.SetFont(wxFont(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL));
	dc.DrawText("Please select a ship to place on the map.",leftOffset,2*BORDER+ICON_SIZE);

}

void FBattleDisplay::zoomMap(wxMouseEvent & event){
	double rate = 1.5;  // rate of zoom per click
	if (event.GetY()<44) {
		m_parent->setScale(rate);
	} else if (event.GetY()>76){
		m_parent->setScale(1/rate);
	}
}

void FBattleDisplay::makeShipChoice(wxMouseEvent & event){
	static unsigned int count = 0;
	wxCoord x,y;
	event.GetPosition(&x,&y);
	int xp = (x-leftOffset)/ICON_SIZE;
	int yp = (y-BORDER)/ICON_SIZE;
	VehicleList vList = m_parent->getShipList();
	if ( y>=BORDER && yp == 0){  // there is only one row in this case
		if ( x>=leftOffset && xp < (int)(vList.size()) && m_vList[xp]!=NULL) { // make sure we actually hit an icon
			count++;
			m_parent->setShip(vList[xp]);  // set the index
			m_parent->toggleControlState();
			m_vList[xp]=NULL;
			wxClientDC dc(this);
			Refresh();
//			std::cerr << "The selection position is (" << xp << "," << yp << ")" << std::endl;
			if (count == m_vList.size()){
				count = 0;
				m_parent->setDone(true);
				m_loaded = false;
			}
		}
	}
}

void FBattleDisplay::drawGetSpeed(wxDC &dc){
	wxColour white(wxT("#FFFFFF"));

	if (m_first){
		m_spinCtrl1->Show();
		m_button1->Show();
		// Connect Events
		m_button1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FBattleDisplay::onSetSpeed ), NULL, this );
		m_first = false;
	}

	dc.SetTextForeground(white);
	dc.SetFont(wxFont(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL));
	std::string txt = "Please choose an initial speed for the ship.";
	dc.DrawText(txt,leftOffset,2*BORDER+ICON_SIZE);
	txt = "Press the 'Set Speed' button when done";
	dc.DrawText(txt,leftOffset,2*BORDER+ICON_SIZE+16);

}

void FBattleDisplay::onSetSpeed( wxCommandEvent& event ){
	// disconnect the button
	m_button1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FBattleDisplay::onSetSpeed ), NULL, this );
	m_parent->getShip()->setSpeed(m_spinCtrl1->GetValue());
	// Hid the set speed controls
	m_spinCtrl1->Hide();
	m_button1->Hide();

	m_first = true;
	m_parent->setPhase(PH_NONE);
	if(m_parent->getDone()){
		if(m_parent->getState()==BS_SetupDefendFleet){
			m_parent->setState(BS_SetupAttackFleet);
			m_parent->toggleSide();
		} else {
			m_parent->setState(BS_Battle);
			m_parent->setPhase(PH_MOVE);
			m_parent->toggleSide();
		}
	}
//	m_parent->setShip(NULL);
	event.Skip();
}

void FBattleDisplay::drawSelectRotation(wxDC &dc){
	wxColour white(wxT("#FFFFFF"));
	wxColour black(wxT("#000000"));
	wxColour lblue(wxT("#9999FF"));
	int tSize =10;
	dc.SetFont(wxFont(tSize,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL));
	dc.SetBrush(wxBrush(lblue));
	dc.SetPen(wxPen(black));
	dc.SetTextForeground(black);
	int w=140;
	int h=30;
	int r=15;
	dc.DrawRoundedRectangle(leftOffset,BORDER,w,h,r);
	dc.DrawText("Clockwise",leftOffset+2*BORDER+27,BORDER+h/2-tSize+1);
	dc.DrawRoundedRectangle(leftOffset+BORDER+w,BORDER,w,h,r);
	dc.DrawText("Counter-clockwise",leftOffset+3*BORDER+w,BORDER+h/2-tSize+1);

	dc.SetTextForeground(white);
	dc.DrawText("Please select the station's direction of rotation",leftOffset,2*BORDER+h);
}

bool FBattleDisplay::setStationRotation(wxMouseEvent &event){
	wxCoord x,y;
	event.GetPosition(&x,&y);
	int w=140;  // these come from the drawSelectRotation() method
	int h=30;
	if (x>leftOffset && x<leftOffset+BORDER+2*w && y>BORDER && y<BORDER+h){
		int heading = m_parent->computeHeading(m_parent->getStationPos(),m_parent->getPlanetPos());
		if(x<leftOffset+w){					// clockwise
			heading = (heading+1)%6;
		} else if (x>leftOffset+w+BORDER){	// ccw
			heading -= 1;
			if (heading < 0) { heading +=6; }
		} else {  // hit the gap
			return false;
		}
		m_parent->getStation()->setHeading(heading);
		m_parent->getStation()->setSpeed(1);
		m_parent->setPhase(PH_NONE);
		m_parent->setState(BS_SetupDefendFleet);
	}
	return true;
}

void FBattleDisplay::drawMoveShip(wxDC &dc){
	wxColour white(wxT("#FFFFFF"));
	dc.SetFont(wxFont(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL));
	std::ostringstream os;
	os << "It is the ";
	if (m_parent->getSide()){
		os << "attacker's ";
	} else {
		os << "defender's ";
	}
	os << "turn.";
	dc.SetTextForeground(white);
	dc.DrawText(os.str(),leftOffset,BORDER);
	os.str("Please select a ship to move.");
	dc.DrawText(os.str(),leftOffset,BORDER+16);

}

void FBattleDisplay::drawCurrentShipStats(wxDC & dc){
	int lMargin = 300;	// left margin for ship display
	int tSize = 10;		// text height
	FVehicle *s = m_parent->getShip();
	wxColour white(wxT("#FFFFFF"));
	wxFont normal(tSize,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL);
	wxFont large((int)(tSize*1.3),wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD);
	wxFont bold(tSize,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD);
	if (m_parent->getShip()!=NULL){
		dc.SetFont(bold);
		dc.DrawText("Speed:",lMargin,BORDER+(int)(1.6*(tSize*1.3)));
		dc.DrawText("Heading: ",lMargin+90,BORDER+(int)(1.6*(tSize*1.3)));
		dc.DrawText("ADF:",lMargin,BORDER+(int)(1.6*(tSize*2.3)));
		dc.DrawText("MR: ",lMargin+80,BORDER+(int)(1.6*(tSize*2.3)));
		dc.DrawText("HP: ",lMargin+160,BORDER+(int)(1.6*(tSize*2.3)));
		dc.DrawText("DCR: ",lMargin+240,BORDER+(int)(1.6*(tSize*2.3)));
		dc.DrawText("Weapons:",lMargin,BORDER+(int)(1.6*(tSize*3.3)));
		dc.DrawText("Defenses:",lMargin,BORDER+(int)(1.6*(tSize*4.3)));
		dc.SetFont(large);
		dc.DrawText(s->getName(),lMargin,BORDER);
		dc.SetFont(normal);
		std::ostringstream os;
		os << s->getSpeed();
		dc.DrawText(os.str(),lMargin+60,BORDER+(int)(1.6*(tSize*1.3)));
		dc.DrawText(getHeadingStr(),lMargin+170,BORDER+(int)(1.6*(tSize*1.3)));
		os.str("");
		os << s->getADF();
		dc.DrawText(os.str(),lMargin+40,BORDER+(int)(1.6*(tSize*2.3)));
		os.str("");
		os << s->getMR();
		dc.DrawText(os.str(),lMargin+115,BORDER+(int)(1.6*(tSize*2.3)));
		os.str("");
		os << s->getHP();
		dc.DrawText(os.str(),lMargin+195,BORDER+(int)(1.6*(tSize*2.3)));
	}
}

std::string FBattleDisplay::getHeadingStr(){
	switch (m_parent->getShip()->getHeading()){
	case 0:
		return "W";
	case 1:
		return "SW";
	case 2:
		return "SE";
	case 3:
		return "E";
	case 4:
		return "NE";
	case 5:
		return "NW";
	default:
		return "";
	}
}

}
