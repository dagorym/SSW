///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  4 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "gui/SelectResolutionGUI.h"

///////////////////////////////////////////////////////////////////////////

SelectResolutionGUI::SelectResolutionGUI( wxWindow* parent, FleetList list, std::string sysName, FVehicle *station, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	std::string msg = "Combat will occur between the UPF and Sathar fleets in the "
					+ sysName + " system.  The forces involved are:\n\n";
	for (unsigned int i = 0; i < list.size(); i++){
		msg += list[i]->getName() + "\n";
	}
	if (station){
		msg += station->getName() + "\n";
	}
	msg += std::string("\nHow would you like to resolve the combat, on the battle board or")
		+ " play it out on the table top and just enter the outcome?  Please"
		+ " select your choice below.";
	msg += "\n\nNOTE: Battle board option is just a teaser until I can implement it.";
	m_staticText1 = new wxStaticText( this, wxID_ANY, msg, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( 400 );
	bSizer1->Add( m_staticText1, 0, wxALL, 5 );

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_button1 = new wxButton( this, wxID_ANY, wxT("To the Battle Board!"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button1->Enable( true );

	fgSizer1->Add( m_button1, 0, wxALL, 5 );

	m_button2 = new wxButton( this, wxID_ANY, wxT("Enter Combat Results"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_button2, 0, wxALIGN_RIGHT|wxALL, 5 );

	bSizer1->Add( fgSizer1, 1, wxEXPAND, 5 );

	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );

	// Connect Events
	m_button1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SelectResolutionGUI::onBattleBoard ), NULL, this );
	m_button2->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SelectResolutionGUI::onManualResults ), NULL, this );
}

SelectResolutionGUI::~SelectResolutionGUI()
{
	// Disconnect Events
	m_button1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SelectResolutionGUI::onBattleBoard ), NULL, this );
	m_button2->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SelectResolutionGUI::onManualResults ), NULL, this );
}
