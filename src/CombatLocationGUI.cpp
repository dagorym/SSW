///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  4 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "CombatLocationGUI.h"

///////////////////////////////////////////////////////////////////////////

CombatLocationGUI::CombatLocationGUI( wxWindow* parent, FPlanet *p, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxSize( -1,-1 ) );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 3, 1, 0, 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	std::string msg = "The Sathar are attacking " + p->getName() + ".  Do you wish to battle around"
					+ " the planet or in deep space?  " + p->getName();
	if (p->getStation() == NULL){
		msg += " does not have a station orbiting it.";
	} else {
		msg += " has " + p->getStation()->getName() + " orbiting it.";
	}
	msg+= "\n\nPlease select where you want the combat to occur.";
	m_staticText1 = new wxStaticText( this, wxID_ANY, msg, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( 300 );
	fgSizer1->Add( m_staticText1, 0, wxALL, 5 );
		
	m_button1 = new wxButton( this, wxID_ANY, wxT("Around the Planet"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_button1, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	m_button2 = new wxButton( this, wxID_ANY, wxT("In Deep Space"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_button2, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	this->SetSizer( fgSizer1 );
	this->Layout();
	fgSizer1->Fit( this );
	
	// Connect Events
	m_button1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CombatLocationGUI::OnButton1 ), NULL, this );
	m_button2->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CombatLocationGUI::OnButton2 ), NULL, this );
}

CombatLocationGUI::~CombatLocationGUI()
{
	// Disconnect Events
	m_button1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CombatLocationGUI::OnButton1 ), NULL, this );
	m_button2->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CombatLocationGUI::OnButton2 ), NULL, this );
}
