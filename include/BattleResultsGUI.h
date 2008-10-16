///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  4 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __BattleResultsGUI__
#define __BattleResultsGUI__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/listbox.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/textctrl.h>
#include <wx/dialog.h>

#include "FFleet.h"
#include "FPlayer.h"
using namespace Frontier;

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class BattleResultsGUI
///////////////////////////////////////////////////////////////////////////////
class BattleResultsGUI : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText1;
		wxListBox* m_listBox1;
		wxChoice* m_choice1;
		wxButton* m_button2;
		wxButton* m_button3;
		wxStaticText* m_staticText2;
		wxStaticText* m_staticText3;
		wxStaticText* m_staticText4;
		wxTextCtrl* m_textCtrl2;
		wxStaticText* m_staticText5;
		wxTextCtrl* m_textCtrl3;
		wxStaticText* m_staticText6;
		
		wxStaticText* m_staticText7;
		wxTextCtrl* m_textCtrl7;
		wxStaticText* m_staticText8;
		wxStaticText* m_staticText9;
		wxTextCtrl* m_textCtrl8;
		wxStaticText* m_staticText10;
		wxButton* m_button4;
		wxButton* m_button1;
		
		FleetList m_fleets;
		FVehicle * m_station;
		bool * m_stationStatus;
		PlayerList * m_playerList;
		FFleet * m_curFleet;
		FVehicle * m_curShip;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onSelectShip( wxCommandEvent& event );
		virtual void onSelectFleet( wxCommandEvent& event );
		virtual void onAllDestroyed( wxCommandEvent& event );
		virtual void onDestroyShips( wxCommandEvent& event );
		virtual void onUpdateShip( wxCommandEvent& event );
		virtual void onDone( wxCommandEvent& event ){ EndModal(0); event.Skip(); }
		virtual void onKeyUp( wxKeyEvent& event );
		
	
	public:
		BattleResultsGUI( wxWindow* parent, FleetList fleet,FVehicle *station, bool *flag, PlayerList *pList, wxWindowID id = wxID_ANY, const wxString& title = wxT("Record Battle Results"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCAPTION );
		~BattleResultsGUI();
	
};

#endif //__BattleResultsGUI__
