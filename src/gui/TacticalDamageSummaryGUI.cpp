#include "gui/TacticalDamageSummaryGUI.h"

namespace {

static wxString toWxString(const std::string & value) {
	return wxString::FromUTF8(value.c_str());
}

}

TacticalDamageSummaryGUI::TacticalDamageSummaryGUI(
	wxWindow * parent,
	const FTacticalCombatReportSummary & summary,
	wxWindowID id,
	const wxString & title,
	const wxPoint & pos,
	const wxSize & size,
	long style)
	: wxDialog(parent, id, title.size() > 0 ? title : toWxString(tacticalCombatReportDialogTitle(summary.context)), pos, size, style),
	  m_summary(summary),
	  m_contextText(NULL),
	  m_summaryText(NULL),
	  m_closeButton(NULL) {
	SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxBoxSizer * rootSizer = new wxBoxSizer(wxVERTICAL);

	m_contextText = new wxStaticText(this, wxID_ANY, buildContextText(), wxDefaultPosition, wxDefaultSize, 0);
	m_contextText->Wrap(520);
	rootSizer->Add(m_contextText, 0, wxALL | wxEXPAND, 8);

	m_summaryText = new wxTextCtrl(
		this,
		wxID_ANY,
		buildSummaryText(),
		wxDefaultPosition,
		wxDefaultSize,
		wxTE_MULTILINE | wxTE_READONLY | wxTE_DONTWRAP);
	rootSizer->Add(m_summaryText, 1, wxALL | wxEXPAND, 8);

	wxBoxSizer * buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	buttonSizer->AddStretchSpacer(1);
	m_closeButton = new wxButton(this, wxID_OK, wxT("Close"));
	buttonSizer->Add(m_closeButton, 0, wxALL, 0);
	buttonSizer->AddStretchSpacer(1);
	rootSizer->Add(buttonSizer, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxEXPAND, 8);

	SetAffirmativeId(wxID_OK);
	m_closeButton->SetDefault();
	SetDefaultItem(m_closeButton);
	m_closeButton->SetFocus();

	SetSizer(rootSizer);
	Layout();
	Centre();

	m_closeButton->Bind(wxEVT_BUTTON, &TacticalDamageSummaryGUI::onClose, this);
}

TacticalDamageSummaryGUI::~TacticalDamageSummaryGUI() {
}

wxString TacticalDamageSummaryGUI::buildContextText() const {
	wxString text;
	text << wxT("Report: ") << toWxString(tacticalCombatReportContextLabel(m_summary.context));
	text << wxT("  |  ") << toWxString(tacticalCombatReportContextModeLabel(m_summary.context));
	if (m_summary.context.phase >= 0) {
		text << wxT("  |  Phase: ") << m_summary.context.phase;
	}
	return text;
}

wxString TacticalDamageSummaryGUI::buildSummaryText() const {
	wxString text;
	if (m_summary.ships.empty()) {
		text << wxT("No ships sustained damage in this report.");
		return text;
	}

	for (unsigned int i = 0; i < m_summary.ships.size(); i++) {
		const FTacticalShipReportSummary & shipSummary = m_summary.ships[i];
		if (shipSummary.displayLines.empty()) {
			text << toWxString(shipSummary.ship.shipName);
			text << wxT(": damage summary unavailable");
		} else {
			for (unsigned int j = 0; j < shipSummary.displayLines.size(); j++) {
				text << toWxString(shipSummary.displayLines[j]);
				if (j + 1 < shipSummary.displayLines.size()) {
					text << wxT("\n");
				}
			}
		}
		if (i + 1 < m_summary.ships.size()) {
			text << wxT("\n");
		}
	}

	return text;
}

void TacticalDamageSummaryGUI::onClose(wxCommandEvent & event) {
	EndModal(wxID_OK);
}
