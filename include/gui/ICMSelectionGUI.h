///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 17 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __ICMSelectionGUI__
#define __ICMSelectionGUI__

#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/font.h>
#include <wx/grid.h>
#include <wx/gdicmn.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/dialog.h>

#include "ships/FVehicle.h"
#include "gui/AssignedICMData.h"
#include <vector>
#include <map>

///////////////////////////////////////////////////////////////////////////

using namespace Frontier;

///////////////////////////////////////////////////////////////////////////////
/// Class ICMSelectionGUI
///////////////////////////////////////////////////////////////////////////////
class ICMSelectionGUI : public wxDialog
{
	private:
	
	protected:
		wxGrid* m_grid1;
		wxPanel* m_panel1;
		wxStaticText* m_staticText1;
		wxStaticText* m_staticText2;
		wxStaticText* m_staticText3;
		wxStaticText* m_staticText4;
		wxStaticText* m_staticText5;
		wxStaticText* m_staticText6;
		wxStaticText* m_staticText7;

		std::vector<wxStaticText*> m_shipNames;
		std::vector<wxStaticText*> m_ICMsAvailable;
		std::vector<wxSpinCtrl*> m_ICMsApplied;

		wxStaticText* m_staticText8;
		wxStaticText* m_staticText9;
		wxSpinCtrl* m_spinCtrl1;
		wxStaticText* m_staticText10;
		wxStaticText* m_staticText11;
		wxSpinCtrl* m_spinCtrl2;

		wxButton* m_button1;
		std::vector<ICMData *> *m_ICMData;
		wxFlexGridSizer* fgSizer3;
		wxFlexGridSizer* fgSizer1;
		wxGridSizer* gSizer1;
		
		// list of ships and where their ICMs have been assigned.  Key is the ship ID value
		std::map<unsigned int, AssignedICMData *> m_shipICMData;
		// data for current weapoin being worked with.
		ICMData * m_currentRowData;
		// current row in weapon list being worked on
		int m_currentRow;

		// Virtual event handlers, overide them in your derived class
		virtual void onPickRow( wxGridEvent& event );
		virtual void onPickWeapon( wxGridEvent& event );
		virtual void updateICMCount( wxSpinEvent& event );
		virtual void finalizeAssignments( wxCommandEvent& event );
		/**
		 * @brief updates the display for the selected weapon
		 *
		 * This method fills in all the data in GUI for the appropriate weapon
		 * selected.
		 *
		 * @param row the row of the display that was selected
		 *
		 * @author Tom Stephens
		 * @date Created:  Apr 25, 2010
		 * @date Last Modified:  Apr 25, 2010
		 */
		void selectWeapon(int row);
	
	public:
		ICMSelectionGUI( wxWindow* parent, std::vector<ICMData *> * ICMData, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = 0|wxTAB_TRAVERSAL );
		~ICMSelectionGUI();
	
};

#endif //__ICMSelectionGUI__
