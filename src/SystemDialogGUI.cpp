///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  4 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "SystemDialogGUI.h"
#include "SelectJumpGUI.h"
#include "ViewFleetGUI.h"
#include "TransferShipsGUI.h"

///////////////////////////////////////////////////////////////////////////

SystemDialogGUI::SystemDialogGUI( wxWindow* parent, FSystem * sys, FMap * map, FPlayer * player,
		const wxString& title, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	m_parent = parent;
	m_map = map;
	m_player = player;
	m_system = sys;
	
//	wxBoxSizer* bSizer1;n
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer1->Add( m_staticText1, 0, wxALL, 5 );
	
	wxGridBagSizer* gbSizer1;
	gbSizer1 = new wxGridBagSizer( 0, 0 );
	gbSizer1->SetFlexibleDirection( wxBOTH );
	gbSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Planets") ), wxVERTICAL );
	
	m_listBox1 = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	m_listBox1->SetMinSize( wxSize( -1,-1 ) );
	PlanetList planets = sys->getPlanetList();
	if (planets.size()==0){
		//m_listBox1->Append("none");
	} else {
		for (unsigned int i = 0; i < planets.size(); i++){
			m_listBox1->Append(planets[i]->getName());
		}
	}
	
	sbSizer1->Add( m_listBox1, 1, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Station Type") ), wxVERTICAL );
	
	m_listBox2 = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	m_listBox2->SetMinSize( wxSize( -1,25 ) );
	m_listBox2->Enable(false);
	
	sbSizer2->Add( m_listBox2, 1, wxALL|wxEXPAND, 5 );
	
	sbSizer1->Add( sbSizer2, 1, wxEXPAND, 5 );
	
	m_button1 = new wxButton( this, wxID_ANY, wxT("Station Status"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button1->Enable(false);
	sbSizer1->Add( m_button1, 0, wxALL|wxEXPAND, 5 );
	
	gbSizer1->Add( sbSizer1, wxGBPosition( 0, 0 ), wxGBSpan( 1, 1 ), wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Fleets") ), wxVERTICAL );
	
	m_listBox3 = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	m_listBox3->SetMinSize( wxSize( -1,-1 ) );
	FleetList fleets = sys->getFleetList();
	for (unsigned int i = 0; i < fleets.size(); i++){
		std::string name = fleets[i]->getName();
		name+=getMovementText(fleets[i]);
		m_listBox3->Append(name);
	}
	
	sbSizer3->Add( m_listBox3, 1, wxALL|wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 1, 3, 0, 0 );
	fgSizer1->AddGrowableCol( 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->AddGrowableCol( 2 );
	
	m_button2 = new wxButton( this, wxID_ANY, wxT("View Ships"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button2->Enable(false);
	fgSizer1->Add( m_button2, 0, wxALL|wxEXPAND, 5 );
	
	m_button3 = new wxButton( this, wxID_ANY, wxT("Transfer Ships"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button3->Enable(false);
	fgSizer1->Add( m_button3, 0, wxALL|wxEXPAND, 5 );
	
	m_button5 = new wxButton( this, wxID_ANY, wxT("Move Fleet"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button5->SetMinSize( wxSize( 115,-1 ) );
	m_button5->Enable(false);
	fgSizer1->Add( m_button5, 0, wxALIGN_RIGHT|wxALL|wxEXPAND, 5 );
	
	sbSizer3->Add( fgSizer1, 0, wxALIGN_BOTTOM|wxEXPAND , 5 );
	
	gbSizer1->Add( sbSizer3, wxGBPosition( 0, 1 ), wxGBSpan( 1, 1 ), wxEXPAND, 5 );
	
	bSizer1->Add( gbSizer1, 1, wxEXPAND, 5 );
	
	m_button4 = new wxButton( this, wxID_ANY, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_button4, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	// Connect Events
	m_listBox1->Connect( wxEVT_LEFT_UP, wxMouseEventHandler( SystemDialogGUI::onSelectPlanet ), NULL, this );
	m_listBox2->Connect( wxEVT_LEFT_UP, wxMouseEventHandler( SystemDialogGUI::onSelectStation ), NULL, this );
	m_button1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SystemDialogGUI::onViewStationStatus ), NULL, this );
	m_listBox3->Connect( wxEVT_LEFT_UP, wxMouseEventHandler( SystemDialogGUI::onSelectFleet ), NULL, this );
	m_button2->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SystemDialogGUI::onViewShips ), NULL, this );
	m_button3->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SystemDialogGUI::onTransferShips ), NULL, this );
	m_button5->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SystemDialogGUI::onMoveFleet ), NULL, this );
	m_button4->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SystemDialogGUI::onOK ), NULL, this );
}

SystemDialogGUI::~SystemDialogGUI()
{
	// Disconnect Events
	m_listBox1->Disconnect( wxEVT_LEFT_UP, wxMouseEventHandler( SystemDialogGUI::onSelectPlanet ), NULL, this );
	m_listBox2->Disconnect( wxEVT_LEFT_UP, wxMouseEventHandler( SystemDialogGUI::onSelectStation ), NULL, this );
	m_button1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SystemDialogGUI::onViewStationStatus ), NULL, this );
	m_listBox3->Disconnect( wxEVT_LEFT_UP, wxMouseEventHandler( SystemDialogGUI::onSelectFleet ), NULL, this );
	m_button2->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SystemDialogGUI::onViewShips ), NULL, this );
	m_button3->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SystemDialogGUI::onTransferShips ), NULL, this );
	m_button4->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SystemDialogGUI::onOK ), NULL, this );
	m_button5->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SystemDialogGUI::onMoveFleet ), NULL, this );
}

void SystemDialogGUI::onSelectFleet( wxMouseEvent& event ){
	m_fleet = m_system->getFleetList()[m_listBox3->GetSelection()];
	if (m_fleet->getOwner()==m_player->getID()){
		if(m_fleet->getInTransit()){
			m_button5->SetLabel("Change Orders");
		} else {
			m_button5->SetLabel("Move Fleet");
		}
		m_button5->Enable(true);
		if (m_fleet->isMilitia()){
			m_button3->Enable(false);
		} else {
			m_button3->Enable(true);
		}
	} else {
		m_button3->Enable(false);
		m_button5->Enable(false);
	}
	m_button2->Enable(true);
	event.Skip();
}

void SystemDialogGUI::onMoveFleet( wxCommandEvent& event ){
	if (m_fleet->getInTransit()){  // we are cancelling the jump
		wxString l[4];
		int count = 1;
		l[0] = "Cancel jump order and return to system";
		if(m_fleet->isHolding()){  // we are paused so we need options to resume
			l[1] = "Resume jump at normal speed";
			if (m_fleet->getMaxSpeed() >= 2){
				l[2] = "Resume jump at Risk Jump 2";
				if (m_fleet->getMaxSpeed() >=3){
					l[3] = "Resume jump at Risk Jump 3";
				}
			}
			count = 4;
		} else {  // we can just pause the jump
			l[1] = "Hold current jump speed";
			count = 2;
		}
		std::string msg = std::string("Please select a new movement order\n")
			           + "for the fleet from the options below and\n"
			           + "press Ok";
		wxSingleChoiceDialog d(m_parent,msg,"Change Fleet Orders",count,l);
		d.ShowModal();
//		std::cerr << "Selected item:  " << d.GetSelection() << std::endl;
		switch (d.GetSelection()){
		case 0:
//			std::cerr << "Calling cancelJump()" << std::endl;
			m_fleet->cancelJump();
			break;
		case 1:
			if (m_fleet->isHolding()){
				m_fleet->setHolding(false);
				m_fleet->setSpeed(1);
			} else {
				m_fleet->setHolding(true);
			}
			break;
		case 2:
			m_fleet->setHolding(false);
			m_fleet->setSpeed(2);
			break;
		case 3:
			m_fleet->setHolding(false);
			m_fleet->setSpeed(3);
			break;
		}
	} else {  // we are setting a new jump
		SelectJumpGUI * d = new SelectJumpGUI(m_parent,m_fleet, m_map, m_system->getName(),m_player->getID());
		d->ShowModal();
	}
	std::string name = m_fleet->getName()+getMovementText(m_fleet);
	m_listBox3->SetString(m_listBox3->GetSelection(),name);
//	this->Layout();
//	bSizer1->Fit( this );
	event.Skip();
}

void SystemDialogGUI::onViewShips( wxCommandEvent& event ){ 
	ViewFleetGUI * d = new ViewFleetGUI(m_parent,m_fleet,m_system,m_map->getSystem(m_fleet->getDestination()));
	d->ShowModal();
	event.Skip();
}

std::string SystemDialogGUI::getMovementText(FFleet * f){
	std::string name = "";
	if (f->getInTransit()){
		name += " (";
		if (f->isHolding()){
			name += "Holding ";
		}
		name += "in Transit to ";
		name += m_map->getSystem(f->getDestination())->getName();
		if (f->getSpeed()==2){
			name += " - RJ2";
		} else if (f->getSpeed()==3){
			name += " - RJ3";
		}
		name += ")";
	}
	return name;
}

void SystemDialogGUI::onSelectPlanet( wxMouseEvent& event ){ 
	m_planet = m_system->getPlanet(std::string(m_listBox1->GetStringSelection()));
	if(!m_listBox2->IsEmpty()){
		m_listBox2->Clear();
	}
	if (m_planet->getStation()!=NULL){
		m_listBox2->Append(m_planet->getStation()->getName());
		m_listBox2->Enable(true);
		bSizer1->Fit( this );
		this->SetSizerAndFit(bSizer1);
	}
	event.Skip(); 
}

void SystemDialogGUI::onSelectStation( wxMouseEvent& event ){ 
	m_station = m_planet->getStation();
//	m_button1->Enable(true);

	event.Skip(); 
}

void SystemDialogGUI::onTransferShips( wxCommandEvent& event ){ 
	TransferShipsGUI * d = new TransferShipsGUI(m_player,m_fleet,m_system,m_parent);
	d->ShowModal();
	// now reset the fleet list
	if(!m_listBox3->IsEmpty()){
		m_listBox3->Clear();
	}
	FleetList fleets = m_system->getFleetList();
	for (unsigned int i = 0; i < fleets.size(); i++){
		std::string name = fleets[i]->getName();
		name+=getMovementText(fleets[i]);
		m_listBox3->Append(name);
	}

	event.Skip(); 
}
