///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  4 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "UPFUnattachedGUI.h"
#include <sstream>

///////////////////////////////////////////////////////////////////////////

UPFUnattachedGUI::UPFUnattachedGUI( wxWindow* parent, FPlayer * player, FMap * map, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	m_player = player;
	m_map = map;
	m_unattached = &(m_player->getShipList());
	m_newFleet = NULL;
	m_sysID = -1;
	m_curFleet = NULL;
	m_system = NULL;
	m_setFleetCount = 3;
	m_window = parent;

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Assign the UPF Unattached Ships to one or more fleets.  Select a fleet on the right to add or remove ships from.  If creating a new fleet, enter a name for the fleet and select the system it will be in.  Once you are done, select the Update Fleet button to commit the changes."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( 450 );
	bSizer1->Add( m_staticText1, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 1, 3, 0, 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Unattached Ships") ), wxVERTICAL );
	
	m_listBox1 = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_EXTENDED|wxLB_NEEDED_SB ); 
	VehicleList ships = m_player->getShipList();
	VehicleList::iterator itr;
	for (itr=ships.begin(); itr < ships.end(); itr++){
		m_listBox1->Append((*itr)->getName());
	}
	m_listBox1->SetMinSize( wxSize( 160,-1 ) );
	m_listBox1->Enable( false );
	
	sbSizer1->Add( m_listBox1, 1, wxALL|wxEXPAND, 5 );
	
	fgSizer1->Add( sbSizer1, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	m_button1 = new wxButton( this, wxID_ANY, wxT("Add -->"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button1->Enable( false );
	
	bSizer2->Add( m_button1, 0, wxALL, 5 );
	
	m_button2 = new wxButton( this, wxID_ANY, wxT("<-- Remove"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button2->Enable( false );
	
	bSizer2->Add( m_button2, 0, wxALL, 5 );
	
	fgSizer1->Add( bSizer2, 1, wxALIGN_CENTER_VERTICAL, 5 );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("New Fleet") ), wxVERTICAL );
	
	wxString m_choice1Choices[] = { wxT("-- Select a Fleet --"), wxT("New Fleet"), wxT("Task Force Prenglar"), wxT("Task Force Cassidine"), wxT("Strike Force Nova") };
	int m_choice1NChoices = sizeof( m_choice1Choices ) / sizeof( wxString );
	m_choice1 = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice1NChoices, m_choice1Choices, 0 );
	m_choice1->SetSelection( 0 );
	sbSizer2->Add( m_choice1, 0, wxBOTTOM|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	m_staticText2->Enable( false );
	
	bSizer3->Add( m_staticText2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_textCtrl1 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl1->Enable( false );
	
	bSizer3->Add( m_textCtrl1, 1, wxEXPAND, 5 );
	
	sbSizer2->Add( bSizer3, 0, wxEXPAND, 5 );
	
	wxString m_choice2Choices[] = { wxT("-- Select a System --") };
	int m_choice2NChoices = sizeof( m_choice2Choices ) / sizeof( wxString );
	m_choice2 = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice2NChoices, m_choice2Choices, 0 );
	std::vector<FSystem *> systems = m_map->getSystemList();
	std::vector<FSystem *>::iterator sItr;
	for (sItr = systems.begin(); sItr < systems.end(); sItr++){
		if ((*sItr)->getOwner() == m_player->getID()){
			m_choice2->Append((*sItr)->getName());
		}
	}
	m_choice2->SetSelection( 0 );
	m_choice2->Enable( false );
	
	sbSizer2->Add( m_choice2, 0, wxBOTTOM|wxEXPAND|wxTOP, 5 );
	
	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Fleet Ships") ), wxVERTICAL );
	
	m_listBox2 = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_EXTENDED|wxLB_NEEDED_SB ); 
	m_listBox2->Enable( false );
	m_listBox2->SetMinSize( wxSize( 160,150 ) );
	
	sbSizer3->Add( m_listBox2, 0, wxALL|wxEXPAND, 5 );
	
	sbSizer2->Add( sbSizer3, 2, wxEXPAND, 5 );
	
	m_button3 = new wxButton( this, wxID_ANY, wxT("Update Fleet"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button3->Enable( false );
	
	sbSizer2->Add( m_button3, 0, wxEXPAND, 5 );
	
	fgSizer1->Add( sbSizer2, 1, wxEXPAND, 5 );
	
	bSizer1->Add( fgSizer1, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_button5 = new wxButton( this, wxID_ANY, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_button5, 0, wxALL, 5 );
	
	m_button4 = new wxButton( this, wxID_ANY, wxT("Done"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button4->Enable( false );
	
	fgSizer2->Add( m_button4, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	bSizer1->Add( fgSizer2, 0, wxEXPAND, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	// Connect Events
	m_button1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UPFUnattachedGUI::onAddShip ), NULL, this );
	m_button2->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UPFUnattachedGUI::onRemoveShip ), NULL, this );
	m_choice1->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( UPFUnattachedGUI::onPickFleet ), NULL, this );
	m_textCtrl1->Connect( wxEVT_KILL_FOCUS, wxFocusEventHandler( UPFUnattachedGUI::onStoreName ), NULL, this );
	m_choice2->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( UPFUnattachedGUI::onSelectSystem ), NULL, this );
	m_listBox1->Connect( wxEVT_LEFT_UP, wxMouseEventHandler( UPFUnattachedGUI::onSelectShip ), NULL, this );
	m_listBox2->Connect( wxEVT_LEFT_UP, wxMouseEventHandler( UPFUnattachedGUI::onCheckShip ), NULL, this );
	m_button3->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UPFUnattachedGUI::onUpdateFleet ), NULL, this );
	m_button4->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UPFUnattachedGUI::onDone ), NULL, this );
	m_button5->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UPFUnattachedGUI::onCancel ), NULL, this );
}

UPFUnattachedGUI::~UPFUnattachedGUI()
{
	// Disconnect Events
	m_button1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UPFUnattachedGUI::onAddShip ), NULL, this );
	m_button2->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UPFUnattachedGUI::onRemoveShip ), NULL, this );
	m_choice1->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( UPFUnattachedGUI::onPickFleet ), NULL, this );
	m_textCtrl1->Disconnect( wxEVT_KILL_FOCUS, wxFocusEventHandler( UPFUnattachedGUI::onStoreName ), NULL, this );
	m_choice2->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( UPFUnattachedGUI::onSelectSystem ), NULL, this );
	m_listBox2->Disconnect( wxEVT_LEFT_UP, wxMouseEventHandler( UPFUnattachedGUI::onCheckShip ), NULL, this );
	m_listBox1->Disconnect( wxEVT_LEFT_UP, wxMouseEventHandler( UPFUnattachedGUI::onSelectShip ), NULL, this );
	m_button3->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UPFUnattachedGUI::onUpdateFleet ), NULL, this );
	m_button4->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UPFUnattachedGUI::onDone ), NULL, this );
	m_button5->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UPFUnattachedGUI::onCancel ), NULL, this );
}

void UPFUnattachedGUI::onPickFleet( wxCommandEvent& event ){
	std::string fleet = std::string(m_choice1->GetStringSelection().c_str());
	if (fleet == "-- Select a Fleet --"){
		if (m_curFleet!=NULL && m_curFleet->getShipCount()>0 ){
//			std::cerr << "Call 1" << std::endl;
			clearFleet();
		}
		m_system = NULL;
		m_listBox1->Enable(false);
		m_button3->Enable(false);
		m_staticText2->Enable( false );
		m_textCtrl1->Enable( false );
		m_choice2->Enable(false);
		if(m_listBox2->IsEnabled() && !(m_listBox2->IsEmpty())){ 
			m_listBox2->Clear();
		}
		m_listBox2->Enable(false);
	} else {
		m_listBox1->Enable(true);
		m_listBox2->Enable(true);
		if (fleet == "New Fleet"){
			if (m_newFleet == NULL){
				m_newFleet = new FFleet;
				std::ostringstream os;
				os << "UPF Fleet " << m_newFleet->getID();
				m_newFleet->setName(os.str());
			}
			if (m_curFleet!=NULL && m_curFleet->getShipCount()>0  
					&& m_curFleet != m_newFleet ){
//				std::cerr << "Call 3" << std::endl;
				clearFleet();
			}
			m_curFleet = m_newFleet;
			m_origSize = 0;
			m_system = NULL;
			m_fleetName = m_curFleet->getName();
			m_staticText2->Enable( true );
			m_textCtrl1->Enable( true );
			m_textCtrl1->ChangeValue(m_newFleet->getName());
			m_choice2->Enable(true);
//			std::vector<FSystem *> systems = m_map->getSystemList();
//			std::vector<FSystem *>::iterator itr;
//			for (itr = systems.begin(); itr < systems.end(); itr++){
//				if ((*itr)->getOwner() == m_player->getID()){
//					m_choice2->Append((*itr)->getName());
//				}
//			}
			if(m_listBox2->IsEnabled() && !(m_listBox2->IsEmpty())){ 
				m_listBox2->Clear();
			}
			VehicleList ships = m_curFleet->getShipList();
			VehicleList::iterator sItr;
			for (sItr=ships.begin(); sItr < ships.end(); sItr++){
				m_listBox2->Append((*sItr)->getName());
			}
		} else {
//			std::cerr << "Picked an existing Fleet" << std::endl;
			if (m_curFleet != NULL && m_curFleet->getShipCount()>0  
					&& m_curFleet != m_player->getFleet(fleet)){
//				std::cerr << "Call 2" << std::endl;
				clearFleet();
			}
//			std::cerr << "Returned to onSelectFleet()" << std::endl;
			if(m_listBox2->IsEnabled() && !(m_listBox2->IsEmpty())){ 
				m_listBox2->Clear();
			}
			m_curFleet = m_player->getFleet(fleet);
			m_origSize = m_curFleet->getShipCount();
			m_system = m_map->getSystem(m_curFleet->getLocation());
			VehicleList ships = m_curFleet->getShipList();
			VehicleList::iterator sItr;
			for (sItr=ships.begin(); sItr < ships.end(); sItr++){
				m_listBox2->Append((*sItr)->getName());
			}
			m_staticText2->Enable( false );
			m_textCtrl1->Enable( false );
			m_choice2->Enable(false);
		}
	}
	event.Skip(); 
}

void UPFUnattachedGUI::onStoreName( wxFocusEvent& event ){ 
	m_fleetName = m_textCtrl1->GetLineText(0);
	event.Skip(); 
}

void UPFUnattachedGUI::onAddShip( wxCommandEvent& event ){
	wxArrayInt sel;
	int count;
	
	count = m_listBox1->GetSelections(sel);
	for (int i = 0; i< count; i++){
		m_curFleet->addShip((*m_unattached)[sel[i]]);
	}
	for (int i = count - 1 ; i>=0; i--){
//		std::cerr << "Removing " << (*m_unattached)[sel[i]]->getName() << std::endl;
		m_unattached->erase(m_unattached->begin()+sel[i]);
//		std::cerr << "Size of unattached vector is now "  << m_unattached->size() << std::endl;
	}
//	std::cout << "Ships left:  " << m_unattached->size() << std::endl;
	redrawListBoxes();
	m_button3->Enable(true);
	event.Skip(); 
}

void UPFUnattachedGUI::clearFleet(){
//	std::cerr << "Entering clearFleeet()" << std::endl;
	VehicleList sList = m_curFleet->getShipList();
	VehicleList::iterator itr;
	unsigned int i=0;
	for (itr = sList.begin(); itr < sList.end(); itr++){
		i++;
		if ((*itr)->getID()>47 && i > m_origSize) {
			m_unattached->push_back((*itr));
			m_curFleet->removeShip((*itr)->getID());
		}
	}
//	std::cerr << "unattached ship list has " << m_unattached.size() << " ships" << std::endl;
	if(!m_listBox1->IsEmpty()){
		m_listBox1->Clear();
	}
	VehicleList::iterator sItr;
	for (sItr=m_unattached->begin(); sItr < m_unattached->end(); sItr++){
		m_listBox1->Append((*sItr)->getName());
	}
}

void UPFUnattachedGUI::onCheckShip( wxMouseEvent& event ){
//	std::cerr << "Entered onCheckShip()"<< std::endl;
	wxArrayInt sel;
	int count = m_listBox2->GetSelections(sel);
//	std::cerr << "There were " << count << " selections" << std::endl;
	for (int i=0; i < count; i++){
//		std::cerr << "Selected entry " << sel[i] << std::endl;
		if ((unsigned int)sel[i] >= m_origSize){
			m_button2->Enable(true);
		} else {
			m_listBox2->Deselect(sel[i]);
		}
	}
	event.Skip(); 
}

void UPFUnattachedGUI::onRemoveShip( wxCommandEvent& event ){ 
	wxArrayInt sel;
	int count = m_listBox2->GetSelections(sel);
	VehicleList sList = m_curFleet->getShipList();
	std::vector<unsigned int> ids;
	for (int i=0; i < count; i++){
		ids.push_back(sList[sel[i]]->getID());
	}
	for (int i = 0; i< count; i++) {
		m_unattached->push_back(m_curFleet->removeShip(ids[i]));
	}
	redrawListBoxes();
	event.Skip(); 
}

void UPFUnattachedGUI::redrawListBoxes(){
	if(!m_listBox2->IsEmpty()){
		m_listBox2->Clear();
	}
	VehicleList shipList = m_curFleet->getShipList();
	VehicleList::iterator sItr;
	for (sItr=shipList.begin(); sItr < shipList.end(); sItr++){
		m_listBox2->Append((*sItr)->getName());
	}
	if (m_unattached->size()>0){
		if(!m_listBox1->IsEmpty()){
			m_listBox1->Clear();
		}
//		VehicleList::iterator sItr;
		for (sItr=m_unattached->begin(); sItr < m_unattached->end(); sItr++){
			m_listBox1->Append((*sItr)->getName());
		}
	} else {
		if(!m_listBox1->IsEmpty()){
			m_listBox1->Clear();
		}
		m_button1->Enable(false);
	}
}

void UPFUnattachedGUI::onSelectSystem( wxCommandEvent& event ){
	std::string sys = std::string(m_choice2->GetStringSelection().c_str());
	if (sys != "-- Select a System --"){
		m_system = m_map->getSystem(sys);
//		std::cerr << "System set to " << m_system->getName() << std::endl;
	}
	event.Skip(); 
}

void UPFUnattachedGUI::onUpdateFleet( wxCommandEvent& event ){ 
	if (m_curFleet->getShipCount()>m_origSize){  // we've added ships and need to update
		int fighterCount = 0;
		int carrierCount = 0;
		VehicleList ships = m_curFleet->getShipList();
		VehicleList::iterator sItr;
		for (sItr=ships.begin(); sItr < ships.end(); sItr++){
			if((*sItr)->getType()!="Fighter"){
				if ((*sItr)->getType() == "AssaultCarrier") {
					carrierCount++;
				}
			} else {
				fighterCount++;
			}
		}
		if (fighterCount > carrierCount*8){
			wxMessageBox("You have to have at least one Assault Carrier\n for every eight fighters"
					,"Error:  Too many fighters", wxOK);
			return;
		}
		if (m_curFleet->getID() >= m_setFleetCount){  // it's one of the new fleets
			if(m_system == NULL){
				wxMessageBox("You must select a home system for the fleet."
						,"Error creating fleet",wxOK);
				return;
			} else { // set the system for the fleet;
				m_curFleet->setName(m_fleetName);
//				std::cerr << "Creating fleet " << m_curFleet->getName() << std::endl;
				m_curFleet->setLocation(m_system,false);
				m_curFleet->setOwner(m_player->getID());
				m_player->addFleet(m_curFleet);
				m_system->addFleet(m_curFleet);
				m_choice1->Append(m_curFleet->getName());
				m_curFleet->setIcon(m_player->getFleetIconName());
				m_setFleetCount++;
			}
		}
//		std::cerr << "Updating fleet " << m_curFleet->getName() << std::endl;
		m_system = NULL;
		m_curFleet = NULL;
		m_origSize = 0;
		m_newFleet = NULL;
		m_choice1->SetSelection(0);
		m_choice2->SetSelection(0);
		m_textCtrl1->Clear();
		
		m_window->Refresh();
//		m_window->Update();

		// reset the dialog box
		if(!m_listBox2->IsEmpty()){
			m_listBox2->Clear();
		}
		m_choice2->Enable(false);
		m_staticText2->Enable(false);
		m_textCtrl1->Enable(false);
		m_listBox1->Enable(false);
		m_listBox2->Enable(false);
		m_button1->Enable(false);
		m_button2->Enable(false);
		m_button3->Enable(false);
		
		// turn on the done button of all the unattached ships are assigned.
		if (m_unattached->size()==0){
			m_button4->Enable(true);
		} else {
			m_button4->Enable(false);
		}
	} else {
		wxMessageBox("You must select ships for the fleet.",
				"Error creating fleet", wxOK);
	}
	event.Skip(); 
}

void UPFUnattachedGUI::onDone( wxCommandEvent& event ){ 
	EndModal(0);
	event.Skip();
}

void UPFUnattachedGUI::onCancel( wxCommandEvent& event){ 
	if (m_curFleet != NULL && m_curFleet->getID() >= m_setFleetCount){ // a new incomplete fleet, we need to delete it
		delete m_curFleet;
	}
	EndModal(1);
	event.Skip();
}

