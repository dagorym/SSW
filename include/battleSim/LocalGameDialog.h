///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 17 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __LocalGameDialog__
#define __LocalGameDialog__

#include <wx/string.h>
#include <wx/button.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class LocalGameDialog
///////////////////////////////////////////////////////////////////////////////
class LocalGameDialog : public wxDialog 
{
	private:
	
	protected:
		wxButton* m_loadButton;
		wxButton* m_customGameButton;
		wxButton* m_predefinedButton;
		wxButton* m_backButton;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onLoad( wxCommandEvent& event ){ event.Skip(); }
		virtual void onCreateNew( wxCommandEvent& event );
		virtual void onPlayPredefined( wxCommandEvent& event );
		virtual void onQuit( wxCommandEvent& event ){ EndModal(0); event.Skip(); }
		
	
	public:
		LocalGameDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Play a Local Game"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~LocalGameDialog();
	
};

#endif //__LocalGameDialog__
