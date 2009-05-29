///////////////////////////////////////////////////////////////////////////
	// C++ code generated with wxFormBuilder (version Feb  4 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "gui/SelectCombatGUI.h"
#include "gui/ViewFleetGUI.h"
#include "gui/TwoPlanetsGUI.h"
#include "gui/CombatLocationGUI.h"
#include "gui/CombatFleetsGUI.h"
#include "gui/SelectResolutionGUI.h"
#include "gui/BattleResultsGUI.h"
#include "tactical/FBattleScreen.h"

///////////////////////////////////////////////////////////////////////////

SelectCombatGUI::SelectCombatGUI( wxWindow* parent, FSystem * sys, FleetList defender, FleetList attacker, PlayerList *pList, bool satharAttacking, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	m_system = sys;
	m_defenderList = defender;
	m_attackerList = attacker;
	m_playerList = pList;
	m_satharAttacking = satharAttacking;
//	std::cerr << "There are " << m_attackerList.size() << " attacking fleets" << std::endl;
//	std::cerr << "There are " << m_defenderList.size() << " defending fleets" << std::endl;

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	std::string msg = "There are both UPF and Sathar ships in " + sys->getName() + ".  ";
	if(m_satharAttacking){
		msg += "Do the Sathar wish to attack the UPF forces?  ";
	} else {
		msg += "Do the UPF wish to attack the Sathar forces?  ";
	}
	msg += std::string("If so select the fleet or")
		+ " fleets you wish to attack with and press the \"Attack!\" button.  If not"
		+ " press the \"Decline Combat\" button.  To view the contents and status of"
		+ " any fleet select it and press the\"View Selected Fleet/Station\" button.\n\n"
		+ "Note:  The \"View Selected Fleet/Station\" button only works when a single "
		+ "fleet or station is selected in the active listbox.";

	m_staticText1 = new wxStaticText( this, wxID_ANY, msg, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( 500 );
	bSizer1->Add( m_staticText1, 0, wxALL, 5 );

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Attacking Fleets") ), wxVERTICAL );
//	std::cerr << "The attacker list has " << attacker.size() << " entries." << std::endl;

	m_listBox1 = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_EXTENDED|wxLB_NEEDED_SB );
	for (FleetList::iterator itr = attacker.begin(); itr < attacker.end(); itr++){
		m_listBox1->Append((*itr)->getName());
	}
	sbSizer1->Add( m_listBox1, 1, wxALL|wxEXPAND, 5 );

	fgSizer1->Add( sbSizer1, 1, wxEXPAND, 5 );

	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Defending Fleets and Stations") ), wxVERTICAL );

	m_listBox2 = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_EXTENDED|wxLB_NEEDED_SB );
	int count = 0;
	for (FleetList::iterator itr = defender.begin(); itr < defender.end(); itr++){
		m_listBox2->Append((*itr)->getName());
		count++;
	}
	// add in the stations if they exist and the Sathar are attacking
	if (m_satharAttacking){
		PlanetList pList = sys->getPlanetList();
		for (PlanetList::iterator itr = pList.begin(); itr < pList.end(); itr++){
			if ((*itr)->getStation()){
				m_listBox2->Append((*itr)->getStation()->getName());
				count++;
			}
		}
	}
	m_listBox2->SetMinSize( wxSize( -1, 26*count ) );
	sbSizer2->Add( m_listBox2, 1, wxALL|wxEXPAND, 5 );

	fgSizer1->Add( sbSizer2, 1, wxEXPAND, 5 );

	bSizer1->Add( fgSizer1, 1, wxEXPAND, 5 );

	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 1, 3, 0, 0 );
	fgSizer2->AddGrowableCol( 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->AddGrowableCol( 2 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_button1 = new wxButton( this, wxID_ANY, wxT("Decline Combat"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_button1, 1, wxALL, 5 );

	m_button2 = new wxButton( this, wxID_ANY, wxT("View Selected Fleet/Station"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button2->Enable( false );

	fgSizer2->Add( m_button2, 1, wxALIGN_CENTER|wxALL, 5 );

	m_button3 = new wxButton( this, wxID_ANY, wxT("Attack!"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button3->Enable( false );

	fgSizer2->Add( m_button3, 1, wxALIGN_RIGHT|wxALL, 5 );

	bSizer1->Add( fgSizer2, 0, wxEXPAND, 5 );

	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );

	// Connect Events
	m_listBox1->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( SelectCombatGUI::OnSelectLeftFleet ), NULL, this );
	m_listBox2->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( SelectCombatGUI::onSelectRightFleet ), NULL, this );
	m_button1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SelectCombatGUI::onDecline ), NULL, this );
	m_button2->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SelectCombatGUI::onView ), NULL, this );
	m_button3->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SelectCombatGUI::onAttack ), NULL, this );
}

SelectCombatGUI::~SelectCombatGUI()
{
	// Disconnect Events
	m_listBox1->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( SelectCombatGUI::OnSelectLeftFleet ), NULL, this );
	m_listBox2->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( SelectCombatGUI::onSelectRightFleet ), NULL, this );
	m_button1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SelectCombatGUI::onDecline ), NULL, this );
	m_button2->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SelectCombatGUI::onView ), NULL, this );
	m_button3->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SelectCombatGUI::onAttack ), NULL, this );
}

void SelectCombatGUI::OnSelectLeftFleet( wxCommandEvent& event ){
	wxArrayInt sel;
	m_listBox1->GetSelections(sel);
	if (sel.GetCount()==1){
		m_button2->Enable(true);
		m_curFleet = m_attackerList[sel[0]];
	} else {
		m_curFleet = NULL;
		m_button2->Enable(false);
	}
	m_active = 0;
	m_button3->Enable(true);
	event.Skip();
}

void SelectCombatGUI::onSelectRightFleet( wxCommandEvent& event ){
	wxArrayInt sel;
	m_listBox2->GetSelections(sel);
	if (sel.GetCount()==1){
		if (sel[0] < (int)(m_defenderList.size())){
			m_curFleet = m_defenderList[sel[0]];
			m_button2->Enable(true);
		} else {
			m_button2->Enable(false);
		}
	} else {
		m_curFleet = NULL;
		m_button2->Enable(false);
	}
	m_active = 1;
	event.Skip();
}

void SelectCombatGUI::onDecline( wxCommandEvent& event ){
	// if both Players have fleets left and the Sathar declined, UPF has the opportunity to attack.
	if (m_satharAttacking){
		switchRoles();
	}
	// otherwise just exit
	EndModal(0);
	event.Skip();
}

void SelectCombatGUI::onView( wxCommandEvent& event ){
	if (m_curFleet){
		ViewFleetGUI d(this,m_curFleet,m_system, NULL);
		d.ShowModal();
	}
	event.Skip();
}

void SelectCombatGUI::onAttack( wxCommandEvent& event ){
	int planet = 0;  // this is the index of the planet in the planet list the attack is against
	if(m_satharAttacking){
		if (m_system->getPlanetList().size()==2){
			TwoPlanetsGUI d(this,m_system);
			planet = d.ShowModal();
		}
	}
//	std::cerr << "Attacking planet " << planet << std::endl;
	wxArrayInt aFleets,dFleets;
	int combatLocation = 0;  // location of the combat 0=around planet, 1=deep space
	if (m_defenderList.size() > 0) {  // There are fleets in the system
		CombatFleetsGUI d1(this,m_system,m_defenderList,&dFleets,m_satharAttacking);
		d1.ShowModal();
		if(m_satharAttacking){
			CombatLocationGUI d(this,m_system->getPlanetList()[planet]);
			combatLocation = d.ShowModal();
		} else {
			combatLocation = 1;
		}
	}
//	std::cerr << "The battle location is " << combatLocation << std::endl;
	// remove fleets used in the battle from the lists and put them in a temporary list
	FleetList battleList,aList,dList;
	m_listBox1->GetSelections(aFleets);
//	std::cerr << "The number of selected attacking fleets is " << aFleets.GetCount() << std::endl;
	for (int i = aFleets.GetCount()-1; i >= 0; i--){
//		std::cerr << "i = " << i << "  aFleets[i] = " << aFleets[i] << std::endl;
		battleList.push_back(*(m_attackerList.begin()+aFleets[i]));
		std::cerr << "Adding Fleet " << (*(m_attackerList.begin()+aFleets[i]))->getName() << std::endl;;
		aList.push_back(*(m_attackerList.begin()+aFleets[i]));
		m_attackerList.erase((m_attackerList.begin()+aFleets[i]));
	}
//	std::cerr << "The number of selected defending fleets is " << dFleets.GetCount() << std::endl;
	if (dFleets.GetCount()>0){
		for (int i = dFleets.GetCount() - 1; i >= 0; i--){
			battleList.push_back(*(m_defenderList.begin()+dFleets[i]));
			std::cerr << "Adding Fleet " << (*(m_defenderList.begin()+dFleets[i]))->getName() << std::endl;
			dList.push_back(*(m_defenderList.begin()+dFleets[i]));
			m_defenderList.erase((m_defenderList.begin()+dFleets[i]));
		}
	}
//	std::cerr << "Setting station pointer  combatLocation has value of " << combatLocation << std::endl;
	//Set a pointer to the station if it is involved (NULL if not)
	FVehicle * station = NULL;
	if (!combatLocation){
		station = m_system->getPlanetList()[planet]->getStation();
//		std::cerr << "Setting sataion pointer value to " << station << std::endl;
	}
	// update status of fleets or go to the combat game
//	std::cerr << "Calling SelectResolutionGUI" << std::endl;
	SelectResolutionGUI d2(this,battleList,m_system->getName(),station);
	int result = d2.ShowModal();
	if (result){
		bool flag = false;
		if (station){
			flag = true;
		}
		BattleResultsGUI d3(this,battleList,station,&flag,m_playerList);
		d3.ShowModal();
		if (station && flag==false){  // check to see if the station was destoryed and remove it
			m_system->getPlanetList()[planet]->destroyStation();
		}
	} else {
		///@todo implement boardgame
		FBattleScreen *bb = new FBattleScreen();
//		bb->MakeModal(true);
		if (m_satharAttacking){
			bb->setupFleets(&aList,&dList,!((bool)combatLocation),station);
		} else {
			bb->setupFleets(&dList,&aList,!((bool)combatLocation),station);
		}
//		bb->Show(true);
		bb->ShowModal();
		///@todo clean up ships
	}

	// if both Players have fleets left, UPF has the opportunity to attack.
	if (m_satharAttacking){
		switchRoles();
	}

	EndModal(1);
	event.Skip();
}

void SelectCombatGUI::switchRoles(){
	if (m_defenderList.size() > 0 && m_attackerList.size() > 0){
		wxMessageDialog d(this,"The UPF now have a turn to attack","UPF attack", wxOK);
		d.ShowModal();
		SelectCombatGUI d2(this,m_system,m_attackerList,m_defenderList,m_playerList,false);
		this->Show(false);
		d2.ShowModal();
		this->Show(true);
	}

}
