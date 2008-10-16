///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  4 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __SatharRetreatGUI__
#define __SatharRetreatGUI__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/radiobut.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class SatharRetreatGUI
///////////////////////////////////////////////////////////////////////////////
class SatharRetreatGUI : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText1;
		wxRadioButton* m_radioBtn1;
		wxRadioButton* m_radioBtn2;
		wxRadioButton* m_radioBtn3;
		wxRadioButton* m_radioBtn4;
		wxRadioButton* m_radioBtn5;
		wxButton* m_button1;
		
		int m_choice;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnButtonSelection( wxCommandEvent& event );
		virtual void OnDone( wxCommandEvent& event );
		
	
	public:
		SatharRetreatGUI( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Select Sathar Retreat Conditions"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCAPTION );
		~SatharRetreatGUI();
	
};

#endif //__SatharRetreatGUI__
