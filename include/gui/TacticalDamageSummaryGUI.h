///////////////////////////////////////////////////////////////////////////
// Tactical damage summary dialog
///////////////////////////////////////////////////////////////////////////

#ifndef __TacticalDamageSummaryGUI__
#define __TacticalDamageSummaryGUI__

#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/gdicmn.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/textctrl.h>

#include "tactical/FTacticalCombatReport.h"

using namespace Frontier;

class TacticalDamageSummaryGUI : public wxDialog
{
private:
	FTacticalCombatReportSummary m_summary;
	wxStaticText * m_contextText;
	wxTextCtrl * m_summaryText;
	wxButton * m_closeButton;

	wxString buildContextText() const;
	wxString buildShipRollupText() const;
	wxString buildHitDetailText() const;
	wxString buildSummaryText() const;
	void onClose(wxCommandEvent & event);

public:
	TacticalDamageSummaryGUI(
		wxWindow * parent,
		const FTacticalCombatReportSummary & summary,
		wxWindowID id = wxID_ANY,
		const wxString & title = wxEmptyString,
		const wxPoint & pos = wxDefaultPosition,
		const wxSize & size = wxSize(560, 380),
		long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
	~TacticalDamageSummaryGUI();
};

#endif // __TacticalDamageSummaryGUI__
