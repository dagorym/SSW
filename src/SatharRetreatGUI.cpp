///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  4 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "SatharRetreatGUI.h"

///////////////////////////////////////////////////////////////////////////

SatharRetreatGUI::SatharRetreatGUI( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	m_choice = 0;
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("The Sathar player needs to select their retreat conditions.  Please choose one of the options from the list below and press the \"Done\" button."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( 450 );
	m_staticText1->SetMaxSize( wxSize( 450,-1 ) );
	
	bSizer1->Add( m_staticText1, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 5, 1, 0, 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_radioBtn1 = new wxRadioButton( this, wxID_ANY, wxT("The Sathar have lost two assault carriers."), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	fgSizer1->Add( m_radioBtn1, 0, wxALL, 5 );
		
	m_radioBtn2 = new wxRadioButton( this, wxID_ANY, wxT("The Sathar have lost five heavy cruisers."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_radioBtn2, 0, wxALL, 5 );
	
	m_radioBtn3 = new wxRadioButton( this, wxID_ANY, wxT("The Sathar have lost 40 ships, including fighters."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_radioBtn3, 0, wxALL, 5 );
	
	m_radioBtn4 = new wxRadioButton( this, wxID_ANY, wxT("A tenday passes without the Sathar destroying at least two \nstations, two fortresses or a station and a fortress."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_radioBtn4, 0, wxALL, 5 );
	
	m_radioBtn5 = new wxRadioButton( this, wxID_ANY, wxT("At the end of any tenday, the Sathar have lost more ships in \ncombat than the UPF.  Fighters and Militia ships are not counted \ntoward this condition."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_radioBtn5, 0, wxALL, 5 );
	
	bSizer1->Add( fgSizer1, 1, wxEXPAND, 5 );
	
	m_button1 = new wxButton( this, wxID_ANY, wxT("Done"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_button1, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	// Connect Events
	m_radioBtn1->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( SatharRetreatGUI::OnButtonSelection ), NULL, this );
	m_radioBtn2->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( SatharRetreatGUI::OnButtonSelection ), NULL, this );
	m_radioBtn3->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( SatharRetreatGUI::OnButtonSelection ), NULL, this );
	m_radioBtn4->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( SatharRetreatGUI::OnButtonSelection ), NULL, this );
	m_radioBtn5->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( SatharRetreatGUI::OnButtonSelection ), NULL, this );
	m_button1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SatharRetreatGUI::OnDone ), NULL, this );
}

SatharRetreatGUI::~SatharRetreatGUI()
{
	// Disconnect Events
	m_radioBtn1->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( SatharRetreatGUI::OnButtonSelection ), NULL, this );
	m_radioBtn2->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( SatharRetreatGUI::OnButtonSelection ), NULL, this );
	m_radioBtn3->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( SatharRetreatGUI::OnButtonSelection ), NULL, this );
	m_radioBtn4->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( SatharRetreatGUI::OnButtonSelection ), NULL, this );
	m_radioBtn5->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( SatharRetreatGUI::OnButtonSelection ), NULL, this );
	m_button1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SatharRetreatGUI::OnDone ), NULL, this );
}

void SatharRetreatGUI::OnButtonSelection( wxCommandEvent& event ){
	if (m_radioBtn1->GetValue()){
		m_choice = 1;
	} else if (m_radioBtn2->GetValue()){
		m_choice = 2;
	} else if (m_radioBtn3->GetValue()){
		m_choice = 3;
	} else if (m_radioBtn4->GetValue()){
		m_choice = 4;
	} else if (m_radioBtn5->GetValue()){
		m_choice = 5;
	} else {
		m_choice = 0;
	}
	event.Skip();
}

void SatharRetreatGUI::OnDone( wxCommandEvent& event ){
	if (m_choice > 0){
		EndModal(m_choice);
	}
	event.Skip(); 
}

