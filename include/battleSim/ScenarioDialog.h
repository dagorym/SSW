///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 17 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __ScenarioDialog__
#define __ScenarioDialog__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ScenarioDialog
///////////////////////////////////////////////////////////////////////////////
class ScenarioDialog : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_buttonHeader;
		wxStaticText* m_secenarioHeader;
		wxButton* m_scenario1Button;
		wxStaticText* m_staticText1;
		wxButton* m_scenario2Button;
		wxStaticText* m_staticText2;
		wxButton* m_scenario3Button;
		wxStaticText* m_staticText3;
		wxButton* m_scenario4Button;
		wxStaticText* m_staticText4;
		wxButton* m_doneButton;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onScenario1( wxCommandEvent& event ){ event.Skip(); }
		virtual void onScenario2( wxCommandEvent& event ){ event.Skip(); }
		virtual void onScenario3( wxCommandEvent& event ){ event.Skip(); }
		virtual void onScenario4( wxCommandEvent& event ){ event.Skip(); }
		virtual void onDone( wxCommandEvent& event ){ EndModal(0); event.Skip(); }
		
	
	public:
		ScenarioDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Play a Predefined Scenario"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~ScenarioDialog();
	
};

#endif //__ScenarioDialog__
