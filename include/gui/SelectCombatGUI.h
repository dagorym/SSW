///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  4 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __SelectCombatGUI__
#define __SelectCombatGUI__

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

#include "strategic/FSystem.h"
#include "strategic/FPlayer.h"

using namespace Frontier;

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class SelectCombatGUI
///////////////////////////////////////////////////////////////////////////////
class SelectCombatGUI : public wxDialog
{
	private:
		void switchRoles();

	protected:
		wxStaticText* m_staticText1;
		wxListBox* m_listBox1;
		wxListBox* m_listBox2;
		wxButton* m_button1;
		wxButton* m_button2;
		wxButton* m_button3;

		// flag for which list box is active 0=left, 1=right
		int m_active;
		FFleet * m_curFleet;
		FSystem * m_system;
		FleetList m_defenderList, m_attackerList;
		bool m_satharAttacking;
		PlayerList *m_playerList;


		// Virtual event handlers, overide them in your derived class
		virtual void OnSelectLeftFleet( wxCommandEvent& event );
		virtual void onSelectRightFleet( wxCommandEvent& event );
		virtual void onDecline( wxCommandEvent& event );
		virtual void onView( wxCommandEvent& event );
		virtual void onAttack( wxCommandEvent& event );


	public:
		SelectCombatGUI( wxWindow* parent, FSystem * sys, FleetList defender, FleetList attacker, PlayerList *pList, bool satharAttacking = true, wxWindowID id = wxID_ANY, const wxString& title = wxT("Combat"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCAPTION );
		~SelectCombatGUI();

};

#endif //__SelectCombatGUI__
