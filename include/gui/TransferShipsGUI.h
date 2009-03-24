///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  4 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __TransferShipsGUI__
#define __TransferShipsGUI__

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
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/gbsizer.h>
#include <wx/dialog.h>

#include "strategic/FPlayer.h"
#include "strategic/FSystem.h"
using namespace Frontier;

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class TransferShipsGUI
///////////////////////////////////////////////////////////////////////////////
class TransferShipsGUI : public wxDialog
{
	private:
		void redrawListBoxes();
		void updateFleet(FFleet * f, VehicleList & sl);
		bool checkFighters(VehicleList & sl);

	protected:
		wxStaticText* m_staticText1;
		wxListBox* m_listBox1;
		wxChoice* m_choice1;
		wxStaticText* m_staticText2;
		wxTextCtrl* m_textCtrl1;
		wxButton* m_button1;
		wxButton* m_button2;
		wxButton* m_button3;
		wxListBox* m_listBox2;
		wxButton* m_button4;
		wxButton* m_button5;

		FFleet * m_fleet;
		FPlayer * m_player;
		VehicleList m_origList;
		FSystem * m_system;
		FleetList m_fleetList;
		std::vector<VehicleList> m_shipLists;
		bool m_isNewFleet;
		VehicleList m_newFleetList;
		VehicleList * m_curList;
		std::string m_fleetName;
		std::vector<std::string> m_nameList;
		unsigned int m_origFleetCount;

		// Virtual event handlers, overide them in your derived class
		virtual void onSelectFleetOneShip( wxMouseEvent& event );
		virtual void onSelectFleet( wxCommandEvent& event );
		virtual void onAdd( wxCommandEvent& event );
		virtual void onRemove( wxCommandEvent& event );
		virtual void onNewFleet( wxCommandEvent& event );
		virtual void onSelectFleetTwoShip( wxMouseEvent& event );
		virtual void onCancel( wxCommandEvent& event ){ EndModal(1); event.Skip(); }
		virtual void onDone( wxCommandEvent& event );
		virtual void onStoreName( wxFocusEvent& event );

	public:
		TransferShipsGUI( FPlayer * player, FFleet * fleet, FSystem * sys, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Tansfer Ships"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~TransferShipsGUI();

};

#endif //__TransferShipsGUI__
