///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  4 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __SelectJumpGUI__
#define __SelectJumpGUI__

#include "FMap.h"

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

using namespace Frontier;

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class SelectJumpGUI
///////////////////////////////////////////////////////////////////////////////
class SelectJumpGUI : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText1;
		wxStaticText* m_staticText5;
		wxListBox* m_listBox1;
		wxStaticText* m_staticText2;
		wxStaticText* m_staticText3;
		wxStaticText* m_staticText4;
		wxButton* m_button1;
		wxButton* m_button2;
		wxButton* m_button3;
		wxButton* m_button4;
		
		FMap * m_map;
		int m_player;
		std::vector<std::string> m_sList;
		FFleet * m_fleet;
		FJumpRoute * m_jump;
		std::string m_system;
		wxWindow * m_parent;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onSelectJump( wxMouseEvent& event );
		virtual void onStandardJump( wxCommandEvent& event );
		virtual void onRiskJump2( wxCommandEvent& event );
		virtual void onRiskJump3( wxCommandEvent& event );
		virtual void onCancel( wxCommandEvent& event ){ EndModal(0); }
		
	
	public:
		SelectJumpGUI( wxWindow* parent, FFleet * fleet, FMap * map, std::string system, int player,
				wxWindowID id = wxID_ANY, const wxString& title = wxT("Select a Jump"),
				const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
				long style = wxDEFAULT_DIALOG_STYLE );
		~SelectJumpGUI();
	
	private:
		void setJump();
	
};

#endif //__SelectJumpGUI__
