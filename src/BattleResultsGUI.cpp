///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  4 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "BattleResultsGUI.h"
#include "FMap.h"
#include "FGame.h"

///////////////////////////////////////////////////////////////////////////

BattleResultsGUI::BattleResultsGUI( wxWindow* parent, FleetList fleets, FVehicle * station,bool *flag, PlayerList *pList, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style ) {
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	m_fleets = fleets;
	m_station = station;
	m_stationStatus = flag;
	m_playerList = pList;
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	std::string msg = std::string("Welcome to the Battle Results dialog.  Here you can enter the results of")
		+ " combat conducted outside the computer program (e.g. on a tabletop board) and have"
		+ " the outcome recorded in the game.";
//	msg+= std::string("In order to record the outcome of the battle,")
//		+ " simply select the fleet you want to update, select the appropriate ship, enter the"
//		+ " ship's updated status and press the 'Update Ship Statisitics' button.\n     If you"
//		+ " simply want to mark one or more ships destroyed, simply highlight the ships in"
//		+ " question and press the 'Destroy Selected Ships' button.  If all the ships in a"
//		+ " fleet were destroyed you can select the 'All Ships Destroyed' button and all the"
//		+ " ships in the selected fleet will be removed.\n     Once you have updated all the fleet"
//		+ " simply press the 'Done' button to resume the game.";
	msg+= std::string("\n\nNOTE:  All actions in this dialog take immediate effect.  Marking a ship")
		+ " destoryed removes it from the game and pressing the 'Update Ship Statisitics'"
		+ " button puts those changes into effect immediately.";
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, msg, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( 420 );
	bSizer1->Add( m_staticText1, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Fleet Ships") ), wxVERTICAL );
	
	m_listBox1 = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_EXTENDED ); 
	m_listBox1->Enable( false );
	m_listBox1->SetMinSize( wxSize( 200,-1 ) );
	
	sbSizer1->Add( m_listBox1, 1, wxALL|wxEXPAND, 5 );
	
	fgSizer1->Add( sbSizer1, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 4, 1, 0, 0 );
	fgSizer2->AddGrowableRow( 3 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
//	fgSizer2->SetMinSize(wxSize(240,-1));
	
	wxString m_choice1Choices[] = { wxT("--- Select Fleet to Edit ---") };
	int m_choice1NChoices = sizeof( m_choice1Choices ) / sizeof( wxString );
	m_choice1 = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice1NChoices, m_choice1Choices, 0 );
	m_choice1->SetSelection( 0 );
	for (FleetList::iterator itr=fleets.begin(); itr < fleets.end(); itr++){
		m_choice1->Append((*itr)->getName());
	}
	if(station){
		m_choice1->Append(station->getName());
	}
	fgSizer2->Add( m_choice1, 0, wxALL|wxEXPAND, 5 );
	
	m_button2 = new wxButton( this, wxID_ANY, wxT("All Ships Destroyed"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button2->Enable( false );
	
	fgSizer2->Add( m_button2, 0, wxEXPAND|wxLEFT|wxRIGHT, 5 );
	
	m_button3 = new wxButton( this, wxID_ANY, wxT("Destroy Selected Ships"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button3->Enable( false );
	
	fgSizer2->Add( m_button3, 0, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Edit Ship Statistics") ), wxVERTICAL );
//	sbSizer2->SetMinSize(wxSize(240,-1));
	
	wxFlexGridSizer* fgSizer5;
	fgSizer5 = new wxFlexGridSizer( 6, 1, 0, 0 );
	fgSizer5->SetFlexibleDirection( wxBOTH );
	fgSizer5->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizer3->AddGrowableCol( 1 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("Ship Type:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	m_staticText2->Enable( false );
	
	fgSizer3->Add( m_staticText2, 0, wxALL, 5 );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	m_staticText3->Enable( false );
	
	fgSizer3->Add( m_staticText3, 1, wxALL, 5 );
	
	fgSizer5->Add( fgSizer3, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizer4->AddGrowableCol( 1 );
	fgSizer4->SetFlexibleDirection( wxBOTH );
	fgSizer4->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, wxT("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	m_staticText4->Enable( false );
	
	fgSizer4->Add( m_staticText4, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );
	
	m_textCtrl2 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl2->Enable( false );
	m_textCtrl2->SetMinSize( wxSize( 140,-1 ) );
	
	fgSizer4->Add( m_textCtrl2, 1, wxEXPAND, 5 );
	
	fgSizer5->Add( fgSizer4, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer6;
	fgSizer6 = new wxFlexGridSizer( 1, 7, 0, 0 );
	fgSizer6->AddGrowableCol( 0 );
	fgSizer6->AddGrowableCol( 1 );
	fgSizer6->AddGrowableCol( 2 );
	fgSizer6->AddGrowableCol( 3 );
	fgSizer6->AddGrowableCol( 4 );
	fgSizer6->AddGrowableCol( 5 );
	fgSizer6->AddGrowableCol( 6 );
	fgSizer6->SetFlexibleDirection( wxBOTH );
	fgSizer6->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText5 = new wxStaticText( this, wxID_ANY, wxT("ADF:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	m_staticText5->Enable( false );
	
	fgSizer6->Add( m_staticText5, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );
	
	m_textCtrl3 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 20,-1 ), 0 );
	m_textCtrl3->Enable( false );
	m_textCtrl3->SetMaxSize( wxSize( 20,-1 ) );
	
	fgSizer6->Add( m_textCtrl3, 0, wxBOTTOM|wxTOP, 5 );
	
	m_staticText6 = new wxStaticText( this, wxID_ANY, wxT("/ N"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	m_staticText6->Enable( false );
	
	fgSizer6->Add( m_staticText6, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	
	fgSizer6->Add( 15, 0, 1, wxEXPAND, 5 );
	
	m_staticText7 = new wxStaticText( this, wxID_ANY, wxT("MR:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	m_staticText7->Enable( false );
	
	fgSizer6->Add( m_staticText7, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_textCtrl7 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 20,-1 ), 0 );
	m_textCtrl7->Enable( false );
	m_textCtrl7->SetMaxSize( wxSize( 20,-1 ) );
	
	fgSizer6->Add( m_textCtrl7, 0, wxBOTTOM|wxTOP, 5 );
	
	m_staticText8 = new wxStaticText( this, wxID_ANY, wxT("/ N"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
	m_staticText8->Enable( false );
	
	fgSizer6->Add( m_staticText8, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	fgSizer5->Add( fgSizer6, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer7;
	fgSizer7 = new wxFlexGridSizer( 1, 3, 0, 0 );
	fgSizer7->AddGrowableCol( 2 );
	fgSizer7->SetFlexibleDirection( wxBOTH );
	fgSizer7->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText9 = new wxStaticText( this, wxID_ANY, wxT("Hull Points:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	m_staticText9->Enable( false );
	
	fgSizer7->Add( m_staticText9, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_textCtrl8 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 45,-1 ), 0 );
	m_textCtrl8->Enable( false );
	m_textCtrl8->SetMaxSize( wxSize( 45,-1 ) );
	
	fgSizer7->Add( m_textCtrl8, 0, 0, 5 );
	
	m_staticText10 = new wxStaticText( this, wxID_ANY, wxT("/ NNN"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText10->Wrap( -1 );
	m_staticText10->Enable( false );
	
	fgSizer7->Add( m_staticText10, 0, wxALL|wxEXPAND, 5 );
	
	fgSizer5->Add( fgSizer7, 1, wxALIGN_LEFT|wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer8;
	fgSizer8 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer8->SetFlexibleDirection( wxBOTH );
	fgSizer8->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	fgSizer8->SetMinSize( wxSize( -1,30 ) ); 
	fgSizer5->Add( fgSizer8, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer9;
	fgSizer9 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer9->SetFlexibleDirection( wxBOTH );
	fgSizer9->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	fgSizer9->SetMinSize( wxSize( -1,30 ) ); 
	fgSizer5->Add( fgSizer9, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer10;
	fgSizer10 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer10->SetFlexibleDirection( wxBOTH );
	fgSizer10->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	fgSizer10->SetMinSize( wxSize( -1,30 ) ); 
	fgSizer5->Add( fgSizer10, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer11;
	fgSizer11 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer11->SetFlexibleDirection( wxBOTH );
	fgSizer11->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	fgSizer11->SetMinSize( wxSize( -1,30 ) ); 
	fgSizer5->Add( fgSizer11, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer12;
	fgSizer12 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer12->SetFlexibleDirection( wxBOTH );
	fgSizer12->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	fgSizer12->SetMinSize( wxSize( -1,30 ) ); 
	fgSizer5->Add( fgSizer12, 1, wxEXPAND, 5 );
	
	m_button4 = new wxButton( this, wxID_ANY, wxT("Update Ship Statistics"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button4->Enable( false );
	
	fgSizer5->Add( m_button4, 0, wxALL|wxEXPAND, 5 );
	
	sbSizer2->Add( fgSizer5, 1, wxEXPAND, 5 );
	
	fgSizer2->Add( sbSizer2, 1, wxEXPAND, 5 );
	
	fgSizer1->Add( fgSizer2, 1, wxEXPAND, 5 );
	
	bSizer1->Add( fgSizer1, 1, wxEXPAND, 5 );
	
	m_button1 = new wxButton( this, wxID_ANY, wxT("Done"), wxDefaultPosition, wxDefaultSize, 0 );
//	m_button1->Enable( false );
	
	bSizer1->Add( m_button1, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	// Connect Events
	m_listBox1->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( BattleResultsGUI::onSelectShip ), NULL, this );
	m_choice1->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( BattleResultsGUI::onSelectFleet ), NULL, this );
	m_button2->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BattleResultsGUI::onAllDestroyed ), NULL, this );
	m_button3->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BattleResultsGUI::onDestroyShips ), NULL, this );
	m_button4->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BattleResultsGUI::onUpdateShip ), NULL, this );
	m_button1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BattleResultsGUI::onDone ), NULL, this );
	m_textCtrl2->Connect( wxEVT_KEY_UP, wxKeyEventHandler( BattleResultsGUI::onKeyUp ), NULL, this );
	m_textCtrl3->Connect( wxEVT_KEY_UP, wxKeyEventHandler( BattleResultsGUI::onKeyUp ), NULL, this );
	m_textCtrl7->Connect( wxEVT_KEY_UP, wxKeyEventHandler( BattleResultsGUI::onKeyUp ), NULL, this );
	m_textCtrl8->Connect( wxEVT_KEY_UP, wxKeyEventHandler( BattleResultsGUI::onKeyUp ), NULL, this );
}

BattleResultsGUI::~BattleResultsGUI()
{
	// Disconnect Events
	m_listBox1->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( BattleResultsGUI::onSelectShip ), NULL, this );
	m_choice1->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( BattleResultsGUI::onSelectFleet ), NULL, this );
	m_button2->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BattleResultsGUI::onAllDestroyed ), NULL, this );
	m_button3->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BattleResultsGUI::onDestroyShips ), NULL, this );
	m_button4->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BattleResultsGUI::onUpdateShip ), NULL, this );
	m_button1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BattleResultsGUI::onDone ), NULL, this );
	m_textCtrl2->Disconnect( wxEVT_KEY_UP, wxKeyEventHandler( BattleResultsGUI::onKeyUp ), NULL, this );
	m_textCtrl3->Disconnect( wxEVT_KEY_UP, wxKeyEventHandler( BattleResultsGUI::onKeyUp ), NULL, this );
	m_textCtrl7->Disconnect( wxEVT_KEY_UP, wxKeyEventHandler( BattleResultsGUI::onKeyUp ), NULL, this );
	m_textCtrl8->Disconnect( wxEVT_KEY_UP, wxKeyEventHandler( BattleResultsGUI::onKeyUp ), NULL, this );
}

void BattleResultsGUI::onSelectFleet( wxCommandEvent& event ){
	std::string fleet = std::string(m_choice1->GetStringSelection().c_str());
	int id = m_choice1->GetSelection();
	if (id==0){
		m_curFleet=NULL;
		if(m_listBox1->IsEnabled() && !(m_listBox1->IsEmpty())){ 
			m_listBox1->Clear();
		}
		m_button2->Enable(false);
		m_button3->Enable(false);
		m_button4->Enable(false);
		m_listBox1->Enable(false);
		m_staticText2->Enable(false);
		m_staticText3->Enable(false);
		m_staticText4->Enable(false);
		m_textCtrl2->Enable(false);
		m_staticText5->Enable(false);
		m_textCtrl3->Enable(false);
		m_staticText6->Enable(false);
		m_staticText7->Enable(false);
		m_textCtrl7->Enable(false);
		m_staticText8->Enable(false);
		m_staticText9->Enable(false);
		m_textCtrl8->Enable(false);
		m_staticText10->Enable(false);
		

	} else {
		m_listBox1->Enable(true);
		if(m_listBox1->IsEnabled() && !(m_listBox1->IsEmpty())){ 
			m_listBox1->Clear();
		}
		if(id==(int)(1+m_fleets.size())){ // this will only occur if there is a station
			m_listBox1->Append(m_station->getName());
			m_curFleet=NULL;
		} else {
			m_curFleet=m_fleets[id-1];
			VehicleList s = m_fleets[id-1]->getShipList();
			for(VehicleList::iterator itr = s.begin(); itr < s.end(); itr++){
				m_listBox1->Append((*itr)->getName());
			}
		}
		m_button2->Enable(true);
		m_button3->Enable(false);
		m_button4->Enable(false);
		m_staticText2->Enable(false);
		m_staticText3->Enable(false);
		m_staticText4->Enable(false);
		m_textCtrl2->Enable(false);
		m_staticText5->Enable(false);
		m_textCtrl3->Enable(false);
		m_staticText6->Enable(false);
		m_staticText7->Enable(false);
		m_textCtrl7->Enable(false);
		m_staticText8->Enable(false);
		m_staticText9->Enable(false);
		m_textCtrl8->Enable(false);
		m_staticText10->Enable(false);
	}
	event.Skip();
}

void BattleResultsGUI::onAllDestroyed( wxCommandEvent& event ){
	int fleetID = m_choice1->GetSelection(); // which fleet are we dealing with
	if(fleetID==(int)(1+m_fleets.size())){ // this will only occur if there is a station
		*m_stationStatus = false;			// set the pointer to null
//		std::cerr << "BattleResultsGUI:: the station is history!" << std::endl;
	} else {  // we can't get here unless a fleet was actually selected
		// first let's remove the fleet from the system it is in and get a pointer to it
		FMap &map = FMap::getMap();
		FFleet *f = map.getSystem(m_fleets[fleetID-1]->getLocation())->removeFleet(m_fleets[fleetID-1]->getID());
		// next remove the fleet from the player's fleet list
		FGame &game = FGame::create();
		FPlayer * player = game.getPlayer(m_fleets[fleetID-1]->getOwner());
		player->removeFleet(m_fleets[fleetID-1]->getID());

		// next move the ships to the dead ship list
		VehicleList s = f->getShipList();
		while (s.size() > 0 ){
//			std::cerr << "deleting " << (*(s.begin()))->getName() << std::endl;
			player->addDestroyedShip(f->removeShip((*(s.begin()))->getID()));
			s.erase(s.begin());
		}
		m_fleets.erase(m_fleets.begin()+fleetID-1); // remove the fleet from the fleet list
	}
	m_choice1->Delete(fleetID);		// remove the fleet or station from the list of fleets
	m_choice1->SetSelection(0);	// Reset the choice box
	onSelectFleet(event);		// trigger the dialog reset to no fleet selected
	
	event.Skip();
}

void BattleResultsGUI::onSelectShip( wxCommandEvent& event ){
	wxArrayInt sel;
	int count = m_listBox1->GetSelections(sel);
	bool on = false;
	m_button3->Enable(true);
	if (count == 1){  // Populate the ship details section and activate it
//		std::cerr << "One ship selected" << std::endl;
		on = true;
		if (m_curFleet == NULL){ // we are dealing with the station
			m_curShip = m_station;
		} else {
			m_curShip = m_curFleet->getShipList()[sel[0]];
		}
		m_staticText3->SetLabel(m_curShip->getType());
		m_textCtrl2->SetValue(m_curShip->getName());
		m_textCtrl3->SetValue(wxString::Format("%d",m_curShip->getADF()));
		m_staticText6->SetLabel(wxString::Format("/ %d",m_curShip->getMaxADF()));
		m_textCtrl7->SetValue(wxString::Format("%d",m_curShip->getMR()));
		m_staticText8->SetLabel(wxString::Format("/ %d",m_curShip->getMaxMR()));
		m_textCtrl8->SetValue(wxString::Format("%d",m_curShip->getHP()));
		m_staticText10->SetLabel(wxString::Format("/ %d",m_curShip->getMaxHP()));
	} else {
		m_curShip=NULL;
		if (count == 0) {
//			std::cerr << "Nothing is selected" << std::endl;
			m_button3->Enable(false);
		} else {
//			std::cerr << "Multiple ships selected" << std::endl;
		}
		m_staticText3->SetLabel("/");
		m_staticText6->SetLabel("/");
		m_staticText8->SetLabel("/");
		m_staticText10->SetLabel("/");
		m_textCtrl2->Clear();
		m_textCtrl3->Clear();
		m_textCtrl7->Clear();
		m_textCtrl8->Clear();
	}
	m_staticText2->Enable(on);
	m_staticText3->Enable(on);
	m_staticText4->Enable(on);
	m_textCtrl2->Enable(on);
	m_staticText5->Enable(on);
	m_textCtrl3->Enable(on);
	m_staticText6->Enable(on);
	m_staticText7->Enable(on);
	m_textCtrl7->Enable(on);
	m_staticText8->Enable(on);
	m_staticText9->Enable(on);
	m_textCtrl8->Enable(on);
	m_staticText10->Enable(on);
	event.Skip();
}

void BattleResultsGUI::onDestroyShips( wxCommandEvent& event ){
	if(m_curFleet == NULL){ // this will only occur if the station is selected
		*m_stationStatus = false;		// set the pointer to null
		m_choice1->Delete(m_choice1->GetCount()-1);	// remove the station from the list of fleets
		m_choice1->SetSelection(0);		// Reset the fleet choice box
		onSelectFleet(event);			// trigger the dialog reset to no fleet selected
	} else {  // we are removing one or more ships from a fleet
		// get the selected ships
		wxArrayInt sel;
		int count = m_listBox1->GetSelections(sel);
		if (count == (int)(m_curFleet->getShipCount())){ // all ships are selected
			onAllDestroyed(event); // we don't need to do anything else
		} else {  // we have to find the ones to remove
			// next move the ships to the dead ship list
			VehicleList s = m_curFleet->getShipList();
			FGame &game = FGame::create();
			FPlayer * player = game.getPlayer(m_curFleet->getOwner());
			// we will run the removal backwards so we can delete them from the list box at the same time
			for (int i = count -1; i >= 0; i--){
//				std::cerr << "deleting " << s[sel[i]]->getName() << std::endl;
				player->addDestroyedShip(m_curFleet->removeShip(s[sel[i]]->getID()));
				m_listBox1->Deselect(sel[i]);
				m_listBox1->Delete(sel[i]);
			}
			onSelectShip(event);	// this will now have no selections so it 
									// will clear out the ship data fields
		}
	}
	event.Skip();
}

void BattleResultsGUI::onKeyUp( wxKeyEvent& event ){
//	wxTextCtrl * c = (wxTextCtrl *)(event.GetEventObject());  // this is a safe cast as this is only called from a wxTextCtrl
//	bool test = c->SetStyle(0,100,wxTextAttr(*wxRED));
//	bool test = c->SetDefaultStyle(wxTextAttr(*wxRED));
//	wxString txt = c->GetValue();
//	c->Clear();
//	c->SetValue(txt);
	m_button4->Enable(true);
//	if (test){
//		std::cerr << "Style updated" << std::endl;
//	}
	event.Skip();
}

void BattleResultsGUI::onUpdateShip( wxCommandEvent& event ){
	m_curShip->setName((std::string)(m_textCtrl2->GetValue()));
	long val;
	(m_textCtrl3->GetValue()).ToLong(&val);
	m_curShip->setADF((int)val);
	(m_textCtrl7->GetValue()).ToLong(&val);
	m_curShip->setMR((int)val);
	(m_textCtrl8->GetValue()).ToLong(&val);
	m_curShip->setHP((int)val);
	m_button4->Enable(false);
	event.Skip();
}
