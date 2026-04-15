/**
 * @file WXTacticalUI.cpp
 * @brief Concrete bridge from tactical game logic to wxWidgets dialogs.
 */

#include "gui/WXTacticalUI.h"

#include "gui/ICMSelectionGUI.h"
#include "gui/TacticalDamageSummaryGUI.h"

#include <wx/generic/msgdlgg.h>
#include <wx/msgdlg.h>
#include <wx/timer.h>
#include <wx/window.h>

namespace Frontier {

int WXTacticalUI::s_modalAutoDismissMs = 0;

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
    class ModalAutoDismissTimer : public wxTimer {
     public:
      explicit ModalAutoDismissTimer(wxDialog* dialog) : m_dialog(dialog) {}
      virtual void Notify() wxOVERRIDE {
        if (m_dialog != NULL && m_dialog->IsModal()) {
          m_dialog->EndModal(wxID_OK);
        }
      }

     private:
      wxDialog* m_dialog;
    };

    wxGenericMessageDialog dialog(m_parent, body, title, wxOK | wxICON_INFORMATION);
    dialog.CentreOnParent(wxBOTH);
    ModalAutoDismissTimer* autoDismiss = NULL;
    if (s_modalAutoDismissMs > 0) {
      autoDismiss = new ModalAutoDismissTimer(&dialog);
      autoDismiss->Start(s_modalAutoDismissMs, true);
    }
    dialog.ShowModal();
    if (autoDismiss != NULL) {
      delete autoDismiss;
    }
    return;
  }
  wxMessageOutputBest().Printf("%s\n%s", title.c_str(), body.c_str());
}

int WXTacticalUI::showDamageSummary(const FTacticalCombatReportSummary& summary) {
  TacticalDamageSummaryGUI dialog(m_parent, summary);
  if (m_parent != NULL) {
    dialog.CentreOnParent(wxBOTH);
  } else {
    dialog.Centre(wxBOTH);
  }
  return dialog.ShowModal();
}

int WXTacticalUI::runICMSelection(std::vector<ICMData*>& icmData,
                                  VehicleList* defenders) {
  if (defenders == NULL || defenders->size() == 0 || icmData.size() == 0) {
    return 1;
  }

  ICMSelectionGUI dialog(m_parent, &icmData);
  if (m_parent != NULL) {
    dialog.CentreOnParent(wxBOTH);
  } else {
    dialog.Centre(wxBOTH);
  }
  return dialog.ShowModal();
}

void WXTacticalUI::notifyWinner(bool attackerWins) {
  std::string msg = "The winner of the battle is \nPlayer ";
  msg += attackerWins ? "Sathar" : "UPF";
  showMessage("Enemy Defeated!", msg);
}

void WXTacticalUI::setModalAutoDismissMs(int timeoutMs) {
  s_modalAutoDismissMs = timeoutMs;
}

}  // namespace Frontier
