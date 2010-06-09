///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 17 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __ScenarioEditorGUI__
#define __ScenarioEditorGUI__

#include <wx/string.h>
#include <wx/choice.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/listbox.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/radiobut.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>

#include <strategic/FFleet.h>
using namespace Frontier;

typedef std::vector<std::string> StringList;

///////////////////////////////////////////////////////////////////////////////
/// Class ScenarioEditorGUI
///////////////////////////////////////////////////////////////////////////////
class ScenarioEditorGUI : public wxDialog 
{
	private:
	/// list of available ships type to be passed around
	StringList m_availableShipTypes;

	/**
	 * @brief Gives list of allowed ships
	 *
	 * Based on the teamID this method returns a list of ship types
	 * that the player can select to have as part of his fleet.
	 *
	 * @param teamID  The selection id of the team name (1=UPF, 2=Sathar, 3=Pirates)
	 *
	 * @author Tom Stephens
	 * @date Created:  Jun 7, 2010
	 * @date Last Modififed:  Jun 7, 2010
	 */
	StringList getAvailableShipTypes(int teamID);
	
	/**
	 * @brief Converts the ship names into the types needed to create them
	 *
	 * This method loops over the list of ships in the list box that is
	 * passed in and returns a StringList of the type names pass to the
	 * createShip factory function to generate the ship.
	 *
	 * @param ships  The wxListBox containing the list of assigned ships
	 *
	 * @author Tom Stephens
	 * @date Created:  Jun 8, 2010
	 * @date Last Modififed:  Jun 8, 2010
	 */
	StringList convertNames(wxListBox * ships);

	FFleet * createFleet(StringList ships, std::string teamName, unsigned int id);

	protected:
		wxChoice* m_defenderTeam;
		wxListBox* m_availableDefenderListBox;
		wxButton* m_defenderAddButton;
		wxStaticText* m_staticText4;
		wxStaticText* m_staticText5;
		wxButton* m_defenderRemoveButton;
		wxListBox* m_assignedDefenderListBox;
		wxStaticText* m_hasPlanetText;
		wxRadioButton* m_planetYes;
		wxRadioButton* m_planetNo;
		wxStaticText* m_staticText3;
		wxChoice* m_stationChoice;
		wxChoice* m_attackerTeam;
		wxListBox* m_availableAttackerListBox;
		wxButton* m_attackerAddButton;
		wxStaticText* m_staticText6;
		wxStaticText* m_staticText7;
		wxButton* m_attackerRemoveButton;
		wxListBox* m_assignedAttackerListBox;
		wxButton* m_cancelButton;
		wxButton* m_startBattleButton;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onSelectDefenderTeam( wxCommandEvent& event );
		virtual void onSelectDefenderType( wxCommandEvent& event );
		virtual void onAddDefenderShip( wxCommandEvent& event );
		virtual void onRemoveDefenderShip( wxCommandEvent& event );
		virtual void onSelectDefenderShip( wxCommandEvent& event );
		virtual void onSelectStationYes( wxCommandEvent& event );
		virtual void onSelectStationNo( wxCommandEvent& event );
		virtual void onSelectStationType( wxCommandEvent& event );
		virtual void onSelectAttackerTeam( wxCommandEvent& event );
		virtual void onSelectAttackerType( wxCommandEvent& event );
		virtual void onAddAttackerShip( wxCommandEvent& event );
		virtual void onRemoveAttackerShip( wxCommandEvent& event );
		virtual void onSelectAttakerShip( wxCommandEvent& event );
		virtual void onCancel( wxCommandEvent& event ){ EndModal(0); event.Skip(); }
		virtual void onStartBattle( wxCommandEvent& event );
		
	
	public:
		ScenarioEditorGUI( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Create a Custom Scenario"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~ScenarioEditorGUI();
	
};

#endif //__ScenarioEditorGUI__
