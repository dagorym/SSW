///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 17 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "battleSim/ScenarioEditorGUI.h"
#include "ships/FVehicle.h"
#include <sstream>
#include <tactical/FBattleScreen.h>

using namespace Frontier;

///////////////////////////////////////////////////////////////////////////

ScenarioEditorGUI::ScenarioEditorGUI( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxGridSizer* gSizer1;
	gSizer1 = new wxGridSizer( 1, 2, 0, 10 );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Defenders") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 4, 1, 0, 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxString m_defenderTeamChoices[] = { wxT("-- Select Team --"), wxT("UPF"), wxT("Sathar")/*, wxT("Pirates") */};
	int m_defenderTeamNChoices = sizeof( m_defenderTeamChoices ) / sizeof( wxString );
	m_defenderTeam = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_defenderTeamNChoices, m_defenderTeamChoices, 0 );
	m_defenderTeam->SetSelection( 0 );
	fgSizer1->Add( m_defenderTeam, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 1, 3, 0, 0 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Available Ship Types") ), wxVERTICAL );
	
	sbSizer2->SetMinSize( wxSize( 150,-1 ) ); 
	m_availableDefenderListBox = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	m_availableDefenderListBox->SetMinSize( wxSize( -1,230 ) );
	
	sbSizer2->Add( m_availableDefenderListBox, 0, wxALL|wxEXPAND, 5 );
	
	fgSizer2->Add( sbSizer2, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_defenderAddButton = new wxButton( this, wxID_ANY, wxT("Add -->"), wxDefaultPosition, wxDefaultSize, 0 );
	m_defenderAddButton->Enable(false);
	bSizer1->Add( m_defenderAddButton, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, wxT("Fleet Value:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	m_staticText4->Enable( false );
	
	bSizer1->Add( m_staticText4, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_staticText5 = new wxStaticText( this, wxID_ANY, wxT("value"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	m_staticText5->Enable( false );
	
	bSizer1->Add( m_staticText5, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_defenderRemoveButton = new wxButton( this, wxID_ANY, wxT("<-- Remove"), wxDefaultPosition, wxDefaultSize, 0 );
	m_defenderRemoveButton->Enable(false);
	bSizer1->Add( m_defenderRemoveButton, 0, wxALIGN_CENTER|wxALL, 5 );
	
	fgSizer2->Add( bSizer1, 1, wxALIGN_CENTER_VERTICAL, 5 );
	
	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Assigned Ships") ), wxVERTICAL );
	
	sbSizer3->SetMinSize( wxSize( 150,-1 ) ); 
	m_assignedDefenderListBox = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	m_assignedDefenderListBox->SetMinSize( wxSize( -1,230 ) );
	
	sbSizer3->Add( m_assignedDefenderListBox, 0, wxALL|wxEXPAND, 5 );
	
	fgSizer2->Add( sbSizer3, 1, wxEXPAND, 5 );
	
	fgSizer1->Add( fgSizer2, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer8;
	fgSizer8 = new wxFlexGridSizer( 1, 3, 0, 0 );
	fgSizer8->SetFlexibleDirection( wxBOTH );
	fgSizer8->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_hasPlanetText = new wxStaticText( this, wxID_ANY, wxT("Does the system have a planet?"), wxDefaultPosition, wxDefaultSize, 0 );
	m_hasPlanetText->Wrap( -1 );
	fgSizer8->Add( m_hasPlanetText, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_planetYes = new wxRadioButton( this, wxID_ANY, wxT("Yes"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	m_planetYes->Enable(false);
	fgSizer8->Add( m_planetYes, 0, wxALL, 5 );
	
	m_planetNo = new wxRadioButton( this, wxID_ANY, wxT("No"), wxDefaultPosition, wxDefaultSize, 0 );
	m_planetNo->SetValue( true ); 
	fgSizer8->Add( m_planetNo, 0, wxALL, 5 );
	
	fgSizer1->Add( fgSizer8, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer9;
	fgSizer9 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer9->SetFlexibleDirection( wxBOTH );
	fgSizer9->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, wxT("Station Type:  "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	m_staticText3->Enable( false );
	
	fgSizer9->Add( m_staticText3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxString m_stationChoiceChoices[] = { wxT("None"), wxT("Armed Station"), wxT("Fortified Station"), wxT("Fortress") };
	int m_stationChoiceNChoices = sizeof( m_stationChoiceChoices ) / sizeof( wxString );
	m_stationChoice = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_stationChoiceNChoices, m_stationChoiceChoices, 0 );
	m_stationChoice->SetSelection( 0 );
	m_stationChoice->Enable( false );
	
	fgSizer9->Add( m_stationChoice, 0, wxALL, 5 );
	
	fgSizer1->Add( fgSizer9, 1, wxEXPAND, 5 );
	
	sbSizer1->Add( fgSizer1, 1, wxEXPAND, 5 );
	
	gSizer1->Add( sbSizer1, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Attackers") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer( 3, 1, 0, 0 );
	fgSizer4->SetFlexibleDirection( wxBOTH );
	fgSizer4->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxString m_attackerTeamChoices[] = { wxT("-- Select Team --"), wxT("UPF"), wxT("Sathar")/*, wxT("Pirates") */};
	int m_attackerTeamNChoices = sizeof( m_attackerTeamChoices ) / sizeof( wxString );
	m_attackerTeam = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_attackerTeamNChoices, m_attackerTeamChoices, 0 );
	m_attackerTeam->SetSelection( 0 );
	fgSizer4->Add( m_attackerTeam, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer21;
	fgSizer21 = new wxFlexGridSizer( 1, 3, 0, 0 );
	fgSizer21->SetFlexibleDirection( wxBOTH );
	fgSizer21->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxStaticBoxSizer* sbSizer21;
	sbSizer21 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Available Ship Types") ), wxVERTICAL );
	
	sbSizer21->SetMinSize( wxSize( 150,-1 ) ); 
	m_availableAttackerListBox = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	m_availableAttackerListBox->SetMinSize( wxSize( -1,230 ) );
	
	sbSizer21->Add( m_availableAttackerListBox, 0, wxALL|wxEXPAND, 5 );
	
	fgSizer21->Add( sbSizer21, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxVERTICAL );
	
	m_attackerAddButton = new wxButton( this, wxID_ANY, wxT("Add -->"), wxDefaultPosition, wxDefaultSize, 0 );
	m_attackerAddButton->Enable(false);
	bSizer11->Add( m_attackerAddButton, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_staticText6 = new wxStaticText( this, wxID_ANY, wxT("Fleet Value:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	m_staticText6->Enable( false );
	
	bSizer11->Add( m_staticText6, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_staticText7 = new wxStaticText( this, wxID_ANY, wxT("value"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	m_staticText7->Enable( false );
	
	bSizer11->Add( m_staticText7, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_attackerRemoveButton = new wxButton( this, wxID_ANY, wxT("<-- Remove"), wxDefaultPosition, wxDefaultSize, 0 );
	m_attackerRemoveButton->Enable(false);
	bSizer11->Add( m_attackerRemoveButton, 0, wxALIGN_CENTER|wxALL, 5 );
	
	fgSizer21->Add( bSizer11, 1, wxALIGN_CENTER_VERTICAL, 5 );
	
	wxStaticBoxSizer* sbSizer31;
	sbSizer31 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Assigned Ships") ), wxVERTICAL );
	
	sbSizer31->SetMinSize( wxSize( 150,-1 ) ); 
	m_assignedAttackerListBox = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	m_assignedAttackerListBox->SetMinSize( wxSize( -1,230 ) );
	
	sbSizer31->Add( m_assignedAttackerListBox, 0, wxALL|wxEXPAND, 5 );
	
	fgSizer21->Add( sbSizer31, 1, wxEXPAND, 5 );
	
	fgSizer4->Add( fgSizer21, 1, wxEXPAND, 5 );
	
	wxGridSizer* gSizer2;
	gSizer2 = new wxGridSizer( 1, 2, 0, 0 );
	
	gSizer2->SetMinSize( wxSize( -1,70 ) ); 
	m_cancelButton = new wxButton( this, wxID_ANY, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer2->Add( m_cancelButton, 0, wxALIGN_BOTTOM|wxALL, 5 );
	
	m_startBattleButton = new wxButton( this, wxID_ANY, wxT("Start Battle!"), wxPoint( -1,-1 ), wxDefaultSize, 0 );
	gSizer2->Add( m_startBattleButton, 0, wxALIGN_BOTTOM|wxALIGN_RIGHT|wxALL, 5 );
	
	fgSizer4->Add( gSizer2, 1, wxEXPAND, 5 );
	
	sbSizer4->Add( fgSizer4, 1, wxEXPAND, 5 );
	
	gSizer1->Add( sbSizer4, 1, wxEXPAND, 5 );
	
	this->SetSizer( gSizer1 );
	this->Layout();
	gSizer1->Fit( this );
	
	// Connect Events
	m_defenderTeam->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( ScenarioEditorGUI::onSelectDefenderTeam ), NULL, this );
	m_availableDefenderListBox->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( ScenarioEditorGUI::onSelectDefenderType ), NULL, this );
	m_availableDefenderListBox->Connect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( ScenarioEditorGUI::onAddDefenderShip ), NULL, this );
	m_defenderAddButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ScenarioEditorGUI::onAddDefenderShip ), NULL, this );
	m_defenderRemoveButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ScenarioEditorGUI::onRemoveDefenderShip ), NULL, this );
	m_assignedDefenderListBox->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( ScenarioEditorGUI::onSelectDefenderShip ), NULL, this );
	m_assignedDefenderListBox->Connect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( ScenarioEditorGUI::onRemoveDefenderShip ), NULL, this );
	m_planetYes->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( ScenarioEditorGUI::onSelectStationYes ), NULL, this );
	m_planetNo->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( ScenarioEditorGUI::onSelectStationNo ), NULL, this );
	m_stationChoice->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( ScenarioEditorGUI::onSelectStationType ), NULL, this );
	m_attackerTeam->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( ScenarioEditorGUI::onSelectAttackerTeam ), NULL, this );
	m_availableAttackerListBox->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( ScenarioEditorGUI::onSelectAttackerType ), NULL, this );
	m_availableAttackerListBox->Connect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( ScenarioEditorGUI::onAddAttackerShip ), NULL, this );
	m_attackerAddButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ScenarioEditorGUI::onAddAttackerShip ), NULL, this );
	m_attackerRemoveButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ScenarioEditorGUI::onRemoveAttackerShip ), NULL, this );
	m_assignedAttackerListBox->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( ScenarioEditorGUI::onSelectAttakerShip ), NULL, this );
	m_assignedAttackerListBox->Connect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( ScenarioEditorGUI::onRemoveAttackerShip ), NULL, this );
	m_cancelButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ScenarioEditorGUI::onCancel ), NULL, this );
	m_startBattleButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ScenarioEditorGUI::onStartBattle ), NULL, this );
}

ScenarioEditorGUI::~ScenarioEditorGUI()
{
	// Disconnect Events
	m_defenderTeam->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( ScenarioEditorGUI::onSelectDefenderTeam ), NULL, this );
	m_availableDefenderListBox->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( ScenarioEditorGUI::onSelectDefenderType ), NULL, this );
	m_availableDefenderListBox->Disconnect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( ScenarioEditorGUI::onAddDefenderShip ), NULL, this );
	m_defenderAddButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ScenarioEditorGUI::onAddDefenderShip ), NULL, this );
	m_defenderRemoveButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ScenarioEditorGUI::onRemoveDefenderShip ), NULL, this );
	m_assignedDefenderListBox->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( ScenarioEditorGUI::onSelectDefenderShip ), NULL, this );
	m_assignedDefenderListBox->Disconnect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( ScenarioEditorGUI::onRemoveDefenderShip ), NULL, this );
	m_planetYes->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( ScenarioEditorGUI::onSelectStationYes ), NULL, this );
	m_planetNo->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( ScenarioEditorGUI::onSelectStationNo ), NULL, this );
	m_stationChoice->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( ScenarioEditorGUI::onSelectStationType ), NULL, this );
	m_attackerTeam->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( ScenarioEditorGUI::onSelectAttackerTeam ), NULL, this );
	m_availableAttackerListBox->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( ScenarioEditorGUI::onSelectAttackerType ), NULL, this );
	m_availableAttackerListBox->Disconnect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( ScenarioEditorGUI::onAddAttackerShip ), NULL, this );
	m_attackerAddButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ScenarioEditorGUI::onAddAttackerShip ), NULL, this );
	m_attackerRemoveButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ScenarioEditorGUI::onRemoveAttackerShip ), NULL, this );
	m_assignedAttackerListBox->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( ScenarioEditorGUI::onSelectAttakerShip ), NULL, this );
	m_assignedAttackerListBox->Disconnect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( ScenarioEditorGUI::onRemoveAttackerShip ), NULL, this );
	m_cancelButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ScenarioEditorGUI::onCancel ), NULL, this );
	m_startBattleButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ScenarioEditorGUI::onStartBattle ), NULL, this );
}

void ScenarioEditorGUI::onSelectDefenderTeam( wxCommandEvent& event ){
	int choice = m_defenderTeam->GetSelection();
	if(choice){
		m_availableDefenderListBox->Clear();
		m_assignedDefenderListBox->Clear();
		m_defenderAddButton->Enable(false);
		m_availableShipTypes = getAvailableShipTypes(choice);
		for(StringList::iterator itr=m_availableShipTypes.begin(); itr<m_availableShipTypes.end(); itr++){
			m_availableDefenderListBox->Append(*itr);
		}
		if (m_attackerTeam->GetSelection() == choice){
			m_attackerTeam->SetSelection(0);
			m_availableAttackerListBox->Clear();
			m_assignedAttackerListBox->Clear();
			m_attackerAddButton->Enable(false);
		}
		if (choice==1){
			m_planetYes->Enable(true);
		} else {
			m_planetYes->Enable(false);
			m_stationChoice->Enable( false );
			m_staticText3->Enable( false );
			m_planetNo->SetValue( true );
		}
	}
//	event.Skip();
}

void ScenarioEditorGUI::onSelectDefenderType( wxCommandEvent& event ){
	m_defenderAddButton->Enable(true);
//	event.Skip();
}

void ScenarioEditorGUI::onAddDefenderShip( wxCommandEvent& event ){
	m_assignedDefenderListBox->Append(m_availableDefenderListBox->GetStringSelection());
//	event.Skip();
}

void ScenarioEditorGUI::onRemoveDefenderShip( wxCommandEvent& event ){
	m_assignedDefenderListBox->Delete(m_assignedDefenderListBox->GetSelection());
//	event.Skip();
}

void ScenarioEditorGUI::onSelectDefenderShip( wxCommandEvent& event ){
	m_defenderRemoveButton->Enable(true);
//	event.Skip();
}

void ScenarioEditorGUI::onSelectStationYes( wxCommandEvent& event ){
	m_stationChoice->Enable( true );
	m_staticText3->Enable( true );
//	event.Skip();
}

void ScenarioEditorGUI::onSelectStationNo( wxCommandEvent& event ){
	m_stationChoice->Enable( false );
	m_staticText3->Enable( false );
//	event.Skip();
}

void ScenarioEditorGUI::onSelectStationType( wxCommandEvent& event ){
//	event.Skip();
}

void ScenarioEditorGUI::onSelectAttackerTeam( wxCommandEvent& event ){
	int choice = m_attackerTeam->GetSelection();
	if(choice){
		m_availableAttackerListBox->Clear();
		m_assignedAttackerListBox->Clear();
		m_attackerAddButton->Enable(false);
		m_availableShipTypes = getAvailableShipTypes(choice);
		for(StringList::iterator itr=m_availableShipTypes.begin(); itr<m_availableShipTypes.end(); itr++){
			m_availableAttackerListBox->Append(*itr);
		}
		if (m_defenderTeam->GetSelection() == choice){
			m_defenderTeam->SetSelection(0);
			m_availableDefenderListBox->Clear();
			m_assignedDefenderListBox->Clear();
			m_defenderAddButton->Enable(false);
		}
		if (choice==1){
			m_planetYes->Enable(false);
			m_stationChoice->Enable( false );
			m_staticText3->Enable( false );
			m_planetNo->SetValue( true );
		}
	}
//	event.Skip();
}

void ScenarioEditorGUI::onSelectAttackerType( wxCommandEvent& event ){
	m_attackerAddButton->Enable(true);
//	event.Skip();
}

void ScenarioEditorGUI::onAddAttackerShip( wxCommandEvent& event ){
	m_assignedAttackerListBox->Append(m_availableAttackerListBox->GetStringSelection());
//	event.Skip();
}

void ScenarioEditorGUI::onRemoveAttackerShip( wxCommandEvent& event ){
	m_assignedAttackerListBox->Delete(m_assignedAttackerListBox->GetSelection());
//	event.Skip();
}

void ScenarioEditorGUI::onSelectAttakerShip( wxCommandEvent& event ){
	m_attackerRemoveButton->Enable(true);
//	event.Skip();
}

void ScenarioEditorGUI::onStartBattle( wxCommandEvent& event ){
	// do we have a planet
	bool hasPlanet = m_planetYes->GetValue();
	// create a station if present
	FVehicle *station=NULL;
	if (hasPlanet){ // there's only a station if there is a planet
		switch (m_stationChoice->GetSelection()){
		case 1:
			station=createShip("ArmedStation");
			break;
		case 2:
			station=createShip("FortifiedStation");
			break;
		case 3:
			station=createShip("Fortress");
			break;
		default:
			station=NULL;
			break;
		}
	}
	unsigned int defenderID = m_defenderTeam->GetSelection();
	unsigned int attackerID = m_attackerTeam->GetSelection();
	if (station != NULL) {
		station->setOwner(defenderID);
	}
	FleetList aList,dList;
	// generate a fleet list for the attackers
	StringList attackers = convertNames(m_assignedAttackerListBox);
	FFleet *attackFleet = createFleet(attackers,m_attackerTeam->GetStringSelection().c_str(),attackerID);
	aList.push_back(attackFleet);
	// generate a fleet list for the defenders
	StringList defenders = convertNames(m_assignedDefenderListBox);
	FFleet *defendFleet = createFleet(defenders,m_defenderTeam->GetStringSelection().c_str(),defenderID);
	dList.push_back(defendFleet);
	// start the battle
	FBattleScreen *bb = new FBattleScreen();
	bb->setupFleets(&aList,&dList,hasPlanet,station);
	Hide();
	bb->ShowModal();
	///@todo clean up ships and fleets
//	VehicleList vList = attackFleet->getShipList();
//	for (VehicleList::iterator itr=vList.begin(); itr< vList.end(); itr++){
//		delete (*itr);
//	}
//	delete attackFleet;
//	vList = defendFleet->getShipList();
//	for (VehicleList::iterator itr=vList.begin(); itr< vList.end(); itr++){
//		delete (*itr);
//	}
//	delete defendFleet;
	EndModal(0);
//	event.Skip();
}

StringList ScenarioEditorGUI::getAvailableShipTypes(int teamID){
	StringList shipList;
	switch (teamID){
	case 1: // UPF
		shipList.push_back("Fighter");
		shipList.push_back("Assault Scout");
		shipList.push_back("Frigate");
		shipList.push_back("Destroyer");
		shipList.push_back("Minelayer");
		shipList.push_back("Light Cruiser");
		shipList.push_back("Heavy Cruiser");
		shipList.push_back("Assault Carrier");
		shipList.push_back("Battleship");
		break;
	case 2: // Sathar
		shipList.push_back("Fighter");
		shipList.push_back("Frigate");
		shipList.push_back("Destroyer");
		shipList.push_back("Light Cruiser");
		shipList.push_back("Heavy Cruiser");
		shipList.push_back("Assault Carrier");
		break;
	default:
		break;
	}

	return shipList;
}

StringList ScenarioEditorGUI::convertNames(wxListBox * ships){
	StringList shipTypes;
	int count = ships->GetCount();
	for (int i=0; i< count; i++){
		if(ships->GetString(i)=="Assault Scout"){
			shipTypes.push_back("AssaultScout");
		} else if(ships->GetString(i)=="Light Cruiser"){
			shipTypes.push_back("LtCruiser");
		} else if(ships->GetString(i)=="Heavy Cruiser"){
			shipTypes.push_back("HvCruiser");
		} else if(ships->GetString(i)=="Assault Carrier"){
			shipTypes.push_back("AssaultCarrier");
		} else {
			shipTypes.push_back(ships->GetString(i).c_str());
		}
	}
	return shipTypes;
}

FFleet * ScenarioEditorGUI::createFleet(StringList ships, std::string teamName,unsigned int id){
	FFleet *f = new FFleet;
	f->setOwner(id);
	f->setName(teamName);
	for(StringList::iterator itr = ships.begin(); itr<ships.end(); itr++){
		FVehicle *s = createShip(*itr);
		s->setOwner(id);
		std::ostringstream os;
		os << "icons/" << teamName;
		if ((*itr)=="Fighter"){
			os << "Fighter.png";
			s->setIcon(os.str());
		} else if((*itr)=="Frigate"){
			os << "Frigate.png";
			s->setIcon(os.str());
		} else if((*itr)=="Destroyer"){
			os << "Destroyer.png";
			s->setIcon(os.str());
		} else if((*itr)=="LtCruiser"){
			os << "LtCruiser.png";
			s->setIcon(os.str());
		} else if((*itr)=="HvCruiser"){
			os << "HvCruiser.png";
			s->setIcon(os.str());
		} else if((*itr)=="AssaultCarrier"){
			os << "AssaultCarrier.png";
			s->setIcon(os.str());
		}
		f->addShip(s);
	}

	return f;
}
