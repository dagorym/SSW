///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  4 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __ViewFleetGUI__
#define __ViewFleetGUI__

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
#include <wx/dialog.h>

#include "strategic/FFleet.h"
#include "strategic/FSystem.h"
using namespace Frontier;

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ViewFleetGUI
///////////////////////////////////////////////////////////////////////////////
class ViewFleetGUI : public wxDialog
{
	private:

	protected:
		wxStaticText* m_staticText1;
		wxStaticText* m_staticText2;
		wxStaticText* m_staticText3;
		wxStaticText* m_staticText4;
		wxStaticText* m_staticText5;
		wxStaticText* m_staticText6;
		wxStaticText* m_staticText7;
		wxStaticText* m_staticText8;
		wxStaticText* m_staticText9;
		wxStaticText *ADF,*MR,*HP,*DCR,*Weapons,*Defenses;
		wxListBox* m_listBox1;
		wxButton* m_button1;
		wxButton* m_button2;

		FFleet * m_fleet;
		FVehicle * m_ship;

		// Virtual event handlers, overide them in your derived class
		virtual void onSelectShip( wxMouseEvent& event );
		virtual void onModifyShip( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnOK( wxCommandEvent& event ){ EndModal(0); event.Skip(); }


	public:
		ViewFleetGUI( wxWindow* parent, FFleet * fleet, FSystem * sys, FSystem *dest, wxWindowID id = wxID_ANY, const wxString& title = wxT("View Fleet"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~ViewFleetGUI();

};

#endif //__ViewFleetGUI__
