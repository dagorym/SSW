///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  4 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "gui/TwoPlanetsGUI.h"

///////////////////////////////////////////////////////////////////////////

TwoPlanetsGUI::TwoPlanetsGUI( wxWindow* parent, FSystem *sys, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxSize( -1,-1 ) );

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 3, 1, 0, 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("     There are two planets in this system.  You must choose one of them to be the focus of your attack.\n     Select the button containing the planet you wish to direct your attack against."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( 300 );
	fgSizer1->Add( m_staticText1, 0, wxALL, 5 );

	std::string msg1 = sys->getPlanetList()[0]->getName();
	if (sys->getPlanetList()[0]->getStation() == NULL){
		msg1 += " (no station)";
	} else {
		msg1 += " (" + sys->getPlanetList()[0]->getStation()->getName() + ")";
	}
	std::string msg2 = sys->getPlanetList()[1]->getName();
	if (sys->getPlanetList()[1]->getStation() == NULL){
		msg2 += " (no station)";
	} else {
		msg2 += " (" + sys->getPlanetList()[1]->getStation()->getName() + ")";
	}

	m_button1 = new wxButton( this, wxID_ANY, msg1, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_button1, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

	m_button2 = new wxButton( this, wxID_ANY, msg2, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_button2, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

	this->SetSizer( fgSizer1 );
	this->Layout();
	fgSizer1->Fit( this );

	// Connect Events
	m_button1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TwoPlanetsGUI::OnButton1 ), NULL, this );
	m_button2->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TwoPlanetsGUI::OnButton2 ), NULL, this );
}

TwoPlanetsGUI::~TwoPlanetsGUI()
{
	// Disconnect Events
	m_button1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TwoPlanetsGUI::OnButton1 ), NULL, this );
	m_button2->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TwoPlanetsGUI::OnButton2 ), NULL, this );
}
