///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 17 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "gui/ICMSelectionGUI.h"
#include "ships/FVehicle.h"
#include "defenses/FDefense.h"
#include <sstream>

using namespace Frontier;

///////////////////////////////////////////////////////////////////////////

ICMSelectionGUI::ICMSelectionGUI( wxWindow* parent, std::vector<ICMData *> *ICMData, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	m_ICMData = ICMData;


	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	fgSizer1 = new wxFlexGridSizer( 3, 1, 0, 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_grid1 = new wxGrid( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	
	// Grid
	m_grid1->CreateGrid( m_ICMData->size(), 4 );
	m_grid1->EnableEditing( false );
	m_grid1->EnableGridLines( true );
	m_grid1->EnableDragGridSize( false );
	m_grid1->SetMargins( 0, 0 );
	
	// Columns
	m_grid1->EnableDragColMove( false );
	m_grid1->EnableDragColSize( false );
//	m_grid1->SetColLabelSize( 100 );
	m_grid1->SetColLabelValue( 0, wxT("Target Ship") );
	m_grid1->SetColLabelValue( 1, wxT("Weapon") );
	m_grid1->SetColLabelValue( 2, wxT("Assigned ICMs") );
	m_grid1->SetColLabelValue( 3, wxT("Nearby Ships") );
	m_grid1->SetColLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Rows
	m_grid1->EnableDragRowSize( true );
	m_grid1->SetRowLabelSize( 20 );
	m_grid1->SetRowLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	Frontier::ICMData * d;
//	std::vector<ICMData *>::iterator itr = m_ICMData->begin();
	for (unsigned int row=0; row< m_ICMData->size();row ++){
		d=(*m_ICMData)[row];
		m_grid1->SetCellValue(row,0,d->weapon->getTarget()->getName());
		m_grid1->SetCellValue(row,1,d->weapon->getLongName());
		m_grid1->SetCellValue(row,2,"0");
		std::ostringstream shipList;
		if (d->vehicles->size() == 0){
			shipList << "None";
		} else {
			for (unsigned int i=0; i<d->vehicles->size(); i++){
				if ( (*(d->vehicles))[i]->getID() != d->weapon->getTarget()->getID()  // it's not the target ship
						&& (*(d->vehicles))[i]->getOwner() == d->weapon->getTarget()->getOwner()){ // and they're on the same team
					shipList << (*(d->vehicles))[i]->getName() << " ";
				}
				if (m_shipICMData.find((*(d->vehicles))[i]->getID()) == m_shipICMData.end()){
					// this ship isn't in the list yet so let's add it
					m_shipICMData[(*(d->vehicles))[i]->getID()] = new AssignedICMData((*(d->vehicles))[i]);
				}
			}
		}
		m_grid1->SetCellValue(row,3,shipList.str());
	}

	// Label Appearance
	m_grid1->SetLabelFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	m_grid1->AutoSizeColumns();
	
	// Cell Defaults
	m_grid1->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_TOP );
	fgSizer1->Add( m_grid1, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_panel1 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER|wxTAB_TRAVERSAL );
	fgSizer3 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 3, 2, 0, 0 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText1 = new wxStaticText( m_panel1, wxID_ANY, wxT("Target Ship: ---"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	m_staticText1->SetMinSize( wxSize( 200,-1 ) );
	
	fgSizer2->Add( m_staticText1, 0, wxALL, 5 );
	
	m_staticText2 = new wxStaticText( m_panel1, wxID_ANY, wxT("Total ICM Count"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
	m_staticText2->Wrap( -1 );
	m_staticText2->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	fgSizer2->Add( m_staticText2, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND, 5 );
	
	m_staticText3 = new wxStaticText( m_panel1, wxID_ANY, wxT("Weapon: ---"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	fgSizer2->Add( m_staticText3, 0, wxALL, 5 );
	
	m_staticText4 = new wxStaticText( m_panel1, wxID_ANY, wxT("-"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
	m_staticText4->Wrap( -1 );
	m_staticText4->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	fgSizer2->Add( m_staticText4, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	fgSizer3->Add( fgSizer2, 1, wxEXPAND, 5 );
	
	gSizer1 = new wxGridSizer( 1, 3, 0, 0 );
	
	m_staticText5 = new wxStaticText( m_panel1, wxID_ANY, wxT("Ship"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	m_staticText5->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	gSizer1->Add( m_staticText5, 0, wxALL, 5 );
	
	m_staticText6 = new wxStaticText( m_panel1, wxID_ANY, wxT("Total ICM Available"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
	m_staticText6->Wrap( -1 );
	m_staticText6->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	gSizer1->Add( m_staticText6, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	m_staticText7 = new wxStaticText( m_panel1, wxID_ANY, wxT("ICMs Applied"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
	m_staticText7->Wrap( -1 );
	m_staticText7->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	gSizer1->Add( m_staticText7, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	fgSizer3->Add( gSizer1, 1, wxEXPAND, 5 );
	
	m_panel1->SetSizer( fgSizer3 );
	m_panel1->Layout();
	fgSizer3->Fit( m_panel1 );
	fgSizer1->Add( m_panel1, 1, wxEXPAND | wxALL, 5 );
	
	m_button1 = new wxButton( this, wxID_ANY, wxT("Done"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_button1, 0, wxALIGN_RIGHT|wxRIGHT, 5 );
	
	this->SetSizer( fgSizer1 );
	this->Layout();
	fgSizer1->Fit( this );
	
	// Connect Events
	m_grid1->Connect( wxEVT_GRID_LABEL_LEFT_CLICK, wxGridEventHandler( ICMSelectionGUI::onPickRow ), NULL, this );
	m_grid1->Connect( wxEVT_GRID_SELECT_CELL, wxGridEventHandler( ICMSelectionGUI::onPickWeapon ), NULL, this );
	m_button1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ICMSelectionGUI::finalizeAssignments ), NULL, this );

}

ICMSelectionGUI::~ICMSelectionGUI()
{
	// Disconnect Events
	m_grid1->Disconnect( wxEVT_GRID_LABEL_LEFT_CLICK, wxGridEventHandler( ICMSelectionGUI::onPickRow ), NULL, this );
	m_grid1->Disconnect( wxEVT_GRID_SELECT_CELL, wxGridEventHandler( ICMSelectionGUI::onPickWeapon ), NULL, this );
	m_button1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ICMSelectionGUI::finalizeAssignments ), NULL, this );
}

void ICMSelectionGUI::onPickRow( wxGridEvent& event ){
	selectWeapon(event.GetRow());
	event.Skip();
}

void ICMSelectionGUI::onPickWeapon( wxGridEvent& event ){
	selectWeapon(event.GetRow());
	event.Skip();
}

void ICMSelectionGUI::updateICMCount( wxSpinEvent& event ){
	unsigned int shipRow = static_cast<wxWindow *>(event.GetEventObject())->GetId();
	// Assign new count to ICMData for this ship
	int ICMs = m_ICMsApplied[shipRow]->GetValue();
//	std::cerr << "ICM count of spin control = " << ICMs << std::endl;
	unsigned int shipID = (*(m_currentRowData->vehicles))[shipRow]->getID();
	m_shipICMData[shipID]->setICMsAllocatedToWeapon(m_currentRowData->weapon,ICMs);
	// add up all ICM's allocated to this weapon
	std::map<unsigned int, AssignedICMData *>::iterator itr = m_shipICMData.begin();
	int totalICMs = 0;
	for (unsigned int i = 0; i< m_shipICMData.size(); i++){
		totalICMs += (*itr).second->getICMsAllocatedToWeapon(m_currentRowData->weapon);
		itr++;
	}
	// update field in main table for number of ICMs allocated
	std::ostringstream os;
	os << totalICMs;
	m_grid1->SetCellValue(m_currentRow,2,os.str());
	Refresh();
	event.Skip();
}

void ICMSelectionGUI::finalizeAssignments( wxCommandEvent& event ){
	std::map<unsigned int,FVehicle *> shipList;
	std::vector<ICMData *>::iterator wItr = m_ICMData->begin();
	for (unsigned int i = 0; i< m_ICMData->size(); i++){  // loop over list of weapons
		std::map<unsigned int, AssignedICMData *>::iterator sItr = m_shipICMData.begin();
		int totalICMs = 0;
		for (unsigned int j = 0; j< m_shipICMData.size(); j++){  //loop of ships
			// total up ICMs allocated to each weapon
			totalICMs += (*sItr).second->getICMsAllocatedToWeapon((*wItr)->weapon);
			sItr++;
		}
		// update the assigned ICMs for each weapon
		(*wItr)->weapon->setAssignedICMCount(totalICMs);
		// build of list of ships
		VehicleList::iterator itr = (*wItr)->vehicles->begin();
		for (unsigned int j=0; j< (*wItr)->vehicles->size(); j++){
			shipList[(*itr)->getID()] = (*itr);
			itr++;
		}
		wItr++;
	}
	// reduce the ships' ICMs by number expended
	std::map<unsigned int,FVehicle *>::iterator vItr = shipList.begin();
	for (unsigned int i=0; i<shipList.size(); i++){
//		std::cerr << vItr->second->getName() << ":  ";
		FDefense * d = vItr->second->getDefense(vItr->second->hasDefense(FDefense::ICM));
		int expendedICMs= m_shipICMData[vItr->first]->getAllocatedICMs();
//		std::cerr << "expendedICMs = " << expendedICMs << " current ammo = " << d->getAmmo();
		d->setCurrentAmmo(d->getAmmo()-expendedICMs);
//		std::cerr << " updated ammo = " << d->getAmmo() << std::endl;
		vItr++;
	}

	EndModal(0);
	event.Skip();
}

void ICMSelectionGUI::selectWeapon(int row){
	m_currentRowData = (*m_ICMData)[row];
	m_currentRow = row;
	// Clear the grid
	gSizer1->Clear();
	// delete all the old objects and disconnect the spin control events
	for (unsigned int i = 0; i< m_ICMsApplied.size(); i++){
		delete m_shipNames[i];
		delete m_ICMsAvailable[i];
		m_ICMsApplied[i]->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( ICMSelectionGUI::updateICMCount ), NULL, this );
		delete m_ICMsApplied[i];
	}
	m_shipNames.clear();
	m_ICMsAvailable.clear();
	m_ICMsApplied.clear();

	// set the size of the new list
	gSizer1->SetRows(m_currentRowData->vehicles->size()+1);

	// Add in the header row text
	gSizer1->Add( m_staticText5, 0, wxALL, 5 );
	gSizer1->Add( m_staticText6, 0, wxALIGN_CENTER|wxALL, 5 );
	gSizer1->Add( m_staticText7, 0, wxALIGN_CENTER|wxALL, 5 );

	// add in the information
	int count = 0;
	for (unsigned int i = 0; i< m_currentRowData->vehicles->size(); i++){
		// ship name
		VehicleList *vList = m_currentRowData->vehicles;
		if ((*vList)[i]->getOwner() == m_currentRowData->weapon->getTarget()->getOwner()){
			m_shipNames.push_back(new wxStaticText( m_panel1, wxID_ANY, (*vList)[i]->getName(), wxDefaultPosition, wxDefaultSize, 0 ));
			m_shipNames[count]->Wrap(-1);
			gSizer1->Add( m_shipNames[count], 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

			// available ICMs
			std::ostringstream os;
			AssignedICMData * ICMdata = (m_shipICMData.find((*vList)[i]->getID()))->second;
			unsigned int maxICMs = ICMdata->getMaxICMs() - ICMdata->getAllocatedICMs() + ICMdata->getICMsAllocatedToWeapon(m_currentRowData->weapon);
			os << maxICMs;
			m_ICMsAvailable.push_back( new wxStaticText( m_panel1, wxID_ANY, os.str(), wxDefaultPosition, wxDefaultSize, 0 ) );
			m_ICMsAvailable[count]->Wrap(-1);
			gSizer1->Add( m_ICMsAvailable[count], 0, wxALIGN_CENTER|wxALL, 5 );

			// add in the spinner control
			m_ICMsApplied.push_back(new wxSpinCtrl( m_panel1, count, wxEmptyString, wxDefaultPosition, wxSize( 50,-1 ), wxSP_ARROW_KEYS, 0, maxICMs, ICMdata->getICMsAllocatedToWeapon(m_currentRowData->weapon) ));
			gSizer1->Add( m_ICMsApplied[count], 0, wxALIGN_CENTER, 5 );
			m_ICMsApplied[count]->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( ICMSelectionGUI::updateICMCount ), NULL, this );
			count++;
		}
	}

	// Update the layout
	m_panel1->Layout();
	fgSizer3->Fit( m_panel1 );
	this->SetSizer( fgSizer1 );
	this->Layout();
	fgSizer1->Fit( this );
	Refresh();

}
