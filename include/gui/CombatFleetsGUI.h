///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  4 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __CombatFleetsGUI__
#define __CombatFleetsGUI__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/listbox.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dialog.h>

#include "strategic/FSystem.h"
using namespace Frontier;

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class CombatFleetsGUI
///////////////////////////////////////////////////////////////////////////////
class CombatFleetsGUI : public wxDialog
{
	private:

	protected:
		wxStaticText* m_staticText1;
		wxListBox* m_listBox1;
		wxButton* m_button1;
		wxButton* m_button2;

		FSystem * m_system;
		FleetList m_fleetList;
		wxArrayInt * m_selections;
		bool m_allowNoDefense;

		// Virtual event handlers, overide them in your derived class
		virtual void onListSelect( wxCommandEvent& event );
		virtual void onViewFleet( wxCommandEvent& event );
		virtual void onOK( wxCommandEvent& event );


	public:
		CombatFleetsGUI( wxWindow* parent, FSystem *sys, FleetList fList, wxArrayInt *sel, bool allowNoDefense, wxWindowID id = wxID_ANY, const wxString& title = wxT("Select Fleets for Combat"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCAPTION );
		~CombatFleetsGUI();

};

#endif //__CombatFleetsGUI__
