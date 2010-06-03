///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 17 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "battleSim/LocalGameDialog.h"
#include "battleSim/ScenarioDialog.h"

///////////////////////////////////////////////////////////////////////////

LocalGameDialog::LocalGameDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_loadButton = new wxButton( this, wxID_ANY, wxT("Load an Existing Game"), wxDefaultPosition, wxDefaultSize, 0 );
	m_loadButton->Enable( false );
	
	bSizer1->Add( m_loadButton, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 10 );
	
	m_customGameButton = new wxButton( this, wxID_ANY, wxT("Create a New Custom Game"), wxDefaultPosition, wxDefaultSize, 0 );
	m_customGameButton->Enable( false );
	
	bSizer1->Add( m_customGameButton, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 10 );
	
	m_predefinedButton = new wxButton( this, wxID_ANY, wxT("Play a Predefined Scenario"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_predefinedButton, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 10 );
	
	m_backButton = new wxButton( this, wxID_ANY, wxT("Back"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_backButton, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 10 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	// Connect Events
	m_loadButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LocalGameDialog::onLoad ), NULL, this );
	m_customGameButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LocalGameDialog::onCreateNew ), NULL, this );
	m_predefinedButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LocalGameDialog::onPlayPredefined ), NULL, this );
	m_backButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LocalGameDialog::onQuit ), NULL, this );
}

LocalGameDialog::~LocalGameDialog()
{
	// Disconnect Events
	m_loadButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LocalGameDialog::onLoad ), NULL, this );
	m_customGameButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LocalGameDialog::onCreateNew ), NULL, this );
	m_predefinedButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LocalGameDialog::onPlayPredefined ), NULL, this );
	m_backButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LocalGameDialog::onQuit ), NULL, this );
}

void LocalGameDialog::onPlayPredefined( wxCommandEvent& event ){
	ScenarioDialog d(this);
	Hide();
	d.ShowModal();
	Show();
	event.Skip();
}
