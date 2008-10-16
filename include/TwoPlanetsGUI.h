///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  4 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __TwoPlanetsGUI__
#define __TwoPlanetsGUI__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dialog.h>

#include "FSystem.h"
using namespace Frontier;

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class TwoPlanetsGUI
///////////////////////////////////////////////////////////////////////////////
class TwoPlanetsGUI : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText1;
		wxButton* m_button1;
		wxButton* m_button2;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnButton1( wxCommandEvent& event ){ EndModal(1); event.Skip(); }
		virtual void OnButton2( wxCommandEvent& event ){ EndModal(2); event.Skip(); }
		
	
	public:
		TwoPlanetsGUI( wxWindow* parent, FSystem *sys, wxWindowID id = wxID_ANY, const wxString& title = wxT("Select which planet to attack."), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCAPTION );
		~TwoPlanetsGUI();
	
};

#endif //__TwoPlanetsGUI__
