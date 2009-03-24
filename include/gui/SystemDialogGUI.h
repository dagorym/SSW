///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  4 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __SystemDialogGUI__
#define __SystemDialogGUI__

//#include "FSystem.h"
#include "strategic/FMap.h"
#include "strategic/FPlayer.h"

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/listbox.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/button.h>
#include <wx/gbsizer.h>
#include <wx/dialog.h>
#include <wx/wx.h>

using namespace Frontier;

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class SystemDialogGUI
///////////////////////////////////////////////////////////////////////////////
class SystemDialogGUI : public wxDialog
{
	private:

	protected:
		wxStaticText* m_staticText1;
		wxListBox* m_listBox1;
		wxListBox* m_listBox2;
		wxListBox* m_listBox3;
		wxButton* m_button1;
		wxButton* m_button2;
		wxButton* m_button3;
		wxButton* m_button4;
		wxButton* m_button5;
		wxBoxSizer* bSizer1;

		FFleet * m_fleet;
		FMap * m_map;
		wxWindow *m_parent;
		FPlayer * m_player;
		FSystem * m_system;
		FPlanet * m_planet;
		FVehicle * m_station;

		// Virtual event handlers, overide them in your derived class
		virtual void onSelectPlanet( wxMouseEvent& event );
		virtual void onSelectStation( wxMouseEvent& event );
		virtual void onViewStationStatus( wxCommandEvent& event ){ event.Skip(); }
		virtual void onSelectFleet( wxMouseEvent& event );
		virtual void onViewShips( wxCommandEvent& event );
		virtual void onTransferShips( wxCommandEvent& event );
		virtual void onMoveFleet( wxCommandEvent& event );
		virtual void onOK (wxCommandEvent& event) { EndModal(0); event.Skip(); }

		std::string getMovementText(FFleet * f);

	public:
		SystemDialogGUI( wxWindow* parent, FSystem * sys, FMap * map, FPlayer * player, const wxString& title = wxT("System Information"), wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~SystemDialogGUI();

};

#endif //__SystemDialogGUI__
