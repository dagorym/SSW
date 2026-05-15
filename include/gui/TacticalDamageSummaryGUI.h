/**
 * @file TacticalDamageSummaryGUI.h
 * @brief Tactical damage summary dialog declaration.
 * @author Tom Stephens, gpt-5.3-codex (medium)
 * @date Created: Mar 22, 2026
 * @date Last Modified: Apr 15, 2026
 */

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

/**
 * @brief Modal dialog that presents tactical combat damage output.
 *
 * Displays report context, per-ship rollup text, and optional hit-detail
 * lines generated from FTacticalCombatReportSummary.
 *
 * @author Tom Stephens, gpt-5.3-codex (medium)
 * @date Created: Mar 22, 2026
 * @date Last Modified: Apr 15, 2026
 */
class TacticalDamageSummaryGUI : public wxDialog
{
private:
	FTacticalCombatReportSummary m_summary;
	wxStaticText * m_contextText;
	wxTextCtrl * m_summaryText;
	wxButton * m_closeButton;

	/**
	 * @brief Build the single-line context text shown above the summary body.
	 *
	 * @return Context label text including report mode and phase data.
	 *
	 * @author Tom Stephens, gpt-5.3-codex (medium)
	 * @date Created: Mar 22, 2026
	 * @date Last Modified: Apr 15, 2026
	 */
	wxString buildContextText() const;
	/**
	 * @brief Build the per-ship damage rollup section.
	 *
	 * @return Multi-line ship summary text, or an empty-damage message.
	 *
	 * @author Tom Stephens, gpt-5.3-codex (medium)
	 * @date Created: Mar 22, 2026
	 * @date Last Modified: Apr 15, 2026
	 */
	wxString buildShipRollupText() const;
	/**
	 * @brief Build optional hit-detail rows when detail output is enabled.
	 *
	 * @return Hit-detail section body text, or empty when hidden/unavailable.
	 *
	 * @author Tom Stephens, gpt-5.3-codex (medium)
	 * @date Created: Apr 08, 2026
	 * @date Last Modified: Apr 08, 2026
	 */
	wxString buildHitDetailText() const;
	/**
	 * @brief Compose the complete text body for the summary text control.
	 *
	 * @return Formatted summary body containing ship rollups and hit details.
	 *
	 * @author Tom Stephens, gpt-5.3-codex (medium)
	 * @date Created: Mar 22, 2026
	 * @date Last Modified: Apr 08, 2026
	 */
	wxString buildSummaryText() const;

public:
	/**
	 * @brief Construct the tactical damage summary dialog.
	 *
	 * Initializes controls, populates report text, and centers the dialog on
	 * the parent window or on the active display when unparented.
	 *
	 * @param parent Parent window for modal ownership.
	 * @param summary Tactical report summary to render.
	 * @param id wxWidgets dialog ID.
	 * @param title Dialog title; defaults to the report-context title.
	 * @param pos Requested dialog position.
	 * @param size Requested dialog size.
	 * @param style wxWidgets dialog style flags.
	 *
	 * @author Tom Stephens, gpt-5.3-codex (medium)
	 * @date Created: Mar 22, 2026
	 * @date Last Modified: Apr 15, 2026
	 */
	TacticalDamageSummaryGUI(
		wxWindow * parent,
		const FTacticalCombatReportSummary & summary,
		wxWindowID id = wxID_ANY,
		const wxString & title = wxEmptyString,
		const wxPoint & pos = wxDefaultPosition,
		const wxSize & size = wxSize(560, 380),
		long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
	/**
	 * @brief Destroy the tactical damage summary dialog.
	 *
	 * @author Tom Stephens, gpt-5.3-codex (medium)
	 * @date Created: Mar 22, 2026
	 * @date Last Modified: Mar 22, 2026
	 */
	~TacticalDamageSummaryGUI();
};

#endif // __TacticalDamageSummaryGUI__
