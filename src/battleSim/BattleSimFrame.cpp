///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 17 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "battleSim/BattleSimFrame.h"
#include "battleSim/LocalGameDialog.h"

///////////////////////////////////////////////////////////////////////////

BattleSimFrame::BattleSimFrame( const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( (wxFrame *)NULL, -1, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BACKGROUND ) );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_localGame = new wxButton( this, wxID_ANY, wxT("Play a Local Game"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_localGame, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 20 );
	
	m_networkGame = new wxButton( this, wxID_ANY, wxT("Play a Network Game"), wxDefaultPosition, wxDefaultSize, 0 );
	m_networkGame->Enable( false );
	
	bSizer1->Add( m_networkGame, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 10 );
	
	m_quitButton = new wxButton( this, wxID_ANY, wxT("Quit"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_quitButton, 0, wxALIGN_CENTER|wxALL, 20 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	// Connect Events
//	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( BattleSimFrame::onQuit ), NULL, this );
	m_localGame->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BattleSimFrame::onPlayLocal ), NULL, this );
	m_networkGame->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BattleSimFrame::onPlayNetwork ), NULL, this );
	m_quitButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BattleSimFrame::onQuit ), NULL, this );
}

BattleSimFrame::~BattleSimFrame()
{
	// Disconnect Events
//	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( BattleSimFrame::onQuit ), NULL, this );
	m_localGame->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BattleSimFrame::onPlayLocal ), NULL, this );
	m_networkGame->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BattleSimFrame::onPlayNetwork ), NULL, this );
	m_quitButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BattleSimFrame::onQuit ), NULL, this );
}

void BattleSimFrame::onPlayLocal( wxCommandEvent& event ){
	LocalGameDialog d(this);
	Hide();
	d.ShowModal();
	Show();
	event.Skip();
}
