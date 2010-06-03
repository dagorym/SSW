///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 17 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __BattleSimFrame__
#define __BattleSimFrame__

#include <wx/string.h>
#include <wx/button.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class BattleSimFrame
///////////////////////////////////////////////////////////////////////////////
class BattleSimFrame : public wxFrame 
{
	private:
	
	protected:
		wxButton* m_localGame;
		wxButton* m_networkGame;
		wxButton* m_quitButton;
		
		// Virtual event handlers, overide them in your derived class
//		virtual void onQuit( wxCloseEvent& event ){ event.Skip(); }
		virtual void onPlayLocal( wxCommandEvent& event );
		virtual void onPlayNetwork( wxCommandEvent& event ){ event.Skip(); }
		virtual void onQuit( wxCommandEvent& event ){ exit(0); event.Skip(); }
		
	
	public:
		BattleSimFrame( const wxString& title = wxT("Star Frontiers Tactical Combat Simulator"), const wxPoint& pos = wxPoint(50,50), const wxSize& size = wxSize( 335,200 ), long style = wxCAPTION|wxTAB_TRAVERSAL );
		~BattleSimFrame();
		/// Event table constuction
		DECLARE_EVENT_TABLE()

	
};

#endif //__BattleSimFrame__
