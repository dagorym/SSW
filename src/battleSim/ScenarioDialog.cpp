///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 17 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "battleSim/ScenarioDialog.h"
#include "tactical/FBattleScreen.h"

using namespace Frontier;

///////////////////////////////////////////////////////////////////////////

ScenarioDialog::ScenarioDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 5, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->AddGrowableRow( 0 );
	fgSizer2->AddGrowableRow( 1 );
	fgSizer2->AddGrowableRow( 2 );
	fgSizer2->AddGrowableRow( 3 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_buttonHeader = new wxStaticText( this, wxID_ANY, wxT("Press Button to Start"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
	m_buttonHeader->Wrap( 90 );
	m_buttonHeader->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	fgSizer2->Add( m_buttonHeader, 0, wxALIGN_BOTTOM|wxALL, 5 );
	
	m_secenarioHeader = new wxStaticText( this, wxID_ANY, wxT("Scenario Description"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
	m_secenarioHeader->Wrap( -1 );
	m_secenarioHeader->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	fgSizer2->Add( m_secenarioHeader, 0, wxALIGN_BOTTOM|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	m_scenario1Button = new wxButton( this, wxID_ANY, wxT("Play"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_scenario1Button, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Surprise Attack! \nUPF - Frigate, Assault Scout & station \nSathar - Destroyer and Heavy Cruiser"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( 300 );
	fgSizer2->Add( m_staticText1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_scenario2Button = new wxButton( this, wxID_ANY, wxT("Play"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_scenario2Button, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("The Battle of Fortress Kdi'Kit\nUPF - BattleShip, Fortress, 2 Fighters, Destroyer, Frigate, 2 Assault Scouts\nSathar - Assault Carrier, 5 Fighters, Heavy Cruiser, 3 Destroyers, Frigate"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( 300 );
	fgSizer2->Add( m_staticText2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_scenario3Button = new wxButton( this, wxID_ANY, wxT("Play"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_scenario3Button, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, wxT("Close Escort\nUPF - Light Cruiser, 2 Assault Scouts, Damaged Assault Carrier\nSathar - Light Cruiser, Destroyer"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( 300 );
	fgSizer2->Add( m_staticText3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_scenario4Button = new wxButton( this, wxID_ANY, wxT("Play"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_scenario4Button, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, wxT("The Stand\nUPF - Fortified Station, 5 Fighters, Minelayer, 2 Frigates, 2 Assault Scouts\nSathar - Assault Carrier, 6 Fighters, 2 Destroyers, Light Cruiser, Heavy Cruiser"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( 300 );
	fgSizer2->Add( m_staticText4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	fgSizer1->Add( fgSizer2, 1, wxEXPAND, 5 );
	
	m_doneButton = new wxButton( this, wxID_ANY, wxT("Done"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_doneButton, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	this->SetSizer( fgSizer1 );
	this->Layout();
	fgSizer1->Fit( this );
	
	// Connect Events
	m_scenario1Button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ScenarioDialog::onScenario1 ), NULL, this );
	m_scenario2Button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ScenarioDialog::onScenario2 ), NULL, this );
	m_scenario3Button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ScenarioDialog::onScenario3 ), NULL, this );
	m_scenario4Button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ScenarioDialog::onScenario4 ), NULL, this );
	m_doneButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ScenarioDialog::onDone ), NULL, this );
}

ScenarioDialog::~ScenarioDialog()
{
	// Disconnect Events
	m_scenario1Button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ScenarioDialog::onScenario1 ), NULL, this );
	m_scenario2Button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ScenarioDialog::onScenario2 ), NULL, this );
	m_scenario3Button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ScenarioDialog::onScenario3 ), NULL, this );
	m_scenario4Button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ScenarioDialog::onScenario4 ), NULL, this );
	m_doneButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ScenarioDialog::onDone ), NULL, this );
}

void ScenarioDialog::onScenario1( wxCommandEvent& event ){
	FBattleScreen *bb = new FBattleScreen();
	FleetList aList,dList;
	FVehicle *st = createShip("ArmedStation", "UPFB Doliin Bay");
	FFleet *f = new FFleet();
	f->setName("UPF");
	FVehicle *s = createShip("Frigate", "UPFS Z'Gata");
	s->setIcon("icons/UPFFrigate.png");
	f->addShip(s);
	f->addShip(createShip("AssaultScout", "UPFS Stiletto"));
	dList.push_back(f);
	f = new FFleet();
	f->setName("Sathar");
	s = createShip("HvCruiser", "SAV Perdition");
	s->setIcon("icons/SatharHvCruiser.png");
	f->addShip(s);
	s = createShip("Destroyer", "SAV Venomous");
	s->setIcon("icons/SatharDestroyer.png");
	f->addShip(s);
	aList.push_back(f);
	bb->setupFleets(&aList,&dList,true,st);

	Hide();
	bb->ShowModal();
	Show();

	event.Skip();
}

void ScenarioDialog::onScenario2( wxCommandEvent& event ){
	FBattleScreen *bb = new FBattleScreen();
	FleetList aList,dList;

	// set up the UPF
	FVehicle *st = createShip("Fortress");
	st->setName("UPFB Kdikit");
	FFleet *f = new FFleet();
	f->setName("UPF");
	FVehicle *s = createShip("Battleship", "UPFS Admiral Clinton");
	f->addShip(s);
	s = createShip("Fighter", "UPF Fighter 1");
	s->setIcon("icons/UPFFighter.png");
	f->addShip(s);
	s = createShip("Fighter", "UPF Fighter 2");
	s->setIcon("icons/UPFFighter.png");
	f->addShip(s);
	s = createShip("Destroyer", "UPFS Allison May");
	s->setIcon("icons/UPFDestroyer.png");
	f->addShip(s);
	s = createShip("Frigate", "UPFS Driadia");
	s->setIcon("icons/UPFFrigate.png");
	f->addShip(s);
	f->addShip(createShip("AssaultScout", "UPFS Dauntless"));
	f->addShip(createShip("AssaultScout", "UPFS Razor"));
	dList.push_back(f);

	// set up the Sathar
	f = new FFleet();
	f->setName("Sathar");
	s = createShip("AssaultCarrier", "SAV Infamous");
	s->setIcon("icons/SatharAssaultCarrier.png");
	f->addShip(s);
	s = createShip("Fighter", "SAV Fighter 1");
	s->setIcon("icons/SatharFighter.png");
	f->addShip(s);
	s = createShip("Fighter", "SAV Fighter 2");
	s->setIcon("icons/SatharFighter.png");
	f->addShip(s);
	s = createShip("Fighter", "SAV Fighter 3");
	s->setIcon("icons/SatharFighter.png");
	f->addShip(s);
	s = createShip("Fighter", "SAV Fighter 4");
	s->setIcon("icons/SatharFighter.png");
	f->addShip(s);
	s = createShip("Fighter", "SAV Fighter 5");
	s->setIcon("icons/SatharFighter.png");
	f->addShip(s);
	s = createShip("HvCruiser", "SAV Star Scourge");
	s->setIcon("icons/SatharHvCruiser.png");
	f->addShip(s);
	s = createShip("Destroyer", "SAV Vicious");
	s->setIcon("icons/SatharDestroyer.png");
	f->addShip(s);
	s = createShip("Destroyer", "SAV Pestilence");
	s->setIcon("icons/SatharDestroyer.png");
	f->addShip(s);
	s = createShip("Destroyer", "SAV Doomfist");
	s->setIcon("icons/SatharDestroyer.png");
	f->addShip(s);
	s = createShip("Frigate", "SAV Stinger");
	s->setIcon("icons/SatharFrigate.png");
	f->addShip(s);
	aList.push_back(f);
	bb->setupFleets(&aList,&dList,true,st);

	Hide();
	bb->ShowModal();
	Show();

	event.Skip();
}

void ScenarioDialog::onScenario3( wxCommandEvent& event ){
	FBattleScreen *bb = new FBattleScreen();
	FleetList aList,dList;

	// set up the UPF
	FFleet *f = new FFleet();
	f->setName("UPF");
	FVehicle *s = createShip("LtCruiser", "UPFS Courageous");
	s->setIcon("icons/UPFLtCruiser.png");
	f->addShip(s);
	f->addShip(createShip("AssaultScout", "UPFS Scimitar"));
	f->addShip(createShip("AssaultScout", "UPFS Dagger"));
	s = createShip("AssaultCarrier", "UPFS Megasaurus");
	s->setADF(0);
	s->getWeapon(0)->setDamageStatus(true);
	s->getWeapon(1)->setDamageStatus(true);
	s->getWeapon(2)->setDamageStatus(true);
	s->getWeapon(2)->setCurrentAmmo(0);
	s->getDefense(1)->setCurrentAmmo(1);
	s->getDefense(2)->setDamageStatus(true);
	s->getDefense(2)->setCurrentAmmo(0);
	s->setIcon("icons/UPFAssaultCarrier.png");
	f->addShip(s);
	dList.push_back(f);

	// set up the Sathar
	f = new FFleet();
	f->setName("Sathar");
	s = createShip("LtCruiser", "SAV Faminewind");
	s->setIcon("icons/SatharLtCruiser.png");
	f->addShip(s);
	s = createShip("Destroyer", "SAV Nemesis");
	s->setIcon("icons/SatharDestroyer.png");
	f->addShip(s);
	aList.push_back(f);
	bb->setupFleets(&aList,&dList,false,NULL);

	Hide();
	bb->ShowModal();
	Show();

	event.Skip();
}

void ScenarioDialog::onScenario4( wxCommandEvent& event ){
	FBattleScreen *bb = new FBattleScreen();
	FleetList aList,dList;

	// set up the UPF
	FVehicle *st = createShip("FortifiedStation");
	st->setName("UPFB Ken'Zah");
	FFleet *f = new FFleet();
	f->setName("UPF");
	FVehicle *s = createShip("Minelayer", "UPFS Z'Rat't Zoz");
	f->addShip(s);
	s = createShip("Fighter", "UPF Fighter 1");
	s->setIcon("icons/UPFFighter.png");
	f->addShip(s);
	s = createShip("Fighter", "UPF Fighter 2");
	s->setIcon("icons/UPFFighter.png");
	f->addShip(s);
	s = createShip("Fighter", "UPF Fighter 3");
	s->setIcon("icons/UPFFighter.png");
	f->addShip(s);
	s = createShip("Fighter", "UPF Fighter 4");
	s->setIcon("icons/UPFFighter.png");
	f->addShip(s);
	s = createShip("Fighter", "UPF Fighter 5");
	s->setIcon("icons/UPFFighter.png");
	f->addShip(s);
	s = createShip("Frigate", "UPFS Shimmer");
	s->setIcon("icons/UPFFrigate.png");
	f->addShip(s);
	s = createShip("Frigate", "UPFS Zz' Nakk'T");
	s->setIcon("icons/UPFFrigate.png");
	f->addShip(s);
	f->addShip(createShip("AssaultScout", "UPFS Rapier"));
	f->addShip(createShip("AssaultScout", "UPFS Lancet"));
	dList.push_back(f);

	// set up the Sathar
	f = new FFleet();
	f->setName("Sathar");
	s = createShip("AssaultCarrier", "SAV Maelstrom");
	s->setIcon("icons/SatharAssaultCarrier.png");
	f->addShip(s);
	s = createShip("Fighter", "SAV Fighter 1");
	s->setIcon("icons/SatharFighter.png");
	f->addShip(s);
	s = createShip("Fighter", "SAV Fighter 2");
	s->setIcon("icons/SatharFighter.png");
	f->addShip(s);
	s = createShip("Fighter", "SAV Fighter 3");
	s->setIcon("icons/SatharFighter.png");
	f->addShip(s);
	s = createShip("Fighter", "SAV Fighter 4");
	s->setIcon("icons/SatharFighter.png");
	f->addShip(s);
	s = createShip("Fighter", "SAV Fighter 5");
	s->setIcon("icons/SatharFighter.png");
	f->addShip(s);
	s = createShip("Fighter", "SAV Fighter 6");
	s->setIcon("icons/SatharFighter.png");
	f->addShip(s);
	s = createShip("HvCruiser", "SAV Carrion");
	s->setIcon("icons/SatharHvCruiser.png");
	f->addShip(s);
	s = createShip("LtCruiser", "SAV Deathstroke");
	s->setIcon("icons/SatharLtCruiser.png");
	f->addShip(s);
	s = createShip("Destroyer", "SAV Viper");
	s->setIcon("icons/SatharDestroyer.png");
	f->addShip(s);
	s = createShip("Destroyer", "SAV Bludgen");
	s->setIcon("icons/SatharDestroyer.png");
	f->addShip(s);
	aList.push_back(f);
	bb->setupFleets(&aList,&dList,true,st);

	Hide();
	bb->ShowModal();
	Show();

	event.Skip();
}
