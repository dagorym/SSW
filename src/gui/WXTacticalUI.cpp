/**
 * @file WXTacticalUI.cpp
 * @brief Concrete bridge from tactical game logic to wxWidgets dialogs.
 */

#include "gui/WXTacticalUI.h"

#include "gui/ICMSelectionGUI.h"
#include "gui/TacticalDamageSummaryGUI.h"

#include <wx/msgdlg.h>
#include <wx/window.h>

namespace Frontier {

WXTacticalUI::WXTacticalUI(wxWindow* parent) : m_parent(parent) {}

WXTacticalUI::~WXTacticalUI() {}

void WXTacticalUI::requestRedraw() {
  if (m_parent != NULL) {
    m_parent->Refresh();
  }
}

void WXTacticalUI::showMessage(const std::string& title,
                               const std::string& body) {
  if (m_parent != NULL) {
    wxMessageDialog dialog(m_parent, body, title, wxOK | wxICON_INFORMATION);
    dialog.ShowModal();
    return;
  }
  wxMessageOutputBest().Printf("%s\n%s", title.c_str(), body.c_str());
}

int WXTacticalUI::showDamageSummary(const FTacticalCombatReportSummary& summary) {
  if (m_parent == NULL) {
    return 1;
  }

  TacticalDamageSummaryGUI dialog(m_parent, summary);
  return dialog.ShowModal();
}

int WXTacticalUI::runICMSelection(std::vector<ICMData*>& icmData,
                                  VehicleList* defenders) {
  if (m_parent == NULL || defenders == NULL || defenders->size() == 0 ||
      icmData.size() == 0) {
    return 1;
  }

  ICMSelectionGUI dialog(m_parent, &icmData);
  return dialog.ShowModal();
}

void WXTacticalUI::notifyWinner(bool attackerWins) {
  std::string msg = "The winner of the battle is \nPlayer ";
  msg += attackerWins ? "Sathar" : "UPF";
  showMessage("Enemy Defeated!", msg);
}

}  // namespace Frontier
