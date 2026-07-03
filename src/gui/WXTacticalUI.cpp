/**
 * @file WXTacticalUI.cpp
 * @brief Concrete bridge from tactical game logic to wxWidgets dialogs.
 * @author Tom Stephens, gpt-5.3-codex (medium), claude-sonnet-4-6 (medium), Claude Sonnet 5 (medium)
 * @date Created: Mar 29, 2026
 * @date Last Modified: Jul 03, 2026
 */

#include "gui/WXTacticalUI.h"

#include "gui/ICMSelectionGUI.h"
#include "gui/TacticalDamageSummaryGUI.h"

#include <wx/dialog.h>
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
    m_dialogStack.push_back(&dialog);
    dialog.ShowModal();
    if (!m_dialogStack.empty()) {
      m_dialogStack.pop_back();
    }
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
    dialog.CentreOnScreen(wxBOTH);
  }
  m_dialogStack.push_back(&dialog);
  int result = dialog.ShowModal();
  if (!m_dialogStack.empty()) {
    m_dialogStack.pop_back();
  }
  return result;
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
    dialog.CentreOnScreen(wxBOTH);
  }
  m_dialogStack.push_back(&dialog);
  int result = dialog.ShowModal();
  if (!m_dialogStack.empty()) {
    m_dialogStack.pop_back();
  }
  return result;
}

void WXTacticalUI::notifyWinner(bool attackerWins) {
  std::string msg = "The winner of the battle is \nPlayer ";
  msg += attackerWins ? "Sathar" : "UPF";
  showMessage("Enemy Defeated!", msg);
}

void WXTacticalUI::setModalAutoDismissMs(int timeoutMs) {
  s_modalAutoDismissMs = timeoutMs;
}

bool WXTacticalUI::hasPendingDialog() const {
  for (std::vector<wxDialog*>::const_reverse_iterator it = m_dialogStack.rbegin();
       it != m_dialogStack.rend(); ++it) {
    wxDialog* dialog = *it;
    if (dialog != NULL && dialog->IsModal()) {
      return true;
    }
  }
  return false;
}

void WXTacticalUI::dismissActiveDialog() {
  // Walk innermost-first (back of the stack, i.e. most recently opened) to
  // outermost so every live child dialog's EndModal() runs before this
  // method returns. Each dialog's own showMessage()/showDamageSummary()/
  // runICMSelection() call pops it from the stack once its ShowModal() call
  // unwinds; this loop only calls EndModal() and never mutates the stack
  // directly.
  for (std::vector<wxDialog*>::const_reverse_iterator it = m_dialogStack.rbegin();
       it != m_dialogStack.rend(); ++it) {
    wxDialog* dialog = *it;
    if (dialog != NULL && dialog->IsModal()) {
      dialog->EndModal(wxID_CANCEL);
    }
  }
}

}  // namespace Frontier
