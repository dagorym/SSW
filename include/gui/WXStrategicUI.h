/**
 * @file WXStrategicUI.h
 * @brief Concrete bridge from strategic game logic to wxWidgets dialogs.
 */

#ifndef _WXSTRATEGICUI_H_
#define _WXSTRATEGICUI_H_

#include <string>

#include "strategic/IStrategicUI.h"

class wxWindow;

namespace Frontier {

/**
 * @brief Concrete bridge from strategic game logic to wx dialogs.
 */
class WXStrategicUI : public IStrategicUI {
 public:
  explicit WXStrategicUI(wxWindow* parent);
  virtual ~WXStrategicUI();

  void showMessage(const std::string& title, const std::string& body);
  void notifyFailedJump(const std::string& fleetName);
  void notifyVictory(int result);

  int selectRetreatCondition();
  int runUPFUnattachedSetup(FPlayer* player, FMap* map);
  int runSatharFleetSetup(FPlayer* player, FMap* map, bool isInitialSetup);

  void showSystemDialog(FSystem* sys, FMap* map, FPlayer* player);
  void showFleetDialog(FFleet* fleet, FSystem* location, FSystem* destination);
  void showRetreatConditions(const std::string& text);

  int selectCombat(FSystem* sys,
                   FleetList defenders,
                   FleetList attackers,
                   PlayerList* players);

  void requestRedraw();

 private:
  wxWindow* m_parent;
};

}  // namespace Frontier

#endif  // _WXSTRATEGICUI_H_
