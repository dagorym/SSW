///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 17 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "battleSim/ScenarioDialog.h"

///////////////////////////////////////////////////////////////////////////

ScenarioDialog::ScenarioDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 5, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->AddGrowableRow( 0 );
	fgSizer2->AddGrowableRow( 1 );
	fgSizer2->AddGrowableRow( 2 );
	fgSizer2->AddGrowableRow( 3 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_buttonHeader = new wxStaticText( this, wxID_ANY, wxT("Press Button to Start"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
	m_buttonHeader->Wrap( 90 );
	m_buttonHeader->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	fgSizer2->Add( m_buttonHeader, 0, wxALIGN_BOTTOM|wxALL, 5 );
	
	m_secenarioHeader = new wxStaticText( this, wxID_ANY, wxT("Scenario Description"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
	m_secenarioHeader->Wrap( -1 );
	m_secenarioHeader->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	fgSizer2->Add( m_secenarioHeader, 0, wxALIGN_BOTTOM|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	m_scenario1Button = new wxButton( this, wxID_ANY, wxT("Play"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_scenario1Button, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Surprise Attack! \nUPF - Frigate, Assault Scout & station \nSathar - Destroyer and Heavy Cruiser"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( 300 );
	fgSizer2->Add( m_staticText1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_scenario2Button = new wxButton( this, wxID_ANY, wxT("Play"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_scenario2Button, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("The Battle of Fortress Kdi'Kit\nUPF - BattleShip, Fortress, 2 Fighters, Destroyer, Frigate, 2 Assault Scouts\nSathar - Assault Carrier, 5 Fighters, Heavy Cruiser, 3 Destroyers, Frigate"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( 300 );
	fgSizer2->Add( m_staticText2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_scenario3Button = new wxButton( this, wxID_ANY, wxT("Play"), wxDefaultPosition, wxDefaultSize, 0 );
	m_scenario3Button->Enable( false );
	
	fgSizer2->Add( m_scenario3Button, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, wxT("Close Escort\nUPF - Light Cruiser, 2 Assault Scouts, Freighter\nSathar - Light Cruiser, Destroyer"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( 300 );
	fgSizer2->Add( m_staticText3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_scenario4Button = new wxButton( this, wxID_ANY, wxT("Play"), wxDefaultPosition, wxDefaultSize, 0 );
	m_scenario4Button->Enable( false );
	
	fgSizer2->Add( m_scenario4Button, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, wxT("The Stand\nUPF - Fortified Station, 5 Fighters, Minelayer, 2 Frigates, 2 Assault Scouts\nSathar - Assault Carrier, 6 Fighters, 2 Destroyers, Light Cruiser, Heavy Cruiser"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( 300 );
	fgSizer2->Add( m_staticText4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	fgSizer1->Add( fgSizer2, 1, wxEXPAND, 5 );
	
	m_doneButton = new wxButton( this, wxID_ANY, wxT("Done"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_doneButton, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	this->SetSizer( fgSizer1 );
	this->Layout();
	fgSizer1->Fit( this );
	
	// Connect Events
	m_scenario1Button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ScenarioDialog::onScenario1 ), NULL, this );
	m_scenario2Button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ScenarioDialog::onScenario2 ), NULL, this );
	m_scenario3Button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ScenarioDialog::onScenario3 ), NULL, this );
	m_scenario4Button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ScenarioDialog::onScenario4 ), NULL, this );
	m_doneButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ScenarioDialog::onDone ), NULL, this );
}

ScenarioDialog::~ScenarioDialog()
{
	// Disconnect Events
	m_scenario1Button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ScenarioDialog::onScenario1 ), NULL, this );
	m_scenario2Button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ScenarioDialog::onScenario2 ), NULL, this );
	m_scenario3Button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ScenarioDialog::onScenario3 ), NULL, this );
	m_scenario4Button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ScenarioDialog::onScenario4 ), NULL, this );
	m_doneButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ScenarioDialog::onDone ), NULL, this );
}
