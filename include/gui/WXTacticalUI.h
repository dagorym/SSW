/**
 * @file WXTacticalUI.h
 * @brief Concrete bridge from tactical game logic to wxWidgets dialogs.
 */

#ifndef _WXTACTICALUI_H_
#define _WXTACTICALUI_H_

#include "tactical/ITacticalUI.h"

class wxWindow;

namespace Frontier {

/**
 * @brief Concrete bridge from tactical game logic to wx dialogs.
 */
class WXTacticalUI : public ITacticalUI {
 public:
  explicit WXTacticalUI(wxWindow* parent);
  virtual ~WXTacticalUI();

  void requestRedraw();
  void showMessage(const std::string& title, const std::string& body);
  int showDamageSummary(const FTacticalCombatReportSummary& summary);
  int runICMSelection(std::vector<ICMData*>& icmData, VehicleList* defenders);
  void notifyWinner(bool attackerWins);

 private:
  wxWindow* m_parent;
};

}  // namespace Frontier

#endif  // _WXTACTICALUI_H_
