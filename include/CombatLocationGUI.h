///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  4 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __CombatLocationGUI__
#define __CombatLocationGUI__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dialog.h>

#include "FPlanet.h"
using namespace Frontier;

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class CombatLocationGUI
///////////////////////////////////////////////////////////////////////////////
class CombatLocationGUI : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText1;
		wxButton* m_button1;
		wxButton* m_button2;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnButton1( wxCommandEvent& event ){ EndModal(0); event.Skip(); }
		virtual void OnButton2( wxCommandEvent& event ){ EndModal(1); event.Skip(); }
		
	
	public:
		CombatLocationGUI( wxWindow* parent, FPlanet *p, wxWindowID id = wxID_ANY, const wxString& title = wxT("Select Combat Location"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCAPTION );
		~CombatLocationGUI();
	
};

#endif //__CombatLocationGUI__
