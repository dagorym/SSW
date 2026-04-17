/**
 * @file WXStrategicUI.cpp
 * @brief Concrete bridge from strategic game logic to wxWidgets dialogs.
 */

#include "gui/WXStrategicUI.h"

#include "gui/SatharFleetsGUI.h"
#include "gui/SatharRetreatGUI.h"
#include "gui/SelectCombatGUI.h"
#include "gui/SystemDialogGUI.h"
#include "gui/UPFUnattachedGUI.h"
#include "gui/ViewFleetGUI.h"

#include <wx/app.h>
#include <wx/generic/msgdlgg.h>
#include <wx/window.h>

namespace Frontier {
namespace {

bool hasUsableWxUIRuntime() {
  return wxTheApp != NULL;
}

int cancelResult() {
  return wxID_CANCEL;
}

}  // namespace

WXStrategicUI::WXStrategicUI(wxWindow* parent) : m_parent(parent) {}

WXStrategicUI::~WXStrategicUI() {}

void WXStrategicUI::showMessage(const std::string& title,
                                const std::string& body) {
  if (m_parent != NULL && hasUsableWxUIRuntime()) {
    wxGenericMessageDialog dialog(m_parent, body, title,
                                  wxOK | wxICON_INFORMATION);
    dialog.CentreOnParent(wxBOTH);
    dialog.ShowModal();
    return;
  }
  wxMessageOutputBest().Printf("%s\n%s", title.c_str(), body.c_str());
}

void WXStrategicUI::notifyFailedJump(const std::string& fleetName) {
  showMessage("Failed Risk Jump",
              "The fleet " + fleetName +
                  " has failed it's risk jump and has been lost");
}

void WXStrategicUI::notifyVictory(int result) {
  switch (result) {
    case 1:
      showMessage(
          "UPF Victory!",
          "The Sathar have been forced to retreat and there\n"
          "are at least two UPF Fortress still intact.\n\n"
          "The UPF win!");
      break;
    case 2:
      showMessage("Sathar Victory!",
                  "The Sathar have destroyed twelve stations including\n"
                  "all four Fortresses.\n\n"
                  "The Sathar win!");
      break;
    case 3:
      showMessage("It's a Draw!",
                  "Both sides have met draw conditions this round.");
      break;
    default:
      break;
  }
}

int WXStrategicUI::selectRetreatCondition() {
  if (!hasUsableWxUIRuntime()) {
    return cancelResult();
  }
  SatharRetreatGUI dialog(m_parent);
  if (m_parent != NULL) {
    dialog.CentreOnParent(wxBOTH);
  } else {
    dialog.CentreOnScreen(wxBOTH);
  }
  return dialog.ShowModal();
}

int WXStrategicUI::runUPFUnattachedSetup(FPlayer* player, FMap* map) {
  if (player == NULL || map == NULL) {
    return 1;
  }
  if (!hasUsableWxUIRuntime()) {
    return cancelResult();
  }
  UPFUnattachedGUI dialog(m_parent, player, map);
  if (m_parent != NULL) {
    dialog.CentreOnParent(wxBOTH);
  } else {
    dialog.CentreOnScreen(wxBOTH);
  }
  return dialog.ShowModal();
}

int WXStrategicUI::runSatharFleetSetup(FPlayer* player,
                                       FMap* map,
                                       bool isInitialSetup) {
  if (player == NULL || map == NULL) {
    return 1;
  }
  if (!hasUsableWxUIRuntime()) {
    return cancelResult();
  }
  SatharFleetsGUI dialog(m_parent, player, map, isInitialSetup);
  if (m_parent != NULL) {
    dialog.CentreOnParent(wxBOTH);
  } else {
    dialog.CentreOnScreen(wxBOTH);
  }
  return dialog.ShowModal();
}

void WXStrategicUI::showSystemDialog(FSystem* sys, FMap* map, FPlayer* player) {
  if (sys == NULL || map == NULL || player == NULL) {
    return;
  }
  if (!hasUsableWxUIRuntime()) {
    return;
  }
  std::string title = sys->getName() + " System Information";
  SystemDialogGUI dialog(m_parent, sys, map, player, title);
  if (m_parent != NULL) {
    dialog.CentreOnParent(wxBOTH);
  } else {
    dialog.CentreOnScreen(wxBOTH);
  }
  dialog.ShowModal();
}

void WXStrategicUI::showFleetDialog(FFleet* fleet,
                                    FSystem* location,
                                    FSystem* destination) {
  if (fleet == NULL || location == NULL) {
    return;
  }
  if (!hasUsableWxUIRuntime()) {
    return;
  }
  ViewFleetGUI dialog(m_parent, fleet, location, destination);
  if (m_parent != NULL) {
    dialog.CentreOnParent(wxBOTH);
  } else {
    dialog.CentreOnScreen(wxBOTH);
  }
  dialog.ShowModal();
}

void WXStrategicUI::showRetreatConditions(const std::string& text) {
  showMessage("Sathar Retreat Conditions", text);
}

int WXStrategicUI::selectCombat(FSystem* sys,
                                FleetList defenders,
                                FleetList attackers,
                                PlayerList* players) {
  if (sys == NULL || players == NULL) {
    return 1;
  }
  if (!hasUsableWxUIRuntime()) {
    return cancelResult();
  }
  SelectCombatGUI dialog(m_parent, sys, defenders, attackers, players);
  if (m_parent != NULL) {
    dialog.CentreOnParent(wxBOTH);
  } else {
    dialog.CentreOnScreen(wxBOTH);
  }
  return dialog.ShowModal();
}

void WXStrategicUI::requestRedraw() {
  if (m_parent != NULL) {
    m_parent->Refresh();
  }
}

}  // namespace Frontier
