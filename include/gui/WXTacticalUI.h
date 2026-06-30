/**
 * @file WXTacticalUI.h
 * @brief Concrete bridge from tactical game logic to wxWidgets dialogs.
 * @author Tom Stephens, gpt-5.3-codex (medium), claude-sonnet-4-6 (medium)
 * @date Created: Mar 29, 2026
 * @date Last Modified: Jun 30, 2026
 */

#ifndef _WXTACTICALUI_H_
#define _WXTACTICALUI_H_

#include "tactical/ITacticalUI.h"

class wxDialog;
class wxWindow;

namespace Frontier {

/**
 * @brief Concrete bridge from tactical game logic to wx dialogs.
 *
 * Implements ITacticalUI callbacks by opening wx modal dialogs and routing
 * redraw/winner notifications to the tactical parent window.
 *
 * A pointer to the currently active modal dialog (if any) is tracked so that
 * FBattleScreen can call dismissActiveDialog() before it hides itself. This
 * prevents the duplicate wxDialog::EndModal assert that otherwise occurs when
 * the parent frame's Show(false) triggers wxGTK's automatic EndModal on a
 * still-modal child dialog.
 *
 * @author Tom Stephens, gpt-5.3-codex (medium), claude-sonnet-4-6 (medium)
 * @date Created: Mar 29, 2026
 * @date Last Modified: Jun 30, 2026
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

  /**
   * @brief Returns true when a child modal dialog is currently open.
   *
   * Used by FBattleScreen to check whether a child dialog must be dismissed
   * before the frame is hidden during the close lifecycle.
   *
   * @return True when m_activeDialog is non-null and currently modal.
   * @author claude-sonnet-4-6 (medium)
   * @date Created: Jun 30, 2026
   * @date Last Modified: Jun 30, 2026
   */
  bool hasPendingDialog() const;

  /**
   * @brief Dismiss the currently active child modal dialog, if any.
   *
   * Calls EndModal(wxID_CANCEL) on the tracked dialog so that its IsModal()
   * flag is cleared before FBattleScreen calls Show(false). This prevents
   * the duplicate wxDialog::EndModal assert that wxGTK raises when it tries
   * to auto-dismiss a still-modal child as a side-effect of hiding the parent
   * frame.
   *
   * Safe to call when no dialog is active (no-op in that case).
   *
   * @author claude-sonnet-4-6 (medium)
   * @date Created: Jun 30, 2026
   * @date Last Modified: Jun 30, 2026
   */
  void dismissActiveDialog();

 private:
  static int s_modalAutoDismissMs;
  wxWindow* m_parent;
  /// Pointer to the child modal dialog currently open via showMessage,
  /// showDamageSummary, or runICMSelection; NULL when no dialog is active.
  wxDialog* m_activeDialog;
};

}  // namespace Frontier

#endif  // _WXTACTICALUI_H_
