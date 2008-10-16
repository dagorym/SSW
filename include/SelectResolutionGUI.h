///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  4 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __SelectResolutionGUI__
#define __SelectResolutionGUI__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dialog.h>

#include "FFleet.h"
#include "FVehicle.h"
using namespace Frontier;

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class SelectResolutionGUI
///////////////////////////////////////////////////////////////////////////////
class SelectResolutionGUI : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText1;
		wxButton* m_button1;
		wxButton* m_button2;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onBattleBoard( wxCommandEvent& event ){ EndModal(0); event.Skip(); }
		virtual void onManualResults( wxCommandEvent& event ){ EndModal(1); event.Skip(); }
		
	
	public:
		SelectResolutionGUI( wxWindow* parent, FleetList list, std::string sysName, FVehicle *station, wxWindowID id = wxID_ANY, const wxString& title = wxT("Select Combat Resolution Method"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCAPTION );
		~SelectResolutionGUI();
	
};

#endif //__SelectResolutionGUI__
