///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  4 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __SatharFleetsGUI__
#define __SatharFleetsGUI__

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
#include <wx/choice.h>
#include <wx/textctrl.h>
#include <wx/dialog.h>

#include "strategic/FPlayer.h"
#include "strategic/FMap.h"
using namespace Frontier;

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class SatharFleetsGUI
///////////////////////////////////////////////////////////////////////////////
class SatharFleetsGUI : public wxDialog
{
	private:
		/// clears out the ships added to m_curFleet if not acutally commited
		void clearFleet();
		/// redraws the updated fleets in the list boxes
		void redrawListBoxes();

	protected:
		wxStaticText* m_staticText1;
		wxListBox* m_listBox1;
		wxButton* m_button1;
		wxButton* m_button2;
		wxChoice* m_choice1;
		wxStaticText* m_staticText2;
		wxTextCtrl* m_textCtrl1;
		wxChoice* m_choice2;
		wxListBox* m_listBox2;
		wxButton* m_button3;
		wxButton* m_button4;
		wxButton* m_button5;

		FPlayer * m_player;
		FMap * m_map;
		std::string m_fleetName;
		FFleet *m_newFleet, *m_curFleet;
		//VehicleList m_shipList;
		VehicleList * m_unattached;
		unsigned int m_origSize;
		unsigned int m_sysID;
		FSystem * m_system;
		unsigned int m_setFleetCount;
		int m_totalShips;
		int m_largestFleet;
		wxWindow * m_window;
		unsigned int m_origMaxFleetID;

		// Virtual event handlers, overide them in your derived class
		virtual void onAddShip( wxCommandEvent& event );
		virtual void onRemoveShip( wxCommandEvent& event );
		virtual void onPickFleet( wxCommandEvent& event );
		virtual void onStoreName( wxFocusEvent& event );
		virtual void onSelectSystem( wxCommandEvent& event );
		virtual void onSelectShip( wxMouseEvent& event ){ m_button1->Enable(true); event.Skip(); }
		virtual void onCheckShip( wxMouseEvent& event );
		virtual void onUpdateFleet( wxCommandEvent& event );
		virtual void onDone( wxCommandEvent& event ){ EndModal(0); event.Skip(); }
		virtual void onCancel( wxCommandEvent& event);

	public:
		SatharFleetsGUI( wxWindow* parent, FPlayer * player, FMap * map, bool setup, wxWindowID id = wxID_ANY, const wxString& title = wxT("Create Sathar Fleets"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCAPTION );
		~SatharFleetsGUI();

};

#endif //__SatharFleetsGUI__
