///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  4 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "ViewFleetGUI.h"
#include <sstream>

///////////////////////////////////////////////////////////////////////////

ViewFleetGUI::ViewFleetGUI( wxWindow* parent, FFleet * fleet, FSystem * sys, FSystem *dest, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	m_fleet = fleet;
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT(fleet->getName()), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer1->Add( m_staticText1, 0, wxALL, 5 );
	
	std::string txt = "Current Location:  " + sys->getName();
	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT(txt), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer1->Add( m_staticText2, 0, wxALL, 5 );
	
	txt = "Current Destination:  ";
	std::ostringstream os;
	os << "    Time until arrival:  ";
	if (fleet->getInTransit()){
		txt += dest->getName();
		os << (int)floor(fleet->getTransitTime()/fleet->getSpeed()+0.5) << " days"; 
	} else {
		txt += "none";
		os << "N/A";
	}
	m_staticText3 = new wxStaticText( this, wxID_ANY, wxT(txt), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	bSizer1->Add( m_staticText3, 0, wxALL, 5 );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, wxT(os.str()), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	bSizer1->Add( m_staticText4, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_ALL );

	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Ship List") ), wxVERTICAL );
	
	m_listBox1 = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE );
	int nShips = fleet->getShipCount();
	m_listBox1->SetMinSize( wxSize( -1, nShips * 26 ) );
	for (unsigned int i = 0; i < fleet->getShipCount(); i++){
		m_listBox1->Append(fleet->getShipList()[i]->getName());
	}
	sbSizer1->Add( m_listBox1, 1, wxALL|wxEXPAND, 5 );
	
	fgSizer2->Add( sbSizer1, 2 , wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Ship Information") ), wxVERTICAL );
	sbSizer2->SetMinSize( wxSize( 200, -1 ) );

	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 3, 2, 0, 0 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_ALL );
	
	m_staticText5 = new wxStaticText( this, wxID_ANY, wxT("Current/Max ADF:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	fgSizer3->Add( m_staticText5, 0, wxTOP|wxBOTTOM|wxLEFT, 5 );

	ADF = new wxStaticText(this,wxID_ANY, wxT(""),wxDefaultPosition,wxDefaultSize, wxALIGN_CENTRE );
	ADF->SetMinSize(wxSize(62,-1));
	ADF->Wrap( -1 );
	fgSizer3->Add( ADF, 0, wxTOP|wxBOTTOM|wxRIGHT, 5 );

	m_staticText6 = new wxStaticText( this, wxID_ANY, wxT("Current/Max MR:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	fgSizer3->Add( m_staticText6, 0, wxTOP|wxBOTTOM|wxLEFT, 5 );

	MR = new wxStaticText(this,wxID_ANY, wxT(""),wxDefaultPosition,wxDefaultSize, wxALIGN_CENTRE );
	MR->SetMinSize(wxSize(62,-1));
	MR->Wrap( -1 );
	fgSizer3->Add( MR, 0, wxTOP|wxBOTTOM|wxRIGHT, 5 );

	m_staticText7 = new wxStaticText( this, wxID_ANY, wxT("Current/Max HP:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	fgSizer3->Add( m_staticText7, 0, wxTOP|wxBOTTOM|wxLEFT, 5 );

	HP = new wxStaticText(this,wxID_ANY, wxT(""),wxDefaultPosition,wxDefaultSize, wxALIGN_CENTRE );
	HP->SetMinSize(wxSize(62,-1));
	HP->Wrap( -1 );
	fgSizer3->Add( HP, 0, wxTOP|wxBOTTOM|wxRIGHT, 5 );

	sbSizer2->Add( fgSizer3, 0, wxALL, 5);
	fgSizer2->Add( sbSizer2, 2 , wxEXPAND, 5 );
		
	bSizer1->Add( fgSizer2, 1 , wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_ALL );
	
	m_button1 = new wxButton( this, wxID_ANY, wxT("Modify Ship"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button1->Enable( false );
	
	fgSizer1->Add( m_button1, 0, wxALL, 5 );
	
	m_button2 = new wxButton( this, wxID_ANY, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_button2, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	bSizer1->Add( fgSizer1, 0, wxEXPAND, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	// Connect Events
	m_listBox1->Connect( wxEVT_LEFT_UP, wxMouseEventHandler( ViewFleetGUI::onSelectShip ), NULL, this );
	m_button1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewFleetGUI::onModifyShip ), NULL, this );
	m_button2->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewFleetGUI::OnOK ), NULL, this );
}

ViewFleetGUI::~ViewFleetGUI()
{
	// Disconnect Events
	m_listBox1->Disconnect( wxEVT_LEFT_UP, wxMouseEventHandler( ViewFleetGUI::onSelectShip ), NULL, this );
	m_button1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewFleetGUI::onModifyShip ), NULL, this );
	m_button2->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ViewFleetGUI::OnOK ), NULL, this );
}

void ViewFleetGUI::onSelectShip( wxMouseEvent& event ){
	m_ship = m_fleet->getShipList()[m_listBox1->GetSelection()];
	std::ostringstream os1,os2,os3,os4,os5,os6;
	os1 << m_ship->getADF() << " / " << m_ship->getMaxADF();
	ADF->SetLabel(os1.str());
	os2 << m_ship->getMR() << " / " << m_ship->getMaxMR();
	MR->SetLabel(os2.str());
	os3 << m_ship->getHP() << " / " << m_ship->getMaxHP();
	HP->SetLabel(os3.str());
//	m_button1->Enable(true);
	event.Skip(); 
}
