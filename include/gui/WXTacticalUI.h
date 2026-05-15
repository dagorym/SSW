/**
 * @file WXTacticalUI.h
 * @brief Concrete bridge from tactical game logic to wxWidgets dialogs.
 * @author Tom Stephens, gpt-5.3-codex (medium)
 * @date Created: Mar 29, 2026
 * @date Last Modified: Apr 15, 2026
 */

#ifndef _WXTACTICALUI_H_
#define _WXTACTICALUI_H_

#include "tactical/ITacticalUI.h"

class wxWindow;

namespace Frontier {

/**
 * @brief Concrete bridge from tactical game logic to wx dialogs.
 *
 * Implements ITacticalUI callbacks by opening wx modal dialogs and routing
 * redraw/winner notifications to the tactical parent window.
 *
 * @author Tom Stephens, gpt-5.3-codex (medium)
 * @date Created: Mar 29, 2026
 * @date Last Modified: Apr 15, 2026
 */
class WXTacticalUI : public ITacticalUI {
 public:
  /**
   * @brief Construct the tactical UI adapter.
   * @param parent Parent window used for modal ownership and redraws.
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created: Mar 29, 2026
   * @date Last Modified: Mar 29, 2026
   */
  explicit WXTacticalUI(wxWindow* parent);
  /**
   * @brief Destroy the tactical UI adapter.
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created: Mar 29, 2026
   * @date Last Modified: Mar 29, 2026
   */
  virtual ~WXTacticalUI();

  /**
   * @brief Request a redraw from the owning parent window.
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created: Mar 29, 2026
   * @date Last Modified: Mar 29, 2026
   */
  void requestRedraw();
  /**
   * @brief Show a tactical message dialog or fallback output.
   * @param title Message-box title text.
   * @param body Message body text.
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created: Mar 29, 2026
   * @date Last Modified: Apr 04, 2026
   */
  void showMessage(const std::string& title, const std::string& body);
  /**
   * @brief Show the tactical damage summary modal dialog.
   * @param summary Tactical report summary to display.
   * @return Modal result code from the summary dialog.
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created: Mar 29, 2026
   * @date Last Modified: Apr 15, 2026
   */
  int showDamageSummary(const FTacticalCombatReportSummary& summary);
  /**
   * @brief Run ICM assignment selection for defender ships.
   * @param icmData Weapon-to-ICM assignment rows to edit.
   * @param defenders Defending ships used to validate ICM selection.
   * @return Dialog result code, or 1 when selection cannot be run.
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created: Mar 29, 2026
   * @date Last Modified: Mar 29, 2026
   */
  int runICMSelection(std::vector<ICMData*>& icmData, VehicleList* defenders);
  /**
   * @brief Announce the battle winner.
   * @param attackerWins True when the attacking side won.
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created: Mar 29, 2026
   * @date Last Modified: Mar 29, 2026
   */
  void notifyWinner(bool attackerWins);
  /**
   * @brief Configure optional auto-dismiss timeout for modal UI tests.
   * @param timeoutMs Timeout in milliseconds; zero disables auto-dismiss.
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created: Mar 29, 2026
   * @date Last Modified: Apr 04, 2026
   */
  static void setModalAutoDismissMs(int timeoutMs);

 private:
  static int s_modalAutoDismissMs;
  wxWindow* m_parent;
};

}  // namespace Frontier

#endif  // _WXTACTICALUI_H_
