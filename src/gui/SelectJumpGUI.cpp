///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  4 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "gui/SelectJumpGUI.h"
#include <sstream>

///////////////////////////////////////////////////////////////////////////

SelectJumpGUI::SelectJumpGUI( wxWindow* parent, FFleet * fleet, FMap * map, std::string system,
		int player, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size,
		long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	m_map = map;
	m_player = player;
	m_fleet = fleet;
	m_system = system;
	m_parent = parent;

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	std::string txt = "Possible jump routes for " + fleet->getName();
	m_staticText1 = new wxStaticText( this, wxID_ANY, txt, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer1->Add( m_staticText1, 0, wxALL, 5 );

	m_staticText5 = new wxStaticText( this, wxID_ANY, wxT("Select a jump and chose the travel method"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	bSizer1->Add( m_staticText5, 0, wxALL, 5 );

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 1, 2, 0, 2 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	fgSizer1->SetMinSize( wxSize( 150,-1 ) );
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Destinations") ), wxVERTICAL );

	m_listBox1 = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE );
	m_listBox1->SetMinSize( wxSize( 150,170 ) );
	m_sList = map->getConnectedSystems(system,player,fleet);
	for (unsigned int i = 0; i < m_sList.size(); i++){
		m_listBox1->Append(m_sList[i]);
	}

	sbSizer1->Add( m_listBox1, 1, wxALL|wxEXPAND, 5 );

	fgSizer1->Add( sbSizer1, 1, wxEXPAND, 5 );

	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Jump Info") ), wxVERTICAL );
	sbSizer2->SetMinSize( wxSize( 230,-1 ) );

	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("Standard Travel Time:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	sbSizer2->Add( m_staticText2, 0, wxALL, 5 );

	m_staticText3 = new wxStaticText( this, wxID_ANY, wxT("Risk Jump 2 Travel Time:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	sbSizer2->Add( m_staticText3, 0, wxALL, 5 );

	m_staticText4 = new wxStaticText( this, wxID_ANY, wxT("Risk Jump 3 Travel Time:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	sbSizer2->Add( m_staticText4, 0, wxALL, 5 );

	fgSizer1->Add( sbSizer2, 0, wxEXPAND|wxALIGN_RIGHT, 5 );

	bSizer1->Add( fgSizer1, 1, wxEXPAND, 5 );

	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 1, 4, 0, 0 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_button1 = new wxButton( this, wxID_ANY, wxT("Standard Jump"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button1->Enable( false );
	fgSizer2->Add( m_button1, 0, wxALL, 5 );

	m_button2 = new wxButton( this, wxID_ANY, wxT("Risk Jump 2"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button2->Enable( false );
	fgSizer2->Add( m_button2, 0, wxALL, 5 );

	m_button3 = new wxButton( this, wxID_ANY, wxT("Risk Jump 3"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button3->Enable( false );
	fgSizer2->Add( m_button3, 0, wxALL, 5 );

	m_button4 = new wxButton( this, wxID_ANY, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_button4, 0, wxALL, 5 );

	bSizer1->Add( fgSizer2, 0, wxEXPAND, 5 );

	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );

	// Connect Events
	m_listBox1->Connect( wxEVT_LEFT_UP, wxMouseEventHandler( SelectJumpGUI::onSelectJump ), NULL, this );
	m_button1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SelectJumpGUI::onStandardJump ), NULL, this );
	m_button2->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SelectJumpGUI::onRiskJump2 ), NULL, this );
	m_button3->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SelectJumpGUI::onRiskJump3 ), NULL, this );
	m_button4->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SelectJumpGUI::onCancel ), NULL, this );
}

SelectJumpGUI::~SelectJumpGUI()
{
	// Disconnect Events
	m_listBox1->Disconnect( wxEVT_LEFT_UP, wxMouseEventHandler( SelectJumpGUI::onSelectJump ), NULL, this );
	m_button1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SelectJumpGUI::onStandardJump ), NULL, this );
	m_button2->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SelectJumpGUI::onRiskJump2 ), NULL, this );
	m_button3->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SelectJumpGUI::onRiskJump3 ), NULL, this );
	m_button4->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SelectJumpGUI::onCancel ), NULL, this );
}

void SelectJumpGUI::onSelectJump( wxMouseEvent& event ){
	// get the selected jump
	m_jump = m_map->getJumpRoute(m_system,std::string(m_listBox1->GetStringSelection()));

	// determine the trip time at different jump speeds
	std::ostringstream os,os2,os3;
	std::string spacer;
	//this just gets the formatting right on the screen.
	if (((int)(m_jump->getLength()))>9){
		spacer = "     ";
	} else {
		spacer = "       ";
	}
	os << "Standard Travel Time:" << spacer << (int)(m_jump->getLength()) << " days";
	m_staticText2->SetLabel(os.str());
	os2 << "Risk Jump 2 Travel Time:  " << m_jump->getJumpTime(2) << " days";
	m_staticText3->SetLabel(os2.str());
	os3 << "Risk Jump 3 Travel Time:  " << m_jump->getJumpTime(3) << " days";
	m_staticText4->SetLabel(os3.str());

	// activate the jump buttons;
	m_button1->Enable(true);
	m_button2->Enable(true);
	m_button3->Enable(true);

	event.Skip();
}

void SelectJumpGUI::onStandardJump( wxCommandEvent& event ){
	if (m_fleet->getMaxSpeed() > 0){
		m_fleet->setSpeed(1);
		setJump();
		event.Skip();
		EndModal(0);
	} else {
		wxMessageDialog d(m_parent,"The fleet cannot jump.  At least one ship has no ADF.",
				"Speed error.", wxOK);
		d.ShowModal();
		event.Skip();
	}
}

void SelectJumpGUI::onRiskJump2( wxCommandEvent& event ){
	if (m_fleet->getMaxSpeed() > 1){
		m_fleet->setSpeed(2);
		setJump();
		event.Skip();
		EndModal(0);
	} else {
		wxMessageDialog d(m_parent,"The fleet cannot risk jump.  At least one ship has an ADF of only 1.",
				"Speed error.", wxOK);
		d.ShowModal();
		event.Skip();
	}
}

void SelectJumpGUI::onRiskJump3( wxCommandEvent& event ){
	if (m_fleet->getMaxSpeed() > 2){
		m_fleet->setSpeed(3);
		setJump();
		event.Skip();
		EndModal(0);
	} else {
		wxMessageDialog d(m_parent,"The fleet cannot travel at that speed.\n  At least one ship has an ADF of only 2.",
				"Speed error.", wxOK);
		d.ShowModal();
		event.Skip();
	}
}

void SelectJumpGUI::setJump(){
	m_fleet->setInTransit(true);
	unsigned int dest;
	if (m_jump->getStart()->getName() == m_system){
		dest = m_jump->getEnd()->getID();
		m_fleet->setJumpRoute(m_jump->getID(),m_jump->getStart(),m_jump->getEnd(),(unsigned int)(m_jump->getLength()));
	} else {
		dest = m_jump->getStart()->getID();
		m_fleet->setJumpRoute(m_jump->getID(),m_jump->getEnd(),m_jump->getStart(),(unsigned int)(m_jump->getLength()));
	}
	m_fleet->setDestination(dest);
}

