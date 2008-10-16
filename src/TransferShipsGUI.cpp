///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  4 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "TransferShipsGUI.h"
#include <sstream>
#include <iostream>

///////////////////////////////////////////////////////////////////////////

TransferShipsGUI::TransferShipsGUI( FPlayer * player, FFleet * fleet, FSystem * sys, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	m_isNewFleet = false;
	m_fleetName = "";
	m_fleet = fleet;
	m_player = player;
	m_origList = fleet->getShipList();
	m_system = sys;
	FleetList::iterator fItr;
	if (!(m_fleet->getInTransit())){
		m_fleetList = m_system->getFleetList();
		for (fItr = m_fleetList.begin(); fItr < m_fleetList.end(); fItr++){
			if ((*fItr)->getOwner() != m_player->getID() 
					|| (*fItr)->getInTransit() == true 
					|| (*fItr) == m_fleet
					|| (*fItr)->isMilitia() == true ){
				m_fleetList.erase(fItr);
				if (m_fleetList.size()==0){ // if there are no more fleets to look at break out of the loop
					break;
				}
			}
		}
		for (fItr = m_fleetList.begin(); fItr < m_fleetList.end(); fItr++){
//			std::cerr << "Adding a list of ships" << std::endl;
			m_shipLists.push_back((*fItr)->getShipList());
			m_nameList.push_back((*fItr)->getName());
		}
	}
	m_origFleetCount = m_shipLists.size();
//	std::cerr << "The usable fleet list has " << m_fleetList.size() << " fleets" << std::endl;
//	std::cerr << "There are " << m_shipLists.size() << " ship lists as well" << std::endl;
//	if(m_shipLists.size()>0){
//		std::cerr << "The first list has " << m_shipLists[0].size() << " ships" << std::endl;
//	}

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Select the fleet you woud like to transfer ships to/from in the drop down list.  To create a new fleet, select 'New Fleet'."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( 500 );
	bSizer1->Add( m_staticText1, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 1, 3, 0, 0 );
	fgSizer1->AddGrowableCol( 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->AddGrowableCol( 2 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, m_fleet->getName() ), wxVERTICAL );
	
	m_listBox1 = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_EXTENDED ); 
	VehicleList::iterator itr;
	for (itr=m_origList.begin(); itr < m_origList.end(); itr++){
		m_listBox1->Append((*itr)->getName());
	}
	m_listBox1->SetMinSize( wxSize( 150,-1 ) );
	m_listBox1->Enable(false);
	
	sbSizer1->Add( m_listBox1, 1, wxALL|wxEXPAND, 5 );
	
	fgSizer1->Add( sbSizer1, 1, wxEXPAND, 5 );
	
	wxGridBagSizer* gbSizer1;
	gbSizer1 = new wxGridBagSizer( 0, 0 );
	gbSizer1->AddGrowableCol( 0 );
	gbSizer1->SetFlexibleDirection( wxBOTH );
	gbSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxString m_choice1Choices[] = { wxT("--- Select Fleet ---"), wxT("New Fleet") };
	int m_choice1NChoices = sizeof( m_choice1Choices ) / sizeof( wxString );
	m_choice1 = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice1NChoices, m_choice1Choices, 0 );
	for (fItr = m_fleetList.begin(); fItr < m_fleetList.end(); fItr++){
		m_choice1->Append((*fItr)->getName());
	}
	m_choice1->SetSelection( 0 );
	gbSizer1->Add( m_choice1, wxGBPosition( 1, 0 ), wxGBSpan( 1, 1 ), wxALL|wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	m_staticText2->Enable( false );
	
	fgSizer2->Add( m_staticText2, 0, wxALIGN_CENTER|wxLEFT, 5 );
	
	m_textCtrl1 = new wxTextCtrl( this, wxID_ANY, m_fleetName, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl1->Enable( false );
	
	fgSizer2->Add( m_textCtrl1, 0, wxEXPAND|wxRIGHT, 5 );
	
	gbSizer1->Add( fgSizer2, wxGBPosition( 2, 0 ), wxGBSpan( 1, 1 ), wxEXPAND, 5 );
	
	m_button1 = new wxButton( this, wxID_ANY, wxT("Move -->"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button1->Enable( false );
	
	gbSizer1->Add( m_button1, wxGBPosition( 4, 0 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER|wxALL, 5 );
	
	m_button2 = new wxButton( this, wxID_ANY, wxT("<-- Move"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button2->Enable( false );
	
	gbSizer1->Add( m_button2, wxGBPosition( 5, 0 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER|wxALL, 5 );
	
	m_button3 = new wxButton( this, wxID_ANY, wxT("Create Fleet"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button3->Enable( false );
	
	gbSizer1->Add( m_button3, wxGBPosition( 7, 0 ), wxGBSpan( 1, 1 ), wxALL|wxEXPAND, 5 );
	
	fgSizer1->Add( gbSizer1, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Transfer to/from") ), wxVERTICAL );
	
	m_listBox2 = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_EXTENDED ); 
	m_listBox2->Enable( false );
	m_listBox2->SetMinSize( wxSize( 150,-1 ) );
	
	sbSizer2->Add( m_listBox2, 1, wxALL|wxEXPAND, 5 );
	
	fgSizer1->Add( sbSizer2, 1, wxEXPAND, 5 );
	
	bSizer1->Add( fgSizer1, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizer3->AddGrowableCol( 0 );
	fgSizer3->AddGrowableCol( 1 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_button4 = new wxButton( this, wxID_ANY, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_button4, 0, wxALL, 5 );
	
	m_button5 = new wxButton( this, wxID_ANY, wxT("Done"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_button5, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	bSizer1->Add( fgSizer3, 0, wxEXPAND, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	// Connect Events
	m_listBox1->Connect( wxEVT_LEFT_UP, wxMouseEventHandler( TransferShipsGUI::onSelectFleetOneShip ), NULL, this );
	m_choice1->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TransferShipsGUI::onSelectFleet ), NULL, this );
	m_button1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TransferShipsGUI::onAdd ), NULL, this );
	m_button2->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TransferShipsGUI::onRemove ), NULL, this );
	m_button3->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TransferShipsGUI::onNewFleet ), NULL, this );
	m_listBox2->Connect( wxEVT_LEFT_UP, wxMouseEventHandler( TransferShipsGUI::onSelectFleetTwoShip ), NULL, this );
	m_button4->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TransferShipsGUI::onCancel ), NULL, this );
	m_button5->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TransferShipsGUI::onDone ), NULL, this );
	m_textCtrl1->Connect( wxEVT_KILL_FOCUS, wxFocusEventHandler( TransferShipsGUI::onStoreName ), NULL, this );
}

TransferShipsGUI::~TransferShipsGUI()
{
	// Disconnect Events
	m_listBox1->Disconnect( wxEVT_LEFT_UP, wxMouseEventHandler( TransferShipsGUI::onSelectFleetOneShip ), NULL, this );
	m_choice1->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TransferShipsGUI::onSelectFleet ), NULL, this );
	m_button1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TransferShipsGUI::onAdd ), NULL, this );
	m_button2->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TransferShipsGUI::onRemove ), NULL, this );
	m_button3->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TransferShipsGUI::onNewFleet ), NULL, this );
	m_listBox2->Disconnect( wxEVT_LEFT_UP, wxMouseEventHandler( TransferShipsGUI::onSelectFleetTwoShip ), NULL, this );
	m_button4->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TransferShipsGUI::onCancel ), NULL, this );
	m_button5->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TransferShipsGUI::onDone ), NULL, this );
	m_textCtrl1->Disconnect( wxEVT_KILL_FOCUS, wxFocusEventHandler( TransferShipsGUI::onStoreName ), NULL, this );
}

void TransferShipsGUI::onStoreName( wxFocusEvent& event ){ 
	m_fleetName = m_textCtrl1->GetLineText(0);
	event.Skip(); 
}

void TransferShipsGUI::onSelectFleet( wxCommandEvent& event ){ 
	std::string fleet = std::string(m_choice1->GetStringSelection().c_str());
	int id = m_choice1->GetSelection();
	m_isNewFleet = false;
	if (fleet == "--- Select Fleet ---"){
		m_staticText2->Enable(false);
		m_textCtrl1->Enable(false);
		if(m_listBox2->IsEnabled() && !(m_listBox2->IsEmpty())){ 
			m_listBox2->Clear();
		}
		m_listBox2->Enable(false);
		m_button1->Enable(false);
		m_button2->Enable(false);
		m_button3->Enable(false);
		m_listBox1->Enable(false);
		m_curList=NULL;
	} else {
		m_listBox1->Enable(true);
		m_listBox2->Enable(true);
		if(m_listBox2->IsEnabled() && !(m_listBox2->IsEmpty())){ 
			m_listBox2->Clear();
		}
		if (fleet == "New Fleet"){
			m_staticText2->Enable(true);
			m_textCtrl1->Enable(true);
			m_isNewFleet = true;
			if (m_newFleetList.size()>0){
				m_button3->Enable(true);
				for (unsigned int i = 0 ; i < m_newFleetList.size(); i++){
					m_listBox2->Append(m_newFleetList[i]->getName());
				}
			}
			m_curList = &m_newFleetList;
		} else {
			for (unsigned int i = 0 ; i < m_shipLists[id-2].size(); i++){
				m_listBox2->Append(m_shipLists[id-2][i]->getName());
			}
			m_curList = &(m_shipLists[id-2]);
			m_button3->Enable(false);
		}
	}
	event.Skip(); 
}

void TransferShipsGUI::onSelectFleetOneShip( wxMouseEvent& event ){ 
	m_button1->Enable(true);
	event.Skip(); 
}

void TransferShipsGUI::onSelectFleetTwoShip( wxMouseEvent& event ){ 
	m_button2->Enable(true);
	event.Skip(); 
}

void TransferShipsGUI::onAdd( wxCommandEvent& event ){ 
	wxArrayInt sel;
	int count;
	
	count = m_listBox1->GetSelections(sel);
	for (int i = 0; i< count; i++){
		m_curList->push_back(m_origList[sel[i]]);
	}
	for (int i = count - 1 ; i>=0; i--){
		m_origList.erase(m_origList.begin()+sel[i]);
	}
	redrawListBoxes();
	if(m_isNewFleet){
		m_button3->Enable(true);
	}
	m_button1->Enable(false);

	event.Skip(); 
}

void TransferShipsGUI::redrawListBoxes(){
	if(!m_listBox2->IsEmpty()){
		m_listBox2->Clear();
	}
	VehicleList::iterator sItr;
	for (sItr=m_curList->begin(); sItr < m_curList->end(); sItr++){
		m_listBox2->Append((*sItr)->getName());
	}
	if (m_origList.size()>0){
		if(!m_listBox1->IsEmpty()){
			m_listBox1->Clear();
		}
//		VehicleList::iterator sItr;
		for (sItr=m_origList.begin(); sItr < m_origList.end(); sItr++){
			m_listBox1->Append((*sItr)->getName());
		}
	} else {
		if(!m_listBox1->IsEmpty()){
			m_listBox1->Clear();
		}
		m_button1->Enable(false);
	}
	m_button1->Enable(false);
	m_button2->Enable(false);
}

void TransferShipsGUI::onRemove( wxCommandEvent& event ){ 
	wxArrayInt sel;
	int count;
	
	count = m_listBox2->GetSelections(sel);
	for (int i = 0; i< count; i++){
		m_origList.push_back((*m_curList)[sel[i]]);
	}
	for (int i = count - 1 ; i>=0; i--){
		m_curList->erase(m_curList->begin()+sel[i]);
	}
	redrawListBoxes();
	if(m_curList->size() == 0){
		m_button3->Enable(false);
	}
	m_button2->Enable(false);

	event.Skip(); 
}

void TransferShipsGUI::onNewFleet( wxCommandEvent& event ){
	if (m_fleetName == ""){
		wxMessageBox("You need to give the fleet a name","Error: No Name",wxOK);
		return;
	}
	if (*m_curList == m_newFleetList && m_curList->size()>0){
		m_shipLists.push_back(m_newFleetList);
		m_nameList.push_back(m_fleetName);
		m_listBox1->Enable(false);
		m_choice1->Append(m_fleetName);
		m_choice1->SetSelection(0);
		m_fleetName = "";
		m_newFleetList.clear();
		m_button1->Enable(false);
		m_button2->Enable(false);
		m_button3->Enable(false);
		m_textCtrl1->SetValue(m_fleetName);
		m_textCtrl1->Enable(false);
		if(m_listBox2->IsEnabled() && !(m_listBox2->IsEmpty())){ 
			m_listBox2->Clear();
		}
		m_listBox2->Enable(false);
	}
	
	event.Skip();
}

void TransferShipsGUI::onDone( wxCommandEvent& event ){
	// first we need to check that all the fleets with fighters have enough Assault Carriers
	// check the orginal fleet
//	std::cerr << "Fleet " << m_fleet->getName();
	if (checkFighters(m_origList)){
		return;
	}
	// check the other fleets
	for (unsigned int i = 0; i < m_shipLists.size(); i++){
//		std::cerr << "Fleet " << m_nameList[i];
		if (checkFighters(m_shipLists[i])){
			return;
		}
	}
	
	// update original fleet
	updateFleet(m_fleet,m_origList);
	
//	std::cerr << "There are " << m_shipLists.size() << " fleets to process" << std::endl;
	// go through the list fleets
	for (unsigned int i = 0; i < m_shipLists.size(); i++){
//		std::cerr << "Processing fleet " << i << std::endl;
		if (i < m_origFleetCount) { // if fleets already exists, check updated ships
//			std::cerr << "Original fleet" << std::endl;
			FFleet *f = m_player->getFleet(m_nameList[i]);
			updateFleet(f,m_shipLists[i]);
		} else {  // if fleet doesn't exist create it
//			std::cerr << "New fleet" << std::endl;
			FFleet *f = new FFleet(*m_fleet);
			f->setName(m_nameList[i]);
			updateFleet(f,m_shipLists[i]);
			// add it to the player's list
			m_player->addFleet(f);
			// add it to the the system's list
			m_system->addFleet(f);
		}
		// purge any empty fleets.
		FleetList fl = m_system->getFleetList();
		for (unsigned int i = 0; i < fl.size(); i++){
			if (fl[i]->getShipCount() == 0){
				m_player->removeFleet(fl[i]->getID());
				FFleet *f = m_system->removeFleet(fl[i]->getID());
				delete f;
			}
		}
	}
	
	EndModal(0);
	event.Skip();
}

void TransferShipsGUI::updateFleet(FFleet * f, VehicleList & sl) {
//	std::cerr << "Working on fleet " << f->getName() << std::endl;
	const VehicleList *sList = &(f->getShipList());
	VehicleList::iterator i1;
	VehicleList::const_iterator i2 ;
	std::vector<unsigned int> oldShips;
	for (i2 =  sList->begin(); i2 < sList->end(); i2++){ // loop over the true fleet's list
		bool exists = false;
		for (i1 = sl.begin(); i1 < sl.end(); i1++){
			if ((*i2)->getID() == (*i1)->getID()) {  // The ship is still in the fleet
				exists = true;
				break;  // break out of this loop
			}
		}
		if (!exists){  // if the ship has been removed
//			std::cerr << "Removing Ship " << (*i2)->getName() << std::endl;
//			f->removeShip((*i2)->getID()); // remove it from the list
//			i2--;
			oldShips.push_back((*i2)->getID());
		}
	}
	for (unsigned int i=0; i<oldShips.size(); i++){
		f->removeShip(oldShips[i]);
	}
	for( i1 = sl.begin(); i1 < sl.end(); i1++){ // loop over the ships in the new fleet list
		if (f->getShip((*i1)->getID())==NULL) {  // if the ship isn't in the original list
//			std::cerr << "Adding ship " << (*i1)->getName() << std::endl;
			f->addShip(*i1);  // add the new ship to the real original fleet
		}
	}
	
}

bool TransferShipsGUI::checkFighters(VehicleList & sl){
	int fighterCount = 0;
	int carrierCount = 0;
	VehicleList::iterator sItr;
	unsigned int i=0;
	for (sItr=sl.begin(); sItr < sl.end(); sItr++){
		if((*sItr)->getType()=="Fighter"){  // count the fighters
			fighterCount++;
		}
		if ((*sItr)->getType() == "AssaultCarrier") {  // count the Assault Carriers
			carrierCount++;
		}
	}
//	std::cerr << " has " << fighterCount << " fighters "
//	          << "and " << carrierCount << " Carriers" << std::endl;
	if (fighterCount > carrierCount*8){
		std::ostringstream os;
		os << "Fleet " << m_nameList[i] << " does not have enough Assault\n"
		   << "Carriers for it's " << fighterCount << " fighters.";
		wxMessageBox(os.str(),"Error:  Too many fighters", wxOK);
		return true;
	}
	return false;
}
