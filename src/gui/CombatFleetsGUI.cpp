///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  4 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "gui/CombatFleetsGUI.h"
#include "gui/ViewFleetGUI.h"

///////////////////////////////////////////////////////////////////////////

CombatFleetsGUI::CombatFleetsGUI( wxWindow* parent, FSystem *sys, FleetList fList, wxArrayInt *sel, bool allowNoDefense, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	m_system = sys;
	m_fleetList = fList;
	m_selections = sel;
	m_allowNoDefense = allowNoDefense;

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 3, 1, 0, 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	std::string msg = "You have the following fleets available in " + sys->getName()
					+ " to participate in the battle.  Please select the ones you want"
					+ " to use and press the OK button.  If you want to check the "
					+ " contents of a fleet, select it and press the View Fleet button.";
	m_staticText1 = new wxStaticText( this, wxID_ANY, msg, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( 300 );
	fgSizer1->Add( m_staticText1, 0, wxALL, 5 );

	m_listBox1 = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_EXTENDED );
	for (FleetList::iterator itr = fList.begin(); itr < fList.end(); itr++){
		m_listBox1->Append((*itr)->getName());
	}
	m_listBox1->SetMinSize( wxSize( -1, 26*fList.size()+1 ) );
	fgSizer1->Add( m_listBox1, 0, wxALL|wxEXPAND, 5 );

	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_button1 = new wxButton( this, wxID_ANY, wxT("View Fleet"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button1->Enable( false );

	fgSizer2->Add( m_button1, 0, wxALL, 5 );

	m_button2 = new wxButton( this, wxID_ANY, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button2->Enable(m_allowNoDefense);

	fgSizer2->Add( m_button2, 0, wxALIGN_RIGHT|wxALL, 5 );

	fgSizer1->Add( fgSizer2, 1, wxEXPAND, 5 );

	this->SetSizer( fgSizer1 );
	this->Layout();
	fgSizer1->Fit( this );

	// Connect Events
	m_listBox1->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( CombatFleetsGUI::onListSelect ), NULL, this );
	m_button1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CombatFleetsGUI::onViewFleet ), NULL, this );
	m_button2->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CombatFleetsGUI::onOK ), NULL, this );
}

CombatFleetsGUI::~CombatFleetsGUI()
{
	// Disconnect Events
	m_listBox1->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( CombatFleetsGUI::onListSelect ), NULL, this );
	m_button1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CombatFleetsGUI::onViewFleet ), NULL, this );
	m_button2->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CombatFleetsGUI::onOK ), NULL, this );
}

void CombatFleetsGUI::onListSelect( wxCommandEvent& event ){
	m_listBox1->GetSelections(*m_selections);
	if (m_selections->GetCount() == 1){
		m_button1->Enable(true);
	} else {
		m_button1->Enable(false);
	}
	if(m_selections->GetCount() > 0){
		m_button2->Enable(true);
	} else {
		m_button2->Enable(m_allowNoDefense);
	}
	event.Skip();
}

void CombatFleetsGUI::onViewFleet( wxCommandEvent& event ){
	ViewFleetGUI d(this,m_fleetList[(*m_selections)[0]],m_system,NULL);
	d.ShowModal();
	event.Skip();
}

void CombatFleetsGUI::onOK( wxCommandEvent& event ){
	if(m_selections->GetCount()>0){
		EndModal(0);
	} else {
		wxMessageDialog d(this,"Are you sure you want to continue without selecting any fleets?",
				"Selecting no fleets", wxYES_NO);
		if (d.ShowModal() == wxID_YES){
			EndModal(0);
		}
	}
	event.Skip();
}
