/**
 * @file FMainFrame.cpp
 * @brief Implementation file for FMainFrame class
 * @author Tom Stephens
 * @date Created:  Feb 28, 2005
 *
 */
#include "FMainFrame.h"
#include "Frontier.h"
#include "tactical/FBattleScreen.h"
#include <iostream>


using namespace Frontier;

FMainFrame::~FMainFrame() {
	if(m_game != NULL){
		delete m_game;
	}
	delete m_gameConfig;
}

FMainFrame::FMainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, -1, title, pos, size) {

	m_novaPlaced = false;
	this->Connect(wxEVT_PAINT, wxPaintEventHandler(FMainFrame::onPaint));

	wxMenu *menuFile = new wxMenu;
	menuFile->Append( ID_New, "&New" );
	menuFile->Append( ID_Open, "&Open" );
	menuFile->Append( ID_Save, "&Save" );
	menuFile->FindItemByPosition(2)->Enable(false);
	menuFile->Append( ID_Close, "&Close" );
	menuFile->FindItemByPosition(3)->Enable(false);
	menuFile->AppendSeparator();
	menuFile->Append( ID_Quit, "E&xit" );

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append( menuFile, "&File" );

	wxMenu *menuShow = new wxMenu;
	menuShow->Append( ID_ShowPlayers, "&Players");
	menuShow->Append( ID_ShowRetreatCond, "&Sathar Retreat Conditions");
	menuShow->FindItemByPosition(1)->Enable(false);
	menuShow->Append( ID_ShowBattleScreen, "Show &Battle Board");
	menuBar->Append (menuShow, "&Show");

	wxMenu *menuTurn = new wxMenu;
	menuTurn->Append(ID_EndSatharTurn, "End &Sathar Turn");
	menuTurn->FindItemByPosition(0)->Enable(false);
	menuTurn->Append(ID_EndUPFTurn, "End &UPF Turn");
	menuTurn->FindItemByPosition(1)->Enable(false);
	menuTurn->AppendSeparator();
	menuTurn->Append(ID_PlaceNova, "&Place Strike Force Nova");
	menuTurn->FindItemByPosition(3)->Enable(false);
	menuTurn->Append(ID_AddSatharShips, "&Add Sathar Ships");
	menuTurn->FindItemByPosition(4)->Enable(false);
	menuBar->Append(menuTurn,"&Turn");

	wxMenu *menuHelp = new wxMenu;
	menuHelp->AppendSeparator();
	menuHelp->Append( ID_About, "&About..." );
	menuBar->Append( menuHelp, "&Help" );

	SetMenuBar( menuBar );

	CreateStatusBar();
	SetStatusText( "Welcome to the Frontier!" );
	m_game=NULL;
	m_gameConfig = &(FGameConfig::create());

}

void FMainFrame::onQuit(wxCommandEvent& WXUNUSED(event)) {
    Close( TRUE );
}

void FMainFrame::onNew(wxCommandEvent& event) {
	if(m_game==NULL){
		m_game = &(FGame::create(this));
//		wxClientDC dc(this);
		int result = m_game->init(this);
		if (result){
			switch(result){
			case 1:
				resetGame();
				return;
				break;
			default:
				resetGame();
				return;
				break;
			}
		}
		m_game->draw();
        GetMenuBar()->GetMenu(2)->FindItemByPosition(0)->Enable(true);
		GetMenuBar()->GetMenu(0)->FindItemByPosition(2)->Enable(true);
		GetMenuBar()->GetMenu(0)->FindItemByPosition(3)->Enable(true);
		GetMenuBar()->GetMenu(1)->FindItemByPosition(1)->Enable(true);
	} else {
		onClose(event);
	}
}

void FMainFrame::onSave(wxCommandEvent& WXUNUSED(event)) {
	wxFileDialog *d = new wxFileDialog(this,"Select a game file to save","","","*.ssw",wxFD_OVERWRITE_PROMPT|wxFD_SAVE|wxFD_CHANGE_DIR);
	d->ShowModal();
	// get the file name to save to
	wxString fname = d->GetFilename();
	// open the file for writing
	std::ofstream os(fname.c_str(),std::ios::binary);
	if (m_game){
		m_game->save(os);
	}
	// close off the file
	os.close();
	delete d;
}

void FMainFrame::onOpen(wxCommandEvent& event) {
	if(m_game!=NULL){  // are we in the middle of a game?  If so offer to save and close it
		onClose(event);
	}
	if (m_game == NULL){ // if the user canceled the save game option we will not be null and skip this
		wxFileDialog *d = new wxFileDialog(this,"Select a game file to open","","","*.ssw",wxFD_FILE_MUST_EXIST|wxFD_OPEN|wxFD_CHANGE_DIR);
		d->ShowModal();
		m_game = &(FGame::create(this));
		// get the file name to open
		wxString fname = d->GetFilename();
		// open the file for reading
		std::ifstream is(fname.c_str(),std::ios::binary);
		// load up the game
		m_game->load(is);
		// draw the screen
		m_game->draw();
		GetMenuBar()->GetMenu(0)->FindItemByPosition(2)->Enable(true);
		GetMenuBar()->GetMenu(0)->FindItemByPosition(3)->Enable(true);
		if(m_game->isUPFTurn()){
			GetMenuBar()->GetMenu(2)->FindItemByPosition(0)->Enable(false);
			GetMenuBar()->GetMenu(2)->FindItemByPosition(1)->Enable(true);
			GetMenuBar()->GetMenu(2)->FindItemByPosition(3)->Enable(!m_novaPlaced);
			GetMenuBar()->GetMenu(2)->FindItemByPosition(4)->Enable(false);
			GetMenuBar()->GetMenu(1)->FindItemByPosition(1)->Enable(false);
		} else {
			GetMenuBar()->GetMenu(2)->FindItemByPosition(0)->Enable(true);
			GetMenuBar()->GetMenu(2)->FindItemByPosition(1)->Enable(false);
			GetMenuBar()->GetMenu(2)->FindItemByPosition(3)->Enable(false);
			GetMenuBar()->GetMenu(2)->FindItemByPosition(4)->Enable(true);
			GetMenuBar()->GetMenu(1)->FindItemByPosition(1)->Enable(true);
		}
		delete d;
	}
}

void FMainFrame::onShowPlayers(wxCommandEvent& WXUNUSED(event)) {
	std::cerr << "Entered onShowPlayers()" << std::endl;
	if(m_game!=NULL){
		m_game->showPlayers();
	} else {
		wxMessageBox(_T("No Players, game not yet initalized."),_T("Error:  Game not initalized")
				,wxOK);
	}
}

void FMainFrame::onAbout(wxCommandEvent& WXUNUSED(event)) {
	wxString body = "Star Frontiers Second Sathar War Simulation";
	body += "\n\nWritten by Tom Stephens";
	body += "\n\nCopyright 2005-2011 - New Frontier Games";
	body += "\n\nVersion 0.0.13";
	body += "\n\nStar Frontiers copyright 1982 - Wizards of the Coast";
    wxMessageBox( body, "Star Frontiers Second Sathar War Simulation",
                  wxOK | wxICON_INFORMATION );
}

void FMainFrame::onPaint(wxPaintEvent & event){
	wxPaintDC dc(this);
	if (m_game!=NULL){
		m_game->draw(dc);
	}
}

void FMainFrame::onEndUPFTurn(wxCommandEvent& event){
    if(m_game != NULL){
    	m_game->endUPFTurn();
        m_game->draw();
        GetMenuBar()->GetMenu(2)->FindItemByPosition(0)->Enable(true);
        GetMenuBar()->GetMenu(2)->FindItemByPosition(1)->Enable(false);
        GetMenuBar()->GetMenu(2)->FindItemByPosition(3)->Enable(false);
        GetMenuBar()->GetMenu(2)->FindItemByPosition(4)->Enable(true);
		GetMenuBar()->GetMenu(1)->FindItemByPosition(1)->Enable(true);
    }
}

void FMainFrame::onEndSatharTurn(wxCommandEvent& event){
    if(m_game != NULL){
    	m_game->endSatharTurn();
        m_game->draw();
        GetMenuBar()->GetMenu(2)->FindItemByPosition(0)->Enable(false);
        GetMenuBar()->GetMenu(2)->FindItemByPosition(1)->Enable(true);
        GetMenuBar()->GetMenu(2)->FindItemByPosition(3)->Enable(!m_novaPlaced);
        GetMenuBar()->GetMenu(2)->FindItemByPosition(4)->Enable(false);
		GetMenuBar()->GetMenu(1)->FindItemByPosition(1)->Enable(false);
    }
}

void FMainFrame::onLeftDClick(wxMouseEvent& event) {
    if(m_game != NULL){
    	m_game->onLeftDClick(event);
    }
}

void FMainFrame::onPlaceNova(wxCommandEvent& event){
    if(m_game != NULL){
    	m_novaPlaced = m_game->placeNova();
    	GetMenuBar()->GetMenu(2)->FindItemByPosition(3)->Enable(!m_novaPlaced);
//    	int id = GetMenuBar()->FindMenuItem("Turn","Place Strike Force Nova");
//    	if (id == wxNOT_FOUND){
//    		wxMessageBox("Didn't find the entry","Entry not found.",wxOK | wxICON_INFORMATION);
//    	} else {
//    		GetMenuBar()->FindItem(id)->Enable(!m_novaPlaced);
//    	}
        m_game->draw();
    }

}

void FMainFrame::onLeftUp(wxMouseEvent& event){
	if(m_game != NULL){
		int result = m_game->onLeftUp(event);
		wxCommandEvent e;
		switch (result){
		case 0:
			break;
		case 1:
	        GetMenuBar()->GetMenu(2)->FindItemByPosition(0)->Enable(true);
	        GetMenuBar()->GetMenu(2)->FindItemByPosition(1)->Enable(false);
	        GetMenuBar()->GetMenu(2)->FindItemByPosition(3)->Enable(false);
	        GetMenuBar()->GetMenu(2)->FindItemByPosition(4)->Enable(true);
			GetMenuBar()->GetMenu(1)->FindItemByPosition(1)->Enable(true);
			break;
		case 2:
	        GetMenuBar()->GetMenu(2)->FindItemByPosition(0)->Enable(false);
	        GetMenuBar()->GetMenu(2)->FindItemByPosition(1)->Enable(true);
	        GetMenuBar()->GetMenu(2)->FindItemByPosition(3)->Enable(!m_novaPlaced);
	        GetMenuBar()->GetMenu(2)->FindItemByPosition(4)->Enable(false);
			GetMenuBar()->GetMenu(1)->FindItemByPosition(1)->Enable(false);
			break;
		default:
			break;
		}
		this->Refresh();
	}
}

void FMainFrame::onSize(wxSizeEvent& event){
	if(m_game != NULL){
		Refresh();
	}
}

void FMainFrame::onClose(wxCommandEvent &event){
	wxMessageDialog d(this,"Do you wish to save the game before closing?"
			,"Save game?",wxYES_NO|wxCANCEL|wxYES_DEFAULT|wxICON_QUESTION);
	int result = d.ShowModal();
	if(result==wxID_CANCEL){
		return;
	}
	if (result == wxID_OK){
		onSave(event);
	}
	resetGame();
}

void FMainFrame::resetGame(){
	delete m_game;
	m_game = NULL;
	m_novaPlaced = false;
	GetMenuBar()->GetMenu(0)->FindItemByPosition(2)->Enable(false);
	GetMenuBar()->GetMenu(0)->FindItemByPosition(3)->Enable(false);
	GetMenuBar()->GetMenu(1)->FindItemByPosition(1)->Enable(false);
	wxClientDC dc(this);
	dc.Clear();
	Refresh();
}

void FMainFrame::onAddSatharShips(wxCommandEvent& event){
	if(m_game!=NULL){
		m_game->onAddSatharShips();
	}
}

void FMainFrame::onShowRetreatConditions(wxCommandEvent& event){
	if (m_game!=NULL){
		m_game->showRetreatConditions();
	}
}

void FMainFrame::onShowBattleScreen(wxCommandEvent& event){
	FBattleScreen *bb = new FBattleScreen();
	bb->Show(true);
	FleetList aList,dList;
	FVehicle *st = createShip("ArmedStation");
	FFleet *f = new FFleet();
	f->setName("UPF");
	f->addShip(createShip("AssaultScout"));
	f->addShip(createShip("AssaultScout"));
	f->addShip(createShip("AssaultScout"));
	FVehicle *s = createShip("Frigate");
	s->setIcon("icons/UPFFrigate.png");
	f->addShip(s);
//	s = createShip("Destroyer");
//	s->setIcon("icons/UPFDestroyer.png");
//	f->addShip(s);
	dList.push_back(f);
	f = new FFleet();
	f->setName("Sathar");
	s = createShip("LtCruiser");
	s->setIcon("icons/SatharLtCruiser.png");
	f->addShip(s);
	s = createShip("Destroyer");
	s->setIcon("icons/SatharDestroyer.png");
	f->addShip(s);
	aList.push_back(f);
	bb->setupFleets(&aList,&dList,true,st);

}
